/**
 * @file Meshcat.cpp
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#include <MeshcatCpp/Material.h>
#include <MeshcatCpp/MatrixView.h>
#include <MeshcatCpp/Meshcat.h>
#include <MeshcatCpp/Property.h>
#include <MeshcatCpp/Shape.h>

#include <MeshcatCpp/impl/FindResource.h>
#include <MeshcatCpp/impl/MsgpackTypes.h>
#include <MeshcatCpp/impl/TreeNode.h>
#include <MeshcatCpp/impl/UUIDGenerator.h>

#include <cstddef>
#include <fstream>
#include <future>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

// uWebSockets
#include <App.h>

// msgpack
#include <msgpack.hpp>
#include <vector>

// cmrc
#include <cmrc/cmrc.hpp>
CMRC_DECLARE(MeshcatCpp);

namespace MeshcatCpp::details
{

struct TransformData
{
    std::string type{"set_transform"};
    std::string path;
    std::array<double, 16> matrix;
    MSGPACK_DEFINE_MAP(type, path, matrix);

    MeshcatCpp::MatrixView<double> transform();
    MeshcatCpp::MatrixView<const double> transform() const;
};

MeshcatCpp::MatrixView<double> TransformData::transform()
{
    constexpr MatrixView<double>::index_type rows = 4;
    constexpr MatrixView<double>::index_type cols = 4;
    constexpr auto order = MeshcatCpp::MatrixStorageOrdering::ColumnMajor;
    return make_matrix_view(this->matrix.data(), rows, cols, order);
}

MeshcatCpp::MatrixView<const double> TransformData::transform() const
{
    constexpr MatrixView<const double>::index_type rows = 4;
    constexpr MatrixView<const double>::index_type cols = 4;
    constexpr auto order = MeshcatCpp::MatrixStorageOrdering::ColumnMajor;
    return make_matrix_view(this->matrix.data(), rows, cols, order);
}

template <typename T> struct PropertyTrampoline : public ::MeshcatCpp::Property<T>
{
    // TOFO make it const
    std::string type{"set_property"};

    MSGPACK_DEFINE_MAP(type,
                       MSGPACK_NVP("path", ::MeshcatCpp::Property<T>::path),
                       MSGPACK_NVP("property", ::MeshcatCpp::Property<T>::property),
                       MSGPACK_NVP("value", ::MeshcatCpp::Property<T>::value));
};

} // namespace MeshcatCpp::details

namespace MeshcatCpp
{

constexpr static bool use_ssl = false;
constexpr static bool is_server = true;

using PerSocketData = std::monostate;

using WebSocket = uWS::WebSocket<use_ssl, is_server, PerSocketData>;

struct Node
{
    // The msgpack'd set_object command.
    std::optional<std::string> object;
    // The msgpack'd set_transform command.
    std::optional<std::string> transform;
    // The msgpack'd set_property command(s).
    std::map<std::string, std::string> properties;

    void send(WebSocket* ws) const
    {
        if (this->object)
        {
            ws->send(this->object.value());
        }
        if (this->transform)
        {
            ws->send(this->transform.value());
        }
        for (const auto& [property, msg] : this->properties)
        {
            ws->send(msg);
        }
    }
};

class Meshcat::Impl
{
public:
    Impl& operator=(const Impl&) = delete;
    Impl(const Impl&) = delete;

    Impl()
    {
        if (!this->load_file("misc/index.html", this->index_html_))
        {
            throw std::runtime_error("Unable to load index.html");
        }

        if (!this->load_file("misc/favicon.ico", this->favicon_))
        {
            throw std::runtime_error("Unable to load index.html");
        }

        if (!this->load_file("misc/main.min.js", this->main_min_js_))
        {
            throw std::runtime_error("Unable to load main.min.js");
        }

        this->root_ = std::make_shared<MeshcatCpp::details::TreeNode<Node>>();
        this->app_future_ = app_promise_.get_future();
    }

    ~Impl()
    {
        loop_->defer([this]() { us_listen_socket_close(0, this->listen_socket_); });
        this->websocket_thread_.join();
    }

    void websocket_main()
    {
        int port = 7001;
        const int kMaxPort = 7099;

        uWS::App::WebSocketBehavior<PerSocketData> behavior;

        // Set maxBackpressure = 0 so that uWS does *not* drop any messages due to
        // back pressure.
        behavior.maxBackpressure = 0;
        behavior.open = [this](WebSocket* ws) {
            ws->subscribe("all");
            // Update this new connection with previously published data.
            this->send_tree(ws, this->root_);
        };

        uWS::App app = uWS::App()
                           .get("/*",
                                [this](uWS::HttpResponse<use_ssl>* res, uWS::HttpRequest* req) {
                                    if (req->getUrl() == "/main.min.js")
                                    {
                                        res->end(this->main_min_js_);
                                    } else if (req->getUrl() == "/favicon.ico")
                                    {
                                        res->end(this->favicon_);
                                    } else
                                    {
                                        res->end(this->index_html_);
                                    }
                                })
                           .ws<PerSocketData>("/*", std::move(behavior));

        us_listen_socket_t* listen_socket = nullptr;
        do
        {
            app.listen(port,
                       LIBUS_LISTEN_EXCLUSIVE_PORT,
                       [port, &listen_socket](us_listen_socket_t* socket) {
                           if (socket)
                           {
                               std::cout << "Meshcat listening for connections at http://127.0.0.1:"
                                         << port << std::endl;
                               listen_socket = socket;
                           }
                       });
        } while (listen_socket == nullptr && port++ <= kMaxPort);

        this->set_app_promise(&app, uWS::Loop::get(), port, listen_socket);

        app.run();

        // run() should not terminate.
        throw std::runtime_error("Meshcat websocket thread failed");
    }

    void get_app_future()
    {
        std::tie(this->app_, this->loop_, this->port_, this->listen_socket_)
            = this->app_future_.get();
    }

    template <typename T>
    void set_property(const Property<T>& property)
    {
        details::PropertyTrampoline<T> data{property};
        this->loop_->defer([this, data = std::move(data)]() {
            std::stringstream message_stream;
            msgpack::pack(message_stream, data);
            const std::string msg = message_stream.str();
            this->app_->publish("all", msg, uWS::OpCode::BINARY, false);
            (*this->root_)[data.path]->value().properties[data.property] = std::move(msg);
        });
    }

    template <typename T>
    void set_property(std::string_view path, const std::string& property, const T& value)
    {
        details::PropertyTrampoline<T> data{
            {.path = this->absolute_path(path), .property = property, .value = value}};

        this->loop_->defer([this, data = std::move(data)]() {
            std::stringstream message_stream;
            msgpack::pack(message_stream, data);
            const std::string msg = message_stream.str();
            this->app_->publish("all", msg, uWS::OpCode::BINARY, false);
            (*this->root_)[data.path]->value().properties[data.property] = std::move(msg);
        });
    }


    template <typename T>
    void set_object(std::string_view path, const T& shape, const Material& material)
    {
        static_assert(std::is_base_of_v<::MeshcatCpp::Shape, T>, "Invalid shape type");

        details::SetObjectData data{.path = this->absolute_path(path)};
        data.object.material = std::make_unique<details::MaterialTrampoline>(material);
        data.object.geometry = std::make_unique<typename details::traits<T>::trampoline>(shape);
        data.object.object.type = "Mesh";
        data.object.object.material = data.object.material->uuid;
        data.object.object.geometry = data.object.geometry->uuid;
        data.object.object.update_matrix_from_shape(shape);

        this->loop_->defer([this, data = std::move(data)]() {
            std::stringstream message_stream;
            msgpack::pack(message_stream, data);
            std::string msg = message_stream.str();
            this->app_->publish("all", msg, uWS::OpCode::BINARY, false);
            (*this->root_)[data.path]->value().object = std::move(msg);
        });
    }

    void set_transform(std::string_view path, const MatrixView<const double>& matrix)
    {
        details::TransformData data{.path = this->absolute_path(path)};

        auto matrix_view = data.transform();
        matrix_view = matrix;

        this->loop_->defer([this, data = std::move(data)]() {
            std::stringstream message_stream;
            msgpack::pack(message_stream, data);
            std::string msg = message_stream.str();
            this->app_->publish("all", msg, uWS::OpCode::BINARY, false);
            (*this->root_)[data.path]->value().transform = std::move(msg);
        });
    }

    std::thread websocket_thread_{};

private:
    static bool load_file(const std::string& filename, std::string& content)
    {
        auto fs = ::cmrc::MeshcatCpp::get_filesystem();
        if (!fs.exists(filename))
        {
            std::cerr << "Unable to find " << filename << " in the embedded filesystem"
                      << std::endl;
            return false;
        }
        try
        {
            const auto file = fs.open(filename);
            content = std::string(file.begin(), file.end());
        } catch (const std::exception& e)
        {
            std::cerr << "Unable to open " << filename << " in the embedded filesystem"
                      << std::endl;
            std::cerr << "The following exception has been throw " << e.what() << std::endl;

            return false;
        }

        return true;
    }

    std::string absolute_path(std::string_view path) const
    {
        assert(this->root_ != nullptr);

        while (!path.empty() && path.find_last_of(this->root_->string_separator) == path.size() - 1)
        {
            path.remove_prefix(this->root_->string_separator.size());
        }

        if (path.empty())
        {
            return this->prefix_;
        }

        if (path.find_first_of(this->root_->string_separator) == 0)
        {
            return std::string(path);
        }

        return this->prefix_ + this->root_->string_separator + std::string(path);
    }

    void set_app_promise(uWS::App* app, uWS::Loop* loop, int port, us_listen_socket_t* socket)
    {
        this->app_promise_.set_value(std::make_tuple(app, loop, port, socket));
    }

    void send_tree(WebSocket* ws, std::shared_ptr<details::TreeNode<Node>> node)
    {
        if (node == nullptr)
        {
            return;
        }

        node->value().send(ws);
        for (const auto& [name, child] : node->children())
        {
            assert(child != nullptr);
            send_tree(ws, child);
        }
    }

    std::string index_html_;
    std::string main_min_js_;
    std::string favicon_;

    std::promise<std::tuple<uWS::App*, uWS::Loop*, int, us_listen_socket_t*>> app_promise_{};
    std::future<std::tuple<uWS::App*, uWS::Loop*, int, us_listen_socket_t*>> app_future_{};

    // As explained in
    // https://github.com/uNetworking/uWebSockets/blob/d94bf2cd43bed5e0de396a8412f156e15c141e98/misc/READMORE.md#threading
    // Only loop_->defer() should be called from outside the websocket_thread.
    uWS::Loop* loop_{nullptr};

    // The remaining variables should only be accessed from the websocket_thread.
    uWS::App* app_{nullptr};
    us_listen_socket_t* listen_socket_{nullptr};
    int port_{-1};

    std::shared_ptr<details::TreeNode<Node>> root_;
    std::string prefix_{"meshcat"};
    };

Meshcat::Meshcat()
{
    // A std::promise is made in the WebSocketPublisher.
    this->pimpl_ = std::make_unique<Impl>();
    this->pimpl_->websocket_thread_
        = std::thread(&Meshcat::Impl::websocket_main, this->pimpl_.get());

    // The std::promise is full-filled in WebsocketMain; we wait here to obtain
    // that value.
    this->pimpl_->get_app_future();
}

Meshcat::~Meshcat() = default;

void Meshcat::join()
{
    this->pimpl_->websocket_thread_.join();
}

void Meshcat::set_property(std::string_view path, const std::string& property, bool value)
{
    this->pimpl_->set_property(path, property, value);
}

void Meshcat::set_object(std::string_view path, const Sphere& sphere, const Material& material)
{
    this->pimpl_->set_object(path, sphere, material);
}

void Meshcat::set_transform(std::string_view path, const MatrixView<const double>& matrix)
{
    this->pimpl_->set_transform(path, matrix);
}

void Meshcat::set_object(std::string_view path, const Cylinder& cylinder, const Material& material)
{
    this->pimpl_->set_object(path, cylinder, material);
}

void Meshcat::set_object(std::string_view path,
                         const Ellipsoid& ellipsoid,
                         const Material& material)
{
    this->pimpl_->set_object(path, ellipsoid, material);
}

void Meshcat::set_object(std::string_view path, const Mesh& mesh, const Material& material)
{
    this->pimpl_->set_object(path, mesh, material);
}

void Meshcat::set_object(std::string_view path, const Box& box, const Material& material)
{
    this->pimpl_->set_object(path, box, material);
}

} // namespace MeshcatCpp

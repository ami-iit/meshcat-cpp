#include <MeshcatCpp/Meshcat.h>

#include <fstream>
#include <future>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <stdexcept>

#include <App.h>
// #include <msgpack.hpp>

namespace MeshcatCpp
{

constexpr static bool use_ssl = false;
constexpr static bool is_server = true;

using PerSocketData = std::monostate;

using WebSocket = uWS::WebSocket<use_ssl, is_server, PerSocketData>;

class Meshcat::Impl
{
public:

    Impl& operator=(const Impl&) = delete;
    Impl(const Impl&) = delete;

    Impl()
    {
        if (!this->load_file("./misc/index.html", this->index_html_))
        {
            throw std::runtime_error("Unable to load index.html");
        }

        if (!this->load_file("./misc/favicon.ico", this->favicon_))
        {
            throw std::runtime_error("Unable to load index.html");
        }

        if (!this->load_file("./misc/main.min.js", this->main_min_js_))
        {
            throw std::runtime_error("Unable to load main.min.js");
        }

        this->app_future_ = app_promise_.get_future();
    }

    void websocket_main()
    {
        int port = 7001;
        const int kMaxPort = 7099;

        uWS::App::WebSocketBehavior<PerSocketData> behavior;
        behavior.open = [this](WebSocket* ws) {
            ws->subscribe("all");
            // Update this new connection with previously published data.
            this->send_tree(ws);
        };

        uWS::App app = uWS::App()
                           .get("/*",
                                [&](uWS::HttpResponse<use_ssl>* res, uWS::HttpRequest* req) {
                                    std::cerr << req->getUrl() << std::endl;
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

        bool listening = false;
        do
        {
            app.listen(port,
                       LIBUS_LISTEN_EXCLUSIVE_PORT,
                       [port, &listening](us_listen_socket_t* listenSocket) {
                           if (listenSocket)
                           {
                               std::cout << "Meshcat listening for connections at http://127.0.0.1:"
                                         << port << std::endl;
                               listening = true;
                           }
                       });
        } while (!listening && port++ <= kMaxPort);

        this->set_app_promise(&app, uWS::Loop::get());

        app.run();

        // run() should not terminate.  If it does, then we've failed.
        throw std::runtime_error("Meshcat websocket thread failed");
    }

    // Call this from main thread.
    void get_app_future()
    {
        std::tie(this->app_, this->loop_) = this->app_future_.get();
    }

    std::thread websocket_thread_{};

private:
    static bool load_file(std::string_view filename, std::string& content)
    {
        std::ifstream file(filename.data(), std::ios::in);
        if (!file.is_open())
        {
            return false;
        }

        std::stringstream ss;
        ss << file.rdbuf();
        file.close();

        content = ss.str();

        return true;
    }

    void set_app_promise(uWS::App* app, uWS::Loop* loop)
    {
        this->app_promise_.set_value(std::make_pair(app, loop));
    }

    void send_tree(WebSocket* ws)
    {
        // // TODO(russt): Generalize this to publishing the entire scene tree.
        // for (const auto& [key, msg] : this->set_properties_)
        // {
        //     ws->send(msg);
        // }
    }

    std::string index_html_;
    std::string main_min_js_;
    std::string favicon_;

    std::promise<std::pair<uWS::App*, uWS::Loop*>> app_promise_{};
    std::future<std::pair<uWS::App*, uWS::Loop*>> app_future_{};

    // Only loop_->defer() should be called from outside the websocket_thread. See
    // the documentation for uWebSockets for further details:
    // https://github.com/uNetworking/uWebSockets/blob/d94bf2cd43bed5e0de396a8412f156e15c141e98/misc/READMORE.md#threading
    uWS::Loop* loop_{nullptr};

    // The remaining variables should only be accessed from the websocket_thread.
    uWS::App* app_{nullptr};
};

Meshcat::Meshcat()
{
    // A std::promise is made in the WebSocketPublisher.
    this->pimpl_ = std::make_unique<Impl>();
    this->pimpl_->websocket_thread_ = std::thread(&Meshcat::Impl::websocket_main, this->pimpl_.get());

    // The std::promise is full-filled in WebsocketMain; we wait here to obtain
    // that value.
    this->pimpl_->get_app_future();
}

Meshcat::~Meshcat() = default;

void Meshcat::join()
{
    this->pimpl_->websocket_thread_.join();
}

// void Meshcat::SetProperty(const std::string& path, const std::string& property, bool value)
// {
//     publisher_->SetProperty(path, property, value);
// }


} // namespace MeshcatCpp

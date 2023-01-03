/**
 * @file TreeNode.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_TREE_NODE_H
#define MESHCAT_CPP_TREE_NODE_H

#include <cstddef>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>

namespace MeshcatCpp::details
{

/**
 * TreeNode represents the Node in Tree struct.
 */
template <class T> class TreeNode : public std::enable_shared_from_this<TreeNode<T>>
{
public:
    /**
     * Check if a child with a given name exist
     * @param[in] name The name of the child
     * @return True if the child exist false otherwise.
     */
    [[nodiscard]] bool child_exists(const std::string& name) const
    {
        return children_.find(name) != children_.end();
    }

    /**
     * Returns a reference to the value that is mapped to a key equivalent to key, performing an
     * insertion if such key does not already exist.
     * @param[in] path the relative path of the node.
     * @return Ponter to the mapped value of the new element if no child with key key existed.
     * Otherwise a pointer to the mapped value of the existing element whose path is equivalent to
     * path.
     * @note operator[] is non-const because it inserts a new node in the tree if it doesn't exist.
     * If this behavior is undesirable or if the container is const, at() may be used.
     */
    std::shared_ptr<TreeNode<T>> operator[](std::string_view path)
    {
        const auto separator_size = TreeNode<T>::string_separator.size();
        while (!path.empty() && path.find_last_of(TreeNode<T>::string_separator) == path.size() - 1)
        {
            path.remove_prefix(separator_size);
        }
        if (path.empty())
        {
            return this->shared_from_this();
        }

        const auto loc = path.find_first_of(TreeNode<T>::string_separator);

        const std::string name(path.substr(0, loc));

        auto child = children_.find(name);

        // Create the child if it doesn't exist.
        if (child == children_.end())
        {
            child = children_.emplace(name, std::make_shared<TreeNode<T>>()).first;
        }
        if (loc == std::string_view::npos)
        {
            return child->second;
        }

        return child->second->operator[](path.substr(loc + separator_size));
    }

    /**
     * Returns a reference to the mapped value of the element with key equivalent to key. If no such
     * element exists, an invalid weak_ptr is provided.
     * @param[in] path the relative path of the node.
     * @return Pointer to the mapped value of the requested element.
     */
    std::weak_ptr<TreeNode<T>> at(std::string_view path) const
    {
        const auto separator_size = TreeNode<T>::string_separator.size();
        while (!path.empty() && path.find_last_of(TreeNode<T>::string_separator) == path.size() - 1)
        {
            path.remove_prefix(separator_size);
        }
        if (path.empty())
        {
            return this->weak_from_this();
        }

        const auto loc = path.find_first_of(TreeNode<T>::string_separator);

        const std::string name(path.substr(0, loc));

        auto child = children_.find(name);

        // Create the child if it doesn't exist.
        if (child == children_.end())
        {
            return nullptr;
        }
        if (loc == std::string_view::npos)
        {
            return child->second;
        } else
        {
            return child->second->at(path.substr(loc + separator_size));
        }
    }

    const std::unordered_map<std::string, std::shared_ptr<TreeNode>>& children() const
    {
        return this->children_;
    }

    /**
     * Get the value stored in the node.
     * @return The value stored in the node.
     */
    const T& value() const
    {
        return node_;
    }

    T& value()
    {
        return node_;
    }

    /**
     * Return a standard text representation of the content of the node.
     * @return a string containing the standard text representation of the content of the object.
     */
    [[nodiscard]] std::string
    to_string(const std::string& name = ".", const unsigned int depth = 0) const
    {
        std::ostringstream oss;
        for (unsigned int i = 0; i < depth; i++)
        {
            if (i != depth - 1)
            {
                oss << "    ";
            } else
            {
                oss << "|-- ";
            }
        }
        oss << name << std::endl;
        for (const auto& [key, child] : children_)
        {
            oss << child->to_string(key, depth + 1);
        }

        return oss.str();
    }

    static std::string string_separator; /**< The string separator the default value is / */

private:
    T node_;
    std::unordered_map<std::string, std::shared_ptr<TreeNode>> children_;
};

template <class T> std::string TreeNode<T>::string_separator = "/";

} // namespace MeshcatCpp::details

#endif // MESHCAT_CPP_TREE_NODE_H

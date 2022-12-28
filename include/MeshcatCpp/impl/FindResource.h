/**
 * @file FindResource.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_FIND_ASSET_H
#define MESHCAT_CPP_FIND_ASSET_H

#include <filesystem>
#include <string>

namespace MeshcatCpp::details
{

std::filesystem::path get_resource_path(const std::string& resource);

} // namespace MeshcatCpp::details

#endif // MESHCAT_CPP_FIND_ASSET_H

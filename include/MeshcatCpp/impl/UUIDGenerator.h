/**
 * @file UUIDGenerator.h
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#ifndef MESHCAT_CPP_UUID_GENERATOR_H
#define MESHCAT_CPP_UUID_GENERATOR_H

#include <memory>
#include <random>

namespace MeshcatCpp::details
{

class UUIDGenerator
{
public:
    std::string operator()();
    static UUIDGenerator& generator();

private:
    UUIDGenerator() = default;
    ~UUIDGenerator() = default;
    UUIDGenerator(const UUIDGenerator&) = delete;
    UUIDGenerator& operator=(const UUIDGenerator&) = delete;


    inline static std::mt19937 generator_{};
};

} // namespace MeshcatCpp

#endif // MESHCAT_CPP_UUID_GENERATOR_H

/**
 * @file UUIDGenerator.cpp
 * @authors Giulio Romualdi
 * @copyright This software may be modified and distributed under the terms of the BSD-3-Clause
 * license.
 */

#include <MeshcatCpp/impl/UUIDGenerator.h>

#include <uuid.h>
#include <string>

using namespace MeshcatCpp::details;

UUIDGenerator& UUIDGenerator::generator()
{
    static UUIDGenerator instance; // Guaranteed to be destroyed.
                                   // Instantiated on first use.
    return instance;
}

std::string UUIDGenerator::operator()()
{
    uuids::uuid_random_generator uuid_generator{this->generator_};
    return uuids::to_string(uuid_generator());
}

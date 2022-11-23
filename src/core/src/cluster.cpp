/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2022                                                             *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 *    of conditions and the following disclaimer.                                        *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 *    list of conditions and the following disclaimer in the documentation and/or other  *
 *    materials provided with the distribution.                                          *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 *    used to endorse or promote products derived from this software without specific    *
 *    prior written permission.                                                          *
 *                                                                                       *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY   *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT   *
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,        *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  *
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR    *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      *
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    *
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                                               *
 *                                                                                       *
 ****************************************************************************************/

#include "cluster.h"

#include "jsonload.h"
#include "logging.h"
#include <fmt/format.h>
#include <assert.h>
#include <filesystem>
#include <string_view>

namespace {
    constexpr std::string_view KeyName = "name";
    constexpr std::string_view KeyEnabled = "enabled";
    constexpr std::string_view KeyNodes = "nodes";
    constexpr std::string_view KeyDescription = "description";
} // namespace

void from_json(const nlohmann::json& j, Cluster& c) {
    j.at(KeyName).get_to(c.name);
    if (j.find(KeyEnabled) != j.end()) {
        j[KeyEnabled].get_to(c.isEnabled);
    }
    if (j.find(KeyDescription) != j.end()) {
        j[KeyDescription].get_to(c.description);
    }

    j.at(KeyNodes).get_to(c.nodes);
}

void to_json(nlohmann::json& j, const Cluster& c) {
    j[KeyName] = c.name;
    j[KeyEnabled] = c.isEnabled;
    j[KeyDescription] = c.description;
    j[KeyNodes] = c.nodes;
}

std::vector<Cluster> loadClustersFromDirectory(std::string_view directory) {
    std::vector<Cluster> clusters = common::loadJsonFromDirectory<Cluster>(directory);

    // Inject the unique identifiers into the nodes
    int id = 0;
    for (Cluster& cluster : clusters) {
        cluster.id = id;
        id++;
    }

    return clusters;
}

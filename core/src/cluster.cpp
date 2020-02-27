/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
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

#include "database.h"
#include "jsonload.h"
#include "logging.h"
#include "node.h"
#include <fmt/format.h>
#include <assert.h>
#include <filesystem>

namespace {
    constexpr const char* KeyName = "name";
    constexpr const char* KeyEnabled = "enabled";
    constexpr const char* KeyNodes = "nodes";
} // namespace

void from_json(const nlohmann::json& j, Cluster& p) {
    j.at(KeyName).get_to(p.name);
    p.isEnabled = true;
    if (j.find(KeyEnabled) != j.end()) {
        j.at(KeyEnabled).get_to(p.isEnabled);
    }
    
    std::vector<std::string> nodes = j.at(KeyNodes).get<std::vector<std::string>>();
    for (const std::string& node : nodes) {
        Node* n = data::findNode(node);
        if (!n) {
            std::string message = fmt::format("Could not find node with name {}", node);
            Log(message);
            throw std::runtime_error(message);
        }

        p.nodes.push_back(n->id);
    }
}

std::vector<Cluster> loadClustersFromDirectory(const std::string& directory) {
    std::vector<Cluster> clusters = common::loadJsonFromDirectory<Cluster>(directory);

    for (const Cluster& cluster : clusters) {
        if (cluster.name.empty()) {
            throw std::runtime_error("Missing name for cluster");
        }

        if (cluster.nodes.empty()) {
            throw std::runtime_error(fmt::format(
                "No clusters specified for cluster {}", cluster.name
            ));
        }
    }

    // @TODO (abock, 2020-02-27) Check whether there are any duplicate clusters specified

    // Inject the unique identifiers into the nodes
    int id = 0;
    for (Cluster& cluster : clusters) {
        cluster.id = id;
        id++;
    }

    return clusters;
}

/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2025                                                               *
 * Alexander Bock                                                                        *
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

#include "node.h"

#include "jsonload.h"
#include "logging.h"
#include <assert.h>
#include <filesystem>
#include <set>
#include <string_view>

namespace {
    constexpr std::string_view KeyName = "name";
    constexpr std::string_view KeyIpAddress = "ip";
    constexpr std::string_view KeyPort = "port";
    constexpr std::string_view KeySecret = "secret";
    constexpr std::string_view KeyDescription = "description";
} // namespace

void from_json(const nlohmann::json& j, Node& n) {
    j.at(KeyName).get_to(n.name);
    j.at(KeyIpAddress).get_to(n.ipAddress);
    j.at(KeyPort).get_to(n.port);
    if (auto it = j.find(KeySecret);  it != j.end()) {
        it->get_to(n.secret);
    }
    if (auto it = j.find(KeyDescription);  it != j.end()) {
        it->get_to(n.description);
    }
}

void to_json(nlohmann::json& j, const Node& n) {
    j[KeyName] = n.name;
    j[KeyIpAddress] = n.ipAddress;
    j[KeyPort] = n.port;
    if (n.secret != Node().secret) {
        j[KeySecret] = n.secret;
    }
    if (n.description != Node().description) {
        j[KeyDescription] = n.description;
    }
}

std::pair<std::vector<Node>, bool> loadNodesFromDirectory(std::string_view directory) {
    std::pair<std::vector<Node>, bool> res = common::loadJsonFromDirectory<Node>(
        directory,
        validation::loadValidator(":/schema/config/node.schema.json")
    );

    std::vector<Node> nodes = res.first;

    // Check for duplicates
    std::sort(
        nodes.begin(), nodes.end(),
        [](const Node& lhs, const Node& rhs) { return lhs.name < rhs.name; }
    );
    const auto it = std::adjacent_find(
        nodes.begin(), nodes.end(),
        [](const Node& lhs, const Node& rhs) { return lhs.name == rhs.name; }
    );
    if (it != nodes.end()) {
        throw std::runtime_error(std::format("Duplicate node name '{}' found", it->name));
    }

    for (const Node& node : nodes) {
        std::string text = std::format(
            "(id: {}, name : \"{}\", ipAddress: \"{}\", port: {}, description: {}, "
            "isConnected: {} )",
            node.id.v, node.name, node.ipAddress, node.port, node.description,
            node.isConnected
        );
    }

    // Inject the unique identifiers into the nodes
    int id = 0;
    for (Node& node : nodes) {
        node.id = id;
        id++;
    }
    return { nodes, res.second };
}

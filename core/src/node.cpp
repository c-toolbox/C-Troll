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

#include "node.h"

#include "jsonload.h"
#include "logging.h"
#include <fmt/format.h>
#include <assert.h>
#include <filesystem>
#include <set>

namespace {
    constexpr const char* KeyName = "name";
    constexpr const char* KeyIpAddress = "ip";
    constexpr const char* KeyPort = "port";
} // namespace

template <>
struct fmt::formatter<Node> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const Node& n, FormatContext& ctx) {
        return format_to(
            ctx.out(),
            "( id: {}, name: \"{}\", ipAddress: \"{}\", port: {}, isConnected: {} )",
            n.id.v, n.name, n.ipAddress, n.port, n.isConnected
        );
    }
};

void from_json(const nlohmann::json& j, Node& p) {
    j.at(KeyName).get_to(p.name);
    j.at(KeyIpAddress).get_to(p.ipAddress);
    j.at(KeyPort).get_to(p.port);
}

std::vector<Node> loadNodesFromDirectory(const std::string& directory) {
    std::vector<Node> nodes = common::loadJsonFromDirectory<Node>(directory);

    // First check that no names for nodes are duplicated
    std::set<std::string> nodeNames;
    for (const Node& n : nodes) {
        if (nodeNames.find(n.name) != nodeNames.end()) {
            throw std::runtime_error(fmt::format("Found duplicate node name: {}", n));
        }
        nodeNames.insert(n.name);
    }

    for (const Node& node : nodes) {
        if (node.name.empty()) {
            throw std::runtime_error(fmt::format("Found node without a name: {}", node));
        }
        if (node.ipAddress.empty()) {
            throw std::runtime_error(
                fmt::format("Found node without an IP address: {}", node)
            );
        }
        // @TODO (abock, 2020-02-26) Add a clever check that makes sure the passed value
        // is a valid DNS name or IP address

        if (node.port <= 0 || node.port >= 65536) {
            throw std::runtime_error(fmt::format(
                "Found node with invalid port: {}", node
            ));
        }
    }

    // Inject the unique identifiers into the nodes
    int id = 0;
    for (Node& node : nodes) {
        node.id = id;
        id++;
    }
    return nodes;
}

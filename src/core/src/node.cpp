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

#include "node.h"

#include "jsonload.h"
#include "logging.h"
#include <fmt/format.h>
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
            "( id: {}, name: \"{}\", ipAddress: \"{}\", port: {}, description: {}, "
            "isConnected: {} )",
            n.id.v, n.name, n.ipAddress, n.port, n.description, n.isConnected
        );
    }
};

void from_json(const nlohmann::json& j, Node& n) {
    j.at(KeyName).get_to(n.name);
    j.at(KeyIpAddress).get_to(n.ipAddress);
    j.at(KeyPort).get_to(n.port);
    if (j.find(KeySecret) != j.end()) {
        j[KeySecret].get_to(n.secret);
    }
    if (j.find(KeyDescription) != j.end()) {
        j[KeyDescription].get_to(n.description);
    }
}

void to_json(nlohmann::json& j, const Node& n) {
    j[KeyName] = n.name;
    j[KeyIpAddress] = n.ipAddress;
    j[KeyPort] = n.port;
    j[KeySecret] = n.secret;
    j[KeyDescription] = n.description;
}

std::vector<Node> loadNodesFromDirectory(std::string_view directory) {
    std::vector<Node> nodes = common::loadJsonFromDirectory<Node>(directory);

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
        throw std::runtime_error(fmt::format("Duplicate node name '{}' found", it->name));
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

        if (node.port <= 0 || node.port >= 65536) {
            throw std::runtime_error(
                fmt::format("Found node with invalid port: {}", node)
            );
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

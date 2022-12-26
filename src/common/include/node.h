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

#ifndef __COMMON__NODE_H__
#define __COMMON__NODE_H__

#include "typedid.h"
#include <nlohmann/json.hpp>

/**
 * This struct contains information about individual computer nodes of the cluster.
 * Each node has a human-readable \m name, an \m ipAddress, and a \m port on which the
 * Tray application is listening.
 */
struct Node {
    using ID = TypedId<int, struct NodeTag>;

    /// Unique identifier for the cluster node
    ID id{ -1 };

    /// The human readable name of the computer node
    std::string name;
    /// The IP address at which the computer is reachable; this can also be a hostname
    std::string ipAddress;
    /// The port on which the Tray application on that computer is listening
    int port = -1;
    /// The secret that is sent to the tray application for authentication
    std::string secret;
    /// A user-friendly description that potentially better identifies the node
    std::string description;


    /// A flag representing whether the node is being connected to, but hasn't received
    /// the ultimate connected message yet
    bool isConnecting = false;
    /// A flag representing whether the node is connected or not
    bool isConnected = false;
};

void from_json(const nlohmann::json& j, Node& n);
void to_json(nlohmann::json& j, const Node& n);

/**
 * This method walks the passed \p directory and looks for all <code>*.json</code>
 * files in it. Any \c JSON file in it will be interpreted as a node configuration and
 * returned.
 *
 * \param directory The directory that is walked in search for <code>*.json</code> files
 * \return A list of all Nodes%s that were found by walking the \p directory, the second
 *         parameter is true if all files loaded successfully
 */
std::pair<std::vector<Node>, bool> loadNodesFromDirectory(std::string_view directory);

#endif // __COMMON__NODE_H__

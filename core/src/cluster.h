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

#ifndef __CORE__CLUSTER_H__
#define __CORE__CLUSTER_H__

#include "guiinitialization.h"
#include <QByteArray>
#include <memory>
#include <json/json.hpp>

class JsonObject;
class Process;

/**
 * This structure represents a cluster setup, that is, a collection of computers that are
 * addressed as a unit. Each cluster has a human readable \m name, a unique
 * \m id, a setting whether or not it is \m enabled and a list of computer \m nodes.
 */
struct Cluster {
    /**
     * This struct contains information about individual computer nodes of the cluster.
     * Each node has a human-readable \m name, an \m ipAddress, and a \m port on which the
     * Tray application is listening.
     */
    struct Node {
        /// Unique identifier for the cluster node
        std::string id;
        /// The human readable name of the computer node
        std::string name;
        /// The IP address at which the computer is reachable; this can also be a
        /// hostname
        std::string ipAddress;
        /// The port on which the Tray application on that computer is listening
        int port;
        /// A flag representing whether the node is connected or not
        bool isConnected;
    };

    /// The human readable name of this Cluster
    std::string name;
    /// The unique identifier of this Cluster
    std::string id;
    /// A flag whether this Cluster is enabled or disabled
    bool isEnabled;
    /// A list of all nodes belonging to this cluster
    std::vector<Node> nodes;
    /// A vector of processes that are active on this cluster
    std::vector<Process*> processes;
};

void to_json(nlohmann::json& j, const Cluster& p);
void from_json(const nlohmann::json& j, Cluster& p);

/**
 * This method walks the passed \p directory and looks for all <code>*.json</code>
 * files in it. Any \c JSON file in it will be interpreted as a cluster configuration
 * and returned.
 * \param directory The directory that is walked in search for <code>*.json</code>
 * files
 * \return A list of all Cluster%s that were found by walking the \p directory
 */
std::vector<Cluster> loadClustersFromDirectory(const std::string& directory);

/**
 * This method converts a Cluster information into the
 * common::GuiInitialization::Cluster format so that it can be send to connected GUIs.
 * \param cluster The Cluster information that is to be converte
 * \return A common::GuiInitialization::Cluster structure that contains all relevant
 * information for the GUI initialization step
 */
common::GuiInitialization::Cluster clusterToGuiCluster(const Cluster& cluster);


#endif // __CORE__CLUSTER_H__

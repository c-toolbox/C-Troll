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

#ifndef __SHARED__CLUSTER_H__
#define __SHARED__CLUSTER_H__

#include "typedid.h"
#include <json/json.hpp>

/**
 * This structure represents a cluster setup, that is, a collection of computers that are
 * addressed as a unit. Each cluster has a human readable \m name, a unique \m id, a
 * setting whether or not it is \m enabled and a list of computer \m nodes.
 */
struct Cluster {
    using ID = TypedId<int, struct ClusterTag>;

    ID id{ -1 };

    /// The human readable name of this Cluster
    std::string name;

    /// A flag whether this Cluster is enabled or disabled
    bool isEnabled = true;

    /// A user-friendly description that potentially better identifies the cluster
    std::string description;

    /// A list of all nodes belonging to this cluster
    std::vector<std::string> nodes;
};

void from_json(const nlohmann::json& j, Cluster& c);
void to_json(nlohmann::json& j, const Cluster& c);

/**
 * This method walks the passed \p directory and looks for all <code>*.json</code>
 * files in it. Any \c JSON file in it will be interpreted as a cluster configuration
 * and returned.
 * \param directory The directory that is walked in search for <code>*.json</code>
 * files
 * \return A list of all Cluster%s that were found by walking the \p directory
 */
std::vector<Cluster> loadClustersFromDirectory(std::string_view directory);

#endif // __SHARED__CLUSTER_H__

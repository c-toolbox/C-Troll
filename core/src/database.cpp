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

#include "database.h"

#include "cluster.h"
#include "node.h"
#include "program.h"
#include "process.h"

namespace {

// @TODO (abock, 2020-02-26) Replace these with in-place storage
std::vector<std::unique_ptr<Cluster>> gClusters;
std::vector<std::unique_ptr<Node>> gNodes;
std::vector<std::unique_ptr<Program>> gPrograms;
std::vector<std::unique_ptr<Process>> gProcesses;

} // namespace

namespace data {

std::vector<Cluster*> clusters() {
    std::vector<Cluster*> clusters;
    for (const std::unique_ptr<Cluster>& c : gClusters) {
        clusters.push_back(c.get());
    }
    return clusters;
}

std::vector<Node*> nodes() {
    std::vector<Node*> nodes;
    for (const std::unique_ptr<Node>& n : gNodes) {
        nodes.push_back(n.get());
    }
    return nodes;
}

std::vector<Program*> programs() {
    std::vector<Program*> programs;
    for (const std::unique_ptr<Program>& c : gPrograms) {
        programs.push_back(c.get());
    }
    return programs;
}

std::vector<Process*> processes() {
    std::vector<Process*> processes;
    for (const std::unique_ptr<Process>& c : gProcesses) {
        processes.push_back(c.get());
    }
    return processes;
}


//////////////////////////////////////////////////////////////////////////////////////////


Cluster* findCluster(int id) {
    const auto it = std::find_if(
        gClusters.begin(), gClusters.end(),
        [id](const std::unique_ptr<Cluster>& c) {
            return c->id == id;
        }
    );
    return it != gClusters.end() ? it->get() : nullptr;
}

Cluster* findCluster(const std::string& name) {
    const auto it = std::find_if(
        gClusters.begin(), gClusters.end(),
        [name](const std::unique_ptr<Cluster>& c) {
            return c->name == name;
        }
    );
    return it != gClusters.end() ? it->get() : nullptr;
}

std::vector<Cluster*> findClustersForProgram(const Program& program) {
    std::vector<Cluster*> clusters;
    for (int clusterId : program.clusters) {
        Cluster* cluster = findCluster(clusterId);
        assert(cluster);
        clusters.push_back(cluster);
    }
    return clusters;
}

std::vector<Cluster*> findClusterForNode(const Node& node) {
    std::vector<Cluster*> clusters;
    for (const std::unique_ptr<Cluster>& cluster : gClusters) {
        const auto it = std::find(cluster->nodes.begin(), cluster->nodes.end(), node.id);
        if (it != cluster->nodes.end()) {
            clusters.push_back(cluster.get());
        }
    }
    return clusters;
}

Node* findNode(int id) {
    const auto it = std::find_if(
        gNodes.begin(), gNodes.end(),
        [id](const std::unique_ptr<Node>& n) {
            return n->id == id;
        }
    );
    return it != gNodes.end() ? it->get() : nullptr;
}

Node* findNode(const std::string& name) {
    const auto it = std::find_if(
        gNodes.begin(), gNodes.end(),
        [name](const std::unique_ptr<Node>& n) {
            return n->name == name;
        }
    );
    return it != gNodes.end() ? it->get() : nullptr;
}

std::vector<Node*> findNodesForCluster(const Cluster& cluster) {
    std::vector<Node*> nodes;
    for (int nodeId : cluster.nodes) {
        Node* node = findNode(nodeId);
        assert(node);
        nodes.push_back(node);
    }
    return nodes;
}

Program* findProgram(int id) {
    const auto it = std::find_if(
        gPrograms.begin(), gPrograms.end(),
        [id](const std::unique_ptr<Program>& p) {
            return p->id == id;
        }
    );
    return it != gPrograms.end() ? it->get() : nullptr;
}

const Program::Configuration& findConfigurationForProgram(const Program& program, int id)
{
    const auto it = std::find_if(
        program.configurations.begin(), program.configurations.end(),
        [id](const Program::Configuration& c) { return c.id == id; }
    );
    return *it;
}

Process* findProcess(Process::ID id) {
    const auto it = std::find_if(
        gProcesses.begin(), gProcesses.end(),
        [id](const std::unique_ptr<Process>& p) {
            return p->id.v == id.v;
        }
    );
    return it != gProcesses.end() ? it->get() : nullptr;
}

void addProcess(std::unique_ptr<Process> process) {
    gProcesses.push_back(std::move(process));
}


//////////////////////////////////////////////////////////////////////////////////////////


void loadData(const std::string& programPath, const std::string& clusterPath,
              const std::string& nodePath)
{
    gNodes.clear();
    std::vector<Node> nodes = loadNodesFromDirectory(nodePath);
    for (Node& node : nodes) {
        std::unique_ptr<Node> n = std::make_unique<Node>(std::move(node));
        gNodes.push_back(std::move(n));
    }

    gClusters.clear();
    std::vector<Cluster> clusters = loadClustersFromDirectory(clusterPath);
    for (Cluster& cluster : clusters) {
        std::unique_ptr<Cluster> c = std::make_unique<Cluster>(std::move(cluster));
        gClusters.push_back(std::move(c));
    }

    gPrograms.clear();
    std::vector<Program> programs = loadProgramsFromDirectory(programPath);
    for (Program& program : programs) {
        std::unique_ptr<Program> p = std::make_unique<Program>(std::move(program));
        gPrograms.push_back(std::move(p));
    }
}

} // namespace data

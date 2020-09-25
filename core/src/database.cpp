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

namespace {

std::vector<std::unique_ptr<Cluster>> gClusters;
std::vector<std::unique_ptr<Node>> gNodes;
std::vector<std::unique_ptr<Program>> gPrograms;
std::vector<std::unique_ptr<Process>> gProcesses;

std::vector<Configuration::Color> gTagColors;

std::size_t gDataHash = 0;

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


Cluster* findCluster(Cluster::ID id) {
    const auto it = std::find_if(
        gClusters.cbegin(), gClusters.cend(),
        [id](const std::unique_ptr<Cluster>& c) { return c->id == id; }
    );
    return it != gClusters.cend() ? it->get() : nullptr;
}

Cluster* findCluster(const std::string& name) {
    const auto it = std::find_if(
        gClusters.cbegin(), gClusters.cend(),
        [name](const std::unique_ptr<Cluster>& c) { return c->name == name; }
    );
    return it != gClusters.end() ? it->get() : nullptr;
}

std::vector<Cluster*> findClustersForProgram(const Program& program) {
    std::vector<Cluster*> clusters;
    for (Cluster::ID clusterId : program.clusters) {
        Cluster* cluster = findCluster(clusterId);
        assert(cluster);
        clusters.push_back(cluster);
    }
    return clusters;
}

std::vector<Cluster*> findClusterForNode(const Node& node) {
    std::vector<Cluster*> clusters;
    for (const std::unique_ptr<Cluster>& cluster : gClusters) {
        const auto i = std::find(cluster->nodes.cbegin(), cluster->nodes.cend(), node.id);
        if (i != cluster->nodes.cend()) {
            clusters.push_back(cluster.get());
        }
    }
    return clusters;
}

Node* findNode(Node::ID id) {
    const auto it = std::find_if(
        gNodes.cbegin(), gNodes.cend(),
        [id](const std::unique_ptr<Node>& n) { return n->id == id; }
    );
    return it != gNodes.cend() ? it->get() : nullptr;
}

Node* findNode(const std::string& name) {
    const auto it = std::find_if(
        gNodes.cbegin(), gNodes.cend(),
        [name](const std::unique_ptr<Node>& n) { return n->name == name; }
    );
    return it != gNodes.cend() ? it->get() : nullptr;
}

std::vector<Node*> findNodesForCluster(const Cluster& cluster) {
    std::vector<Node*> nodes;
    for (Node::ID nodeId : cluster.nodes) {
        Node* node = findNode(nodeId);
        assert(node);
        nodes.push_back(node);
    }
    return nodes;
}

Program* findProgram(Program::ID id) {
    const auto it = std::find_if(
        gPrograms.cbegin(), gPrograms.cend(),
        [id](const std::unique_ptr<Program>& p) { return p->id == id; }
    );
    return it != gPrograms.cend() ? it->get() : nullptr;
}

const Program::Configuration& findConfigurationForProgram(const Program& program,
                                                          Program::Configuration::ID id)
{
    const auto it = std::find_if(
        program.configurations.cbegin(), program.configurations.cend(),
        [id](const Program::Configuration& c) { return c.id == id; }
    );
    return *it;
}

bool hasTag(Program::ID id, const std::vector<std::string>& tags) {
    assert(!tags.empty());
    Program* program = findProgram(id);

    bool res = true;
    for (const std::string& tag : tags) {
        const auto it = std::find(program->tags.cbegin(), program->tags.cend(), tag);
        bool foundTag = it != program->tags.cend();
        res &= foundTag;
    }
    return res;
}

std::set<std::string> findTags() {
    std::set<std::string> res;
    for (Program* program : programs()) {
        for (const std::string& tag : program->tags) {
            res.insert(tag);
        }
    }
    return res;
}

Process* findProcess(Process::ID id) {
    const auto it = std::find_if(
        gProcesses.cbegin(), gProcesses.cend(),
        [id](const std::unique_ptr<Process>& p) { return p->id.v == id.v; }
    );
    return it != gProcesses.cend() ? it->get() : nullptr;
}

void addProcess(std::unique_ptr<Process> process) {
    gProcesses.push_back(std::move(process));
}

Configuration::Color colorForTag(const std::string& tag) {
    static int LastColor = -1;
    static std::map<std::string, Configuration::Color> Colors;

    if (Colors.find(tag) != Colors.end()) {
        // Someone already requested the color for this tag
        return Colors[tag];
    }
    else {
        // Color not yet requested
        ++LastColor;
        Configuration::Color c = LastColor < gTagColors.size() ?
            gTagColors[LastColor] :
            Configuration::Color{ 255, 255, 255 };

        Colors[tag] = c;
        return c;
    }
}

void setTagColors(std::vector<Configuration::Color> colors) {
    // This function should only be called once, so if the colors are already filled,
    // something went wrong
    assert(gTagColors.empty());
    gTagColors = std::move(colors);
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

    // Calculate the hash of all the data that was just loaded
    std::size_t hash = 0;

    auto addHash = [&hash](std::size_t rhs) {
        // Based on boost:hash_combine (Golden Ratio Hashing)
        // The Art of Computer Programming: Volume 3: Sorting and Searching

        // Unfortunately, sqrt is not constexpr, so:
        // constexpr const double phi = (std::sqrt(5) - 1.0) / 2.0;
        // constexpr const double salt = round(phi * std::numeric_limits<uint_32>::max());
        // constexpr const uint32_t hashConstant = static_cast<uint32_t>(salt);
        constexpr const uint32_t hashConstant = 0x9e3779b9;

        hash ^= rhs + hashConstant + (hash << 6) + (hash >> 2);
    };

    // Clusters
    for (const std::unique_ptr<Cluster>& cluster : gClusters) {
        addHash(std::hash<int>()(cluster->id.v));
        addHash(std::hash<std::string>()(cluster->name));
        addHash(std::hash<bool>()(cluster->isEnabled));
        for (Node::ID node : cluster->nodes) {
            addHash(std::hash<int>()(node.v));
        }
    }

    // Nodes
    for (const std::unique_ptr<Node>& node : gNodes) {
        addHash(std::hash<int>()(node->id.v));
        addHash(std::hash<std::string>()(node->name));
        addHash(std::hash<std::string>()(node->ipAddress));
        addHash(std::hash<int>()(node->port));
        addHash(std::hash<std::string>()(node->secret));
        addHash(std::hash<bool>()(node->isConnected));
    }

    // Programs
    for (const std::unique_ptr<Program>& program : gPrograms) {
        addHash(std::hash<int>()(program->id.v));
        addHash(std::hash<std::string>()(program->name));
        addHash(std::hash<std::string>()(program->executable));
        addHash(std::hash<std::string>()(program->commandlineParameters));
        addHash(std::hash<std::string>()(program->workingDirectory));
        for (const std::string& tag : program->tags) {
            addHash(std::hash<std::string>()(tag));
        }
        for (const Program::Configuration& conf : program->configurations) {
            addHash(std::hash<int>()(conf.id.v));
            addHash(std::hash<std::string>()(conf.name));
            addHash(std::hash<std::string>()(conf.parameters));
        }
        for (Cluster::ID cluster : program->clusters) {
            addHash(std::hash<int>()(cluster.v));
        }
    }

    gDataHash = hash;
}

std::size_t dataHash() {
    return gDataHash;
}

} // namespace data

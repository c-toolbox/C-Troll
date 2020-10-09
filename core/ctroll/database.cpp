/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
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

#include "database.h"

#include <QObject>
#include <fmt/format.h>
#include <random>

namespace {

std::vector<std::unique_ptr<Cluster>> gClusters;
std::vector<std::unique_ptr<Node>> gNodes;
std::vector<std::unique_ptr<Program>> gPrograms;
std::vector<std::unique_ptr<Process>> gProcesses;

std::vector<Color> gTagColors;

std::size_t gDataHash = 0;

} // namespace

namespace data {

std::vector<const Cluster*> clusters() {
    std::vector<const Cluster*> clusters;
    for (const std::unique_ptr<Cluster>& c : gClusters) {
        assert(c.get());
        clusters.push_back(c.get());
    }
    return clusters;
}

std::vector<const Node*> nodes() {
    std::vector<const Node*> nodes;
    for (const std::unique_ptr<Node>& n : gNodes) {
        assert(n.get());
        nodes.push_back(n.get());
    }
    return nodes;
}

std::vector<const Program*> programs() {
    std::vector<const Program*> programs;
    for (const std::unique_ptr<Program>& p : gPrograms) {
        assert(p.get());
        programs.push_back(p.get());
    }
    return programs;
}

std::vector<const Process*> processes() {
    std::vector<const Process*> processes;
    for (const std::unique_ptr<Process>& p : gProcesses) {
        assert(p.get());
        processes.push_back(p.get());
    }
    return processes;
}


//////////////////////////////////////////////////////////////////////////////////////////


const Cluster* findCluster(Cluster::ID id) {
    const auto it = std::find_if(
        gClusters.cbegin(), gClusters.cend(),
        [id](const std::unique_ptr<Cluster>& c) { return c->id == id; }
    );
    return it != gClusters.cend() ? it->get() : nullptr;
}

const Cluster* findCluster(std::string_view name) {
    const auto it = std::find_if(
        gClusters.cbegin(), gClusters.cend(),
        [name](const std::unique_ptr<Cluster>& c) { return c->name == name; }
    );
    return it != gClusters.end() ? it->get() : nullptr;
}

std::vector<const Cluster*> findClustersForProgram(const Program& program) {
    std::vector<const Cluster*> clusters;
    for (const std::string& clusterName : program.clusters) {
        const Cluster* cluster = findCluster(clusterName);
        assert(cluster);
        clusters.push_back(cluster);
    }
    return clusters;
}

std::vector<const Cluster*> findClusterForNode(const Node& node) {
    std::vector<const Cluster*> clusters;
    for (const std::unique_ptr<Cluster>& cluster : gClusters) {
        auto i = std::find(cluster->nodes.cbegin(), cluster->nodes.cend(), node.name);
        if (i != cluster->nodes.cend()) {
            clusters.push_back(cluster.get());
        }
    }
    return clusters;
}

const Node* findNode(Node::ID id) {
    const auto it = std::find_if(
        gNodes.cbegin(), gNodes.cend(),
        [id](const std::unique_ptr<Node>& n) { return n->id == id; }
    );
    return it != gNodes.cend() ? it->get() : nullptr;
}

const Node* findNode(std::string_view name) {
    const auto it = std::find_if(
        gNodes.cbegin(), gNodes.cend(),
        [name](const std::unique_ptr<Node>& n) { return n->name == name; }
    );
    return it != gNodes.cend() ? it->get() : nullptr;
}

std::vector<const Node*> findNodesForCluster(const Cluster& cluster) {
    std::vector<const Node*> nodes;
    for (const std::string& nodeName : cluster.nodes) {
        const Node* node = findNode(nodeName);
        assert(node);
        nodes.push_back(node);
    }
    return nodes;
}

void setNodeConnected(Node::ID id, bool connected) {
    const auto it = std::find_if(
        gNodes.cbegin(), gNodes.cend(),
        [id](const std::unique_ptr<Node>& n) { return n->id == id; }
    );
    if (it != gNodes.end()) {
        (*it)->isConnected = connected;
    }
}

const Program* findProgram(Program::ID id) {
    const auto it = std::find_if(
        gPrograms.cbegin(), gPrograms.cend(),
        [id](const std::unique_ptr<Program>& p) { return p->id == id; }
    );
    return it != gPrograms.cend() ? it->get() : nullptr;
}

const Program* findProgram(std::string_view name) {
    const auto it = std::find_if(
        gPrograms.cbegin(), gPrograms.cend(),
        [name](const std::unique_ptr<Program>& p) { return p->name == name; }
    );
    return it != gPrograms.cend() ? it->get() : nullptr;
}

const Program::Configuration* findConfigurationForProgram(const Program& program,
                                                          Program::Configuration::ID id)
{
    const auto it = std::find_if(
        program.configurations.cbegin(), program.configurations.cend(),
        [id](const Program::Configuration& c) { return c.id == id; }
    );
    return it != program.configurations.cend() ? &(*it) : nullptr;
}

const Program::Configuration* findConfigurationForProgram(const Program& program,
                                                          std::string_view name)
{
    const auto it = std::find_if(
        program.configurations.cbegin(), program.configurations.cend(),
        [name](const Program::Configuration& c) { return c.name == name; }
    );
    return it != program.configurations.cend() ? &(*it) : nullptr;
}

bool hasTag(Program::ID id, const std::vector<std::string>& tags) {
    assert(!tags.empty());
    const Program* program = findProgram(id);

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
    for (const Program* program : programs()) {
        for (const std::string& tag : program->tags) {
            res.insert(tag);
        }
    }
    return res;
}

const Process* findProcess(Process::ID id) {
    const auto it = std::find_if(
        gProcesses.cbegin(), gProcesses.cend(),
        [id](const std::unique_ptr<Process>& p) { return p->id.v == id.v; }
    );
    return it != gProcesses.cend() ? it->get() : nullptr;
}

void addProcess(std::unique_ptr<Process> process) {
    gProcesses.push_back(std::move(process));
}

void setProcessStatus(Process::ID id, common::ProcessStatusMessage::Status status) {
    const auto it = std::find_if(
        gProcesses.cbegin(), gProcesses.cend(),
        [id](const std::unique_ptr<Process>& p) { return p->id.v == id.v; }
    );
    if (it != gProcesses.end()) {
        (*it)->status = status;
    }
}

Color colorForTag(std::string_view tag) {
    // It doesn't make sense if someone requests the color for an empty tag
    assert(!tag.empty());

    // First check if we have a dedicated color for this tag
    auto it = std::find_if(
        gTagColors.cbegin(), gTagColors.cend(),
        [tag](const Color& c) { return c.tag == tag; }
    );

    if (it != gTagColors.end()) {
        // If we do, great, and we return it
        return *it;
    }
    else {
        // Otherwise we pick the first without a tag
        for (Color& c : gTagColors) {
            if (c.tag.empty()) {
                c.tag = tag;
                return c;
            }
        }

        // If we got this far, there was no tag specified;  if we are debugging, we make
        // the colors white, otherwise we pick a random color
#ifdef QT_DEBUG
        Color res;
        res.r = 255;
        res.g = 255;
        res.b = 255;
        return res;
#else // QT_DEBUG
        std::random_device rd;
        std::uniform_int_distribution<int> dist(0, 255);
        Color res;
        res.r = dist(rd);
        res.g = dist(rd);
        res.b = dist(rd);
        return res;
#endif // QT_DEBUG
    }
}

void setTagColors(std::vector<Color> colors) {
    // This function should only be called once, so if the colors are already filled,
    // something went wrong
    assert(gTagColors.empty());
    gTagColors = std::move(colors);
}


//////////////////////////////////////////////////////////////////////////////////////////


void loadData(std::string_view programPath, std::string_view clusterPath,
              std::string_view nodePath)
{
    //
    //  Nodes
    //
    gNodes.clear();
    std::vector<Node> nodes = loadNodesFromDirectory(nodePath);
    for (Node& node : nodes) {
        std::unique_ptr<Node> n = std::make_unique<Node>(std::move(node));
        gNodes.push_back(std::move(n));
    }

    gClusters.clear();
    std::vector<Cluster> clusters = loadClustersFromDirectory(clusterPath);

    //
    //  Clusters
    //

    // Check for duplicates
    std::sort(
        clusters.begin(), clusters.end(),
        [](const Cluster& lhs, const Cluster& rhs) { return lhs.name < rhs.name; }
    );
    const auto it = std::adjacent_find(
        clusters.begin(), clusters.end(),
        [](const Cluster& lhs, const Cluster& rhs) { return lhs.name == rhs.name; }
    );
    if (it != clusters.end()) {
        throw std::runtime_error(fmt::format(
            "Duplicate cluster name '{}' found", it->name
        ));
    }

    for (Cluster& cluster : clusters) {
        if (cluster.name.empty()) {
            throw std::runtime_error("Missing name for cluster");
        }

        if (cluster.nodes.empty()) {
            throw std::runtime_error(fmt::format(
                "No nodes specified for cluster {}", cluster.name
            ));
        }

        for (const std::string& node : cluster.nodes) {
            const Node* n = findNode(node);
            if (!n) {
                throw std::runtime_error(
                    fmt::format("Could not find node with name {}", node)
                );
            }
        }

        std::unique_ptr<Cluster> c = std::make_unique<Cluster>(std::move(cluster));
        gClusters.push_back(std::move(c));
    }


    //
    //  Programs
    //
    gPrograms.clear();
    std::vector<Program> programs = loadProgramsFromDirectory(programPath);


    for (Program& program : programs) {
        if (program.name.empty()) {
            throw std::runtime_error("No name specified for program");
        }
        if (program.executable.empty()) {
            throw std::runtime_error(fmt::format(
                "No executable specified for program {}", program.name
            ));
        }
        if (program.clusters.empty()) {
            throw std::runtime_error(fmt::format(
                "No clusters specified for program {}", program.name
            ));
        }

        const bool hasEmptyTag = std::any_of(
            program.tags.cbegin(), program.tags.cend(),
            std::mem_fn(&std::string::empty)
        );
        if (hasEmptyTag) {
            throw std::runtime_error(fmt::format(
                "At least one tag of the program {} has an empty tag", program.name
            ));
        }

        for (const std::string& cluster : program.clusters) {
            const Cluster* c = data::findCluster(cluster);
            if (!c) {
                std::string message = fmt::format("Could not find cluster '{}'", cluster);
                throw std::runtime_error(message);
            }
        }

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
        for (const std::string& nodeName : cluster->nodes) {
            const Node* node = findNode(nodeName);
            addHash(std::hash<int>()(node->id.v));
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
        for (const std::string& clusterName : program->clusters) {
            const Cluster* cluster = findCluster(clusterName);
            addHash(std::hash<int>()(cluster->id.v));
        }
    }

    gDataHash = hash;
}

std::size_t dataHash() {
    return gDataHash;
}

} // namespace data

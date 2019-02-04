/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 * of conditions and the following disclaimer.                                           *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 * list of conditions and the following disclaimer in the documentation and/or other     *
 * materials provided with the distribution.                                             *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 * used to endorse or promote products derived from this software without specific prior *
 * written permission.                                                                   *
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

#include <logging.h>
#include <QDirIterator>
#include <QVector>
#include <QCryptographicHash>
#include <assert.h>

namespace {
    constexpr const char* KeyName = "name";
    constexpr const char* KeyId = "id";
    constexpr const char* KeyEnabled = "enabled";
    constexpr const char* KeyNodes = "nodes";

    constexpr const char* KeyNodeName = "name";
    constexpr const char* KeyNodeIpAddress = "ip";
    constexpr const char* KeyNodePort = "port";
} // namespace

void to_json(nlohmann::json& j, const Cluster& p) {
    j = {
        { KeyName, p.name },
        { KeyId, p.id },
        { KeyEnabled, p.enabled }
    };

    std::map<std::string, Cluster::Node> nodes;
    for (const Cluster::Node& n : p.nodes) {
        nodes[n.id] = n;
    }
    j[KeyNodes] = nodes;
}

void to_json(nlohmann::json& j, const Cluster::Node& p) {
    j = {
        { KeyNodeName, p.name },
        { KeyNodeIpAddress, p.ipAddress },
        { KeyNodePort, p.port }
    };
}

void from_json(const nlohmann::json& j, Cluster& p) {
    j.at(KeyName).get_to(p.name);
    j.at(KeyId).get_to(p.id);
    p.enabled = true;
    if (j.find(KeyEnabled) != j.end()) {
        j.at(KeyEnabled).get_to(p.enabled);
    }

    std::map<std::string, Cluster::Node> nodes;
    j.at(KeyNodes).get_to(nodes);
    for (const std::pair<std::string, Cluster::Node>& node : nodes) {
        Cluster::Node n = node.second;
        n.id = node.first;
        p.nodes.push_back(std::move(n));
    }
}

void from_json(const nlohmann::json& j, Cluster::Node& p) {
    j.at(KeyNodeName).get_to(p.name);
    j.at(KeyNodeIpAddress).get_to(p.ipAddress);
    j.at(KeyNodePort).get_to(p.port);
}

std::vector<Cluster> loadClustersFromDirectory(const std::string& directory) {
    std::vector<Cluster> res;

    // First, get all the *.json files from the directory and subdirectories
    QDirIterator it(
        QString::fromStdString(directory),
        QStringList() << "*.json",
        QDir::Files,
        QDirIterator::Subdirectories
    );
    while (it.hasNext()) {
        QString file = it.next();

        Log("Loading cluster file " + file.toStdString());
        try {
            Cluster c = loadCluster(file.toStdString(), directory);
            res.push_back(std::move(c));
        }
        catch (const std::runtime_error& e) {
            Log(std::string("Error loading cluster: ") + e.what());
        }
    }

    return res;
}

Cluster loadCluster(const std::string& jsonFile, const std::string& baseDirectory) {
    QString id = QDir(QString::fromStdString(baseDirectory)).relativeFilePath(QString::fromStdString(jsonFile));

    // relativeFilePath will have the baseDirectory in the beginning of the relative path
    // and we want to remove it:  baseDirectory.length() + 1
    // then, we want to remove the extension of 5 characters (.json)
    // So we take the middle part of the string:
    id = id.mid(
        // length of the base directory + '/'
        baseDirectory.length() + 1,
        // total length - (stuff we removed in the beginning) - length('.json')
        id.size() - (baseDirectory.length() + 1) - 5
    );

    std::ifstream f(jsonFile);
    nlohmann::json obj;
    f >> obj;
    obj[KeyId] = id.toStdString();

    return Cluster(obj);
}

bool Cluster::connected() const {
    return std::accumulate(
        nodes.begin(),
        nodes.end(),
        true,
        [](bool othersConnected, const Node& node) {
            return othersConnected && node.connected;
        }
    );
}

QByteArray Cluster::hash() const {
    nlohmann::json doc = *this;
    std::string input = doc.dump();
    return QCryptographicHash::hash(input.c_str(), QCryptographicHash::Sha1);
}

common::GuiInitialization::Cluster Cluster::toGuiInitializationCluster() const {
    common::GuiInitialization::Cluster cluster;
    cluster.name = name;
    cluster.id = id;
    cluster.enabled = enabled;
    cluster.connected = connected();
    return cluster;
}

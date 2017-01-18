/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
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

#include "jsonsupport.h"
#include <logging.h>

#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>

#include <assert.h>

namespace {
    const QString KeyName = "name";
    const QString KeyId = "id";
    const QString KeyEnabled = "enabled";
    const QString KeyNodes = "nodes";

    const QString KeyNodeName = "name";
    const QString KeyNodeIpAddress = "ip";
    const QString KeyNodePort = "port";
}

Cluster::Cluster(const QJsonObject& jsonObject) {
    _name = common::testAndReturnString(jsonObject, KeyName);
    _id = common::testAndReturnString(jsonObject, KeyId);
    _enabled = common::testAndReturnBool(jsonObject, KeyEnabled, Optional::Yes, true);
    
    QJsonObject nodes = common::testAndReturnObject(jsonObject, KeyNodes);
    _nodes.clear();

    for (auto iter = nodes.begin(); iter != nodes.end(); iter++) {
        QString id = iter.key();
        QJsonObject obj = iter.value().toObject();
        assert(obj.size() == 3);

        QString name = common::testAndReturnString(obj, KeyNodeName);
        QString ipAddress = common::testAndReturnString(obj, KeyNodeIpAddress);
        int port = common::testAndReturnInt(obj, KeyNodePort);

        _nodes.push_back({ id, name, ipAddress, port });
    }

}

QString Cluster::name() const {
    return _name;
}

QString Cluster::id() const {
    return _id;
}

bool Cluster::enabled() const {
    return _enabled;
}

QList<Cluster::Node> Cluster::nodes() const {
    return _nodes;
}

Cluster loadCluster(QString jsonFile, QString baseDirectory) {
    QString id = QDir(baseDirectory).relativeFilePath(jsonFile);
    
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
    
    QFile f(jsonFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject obj = d.object();
    obj[KeyId] = id;
    return Cluster(obj);
}

QList<Cluster> loadClustersFromDirectory(QString directory) {
    QList<Cluster> result;
    // First, get all the *.json files from the directory and subdirectories
    QDirIterator it(
        directory,
        QStringList() << "*.json",
        QDir::Files,
        QDirIterator::Subdirectories
    );
    while (it.hasNext()) {
        QString file = it.next();
        
        Log("Loading cluster file " + file);
        Cluster c = loadCluster(file, directory);
        result.push_back(c);
    }

    return result;
}

common::GuiInitialization::Cluster clusterToGuiInitializationCluster(Cluster c) {
    common::GuiInitialization::Cluster cluster;
    cluster.name = c.name();
    cluster.id = c.id();
    cluster.enabled = c.enabled();
    return cluster;
}

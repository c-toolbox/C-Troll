/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
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

#include "guiinitialization.h"

#include "jsonsupport.h"
#include <QJsonArray>
#include <QJsonObject>

namespace {
    const QString KeyApplications = "applications";
    const QString KeyClusters = "clusters";
    const QString KeyProcesses = "processes";
    
    const QString KeyApplicationName = "name";
    const QString KeyApplicationId = "id";
    const QString KeyApplicationTags = "tags";
    const QString KeyApplicationConfigurations = "configurations";
    const QString KeyApplicationConfigurationClusters = "clusters";
    const QString KeyApplicationConfigurationName = "name";
    const QString KeyApplicationConfigurationId = "id";

    const QString KeyApplicationDefaults = "defaults";
    const QString KeyApplicationDefaultConfiguration = "configuration";
    const QString KeyApplicationDefaultCluster = "cluster";

    const QString KeyClusterName = "name";
    const QString KeyClusterId = "id";
    const QString KeyClusterEnabled = "enabled";
    const QString KeyClusterConnected = "connected";

    const QString KeyProcessId = "processId";
    const QString KeyProcessApplicationId = "applicationId";
    const QString KeyProcessConfigurationId = "configurationId";
    const QString KeyProcessClusterId = "clusterId";
    const QString KeyProcessNodeStatusHistory = "nodeStatusHistory";
    const QString KeyProcessClusterStatus = "clusterStatus";
    const QString KeyProcessClusterStatusTime = "clusterStatusTime";
    
    const QString KeyNodeStatusHistoryNodeId = "nodeId";
    const QString KeyNodeStatusHistoryStatus = "status";
    const QString KeyNodeStatusHistoryTime = "time";
    const QString KeyNodeStatusHistoryId = "id";
} // namespace

namespace common {

const QString GuiInitialization::Type = "GuiInit";
    
GuiInitialization::Application::Application(QJsonObject obj) {
    name = common::testAndReturnString(obj, KeyApplicationName);
    id = common::testAndReturnString(obj, KeyApplicationId);

    tags = common::testAndReturnStringList(obj, KeyApplicationTags);
    //clusters = common::testAndReturnStringList(obj, KeyApplicationClusters);
    QJsonObject configs = common::testAndReturnObject(obj, KeyApplicationConfigurations);
    
    for (const QString& configId : configs.keys()) {
        QJsonObject config = common::testAndReturnObject(obj, configId);
        
        Configuration configuration;
        configuration.name = common::testAndReturnString(
            config,
            KeyApplicationConfigurationName
        );
        configuration.id = common::testAndReturnString(
            config,
            KeyApplicationConfigurationId
        );
        configuration.clusters = common::testAndReturnStringList(
            config,
            KeyApplicationConfigurationClusters
        );
        
        configurations.push_back(configuration);
    }

    QJsonObject defaults = common::testAndReturnObject(obj, KeyApplicationDefaults);
    defaultConfiguration = common::testAndReturnString(
        defaults,
        KeyApplicationDefaultConfiguration
    );
    defaultCluster = common::testAndReturnString(defaults, KeyApplicationDefaultCluster);
}
    
QJsonObject GuiInitialization::Application::toJson() const {
    QJsonObject res;

    res[KeyApplicationName] = name;
    res[KeyApplicationId] = id;
    res[KeyApplicationTags] = QJsonArray::fromStringList(tags);

    QJsonObject confs;
    for (const Configuration& conf : configurations) {
        QJsonObject obj;
        obj[KeyApplicationConfigurationName] = conf.name;
        obj[KeyApplicationConfigurationId] = conf.id;
        QJsonArray cs = QJsonArray::fromStringList(conf.clusters);
        obj[KeyApplicationConfigurationClusters] = cs;
        confs[conf.id] = obj;
    }
    res[KeyApplicationConfigurations] = confs;

    QJsonObject defaults;
    defaults[KeyApplicationDefaultConfiguration] = defaultConfiguration;
    defaults[KeyApplicationDefaultCluster] = defaultCluster;

    res[KeyApplicationDefaults] = defaults;

    return res;
}
    
GuiInitialization::Cluster::Cluster(QJsonObject cluster) {
    name = common::testAndReturnString(cluster, KeyClusterName);
    id = common::testAndReturnString(cluster, KeyClusterId);
    enabled = common::testAndReturnBool(cluster, KeyClusterEnabled);
    connected = false;
}
    
QJsonObject GuiInitialization::Cluster::toJson() const {
    QJsonObject res;
    res[KeyClusterName] = name;
    res[KeyClusterId] = id;
    res[KeyClusterEnabled] = enabled;
    res[KeyClusterConnected] = connected;
    return res;
}

GuiInitialization::Process::Process(QJsonObject process) {
    id = common::testAndReturnInt(process, KeyProcessId);
    applicationId = common::testAndReturnString(process, KeyProcessApplicationId);
    configurationId = common::testAndReturnString(process, KeyProcessConfigurationId);
    clusterId = common::testAndReturnString(process, KeyProcessClusterId);
    QJsonArray statusHistory = common::testAndReturnArray(process, KeyProcessNodeStatusHistory);

    for (int i = 0; i < statusHistory.size(); i++) {
        nodeStatusHistory.push_back(GuiInitialization::Process::NodeStatus(
            common::testAndReturnObject(statusHistory, i))
        );
    }

    clusterStatus = common::testAndReturnString(process, KeyProcessClusterStatus);
    clusterStatusTime = common::testAndReturnDouble(process, KeyProcessClusterStatusTime);
}

QJsonObject GuiInitialization::Process::toJson() const {
    QJsonObject res;
    res[KeyProcessId] = id;
    res[KeyProcessApplicationId] = applicationId;
    res[KeyProcessConfigurationId] = configurationId;
    res[KeyProcessClusterId] = clusterId;

    QJsonArray statusHistory;
    for (const Process::NodeStatus& s : nodeStatusHistory) {
        statusHistory.push_back(s.toJson());
    }

    res[KeyProcessNodeStatusHistory] = statusHistory;
    res[KeyProcessClusterStatus] = clusterStatus;
    res[KeyProcessClusterStatusTime] = clusterStatusTime;

    return res;
}

GuiInitialization::Process::NodeStatus::NodeStatus(QJsonObject nodeStatus) {
    node = common::testAndReturnString(nodeStatus, KeyNodeStatusHistoryNodeId);
    status = common::testAndReturnString(nodeStatus, KeyNodeStatusHistoryStatus);
    time = common::testAndReturnDouble(nodeStatus, KeyNodeStatusHistoryTime);
    id = common::testAndReturnInt(nodeStatus, KeyNodeStatusHistoryId);
}

QJsonObject GuiInitialization::Process::NodeStatus::toJson() const {
    QJsonObject res;
    res[KeyNodeStatusHistoryStatus] = status;
    res[KeyNodeStatusHistoryTime] = time;
    res[KeyNodeStatusHistoryNodeId] = node;
    res[KeyNodeStatusHistoryId] = id;
    return res;
}

GuiInitialization::GuiInitialization(const QJsonDocument& document) {
    QJsonObject obj = document.object();
    
    QJsonArray applicationsJson = common::testAndReturnArray(obj, KeyApplications);
    for (const QJsonValue& val : applicationsJson) {
        if (!val.isObject()) {
            throw std::runtime_error("Value inside an application was not an object");
        }
        applications.push_back(Application(val.toObject()));
    }
    
    QJsonArray clustersJson = common::testAndReturnArray(obj, KeyClusters);
    for (const QJsonValue& val : clustersJson) {
        if (!val.isObject()) {
            throw std::runtime_error("Value inside a cluster was not an object");
        }
        clusters.push_back(Cluster(val.toObject()));
    }

    QJsonArray processesJson = common::testAndReturnArray(obj, KeyProcesses);
    for (const QJsonValue& val : processesJson) {
        if (!val.isObject()) {
            throw std::runtime_error("Value inside a process was not an object");
        }
        processes.push_back(Process(val.toObject()));
    }
}

QJsonDocument GuiInitialization::toJson() const {
    QJsonObject obj;

    QJsonArray apps;
    for (const Application& app : applications) {
        apps.append(app.toJson());
    }
    obj[KeyApplications] = apps;
    
    QJsonArray cls;
    for (const Cluster& c : clusters) {
        cls.append(c.toJson());
    }
    obj[KeyClusters] = cls;
    
    QJsonArray procs;
    for (const Process& p : processes) {
        procs.append(p.toJson());
    }
    obj[KeyProcesses] = procs;

    return QJsonDocument(obj);
}

} // namespace common

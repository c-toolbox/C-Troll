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

namespace {
    constexpr const char* KeyApplications = "applications";
    constexpr const char* KeyClusters = "clusters";
    constexpr const char* KeyProcesses = "processes";
    
    constexpr const char* KeyApplicationName = "name";
    constexpr const char* KeyApplicationId = "id";
    constexpr const char* KeyApplicationTags = "tags";
    constexpr const char* KeyApplicationConfigurations = "configurations";
    constexpr const char* KeyApplicationConfigurationClusters = "clusters";
    constexpr const char* KeyApplicationConfigurationName = "name";
    constexpr const char* KeyApplicationConfigurationId = "id";

    constexpr const char* KeyApplicationDefaults = "defaults";
    constexpr const char* KeyApplicationDefaultConfiguration = "configuration";
    constexpr const char* KeyApplicationDefaultCluster = "cluster";

    constexpr const char* KeyClusterName = "name";
    constexpr const char* KeyClusterId = "id";
    constexpr const char* KeyClusterEnabled = "enabled";
    constexpr const char* KeyClusterConnected = "connected";

    constexpr const char* KeyProcessId = "processId";
    constexpr const char* KeyProcessApplicationId = "applicationId";
    constexpr const char* KeyProcessConfigurationId = "configurationId";
    constexpr const char* KeyProcessClusterId = "clusterId";
    constexpr const char* KeyProcessNodeStatusHistory = "nodeStatusHistory";
    constexpr const char* KeyProcessClusterStatus = "clusterStatus";
    constexpr const char* KeyProcessClusterStatusTime = "clusterStatusTime";
    
    constexpr const char* KeyNodeStatusHistoryNodeId = "nodeId";
    constexpr const char* KeyNodeStatusHistoryStatus = "status";
    constexpr const char* KeyNodeStatusHistoryTime = "time";
    constexpr const char* KeyNodeStatusHistoryId = "id";
} // namespace

namespace common {

void to_json(nlohmann::json& j, const GuiInitialization& p) {
    j = {
        { KeyApplications, p.applications },
        { KeyClusters, p.clusters },
        { KeyProcesses, p.processes }
    };
}

void to_json(nlohmann::json& j, const GuiInitialization::Application& p) {
    nlohmann::json json;
    json[KeyApplicationName] = p.name;
    json[KeyApplicationId] = p.id;
    json[KeyApplicationTags] = p.tags;
    for (const GuiInitialization::Application::Configuration& c : p.configurations) {
        nlohmann::json jc = c;
        json[KeyApplicationConfigurations][c.id] = jc;
    }
    json[KeyApplicationDefaults] = {
        { KeyApplicationDefaultConfiguration, p.defaultConfiguration },
        { KeyApplicationDefaultCluster, p.defaultCluster }
    };
    j = std::move(json);
}

void to_json(nlohmann::json& j, const GuiInitialization::Application::Configuration& p) {
    j = nlohmann::json {
        { KeyApplicationConfigurationName, p.name },
        { KeyApplicationConfigurationId, p.id },
        { KeyApplicationConfigurationClusters, p.clusters }
    };
}

void to_json(nlohmann::json& j, const GuiInitialization::Cluster& p) {
    j = nlohmann::json {
        { KeyClusterName, p.name },
        { KeyClusterId, p.id },
        { KeyClusterEnabled, p.enabled },
        { KeyClusterConnected, p.connected }
    };
}

void to_json(nlohmann::json& j, const GuiInitialization::Process& p) {
    j = nlohmann::json {
        { KeyProcessId, p.id },
        { KeyProcessApplicationId, p.applicationId },
        { KeyProcessConfigurationId, p.configurationId },
        { KeyProcessClusterId, p.clusterId },
        { KeyProcessNodeStatusHistory, p.nodeStatusHistory },
        { KeyProcessClusterStatus, p.clusterStatus },
        { KeyProcessClusterStatusTime, p.clusterStatusTime }
    };
}

void to_json(nlohmann::json& j, const GuiInitialization::Process::NodeStatus& p) {
    j = nlohmann::json {
        { KeyNodeStatusHistoryNodeId, p.node},
        { KeyNodeStatusHistoryStatus, p.status },
        { KeyNodeStatusHistoryTime, p.time },
        { KeyNodeStatusHistoryId, p.id},
    };
}

void from_json(const nlohmann::json& j, GuiInitialization& p) {
    j.at(KeyApplications).get_to(p.applications);
    j.at(KeyClusters).get_to(p.clusters);
    j.at(KeyProcesses).get_to(p.processes);
}

void from_json(const nlohmann::json& j, GuiInitialization::Application& p) {
    j.at(KeyApplicationName).get_to(p.name);
    j.at(KeyApplicationId).get_to(p.id);
    j.at(KeyApplicationTags).get_to(p.tags);
    j.at(KeyApplicationConfigurations).get_to(p.configurations);

    nlohmann::json defaults = j.at(KeyApplicationDefaults);
    defaults.at(KeyApplicationDefaultConfiguration).get_to(p.defaultConfiguration);
    defaults.at(KeyApplicationDefaultCluster).get_to(p.defaultCluster);
}

void from_json(const nlohmann::json& j, GuiInitialization::Application::Configuration& p)
{
    j.at(KeyApplicationConfigurationName).get_to(p.name);
    j.at(KeyApplicationConfigurationId).get_to(p.id);
    j.at(KeyApplicationConfigurationClusters).get_to(p.clusters);
}

void from_json(const nlohmann::json& j, GuiInitialization::Cluster& p) {
    j.at(KeyClusterName).get_to(p.name);
    j.at(KeyClusterId).get_to(p.id);
    j.at(KeyClusterEnabled).get_to(p.enabled);
}

void from_json(const nlohmann::json& j, GuiInitialization::Process& p) {
    j.at(KeyProcessId).get_to(p.id);
    j.at(KeyProcessApplicationId).get_to(p.applicationId);
    j.at(KeyProcessConfigurationId).get_to(p.configurationId);
    j.at(KeyProcessClusterId).get_to(p.clusterId);
    j.at(KeyProcessNodeStatusHistory).get_to(p.nodeStatusHistory);
    j.at(KeyProcessClusterStatus).get_to(p.clusterStatus);
    j.at(KeyProcessClusterStatusTime).get_to(p.clusterStatusTime);
}

void from_json(const nlohmann::json& j, GuiInitialization::Process::NodeStatus& p) {
    j.at(KeyNodeStatusHistoryNodeId).get_to(p.node);
    j.at(KeyNodeStatusHistoryStatus).get_to(p.status);
    j.at(KeyNodeStatusHistoryTime).get_to(p.time);
    j.at(KeyNodeStatusHistoryId).get_to(p.id);
}

} // namespace common

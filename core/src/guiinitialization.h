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

#ifndef __CORE__GUIINITIALIZATION_H__
#define __CORE__GUIINITIALIZATION_H__

#include <vector>
#include <json/json.hpp>

namespace common {
    
/// This struct is the data structure that gets send from the Core to the GUI informing
/// the GUI about the available applications, configurations, and clusters
struct GuiInitialization {
    /// The string representing this command type, for usage in the common::GenericMessage
    static constexpr const char* Type = "GuiInit";
    
    /// This struct represents the information about an application that is registered
    /// with the Core library
    struct Application {
        std::string name;
        std::string id;
        std::vector<std::string> tags;
        
        struct Configuration {
            std::string name;
            std::string id;
            std::vector<std::string> clusters;
        };
        std::vector<Configuration> configurations;
        std::string defaultConfiguration;
        std::string defaultCluster;
    };
    
    /// This struct represents the information about the clusters that are registered with
    /// the Core library
    struct Cluster {
        /// The human-readable name of the cluster
        std::string name;
        /// The unique identifier for the cluster
        std::string id;
        /// Whether the cluster is currently enabled or not
        bool isEnabled = false;
        /// Whether the cluster is currently connected or not
        bool connected = false;
    };

    /// This struct represents the information about the processes that are registered with
    /// the Core library
    struct Process {
        struct NodeStatus {
            int id = -1;
            std::string node;
            std::string status;
            double time = -1.0;
        };

        /// The unique identifier for the process
        int id = -1;
        std::string applicationId;
        std::string configurationId;
        std::string clusterId;
        std::string clusterStatus;
        double clusterStatusTime = -1.0;
        std::vector<GuiInitialization::Process::NodeStatus> nodeStatusHistory;
    };
    
    /// All applications that are registered with the Core
    std::vector<Application> applications;
    /// All clusters that the Core is responsible for
    std::vector<Cluster> clusters;
    /// All current processes
    std::vector<Process> processes;
};

void to_json(nlohmann::json& j, const GuiInitialization& p);
void to_json(nlohmann::json& j, const GuiInitialization::Application& p);
void to_json(nlohmann::json& j, const GuiInitialization::Application::Configuration& p);
void to_json(nlohmann::json& j, const GuiInitialization::Cluster& p);
void to_json(nlohmann::json& j, const GuiInitialization::Process& p);
void to_json(nlohmann::json& j, const GuiInitialization::Process::NodeStatus& p);
void from_json(const nlohmann::json& j, GuiInitialization& p);
void from_json(const nlohmann::json& j, GuiInitialization::Application& p);
void from_json(const nlohmann::json& j, GuiInitialization::Application::Configuration& p);
void from_json(const nlohmann::json& j, GuiInitialization::Cluster& p);
void from_json(const nlohmann::json& j, GuiInitialization::Process& p);
void from_json(const nlohmann::json& j, GuiInitialization::Process::NodeStatus& p);

} // namespace

#endif // __CORE__CORECOMMAND_H__

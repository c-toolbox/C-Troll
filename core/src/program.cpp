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

#include "program.h"

#include "jsonload.h"
#include "logging.h"
#include <assert.h>

namespace {
    constexpr const char* KeyId = "id";
    constexpr const char* KeyName = "name";
    constexpr const char* KeyExecutable = "executable";
    constexpr const char* KeyBaseDirectory = "baseDirectory";
    constexpr const char* KeyCommandlineParameters = "commandlineParameters";
    constexpr const char* KeyCurrentWorkingDirectory = "currentWorkingDirectory";
    constexpr const char* KeyClusterCommandlineParameters = "clusters";
    constexpr const char* KeyDefaults = "defaults";
    constexpr const char* KeyDefaultCluster = "cluster";
    constexpr const char* KeyDefaultConfiguration = "configuration";
    constexpr const char* KeyTags = "tags";
    constexpr const char* KeyConfigurations = "configurations";

    constexpr const char* KeyConfigurationName = "name";
    constexpr const char* KeyConfigurationIdentifier = "identifier";
    constexpr const char* KeyConfigurationParameters = "commandlineParameters";
} // namespace

void to_json(nlohmann::json& j, const Program::Configuration& p) {
    j = {
        { KeyConfigurationName, p.name },
        { KeyClusterCommandlineParameters, p.clusterCommandlineParameters }
    };
}

void from_json(const nlohmann::json& j, Program::Configuration& p) {
    j.at(KeyConfigurationName).get_to(p.name);
    j.at(KeyClusterCommandlineParameters).get_to(p.clusterCommandlineParameters);
}


void to_json(nlohmann::json& j, const Program& p) {
    j = {
        { KeyId, p.id },
        { KeyName, p.name },
        { KeyExecutable, p.executable },
    };
    if (!p.baseDirectory.empty()) {
        j[KeyBaseDirectory] = p.baseDirectory;
    }
    if (!p.commandlineParameters.empty()) {
        j[KeyCommandlineParameters] = p.commandlineParameters;
    }
    if (!p.currentWorkingDirectory.empty()) {
        j[KeyCurrentWorkingDirectory] = p.currentWorkingDirectory;
    }
    if (!p.tags.empty()) {
        j[KeyTags] = p.tags;
    }
    if (!p.configurations.empty()) {
        std::map<std::string, Program::Configuration> confs;
        for (const Program::Configuration& c : p.configurations) {
            confs[c.id] = c;
        }

        j[KeyConfigurations] = confs;
    }

    if (!p.defaultConfiguration.empty() || !p.defaultCluster.empty()) {
        j[KeyDefaults][KeyDefaultCluster] = p.defaultCluster;
        j[KeyDefaults][KeyDefaultConfiguration] = p.defaultConfiguration;
    }
}

void from_json(const nlohmann::json& j, Program& p) {
    j.at(KeyId).get_to(p.id);
    j.at(KeyName).get_to(p.name);
    j.at(KeyExecutable).get_to(p.executable);
    if (j.find(KeyBaseDirectory) != j.end()) {
        j.at(KeyBaseDirectory).get_to(p.baseDirectory);
    }
    if (j.find(KeyCommandlineParameters) != j.end()) {
        j.at(KeyCommandlineParameters).get_to(p.commandlineParameters);
    }
    p.currentWorkingDirectory = p.baseDirectory;
    if (j.find(KeyCurrentWorkingDirectory) != j.end()) {
        j.at(KeyCurrentWorkingDirectory).get_to(p.currentWorkingDirectory);
    }
    if (j.find(KeyTags) != j.end()) {
        j.at(KeyTags).get_to(p.tags);
    }
    if (j.find(KeyDefaults) != j.end()) {
        nlohmann::json defaults = j.at(KeyDefaults);
        defaults.at(KeyDefaultConfiguration).get_to(p.defaultConfiguration);
        defaults.at(KeyDefaultCluster).get_to(p.defaultCluster);
    }
    if (j.find(KeyConfigurations) != j.end()) {
        std::map<std::string, Program::Configuration> confs;
        j.at(KeyConfigurations).get_to(confs);
        for (const std::pair<std::string, Program::Configuration>& conf : confs) {
            Program::Configuration c = conf.second;
            c.id = conf.first;
            p.configurations.push_back(std::move(c));
        }
    }
}

std::vector<Program> loadProgramsFromDirectory(const std::string& directory) {
    return common::loadJsonFromDirectory<Program>(directory, "application");
}

common::GuiInitialization::Application programToGuiApplication(const Program& prog) {
    common::GuiInitialization::Application app;
    app.name = prog.name;
    app.id = prog.id;
    app.tags = prog.tags;

    for (const Program::Configuration& conf : prog.configurations) {
        common::GuiInitialization::Application::Configuration c;
        c.name = conf.name;
        c.id = conf.id;
        for (const std::pair<std::string, std::string>& p :
             conf.clusterCommandlineParameters)
        {
            c.clusters.push_back(p.first);
        }

        app.configurations.push_back(c);
    }

    app.defaultCluster = prog.defaultCluster;
    app.defaultConfiguration = prog.defaultConfiguration;
    
    return app;
}

Program::~Program() {
    assert(processes.empty());
}

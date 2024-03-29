/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                               *
 * Alexander Bock                                                                        *
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

#include "program.h"

#include "jsonload.h"
#include "logging.h"
#include <fmt/format.h>
#include <assert.h>
#include <string_view>

namespace {
    constexpr std::string_view KeyName = "name";
    constexpr std::string_view KeyExecutable = "executable";
    constexpr std::string_view KeyCommandlineParameters = "commandlineParameters";
    constexpr std::string_view KeyWorkingDirectory = "workingDirectory";
    constexpr std::string_view KeyClusters = "clusters";
    constexpr std::string_view KeyTags = "tags";
    constexpr std::string_view KeyDescription = "description";
    constexpr std::string_view KeySendConsole = "shouldForwardMessages";
    constexpr std::string_view KeyEnabled = "enabled";
    constexpr std::string_view KeyDelay = "delay";
    constexpr std::string_view KeyPreStart = "prestart";
    constexpr std::string_view KeyConfigurations = "configurations";

    constexpr std::string_view KeyConfigurationName = "name";
    constexpr std::string_view KeyConfigurationParameters = "parameters";
    constexpr std::string_view KeyConfigurationDescription = "description";

    constexpr std::string_view KeyClusterName = "name";
    constexpr std::string_view KeyClusterParameters = "parameters";
} // namespace

void from_json(const nlohmann::json& j, Program::Configuration& p) {
    j.at(KeyConfigurationName).get_to(p.name);
    if (auto it = j.find(KeyConfigurationParameters);  it != j.end()) {
        it->get_to(p.parameters);
    }
    if (auto it = j.find(KeyConfigurationDescription);  it != j.end()) {
        it->get_to(p.description);
    }
}

void to_json(nlohmann::json& j, const Program::Configuration& p) {
    j[KeyConfigurationName] = p.name;
    j[KeyConfigurationParameters] = p.parameters;
    if (p.description != Program::Configuration().description) {
        j[KeyConfigurationDescription] = p.description;
    }
}

void from_json(const nlohmann::json& j, Program::Cluster& c) {
    j.at(KeyClusterName).get_to(c.name);
    if (auto it = j.find(KeyClusterParameters);  it != j.end()) {
        it->get_to(c.parameters);
    }
}

void to_json(nlohmann::json& j, const Program::Cluster& c) {
    j[KeyClusterName] = c.name;
    if (c.parameters != Program::Cluster().parameters) {
        j[KeyClusterParameters] = c.parameters;
    }
}

void from_json(const nlohmann::json& j, Program& p) {
    j.at(KeyName).get_to(p.name);
    j.at(KeyExecutable).get_to(p.executable);
    if (auto it = j.find(KeyCommandlineParameters);  it != j.end()) {
        it->get_to(p.commandlineParameters);
    }
    if (auto it = j.find(KeyWorkingDirectory);  it != j.end()) {
        it->get_to(p.workingDirectory);
    }
    if (auto it = j.find(KeyTags);  it != j.end()) {
        it->get_to(p.tags);
    }
    if (auto it = j.find(KeyDescription);  it != j.end()) {
        it->get_to(p.description);
    }
    if (auto it = j.find(KeySendConsole);  it != j.end()) {
        it->get_to(p.shouldForwardMessages);
    }
    if (auto it = j.find(KeyEnabled);  it != j.end()) {
        it->get_to(p.isEnabled);
    }
    if (auto it = j.find(KeyDelay);  it != j.end()) {
        const unsigned int delay = it->get<unsigned int>();
        p.delay = std::chrono::milliseconds(delay);
    }
    if (auto it = j.find(KeyPreStart);  it != j.end()) {
        it->get_to(p.preStart);
    }
    if (auto it = j.find(KeyConfigurations);  it != j.end()) {
        it->get_to(p.configurations);
    }
    else {
        // There always has to be at least a default configuration
        p.configurations.push_back({ Program::Configuration::ID(0), "Default", "", "" });
    }

    // For backwards-compatibility with v1, we still support the option to provide the
    // clusters as an array of strings, in which case we have to manually wrap them
    nlohmann::json::array_t it = j.at(KeyClusters);
    if (!it.empty()) {
        if (it[0].is_object()) {
            j.at(KeyClusters).get_to(p.clusters);
        }
        else {
            std::vector<std::string> clusters;
            j.at(KeyClusters).get_to(clusters);
            for (std::string cluster : clusters) {
                p.clusters.push_back({ .name = std::move(cluster) });
            }
        }
    }
}

void to_json(nlohmann::json& j, const Program& p) {
    j[KeyName] = p.name;
    j[KeyExecutable] = p.executable;
    if (p.commandlineParameters != Program().commandlineParameters) {
        j[KeyCommandlineParameters] = p.commandlineParameters;
    }
    if (p.workingDirectory != Program().workingDirectory) {
        j[KeyWorkingDirectory] = p.workingDirectory;
    }
    if (p.tags != Program().tags) {
        j[KeyTags] = p.tags;
    }
    if (p.description != Program().description) {
        j[KeyDescription] = p.description;
    }
    if (p.shouldForwardMessages != Program().shouldForwardMessages) {
        j[KeySendConsole] = p.shouldForwardMessages;
    }
    if (p.isEnabled != Program().isEnabled) {
        j[KeyEnabled] = p.isEnabled;
    }
    if (p.delay != Program().delay && p.delay.has_value() ) {
        j[KeyDelay] = p.delay->count();
    }
    if (p.preStart != Program().preStart) {
        j[KeyPreStart] = p.preStart;
    }
    j[KeyConfigurations] = p.configurations;
    j[KeyClusters] = p.clusters;
}

std::pair<std::vector<Program>, bool> loadProgramsFromDirectory(
                                                               std::string_view directory)
{
    std::pair<std::vector<Program>, bool> res = common::loadJsonFromDirectory<Program>(
        directory,
        validation::loadValidator(":/schema/config/program.schema.json")
    );

    // Inject the unique identifiers into the nodes
    int programId = 0;
    for (Program& program : res.first) {
        program.id = programId;
        programId++;

        int configurationId = 0;
        for (Program::Configuration& conf : program.configurations) {
            conf.id = configurationId;
            configurationId++;
        }
    }

    return res;
}

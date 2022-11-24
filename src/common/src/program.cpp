/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2022                                                             *
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
    constexpr std::string_view KeyDelay = "delay";
    constexpr std::string_view KeyConfigurations = "configurations";

    constexpr std::string_view KeyConfigurationName = "name";
    constexpr std::string_view KeyConfigurationParameters = "parameters";
    constexpr std::string_view KeyConfigurationDescription = "description";
} // namespace

void from_json(const nlohmann::json& j, Program::Configuration& p) {
    j.at(KeyConfigurationName).get_to(p.name);
    if (j.find(KeyConfigurationParameters) != j.end()) {
        j[KeyConfigurationParameters].get_to(p.parameters);
    }
    if (j.find(KeyConfigurationDescription) != j.end()) {
        j[KeyConfigurationDescription].get_to(p.description);
    }
}

void to_json(nlohmann::json& j, const Program::Configuration& p) {
    j[KeyConfigurationName] = p.name;
    j[KeyConfigurationParameters] = p.parameters;
    j[KeyConfigurationDescription] = p.description;
}

void from_json(const nlohmann::json& j, Program& p) {
    j.at(KeyName).get_to(p.name);
    j.at(KeyExecutable).get_to(p.executable);
    if (j.find(KeyCommandlineParameters) != j.end()) {
        j.at(KeyCommandlineParameters).get_to(p.commandlineParameters);
    }
    if (j.find(KeyWorkingDirectory) != j.end()) {
        j.at(KeyWorkingDirectory).get_to(p.workingDirectory);
    }
    if (j.find(KeyTags) != j.end()) {
        j.at(KeyTags).get_to(p.tags);
    }
    if (j.find(KeyDescription) != j.end()) {
        j.at(KeyDescription).get_to(p.description);
    }
    if (j.find(KeySendConsole) != j.end()) {
        j.at(KeySendConsole).get_to(p.shouldForwardMessages);
    }
    if (j.find(KeyDelay) != j.end()) {
        const unsigned int delay = j.at(KeyDelay).get<unsigned int>();
        p.delay = std::chrono::milliseconds(delay);
    }
    if (j.find(KeyConfigurations) != j.end()) {
        j.at(KeyConfigurations).get_to(p.configurations);
    }
    else {
        // There always has to be at least a default configuration
        p.configurations.push_back({ Program::Configuration::ID(0), "Default", "", "" });
    }

    j.at(KeyClusters).get_to(p.clusters);
}

void to_json(nlohmann::json& j, const Program& p) {
    j[KeyName] = p.name;
    j[KeyExecutable] = p.executable;
    if (!p.commandlineParameters.empty()) {
        j[KeyCommandlineParameters] = p.commandlineParameters;
    }
    if (!p.workingDirectory.empty()) {
        j[KeyWorkingDirectory] = p.workingDirectory;
    }
    if (!p.tags.empty()) {
        j[KeyTags] = p.tags;
    }
    if (!p.description.empty()) {
        j[KeyDescription] = p.description;
    }
    j[KeySendConsole] = p.shouldForwardMessages;
    if (p.delay.has_value()) {
        j[KeyDelay] = p.delay->count();
    }
    j[KeyConfigurations] = p.configurations;
    j[KeyClusters] = p.clusters;
}

std::vector<Program> loadProgramsFromDirectory(std::string_view directory) {
    std::vector<Program> programs = common::loadJsonFromDirectory<Program>(directory);

    // Inject the unique identifiers into the nodes
    int programId = 0;
    for (Program& program : programs) {
        program.id = programId;
        programId++;

        int configurationId = 0;
        for (Program::Configuration& conf : program.configurations) {
            conf.id = configurationId;
            configurationId++;
        }
    }

    return programs;
}

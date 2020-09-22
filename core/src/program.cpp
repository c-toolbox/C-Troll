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

#include "program.h"

#include "cluster.h"
#include "database.h"
#include "jsonload.h"
#include "logging.h"
#include <fmt/format.h>
#include <assert.h>

namespace {
    constexpr const char* KeyName = "name";
    constexpr const char* KeyExecutable = "executable";
    constexpr const char* KeyCommandlineParameters = "commandlineParameters";
    constexpr const char* KeyWorkingDirectory = "workingDirectory";
    constexpr const char* KeyClusters = "clusters";
    constexpr const char* KeyTags = "tags";
    constexpr const char* KeySendConsole = "shouldForwardMessages";
    constexpr const char* KeyDelay = "delay";
    constexpr const char* KeyConfigurations = "configurations";

    constexpr const char* KeyConfigurationName = "name";
    constexpr const char* KeyConfigurationParameters = "parameters";
} // namespace

void from_json(const nlohmann::json& j, Program::Configuration& p) {
    j.at(KeyConfigurationName).get_to(p.name);
    j.at(KeyConfigurationParameters).get_to(p.parameters);
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
        p.configurations.push_back({ Program::Configuration::ID{ 0 }, "Default", "" });
    }

    std::vector<std::string> clusters = j.at(KeyClusters).get<std::vector<std::string>>();
    for (const std::string& cluster : clusters) {
        Cluster* c = data::findCluster(cluster);
        if (!c) {
            std::string message = fmt::format("Could not find cluster {}", cluster);
            ::Log("Error", message);
            throw std::runtime_error(message);
        }
        p.clusters.push_back(c->id);
    }
}

std::vector<Program> loadProgramsFromDirectory(const std::string& directory) {
    std::vector<Program> programs = common::loadJsonFromDirectory<Program>(directory);

    for (const Program& program : programs) {
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
            program.tags.begin(), program.tags.end(),
            std::mem_fn(&std::string::empty)
        );
        if (hasEmptyTag) {
            throw std::runtime_error(fmt::format(
                "At least one tag of the program {} has an empty tag", program.name
            ));
        }
    }

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

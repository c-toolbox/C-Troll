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

#ifndef __SHARED__PROGRAM_H__
#define __SHARED__PROGRAM_H__

#include "cluster.h"
#include "typedid.h"
#include <json/json.hpp>
#include <chrono>
#include <optional>
#include <string>
#include <vector>

struct Program {
    struct Configuration {
        using ID = TypedId<int, struct ConfigurationTag>;

        /// Unique identifier of the configuration
        ID id{ -1 };

        /// User-facing name of the configuration
        std::string name;

        /// Commandline parameters that are associated with the configuration
        std::string parameters;

        /// A user-friendly description that better identifies this configuration
        std::string description;
    };

    using ID = TypedId<int, struct ProgramTag>;

    /// A unique identifier
    ID id = ID(-1);
    /// A human readable name for this Program
    std::string name;
    /// The full path to the executable
    std::string executable;
    /// A fixed set of commandline parameters
    std::string commandlineParameters;
    /// The current working directory from which the Program is started
    std::string workingDirectory;
    /// If this is set to 'true', child processes will forward the Std and error streams
    bool shouldForwardMessages = false;
    /// An optional delay that is introduced between startup of individual instances
    std::optional<std::chrono::milliseconds> delay;
    /// A list of tags that are associated with this Program
    std::vector<std::string> tags;
    /// A user-friendly description that potentially better identifies the whole program
    std::string description;
    /// List of all configurations
    std::vector<Configuration> configurations;
    /// List of all clusters
    std::vector<std::string> clusters;
};

std::vector<Program> loadProgramsFromDirectory(std::string_view directory);

void from_json(const nlohmann::json& j, Program& p);
void to_json(nlohmann::json& j, const Program& p);

#endif // __SHARED__PROGRAM_H__

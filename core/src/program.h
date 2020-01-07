/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sund�n, Emil Axelsson                                            *
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

#ifndef __CORE__PROGRAM_H__
#define __CORE__PROGRAM_H__

#include "guiinitialization.h"
#include <json/json.hpp>
#include <string>
#include <vector>

class Process;

struct Program {
    struct Configuration {
        std::string id;
        std::string name;
        std::map<std::string, std::string> clusterCommandlineParameters;
    };

    Program() = default;
    ~Program();
    
    /// A unique identifier
    std::string id;
    /// A human readable name for this Program
    std::string name;
    /// The full path to the executable
    std::string executable;
    /// The base directory of the application
    std::string baseDirectory;
    /// A fixed set of commandline parameters
    std::string commandlineParameters;
    /// The current working directory from which the Program is started
    std::string currentWorkingDirectory;
    /// A list of tags that are associated with this Program
    std::vector<std::string> tags;
    // List of all configurations
    std::vector<Configuration> configurations;
    // Default configuration id
    std::string defaultConfiguration;
    // Default cluster id
    std::string defaultCluster;
    /// A vector of processes that derive from this program.
    std::vector<Process*> processes;
};

std::vector<Program> loadProgramsFromDirectory(const std::string& directory);

common::GuiInitialization::Application programToGuiApplication(const Program& p);

void to_json(nlohmann::json& j, const Program& p);
void from_json(const nlohmann::json& j, Program& p);

#endif // __CORE__PROGRAM_H__

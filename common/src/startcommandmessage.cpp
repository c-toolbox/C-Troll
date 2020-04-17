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

#include "startcommandmessage.h"

#include "logging.h"
#include <fmt/format.h>
#include <optional>

namespace {
    constexpr const char* KeyId = "id";
    constexpr const char* KeyForwardOutErr = "forwardOutErr";
    constexpr const char* KeyExecutable = "executable";
    constexpr const char* KeyWorkingDirectory = "workingDirectory";
    constexpr const char* KeyCommandlineArguments = "commandlineArguments";

    constexpr const char* KeyProgramId = "programId";
    constexpr const char* KeyConfigurationId = "configurationId";
    constexpr const char* KeyClusterId = "clusterId";
    constexpr const char* KeyNodeId = "nodeId";
    constexpr const char* KeyDataHash = "datahash";
} // namespace

namespace common {

void to_json(nlohmann::json& j, const StartCommandMessage& p) {
    j = {
        { Message::KeyType, StartCommandMessage::Type },
        { Message::KeyVersion, p.CurrentVersion },
        { Message::KeySecret, p.secret },
        { KeyId, p.id },
        { KeyForwardOutErr, p.forwardStdOutStdErr },
        { KeyExecutable, p.executable },
        { KeyWorkingDirectory, p.workingDirectory },
        { KeyCommandlineArguments, p.commandlineParameters },
        { KeyProgramId, p.programId },
        { KeyConfigurationId, p.configurationId },
        { KeyClusterId, p.clusterId },
        { KeyNodeId, p.nodeId },
        { KeyDataHash, p.dataHash }
    };
}

void from_json(const nlohmann::json& j, StartCommandMessage& p) {
    validateMessage(j, StartCommandMessage::Type);

    j.at(KeyId).get_to(p.id);
    j.at(KeyForwardOutErr).get_to(p.forwardStdOutStdErr);
    
    j.at(KeyExecutable).get_to(p.executable);
    j.at(KeyWorkingDirectory).get_to(p.workingDirectory);
    j.at(KeyCommandlineArguments).get_to(p.commandlineParameters);

    j.at(KeyProgramId).get_to(p.programId);
    j.at(KeyConfigurationId).get_to(p.configurationId);
    j.at(KeyClusterId).get_to(p.clusterId);
    j.at(KeyNodeId).get_to(p.nodeId);
    j.at(KeyDataHash).get_to(p.dataHash);
}

} // namespace common

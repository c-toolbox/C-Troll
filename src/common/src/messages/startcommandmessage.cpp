/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2025                                                               *
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

#include "messages/startcommandmessage.h"

namespace {
    constexpr std::string_view KeyId = "id";
    constexpr std::string_view KeyForwardOutErr = "forwardOutErr";
    constexpr std::string_view KeyExecutable = "executable";
    constexpr std::string_view KeyWorkingDirectory = "workingDirectory";
    constexpr std::string_view KeyCommandlineArguments = "commandlineArguments";

    constexpr std::string_view KeyProgramId = "programId";
    constexpr std::string_view KeyConfigurationId = "configurationId";
    constexpr std::string_view KeyClusterId = "clusterId";
    constexpr std::string_view KeyNodeId = "nodeId";
    constexpr std::string_view KeyDataHash = "datahash";
} // namespace

namespace common {

StartCommandMessage::StartCommandMessage()
    : Message(std::string(StartCommandMessage::Type))
{}

void to_json(nlohmann::json& j, const StartCommandMessage& m) {
    j[Message::KeyType] = StartCommandMessage::Type;
    j[Message::KeyVersion] = { api::MajorVersion, api::MinorVersion, api::PatchVersion };
    j[Message::KeySecret] = m.secret;
    j[KeyId] = m.id;
    if (m.forwardStdOutStdErr) {
        j[KeyForwardOutErr] = m.forwardStdOutStdErr;
    }
    j[KeyExecutable] = m.executable;
    j[KeyWorkingDirectory] = m.workingDirectory;
    if (!m.commandlineParameters.empty()) {
        j[KeyCommandlineArguments] = m.commandlineParameters;
    }
    j[KeyProgramId] = m.programId;
    j[KeyConfigurationId] = m.configurationId;
    j[KeyClusterId] = m.clusterId;
    j[KeyNodeId] = m.nodeId;
    j[KeyDataHash] = m.dataHash;
}

void from_json(const nlohmann::json& j, StartCommandMessage& m) {
    validateMessage(j, StartCommandMessage::Type);
    from_json(j, static_cast<Message&>(m));

    j.at(KeyId).get_to(m.id);
    if (auto it = j.find(KeyForwardOutErr);  it != j.end()) {
        it->get_to(m.forwardStdOutStdErr);
    }

    j.at(KeyExecutable).get_to(m.executable);
    j.at(KeyWorkingDirectory).get_to(m.workingDirectory);

    if (auto it = j.find(KeyCommandlineArguments);  it != j.end()) {
        it->get_to(m.commandlineParameters);
    }

    j.at(KeyProgramId).get_to(m.programId);
    j.at(KeyConfigurationId).get_to(m.configurationId);
    j.at(KeyClusterId).get_to(m.clusterId);
    j.at(KeyNodeId).get_to(m.nodeId);
    j.at(KeyDataHash).get_to(m.dataHash);
}

} // namespace common

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

#include "traystatusmessage.h"

namespace {
    constexpr const char* KeyProcesses = "processes";

    constexpr const char* KeyProcessId = "processId";
    constexpr const char* KeyProgramId = "programId";
    constexpr const char* KeyConfigurationId = "configurationId";
    constexpr const char* KeyClusterId = "clusterId";
    constexpr const char* KeyNodeId = "nodeId";
    constexpr const char* KeyDataHash = "datahash";
} // namespace

namespace common {

void to_json(nlohmann::json& j, const TrayStatusMessage::ProcessInfo& p) {
    j[KeyProcessId] = p.processId;
    j[KeyProgramId] = p.programId;
    j[KeyConfigurationId] = p.configurationId;
    j[KeyClusterId] = p.clusterId;
    j[KeyNodeId] = p.nodeId;
    j[KeyDataHash] = p.dataHash;
}

void from_json(const nlohmann::json & j, TrayStatusMessage::ProcessInfo& p) {
    j.at(KeyProcessId).get_to(p.processId);
    j.at(KeyProgramId).get_to(p.programId);
    j.at(KeyConfigurationId).get_to(p.configurationId);
    j.at(KeyClusterId).get_to(p.clusterId);
    j.at(KeyNodeId).get_to(p.nodeId);
    j.at(KeyDataHash).get_to(p.dataHash);
}

void to_json(nlohmann::json& j, const TrayStatusMessage& m) {
    j[Message::KeyType] = TrayStatusMessage::Type;
    j[Message::KeyVersion] = m.CurrentVersion;
    j[KeyProcesses] = m.processes;
}

void from_json(const nlohmann::json& j, TrayStatusMessage& m) {
    validateMessage(j, TrayStatusMessage::Type);
    from_json(j, static_cast<Message&>(m));

    j.at(KeyProcesses).get_to(m.processes);
}

} // namespace

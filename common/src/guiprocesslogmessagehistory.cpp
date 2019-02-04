/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
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

#include "guiprocesslogmessagehistory.h"

namespace {
    constexpr const char* KeyProcessId = "processId";
    constexpr const char* KeyApplicationId = "applicationId";
    constexpr const char* KeyClusterId = "clusterId";

    constexpr const char* KeyConfigurationId = "configurationId";
    constexpr const char* KeyMessages = "messages";

    constexpr const char* KeyLogMessageId = "id";
    constexpr const char* KeyLogMessageNodeId = "nodeId";
    constexpr const char* KeyLogMessageOutputType = "outputType";
    constexpr const char* KeyLogMessageTime = "time";
    constexpr const char* KeyLogMessageMessage = "message";
} // namespace

namespace common {

void to_json(nlohmann::json& j, const GuiProcessLogMessageHistory& p) {
    j = {
        { KeyProcessId, p.processId },
        { KeyApplicationId, p.applicationId },
        { KeyClusterId, p.clusterId },
        { KeyMessages, p.logMessages }
    };
}

void to_json(nlohmann::json& j, const GuiProcessLogMessageHistory::LogMessage& p) {
    j = {
        { KeyLogMessageId, p.id },
        { KeyLogMessageNodeId, p.nodeId },
        { KeyLogMessageMessage, p.message },
        { KeyLogMessageOutputType, p.outputType },
        { KeyLogMessageTime, p.time },
    };
}

void from_json(const nlohmann::json& j, GuiProcessLogMessageHistory& p) {
    j.at(KeyProcessId).get_to(p.processId);
    j.at(KeyApplicationId).get_to(p.applicationId);
    j.at(KeyClusterId).get_to(p.clusterId);
    j.at(KeyMessages).get_to(p.logMessages);
}

void from_json(const nlohmann::json& j, GuiProcessLogMessageHistory::LogMessage& p) {
    j.at(KeyLogMessageId).get_to(p.id);
    j.at(KeyLogMessageMessage).get_to(p.message);
    j.at(KeyLogMessageNodeId).get_to(p.nodeId);
    j.at(KeyLogMessageOutputType).get_to(p.outputType);
    j.at(KeyLogMessageTime).get_to(p.time);
}

} // namespace common

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

#include "guiprocessstatus.h"

namespace {
    constexpr const char* KeyProcessId = "processId";
    constexpr const char*  KeyApplicationId = "applicationId";
    constexpr const char*  KeyClusterId = "clusterId";
    constexpr const char*  KeyConfigurationId = "configurationId";
    constexpr const char*  KeyClusterStatus = "clusterStatus";
    constexpr const char*  KeyNodeStatus = "nodeStatus";
    constexpr const char*  KeyTime = "time";
    constexpr const char*  KeyId = "id";
} // namespace

namespace common {
    
void to_json(nlohmann::json& j, const GuiProcessStatus& p) {
    j = {
        { KeyId, p.id },
        { KeyProcessId, p.processId },
        { KeyApplicationId, p.applicationId },
        { KeyClusterId, p.clusterId },
        { KeyConfigurationId, p.configurationId },
        { KeyClusterStatus, p.clusterStatus },
        { KeyNodeStatus, p.nodeStatus },
        { KeyTime, p.time }
    };
}

void from_json(const nlohmann::json& j, GuiProcessStatus& p) {
    j.at(KeyId).get_to(p.id);
    j.at(KeyProcessId).get_to(p.processId);
    j.at(KeyClusterStatus).get_to(p.clusterStatus);
    j.at(KeyNodeStatus).get_to(p.nodeStatus);
    j.at(KeyTime).get_to(p.time);
}

} // namespace common

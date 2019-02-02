/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
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

#include "jsonsupport.h"
#include <QJsonObject>

namespace {
    const QString KeyProcessId = "processId";
    const QString KeyApplicationId = "applicationId";
    const QString KeyClusterId = "clusterId";
    const QString KeyConfigurationId = "configurationId";
    const QString KeyClusterStatus = "clusterStatus";
    const QString KeyNodeStatus = "nodeStatus";
    const QString KeyTime = "time";
    const QString KeyId = "id";
} // namespace

namespace common {
    
const QString GuiProcessStatus::Type = "GuiProcessStatus";

GuiProcessStatus::GuiProcessStatus(const QJsonDocument& document) {
    QJsonObject obj = document.object();
    
    id = common::testAndReturnInt(obj, KeyId);
    processId = common::testAndReturnInt(obj, KeyProcessId);
    clusterStatus = common::testAndReturnString(obj, KeyClusterStatus);
    QJsonObject nodeStatusObject = common::testAndReturnObject(obj, KeyNodeStatus);
    for (auto it = nodeStatusObject.begin(); it != nodeStatusObject.end(); it++) {
        nodeStatus[it.key()] = common::testAndReturnString(nodeStatusObject, it.key());
    }
    time = common::testAndReturnDouble(nodeStatusObject, KeyTime);   
}

QJsonDocument GuiProcessStatus::toJson() const {
    QJsonObject obj;
    obj[KeyId] = id;
    obj[KeyProcessId] = processId;
    obj[KeyApplicationId] = applicationId;
    obj[KeyClusterId] = clusterId;
    obj[KeyConfigurationId] = configurationId;
    obj[KeyClusterStatus] = clusterStatus;

    QJsonObject nodeStatusObject;
    for (auto it = nodeStatus.begin(); it != nodeStatus.end(); it++) {
        nodeStatusObject[it.key()] = it.value();
    }

    obj[KeyNodeStatus] = nodeStatusObject;
    obj[KeyTime] = time;

    return QJsonDocument(obj);
}
    
} // namespace common

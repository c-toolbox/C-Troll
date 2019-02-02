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

#include "guiprocesslogmessagehistory.h"

#include "jsonsupport.h"
#include <QJsonObject>

namespace {
    const QString KeyProcessId = "processId";
    const QString KeyApplicationId = "applicationId";
    const QString KeyClusterId = "clusterId";

    const QString KeyConfigurationId = "configurationId";
    const QString KeyMessages = "messages";

    const QString KeyLogMessageId = "id";
    const QString KeyLogMessageNodeId = "nodeId";
    const QString KeyLogMessageOutputType = "outputType";
    const QString KeyLogMessageTime = "time";   
    const QString KeyLogMessageMessage = "message";
} // namespace

namespace common {

const QString GuiProcessLogMessageHistory::Type = "GuiProcessLogMessageHistory";

GuiProcessLogMessageHistory::GuiProcessLogMessageHistory(const QJsonDocument& document) {
    QJsonObject obj = document.object();

    processId = common::testAndReturnInt(obj, KeyProcessId);
    applicationId = common::testAndReturnString(obj, KeyApplicationId);
    clusterId = common::testAndReturnString(obj, KeyClusterId);
    QJsonArray messageArray = common::testAndReturnArray(obj, KeyMessages);

    for (int i = 0; i < messageArray.size(); i++) {
        QJsonObject message = common::testAndReturnObject(messageArray, i);
        GuiProcessLogMessageHistory::LogMessage logMessage;
        logMessage.id = common::testAndReturnInt(obj, KeyLogMessageId);
        logMessage.message = common::testAndReturnString(obj, KeyLogMessageMessage);
        logMessage.nodeId = common::testAndReturnString(obj, KeyLogMessageNodeId);
        logMessage.outputType = common::testAndReturnString(obj, KeyLogMessageOutputType);
        logMessage.time = common::testAndReturnDouble(obj, KeyLogMessageTime);
        logMessages.push_back(logMessage);
    }
}

QJsonDocument GuiProcessLogMessageHistory::toJson() const {
    QJsonObject obj;
    obj[KeyProcessId] = processId;
    obj[KeyApplicationId] = applicationId;
    obj[KeyClusterId] = clusterId;

    QJsonArray messageArray;
    std::transform(
        logMessages.begin(),
        logMessages.end(),
        std::back_inserter(messageArray),
        [](const GuiProcessLogMessageHistory::LogMessage& logMessage) {
            QJsonObject message;
            message[KeyLogMessageId] = logMessage.id;
            message[KeyLogMessageNodeId] = logMessage.nodeId;
            message[KeyLogMessageMessage] = logMessage.message;
            message[KeyLogMessageOutputType] = logMessage.outputType;
            message[KeyLogMessageTime] = logMessage.time;
            return message;
        }
    );
    obj[KeyMessages] = messageArray;

    return QJsonDocument(obj);
}

} // namespace common

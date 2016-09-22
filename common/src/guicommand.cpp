/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
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

#include "guicommand.h"

#include "jsonsupport.h"

#include <QJsonObject>

namespace {
    const QString KeyCommand = "command";
    const QString KeyApplicationId = "application_identifier";
    const QString KeyConfigurationId = "configuration_identifier";
    const QString KeyClusterId = "cluster_identifier";
}

namespace common {

const QString GuiCommand::Type = "GuiCommand";
    
GuiCommand::GuiCommand(const QJsonDocument& document) {
    QJsonObject obj = document.object();

    command = common::testAndReturnString(obj, KeyCommand);
    applicationId = common::testAndReturnString(obj, KeyApplicationId);
    configurationId = common::testAndReturnString(obj, KeyConfigurationId);
    clusterId = common::testAndReturnString(obj, KeyClusterId);
}

QJsonDocument GuiCommand::toJson() const {
    QJsonObject obj;
    obj[KeyCommand] = command;
    obj[KeyApplicationId] = applicationId;
    obj[KeyConfigurationId] = configurationId;
    obj[KeyClusterId] = clusterId;

    return QJsonDocument(obj);
}

} // namespace common

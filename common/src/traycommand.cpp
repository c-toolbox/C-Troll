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

#include "traycommand.h"

#include "jsonsupport.h"

#include <QJsonObject>

namespace {
    const QString KeyId = "id";
    const QString KeyCommand = "command";
    const QString KeyExecutable = "executable";
    const QString KeyBaseDirectory = "baseDirectory";
    const QString KeyWorkingDirectory = "currentWorkingDirectory";
    const QString KeyCommandlineArguments = "commandlineArguments";
    const QString KeyEnvironmentVariables = "environmentVariables";
}

namespace common {

const QString TrayCommand::Type = "TrayCommand";
    
TrayCommand::TrayCommand(const QJsonDocument& document) {
    QJsonObject payload = document.object();
    QJsonObject obj = common::testAndReturnObject(payload, "payload");

    id = common::testAndReturnString(obj, KeyId);
    command = common::testAndReturnString(obj, KeyCommand);
    executable = common::testAndReturnString(obj, KeyExecutable);
    baseDirectory = common::testAndReturnString(obj, KeyBaseDirectory);
    currentWorkingDirectory = common::testAndReturnString(obj, KeyWorkingDirectory);
    commandlineParameters = common::testAndReturnString(obj, KeyCommandlineArguments);
    environmentVariables = common::testAndReturnString(obj, KeyEnvironmentVariables);
}

QJsonDocument TrayCommand::toJson() const {
    QJsonObject obj;
    obj[KeyId] = id;
    obj[KeyCommand] = command;
    obj[KeyExecutable] = executable;
    obj[KeyBaseDirectory] = baseDirectory;
    obj[KeyWorkingDirectory] = currentWorkingDirectory;
    obj[KeyCommandlineArguments] = commandlineParameters;
    obj[KeyEnvironmentVariables] = environmentVariables;

    return QJsonDocument(obj);
}

} // namespace common

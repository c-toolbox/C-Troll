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

#include "processhandler.h"

#include <genericmessage.h>
#include <trayprocesslogmessage.h>
#include <trayprocessstatus.h>
#include <QDebug>
#include <QJsonDocument>
#include <functional>
#include <jsonsupport.h>
#include <logging.h>

namespace {

std::vector<std::string> tokenizeString(const std::string& input, char separator) {
    size_t separatorPos = input.find(separator);
    if (separatorPos == std::string::npos) {
        return { input };
    }
    else {
        std::vector<std::string> result;
        size_t prevSeparator = 0;
        while (separatorPos != std::string::npos) {
            result.push_back(input.substr(prevSeparator, separatorPos - prevSeparator));
            prevSeparator = separatorPos + 1;
            separatorPos = input.find(separator, separatorPos + 1);
        }
        result.push_back(input.substr(prevSeparator));
        return result;
    }
}

} // namespace

void ProcessHandler::handleSocketMessage(const QJsonDocument& message) {
    // qDebug() << messageDoc;
    common::TrayCommand command = common::conv::from_qtjsondoc(message);
    
    Log("Received TrayCommand");
    Log("Command: " + command.command);
    Log("Id: " + command.id);
    Log("Executable: " + command.executable);
    Log("CommandLineParameters: " + command.commandlineParameters);
    Log("BaseDirectory: " + command.baseDirectory);
    Log("CurrentWorkingDirectory: " + command.currentWorkingDirectory);
    Log("EnvironmentVariables: " + command.environmentVariables);

    // Check if identifer of traycommand already is tied to a process
    // We don't allow the same id for multiple processes
    auto p = _processes.find(command.id);
    if (p == _processes.end()) {
        if (command.command == "Start") {
            // Not Found, create and run a process with it
            createAndRunProcessFromTrayCommand(command);
        }
        else {
            handlerErrorOccurred(QProcess::ProcessError::FailedToStart);
        }
    } else {
        // Found
        executeProcessWithTrayCommand(p->second, command);
    }
}

void ProcessHandler::handlerErrorOccurred(QProcess::ProcessError error) {
    qDebug() << "Error occurred: " << error;
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(
        _processes.begin(),
        _processes.end(),
        [process](const std::pair<int, QProcess*>& p) { return p.second == process; }
    );
    
    if (p2T != _processes.end() ) {
        common::TrayProcessStatus ps;
        ps.processId = p2T->first;
        bool sendError = true;
        switch (error) {
            case QProcess::FailedToStart:
                ps.status = common::TrayProcessStatus::Status::FailedToStart;
                break;
            case QProcess::Timedout:
                ps.status = common::TrayProcessStatus::Status::TimedOut;
                break;
            case QProcess::WriteError:
                ps.status = common::TrayProcessStatus::Status::WriteError;
                break;
            case QProcess::ReadError:
                ps.status = common::TrayProcessStatus::Status::ReadError;
                break;
            case QProcess::UnknownError:
                ps.status = common::TrayProcessStatus::Status::UnknownError;
                break;
            default:
                sendError = false;
                break;
        }
        if (sendError) {
            // Send out the TrayProcessStatus with the error/status string
            common::GenericMessage msg;
            msg.type = common::TrayProcessStatus::Type;
            msg.payload = ps;
            nlohmann::json j = msg;
            emit sendSocketMessage(common::conv::to_qtjsondoc(j));
        }
    }
}

void ProcessHandler::handleStarted() {
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(
        _processes.begin(),
        _processes.end(),
        [process](const std::pair<int, QProcess*>& p) { return p.second == process; }
    );
    
    if (p2T != _processes.end()) {
        // Send out the TrayProcessStatus with the status string
        common::TrayProcessStatus ps;
        ps.processId = p2T->first;
        ps.status = common::TrayProcessStatus::Status::Running;
        common::GenericMessage msg;
        msg.type = common::TrayProcessStatus::Type;
        msg.payload = ps;
        nlohmann::json j = msg;
        emit sendSocketMessage(common::conv::to_qtjsondoc(j));
    }
}

void ProcessHandler::handleFinished(int exitCode) {
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    handleFinished(exitCode, process->exitStatus());
}

void ProcessHandler::handleFinished(int, QProcess::ExitStatus exitStatus) {
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(
        _processes.begin(),
        _processes.end(),
        [process](const std::pair<int, QProcess*>& p) { return p.second == process; }
    );
    
    if (p2T != _processes.end()) {
        common::TrayProcessStatus ps;
        ps.processId = p2T->first;
        switch (exitStatus) {
            case QProcess::NormalExit:
                ps.status = common::TrayProcessStatus::Status::NormalExit;
                break;
            case QProcess::CrashExit:
                ps.status = common::TrayProcessStatus::Status::CrashExit;
                break;
            default:
                break;
        }
        // Send out the TrayProcessStatus with the error/status string
        common::GenericMessage msg;
        msg.type = common::TrayProcessStatus::Type;
        msg.payload = ps;
        nlohmann::json j = msg;
        emit sendSocketMessage(common::conv::to_qtjsondoc(j));
        
        // Remove this process from the list as we consider it finsihed
        _processes.erase(p2T);
    }
}

void ProcessHandler::handleReadyReadStandardError() {
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(
        _processes.begin(),
        _processes.end(),
        [process](const std::pair<int, QProcess*>& p) { return p.second == process; }
    );
    
    if (p2T != _processes.end()) {
        // Send out the TrayProcessLogMessage with the stderror key
        common::TrayProcessLogMessage pm;
        pm.processId = p2T->first;
        pm.outputType = common::TrayProcessLogMessage::OutputType::StdErr;
        pm.message = QString::fromLatin1(process->readAllStandardError()).toStdString();
        common::GenericMessage msg;
        msg.type = common::TrayProcessLogMessage::Type;
        msg.payload = pm;
        nlohmann::json j = msg;
        emit sendSocketMessage(common::conv::to_qtjsondoc(j));
    }
}

void ProcessHandler::handleReadyReadStandardOutput() {
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(
        _processes.begin(),
        _processes.end(),
        [process](const std::pair<int, QProcess*>& p) { return p.second == process; }
    );
    
    if (p2T != _processes.end()) {
        common::TrayProcessLogMessage pm;
        pm.processId = p2T->first;
        pm.message = QString::fromLatin1(process->readAllStandardOutput()).toStdString();
        pm.outputType = common::TrayProcessLogMessage::OutputType::StdOut;
        common::GenericMessage msg;
        msg.type = common::TrayProcessLogMessage::Type;
        msg.payload = pm;
        nlohmann::json j = msg;
        emit sendSocketMessage(common::conv::to_qtjsondoc(j));
    }
}

void ProcessHandler::executeProcessWithTrayCommand(QProcess* process,
                                                   const common::TrayCommand& command)
{
    if (command.command == "Start") {
        if (!command.environmentVariables.empty()) {
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
            // First split per variable, such that each string contains "name,value"
            std::vector<std::string> envVariables = tokenizeString(
                command.environmentVariables,
                ';'
            );
            for (const std::string& var : envVariables) {
                std::vector<std::string> envVariableNameValue = tokenizeString(
                    var,
                    ','
                );
                // Split name and value
                if (envVariableNameValue.size() == 2) {
                    env.insert(
                        QString::fromStdString(envVariableNameValue[0]),
                        QString::fromStdString(envVariableNameValue[1])
                    );
                }
            }
            process->setProcessEnvironment(env);
        }
        
        if (!command.currentWorkingDirectory.empty()) {
            process->setWorkingDirectory(
                QString::fromStdString(command.currentWorkingDirectory)
            );
        }
        else if (!command.baseDirectory.empty()) {
            process->setWorkingDirectory(QString::fromStdString(command.baseDirectory));
        }
        
        if (command.commandlineParameters.empty()) {
            process->start(QString::fromStdString("\"" + command.executable + "\""));
        }
        else {
            process->start(QString::fromStdString(
                "\"" + command.executable + "\" " + command.commandlineParameters
            ));
        }
    } else if (command.command == "Kill" || command.command == "Exit") {
        common::TrayProcessStatus ps;
        if (command.command == "Kill") {
            process->kill();
            ps.status = common::TrayProcessStatus::Status::CrashExit;
        }
        else {
            process->terminate();
            ps.status = common::TrayProcessStatus::Status::NormalExit;
        }
        // Find specifc value in process map i.e. process
        ProcessMap::iterator p2T = std::find_if(
            _processes.begin(),
            _processes.end(),
            [process](const std::pair<int, QProcess*>& p) { return p.second == process; }
        );

        if (p2T != _processes.end()) {
            ps.processId = p2T->first;
            // Send out the TrayProcessStatus with the error/status string
            common::GenericMessage msg;
            msg.type = common::TrayProcessStatus::Type;
            msg.payload = ps;
            nlohmann::json j = msg;
            emit sendSocketMessage(common::conv::to_qtjsondoc(j));
            // Remove this process from the list as we consider it finsihed
            _processes.erase(p2T);
        }
    }
}

void ProcessHandler::createAndRunProcessFromTrayCommand(
                                                       const common::TrayCommand& command)
{
    QProcess* newProcess = new QProcess(this);
    
    // Connect all process signals for logging feedback to core
    QObject::connect(
        newProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
        this, SLOT(handlerErrorOccurred(QProcess::ProcessError))
    );
    QObject::connect(
        newProcess, SIGNAL(finished(int)),
        this, SLOT(handleFinished(int))
    );
    QObject::connect(
        newProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
        this, SLOT(handleFinished(int, QProcess::ExitStatus))
    );
    QObject::connect(
        newProcess, SIGNAL(readyReadStandardError()),
        this, SLOT(handleReadyReadStandardError())
    );
    QObject::connect(
        newProcess, SIGNAL(readyReadStandardOutput()),
        this, SLOT(handleReadyReadStandardOutput())
    );
    QObject::connect(
        newProcess, SIGNAL(started()),
        this, SLOT(handleStarted())
    );
    
    // Insert command identifier and process into out lists
    _processes.insert(std::make_pair(command.id, newProcess));
    
    // Run the process with the command
    executeProcessWithTrayCommand(newProcess, command);
}

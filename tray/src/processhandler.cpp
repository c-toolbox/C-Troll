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

#include "processhandler.h"

#include "killallmessage.h"
#include "logging.h"
#include "message.h"
#include "processoutputmessage.h"
#include "processstatusmessage.h"
#include "traystatusmessage.h"
#include <fmt/format.h>
#include <functional>

namespace {
    std::vector<std::string> tokenizeString(const std::string& input, char separator) {
        size_t separatorPos = input.find(separator);
        if (separatorPos == std::string::npos) {
            return { input };
        }
        else {
            std::vector<std::string> res;
            size_t prevSeparator = 0;
            while (separatorPos != std::string::npos) {
                res.push_back(input.substr(prevSeparator, separatorPos - prevSeparator));
                prevSeparator = separatorPos + 1;
                separatorPos = input.find(separator, separatorPos + 1);
            }
            res.push_back(input.substr(prevSeparator));
            return res;
        }
    }

    common::ProcessStatusMessage::Status toTrayStatus(QProcess::ProcessError error) {
        switch (error) {
            case QProcess::FailedToStart:
                return common::ProcessStatusMessage::Status::FailedToStart;
            case QProcess::Crashed:
                return common::ProcessStatusMessage::Status::CrashExit;
            case QProcess::Timedout:
                return common::ProcessStatusMessage::Status::TimedOut;
            case QProcess::WriteError:
                return common::ProcessStatusMessage::Status::WriteError;
            case QProcess::ReadError:
                return common::ProcessStatusMessage::Status::ReadError;
            case QProcess::UnknownError:
                return common::ProcessStatusMessage::Status::UnknownError;
            default:
                throw std::logic_error("Unhandled case exception");
        }
    }

    common::ProcessStatusMessage::Status toTrayStatus(QProcess::ExitStatus status) {
        switch (status) {
            case QProcess::NormalExit:
                return common::ProcessStatusMessage::Status::NormalExit;
            case QProcess::CrashExit:
                return common::ProcessStatusMessage::Status::CrashExit;
            default:
                throw std::logic_error("Unhandled case exception");
        }
    }
} // namespace

void ProcessHandler::newConnection() {
    common::TrayStatusMessage msg;
    for (const std::pair<const int, QProcess*>& p : _processes) {
        msg.runningProcesses.push_back(p.first);
    }

    nlohmann::json j = msg;
    emit sendSocketMessage(j);
}

void ProcessHandler::handleSocketMessage(const nlohmann::json& message) {
    common::Message msg = message;

    if (common::isValidMessage<common::CommandMessage>(message)) {
        common::CommandMessage command = message;

        Log("Received CommandMessage");
        Log(fmt::format("\tCommand: {}", command.command));
        Log(fmt::format("\tId: {}", command.id));
        Log(fmt::format("\tExecutable: {}", command.executable));
        Log(fmt::format("\tCommandline Parameters: {}", command.commandlineParameters));
        Log(fmt::format("\tCurrent Working Directory: {}", command.workingDirectory));

        // Check if the identifier of traycommand already is tied to a process
        // We don't allow the same id for multiple processes
        const auto p = _processes.find(command.id);
        if (p == _processes.end()) {
            if (command.command == common::CommandMessage::Command::Start) {
                // Not Found, create and run a process with it
                createAndRunProcessFromCommandMessage(command);
            }
            else {
                handlerErrorOccurred(QProcess::ProcessError::FailedToStart);
            }
        }
        else {
            // Found
            executeProcessWithCommandMessage(p->second, command);
        }
    }
    else if (common::isValidMessage<common::KillAllMessage>(message)) {
        Log("Received KillAllMessage");

        for (const std::pair<const int, QProcess*>& p : _processes) {
            Log("Killing process " + std::to_string(p.first));

            p.second->close();
            p.second->deleteLater();
        }
        _processes.clear();
    }
}

void ProcessHandler::handlerErrorOccurred(QProcess::ProcessError error) {
    Log("Error occurred: " + error);
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    const auto p = processIt(process);
    
    if (p != _processes.end() ) {
        // @TODO (abock, 2020-02-26) Me thinks this codepath should also send a
        // "failedtostart" message. When trying to open the same process multiple times,
        // the core got stuck in a "Starting" state

        common::ProcessStatusMessage msg;
        msg.processId = p->first;
        msg.status = toTrayStatus(error);
        nlohmann::json j = msg;
        emit sendSocketMessage(j);
    }
}

void ProcessHandler::handleStarted() {
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    auto p = processIt(process);
    
    if (p != _processes.end()) {
        // Send out the TrayProcessStatus with the status string
        common::ProcessStatusMessage msg;
        msg.processId = p->first;
        msg.status = common::ProcessStatusMessage::Status::Running;
        nlohmann::json j = msg;
        emit sendSocketMessage(j);
    }
}

void ProcessHandler::handleFinished(int, QProcess::ExitStatus exitStatus) {
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    auto p = processIt(process);
    
    if (p != _processes.end()) {
        common::ProcessStatusMessage msg;
        msg.processId = p->first;
        msg.status = toTrayStatus(exitStatus);
        nlohmann::json j = msg;
        emit sendSocketMessage(j);
        
        // Remove this process from the list as we consider it finished
        _processes.erase(p);
    }
}

void ProcessHandler::handleReadyReadStandardError() {
    QProcess* proc = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    auto p = processIt(proc);
    if (p != _processes.end()) {
        // Send out the TrayProcessLogMessage with the stderror key
        common::ProcessOutputMessage msg;
        msg.processId = p->first;
        msg.outputType = common::ProcessOutputMessage::OutputType::StdErr;
        msg.message =
            QString::fromLatin1(proc->readAllStandardError()).toLocal8Bit().constData();
        nlohmann::json j = msg;
        emit sendSocketMessage(j);
    }
}

void ProcessHandler::handleReadyReadStandardOutput() {
    QProcess* proc = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    auto p = processIt(proc);
    if (p != _processes.end()) {
        common::ProcessOutputMessage msg;
        msg.processId = p->first;
        msg.message =
            QString::fromLatin1(proc->readAllStandardOutput()).toLocal8Bit().constData();
        msg.outputType = common::ProcessOutputMessage::OutputType::StdOut;
        nlohmann::json j = msg;
        emit sendSocketMessage(j);
    }
}

void ProcessHandler::executeProcessWithCommandMessage(QProcess* process,
                                                    const common::CommandMessage& command)
{
    if (command.command == common::CommandMessage::Command::Start) {
        // Send out the TrayProcessStatus with the status "Started"
        common::ProcessStatusMessage msg;
        msg.processId = command.id;
        msg.status = common::ProcessStatusMessage::Status::Starting;
        nlohmann::json j = msg;
        emit sendSocketMessage(j);

        if (!command.workingDirectory.empty()) {
            process->setWorkingDirectory(command.workingDirectory.c_str());
        }
        
        if (command.commandlineParameters.empty()) {
            std::string cmd = fmt::format("\"{}\"", command.executable);
            process->start(cmd.c_str());
        }
        else {
            std::string cmd = fmt::format(
                "\"{}\" {}", command.executable, command.commandlineParameters
            );
            process->start(cmd.c_str());
        }
    }
    else if (command.command == common::CommandMessage::Command::Kill ||
             command.command == common::CommandMessage::Command::Exit)
    {
        common::ProcessStatusMessage msg;
        if (command.command == common::CommandMessage::Command::Kill) {
            process->kill();
            msg.status = common::ProcessStatusMessage::Status::CrashExit;
        }
        else {
            process->terminate();
            msg.status = common::ProcessStatusMessage::Status::NormalExit;
        }
        // Find specifc value in process map i.e. process
        const auto p = std::find_if(
            _processes.cbegin(),
            _processes.cend(),
            [process](const std::pair<int, QProcess*>& p) { return p.second == process; }
        );

        if (p != _processes.end()) {
            msg.processId = p->first;
            nlohmann::json j = msg;
            emit sendSocketMessage(j);
            // Remove this process from the list as we consider it finished
            _processes.erase(p);
        }
    }
}

void ProcessHandler::createAndRunProcessFromCommandMessage(
                                                        const common::CommandMessage& cmd)
{
    QProcess* proc = new QProcess(this);
    
    // Connect all process signals for logging feedback to core
    connect(proc, &QProcess::errorOccurred, this, &ProcessHandler::handlerErrorOccurred);
    connect(
        proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
        this, &ProcessHandler::handleFinished
    );

    if (cmd.forwardStdOutStdErr) {
        // Only forward the standard out and standard error pipes if the core wanted to
        connect(
            proc, &QProcess::readyReadStandardError,
            this, &ProcessHandler::handleReadyReadStandardError
        );
        connect(
            proc, &QProcess::readyReadStandardOutput,
            this, &ProcessHandler::handleReadyReadStandardOutput
        );
    }
    connect(proc, &QProcess::started, this, &ProcessHandler::handleStarted);
    
    // Insert command identifier and process into out lists
    _processes.insert(std::make_pair(cmd.id, proc));
    
    // Run the process with the command
    executeProcessWithCommandMessage(proc, cmd);
}

std::map<int, QProcess*>::const_iterator ProcessHandler::processIt(QProcess* process) {
    const auto p = std::find_if(
        _processes.cbegin(),
        _processes.cend(),
        [process](const std::pair<int, QProcess*>& p) { return p.second == process; }
    );
    return p;
}

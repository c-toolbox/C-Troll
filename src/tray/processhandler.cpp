/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2022                                                             *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
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

#include <QMetaEnum>
#include "logging.h"
#include "messages.h"
#include <fmt/format.h>
#include <filesystem>
#include <functional>

namespace {
    common::ProcessStatusMessage::Status toTrayStatus(QProcess::ProcessError error) {
        using PSM = common::ProcessStatusMessage;
        switch (error) {
            case QProcess::FailedToStart: return PSM::Status::FailedToStart;
            case QProcess::Crashed:       return PSM::Status::CrashExit;
            case QProcess::Timedout:      return PSM::Status::TimedOut;
            case QProcess::WriteError:    return PSM::Status::WriteError;
            case QProcess::ReadError:     return PSM::Status::ReadError;
            case QProcess::UnknownError:  return PSM::Status::UnknownError;
        }
        throw std::logic_error("Unhandled case exception");
    }

    common::ProcessStatusMessage::Status toTrayStatus(QProcess::ExitStatus status) {
        using PSM = common::ProcessStatusMessage;
        switch (status) {
            case QProcess::NormalExit: return PSM::Status::NormalExit;
            case QProcess::CrashExit:  return PSM::Status::CrashExit;
        }
        throw std::logic_error("Unhandled case exception");
    }

    void Debug(std::string msg) {
        ::Debug("ProcessHandler", std::move(msg));
    }

    void Log(std::string msg) {
        ::Log("ProcessHandler", std::move(msg));
    }
} // namespace

ProcessHandler::ProcessHandler() {
    Debug("Creating process handler");
}

ProcessHandler::~ProcessHandler() {
    Debug("Destroying process handler");
}

void ProcessHandler::newConnection() {
    common::TrayStatusMessage msg;
    for (const ProcessInfo& p : _processes) {
        common::TrayStatusMessage::ProcessInfo pi;
        pi.processId = p.processId;
        pi.programId = p.programId;
        pi.configurationId = p.configurationId;
        pi.clusterId = p.clusterId;
        pi.nodeId = p.nodeId;
        pi.dataHash = p.dataHash;
        msg.processes.push_back(std::move(pi));
    }

    emit sendSocketMessage(msg);
}

void ProcessHandler::handleSocketMessage(const nlohmann::json& message,
                                         const std::string& peer)
{
    try {
        Debug(fmt::format("Received message: {}", message.dump(2)));

        const bool validMessage = common::isValidMessage(message);
        if (!validMessage) {
            Debug("Message was invalid");
            return;
        }

        Debug("Parsing base message");
        common::Message msg = message;

        if (common::isValidMessage<common::StartCommandMessage>(message)) {
            common::StartCommandMessage command = message;
            Log(fmt::format("Received [{}]: {}", peer, message.dump()));

            // Check if the identifier of traycommand already is tied to a process
            // We don't allow the same id for multiple processes
            const auto p = processIt(command.id);
            if (p == _processes.end()) {
                // Not Found, create and run a process with it
                Debug("Creating new process for command");
                createAndRunProcessFromCommandMessage(command);
            }
            else {
                // Found
                // @TODO When would this be executed? We shouldn't be able to start a
                // process twice with the same id?
                Debug("Starting existing process for command");
                executeProcessWithCommandMessage(p->process, command);
            }
        }
        else if (common::isValidMessage<common::ExitCommandMessage>(message)) {
            common::ExitCommandMessage command = message;
            Log(fmt::format("Received [{}]: {}", peer, message.dump()));

            // Check if the identifier of traycommand already is tied to a process
            // We don't allow the same id for multiple processes
            const auto p = processIt(command.id);
            if (p == _processes.end()) {
                // @TODO This should probably send a different message to inform the
                // controller about this fact. It should not be possible to send a request
                // to exit an application that is already running
                Debug("Process was not found");
                handlerErrorOccurred(QProcess::ProcessError::FailedToStart);
            }
            else {
                // Found
                Debug("Terminating existing process");
                common::ProcessStatusMessage returnMsg;
                // @TODO How does the terminate behave when the program is hanging? There
                // seems to be a problem that a program is not correctly terminated in
                // those cases
                p->process->terminate();
                returnMsg.status = common::ProcessStatusMessage::Status::NormalExit;
                // Find specifc value in process map i.e. process
                const auto pIt = processIt(p->process);

                if (pIt != _processes.end()) {
                    Debug("Found process");
                    returnMsg.processId = pIt->processId;
                    emit sendSocketMessage(returnMsg);

                    // Remove this process from the list as we consider it finished
                    ProcessInfo info = *pIt;
                    _processes.erase(pIt);
                    emit closedProcess(info);
                }
            }
        }
        else if (common::isValidMessage<common::KillAllMessage>(message)) {
            Log(fmt::format("Received [{}]: {}", peer, message.dump()));

            for (ProcessInfo& p : _processes) {
                Log(fmt::format("Killing process {}", p.processId));

                p.wasUserTerminated = true;
                p.process->kill();
                p.process->close();
                p.process->deleteLater();
            }
            _processes.clear();
        }
        else if (common::isValidMessage<common::KillTrayMessage>(message)) {
            Log(fmt::format("Received [{}]: {}", peer, message.dump()));

            emit closeApplication();
        }
        else if (common::isValidMessage<common::RestartNodeMessage>(message)) {
            Log(fmt::format("Received [{}]: {}", peer, message.dump()));

            QProcess::startDetached("shutdown", { "/r", "/t", "0" });
        }
    }
    catch (const std::exception& e) {
        Log(fmt::format(
            "Caught exception {} with message: {}", e.what(), message.dump()
        ));
    }
}

void ProcessHandler::handlerErrorOccurred(QProcess::ProcessError error) {
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());

    // Find specifc value in process map i.e. process
    const auto p = processIt(process);
    if (p->wasUserTerminated && error == QProcess::Crashed) {
        // If the processe was terminated on behest of the user, than this error message
        // is going to be the error that tells us that the program "crashed", which we
        // don't want to sent to the UI as it would be confusing. If the user wanted to
        // terminate the process, they wouldn't expect it to show up as crashed instead
        return;
    }

    std::string err = QMetaEnum::fromType<QProcess::ProcessError>().valueToKey(error);
    Log("Process Error", err);
    if (p != _processes.end()) {
        Debug(fmt::format("Found process {}", p->processId));
        common::ProcessStatusMessage msg;
        msg.processId = p->processId;
        msg.status = toTrayStatus(error);
        emit sendSocketMessage(msg);

        // The FailedToStart error is handled differently since that is the one that will
        // not also lead to a `handleFinished` call
        if (error == QProcess::ProcessError::FailedToStart) {
            Debug(fmt::format("Removing process {}", p->processId));
            ProcessInfo info = *p;
            _processes.erase(p);
            emit closedProcess(info);
        }
    }
}

void ProcessHandler::handleStarted() {
    Debug("Process started");
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());

    // Find specifc value in process map i.e. process
    auto p = processIt(process);
    assert(p != _processes.end());
    if (p != _processes.end()) {
        Debug(fmt::format("Found process {}", p->processId));

        // Send out the TrayProcessStatus with the status string
        common::ProcessStatusMessage msg;
        msg.processId = p->processId;
        msg.status = common::ProcessStatusMessage::Status::Running;
        emit sendSocketMessage(msg);
    }
}

void ProcessHandler::handleFinished(int, QProcess::ExitStatus exitStatus) {
    Debug("Process finished");
    
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());

    // Find specifc value in process map i.e. process
    auto p = processIt(process);
    // p might be .end() if we caused the process to be killed which also removes it from
    // the _processes list immediately before the handleFinished function can be called
    if (p != _processes.end()) {
        Debug(fmt::format("Found process {}", p->processId));
        
        common::ProcessStatusMessage msg;
        msg.processId = p->processId;
        if (p->wasUserTerminated) {
            // If the user terminated the process it will report back an exitStatus of
            // 'CrashExit', which does not really convey the write reason to the user
            msg.status = common::ProcessStatusMessage::Status::NormalExit;
        }
        else {
            msg.status = toTrayStatus(exitStatus);
        }
        emit sendSocketMessage(msg);

        // Remove this process from the list as we consider it finished
        ProcessInfo info = *p;
        _processes.erase(p);
        emit closedProcess(info);
    }
}

void ProcessHandler::handleReadyReadStandardError() {
    Debug("Reading stderr message");
    
    QProcess* proc = qobject_cast<QProcess*>(QObject::sender());

    // Find specifc value in process map i.e. process
    auto p = processIt(proc);
    assert(p != _processes.end());
    if (p != _processes.end()) {
        Debug(fmt::format("Found process {}", p->processId));
        
        // Send out the TrayProcessLogMessage with the stderror key
        common::ProcessOutputMessage msg;
        msg.processId = p->processId;
        msg.outputType = common::ProcessOutputMessage::OutputType::StdErr;
        msg.message =
            QString::fromLatin1(proc->readAllStandardError()).toLocal8Bit().constData();
        emit sendSocketMessage(msg);
    }
}

void ProcessHandler::handleReadyReadStandardOutput() {
    QProcess* proc = qobject_cast<QProcess*>(QObject::sender());

    // Find specifc value in process map i.e. process
    auto p = processIt(proc);
    assert(p != _processes.end());
    if (p != _processes.end()) {
        common::ProcessOutputMessage msg;
        msg.processId = p->processId;
        msg.message =
            QString::fromLatin1(proc->readAllStandardOutput()).toLocal8Bit().constData();
        msg.outputType = common::ProcessOutputMessage::OutputType::StdOut;

        // We don't need to print every console message to the log of the tray application
        emit sendSocketMessage(msg, false);
    }
}

void ProcessHandler::executeProcessWithCommandMessage(QProcess* process,
                                               const common::StartCommandMessage& command)
{
    Debug("Executing process");

    // Send out the TrayProcessStatus with the status "Started"
    common::ProcessStatusMessage msg;
    msg.processId = command.id;
    msg.status = common::ProcessStatusMessage::Status::Starting;
    emit sendSocketMessage(msg);

    if (!command.workingDirectory.empty()) {
        process->setWorkingDirectory(QString::fromStdString(command.workingDirectory));
    }
    else {
        std::filesystem::path executablePath = std::filesystem::path(command.executable);
        std::string workingDirectory = executablePath.parent_path().string();
        process->setWorkingDirectory(QString::fromStdString(workingDirectory));
    }

    if (command.commandlineParameters.empty()) {
        std::string cmd = fmt::format("\"{}\"", command.executable);
        process->start(QString::fromStdString(cmd));
    }
    else {
        std::string cmd = fmt::format(
            "\"{}\" {}", command.executable, command.commandlineParameters
        );
        process->start(QString::fromStdString(cmd));
    }

    // If the executable does not exist, the process might still be in the NotRunning
    // state. It also will have already triggered the `errorOccurred` message by that time
    Debug(fmt::format("State: {}", static_cast<int>(process->state())));
    if (process->state() != QProcess::ProcessState::NotRunning) {
        process->waitForStarted();
        const auto p = processIt(process);
        assert(p != _processes.end());
        emit startedProcess(*p);
    }
}

void ProcessHandler::createAndRunProcessFromCommandMessage(
                                                   const common::StartCommandMessage& cmd)
{
    Debug("Starting process");
    
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
    else {
        // We are not interested in the output of the process
        proc->closeReadChannel(QProcess::ProcessChannel::StandardOutput);
        proc->closeReadChannel(QProcess::ProcessChannel::StandardError);
    }
    connect(proc, &QProcess::started, this, &ProcessHandler::handleStarted);

    // Insert command identifier and process into out lists
    ProcessInfo info;
    info.processId = cmd.id;
    info.process = proc;
    info.executable = cmd.executable;
    info.programId = cmd.programId;
    info.configurationId = cmd.configurationId;
    info.clusterId = cmd.clusterId;
    info.nodeId = cmd.nodeId;
    info.dataHash = cmd.dataHash;
    _processes.push_back(info);

    // Run the process with the command
    executeProcessWithCommandMessage(proc, cmd);
}

std::vector<ProcessHandler::ProcessInfo>::const_iterator ProcessHandler::processIt(
                                                                        QProcess* process)
{
    const auto p = std::find_if(
        _processes.cbegin(),
        _processes.cend(),
        [process](const ProcessInfo& proc) { return proc.process == process; }
    );
    return p;
}

std::vector<ProcessHandler::ProcessInfo>::const_iterator ProcessHandler::processIt(int id)
{
    const auto p = std::find_if(
        _processes.cbegin(),
        _processes.cend(),
        [id](const ProcessInfo& proc) { return proc.processId == id; }
    );
    return p;
}

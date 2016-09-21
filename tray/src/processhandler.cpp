#include "processhandler.h"
#include <QJsonDocument>

ProcessHandler::ProcessHandler() {}

ProcessHandler::~ProcessHandler() {}

void ProcessHandler::handleSocketMessage(std::string message){
    // Create new traycommand
    QJsonDocument messageDoc = QJsonDocument::fromJson(QByteArray::fromStdString(message));
    common::TrayCommand command(messageDoc);
    
    // Check if identifer of traycommand already is tied to a process
    // We don't allow the same identifier for multiple processes
    std::map<QString, QProcess*>::iterator p = _processes.find(command.identifier);
    if (p == _processes.end() ) {
        // Not Found
        // Save the command and run a process with it
        _commands.insert(std::pair<QString, common::TrayCommand>(command.identifier, command));
        createAndRunProcessFromTrayCommand(command);
    } else {
        // Found
        runProcessWithTrayCommand(p->second, command);
    }
    
}

void ProcessHandler::handlerErrorOccurred(QProcess::ProcessError error){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        std::string errorStr;
        bool sendError = true;
        switch (error) {
            case QProcess::FailedToStart:
                errorStr = "FailedToStart";
                break;
            case QProcess::Timedout:
                errorStr = "Timedout";
                break;
            case QProcess::WriteError:
                errorStr = "WriteError";
                break;
            case QProcess::ReadError:
                errorStr = "ReadError";
                break;
            case QProcess::UnknownError:
                errorStr = "UnknownError";
                break;
            default:
                sendError = false;
                break;
        }
        if(sendError) {
            // Send out the TrayProcessStatus with the error string
            emit sendSocketMessage(errorStr);
        }
    }
}

void ProcessHandler::handleStarted(){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        // Send out the TrayProcessStatus
        emit sendSocketMessage("Running");
    }
}

void ProcessHandler::handleFinished(int exitCode, QProcess::ExitStatus exitStatus){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        std::string exitStr;
        switch (exitStatus) {
            case QProcess::NormalExit:
                exitStr = "NormalExit";
                break;
            case QProcess::CrashExit:
                exitStr = "CrashExit";
                break;
            default:
                break;
        }
        // Send out the TrayProcessStatus with the error string
        emit sendSocketMessage(exitStr);
    }
}

void ProcessHandler::handleReadyReadStandardError(){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        // Send out the TrayProcessLogMessage with the stderror key
        emit sendSocketMessage("StdError");
    }
}

void ProcessHandler::handleReadyReadStandardOutput(){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        // Send out the TrayProcessLogMessage with the stdout key
        emit sendSocketMessage("StdOut");
    }
}

void ProcessHandler::runProcessWithTrayCommand(QProcess* process, const common::TrayCommand& command) {
    QStringList arguments;
    arguments << command.commandlineParameters;
    
    process->start(command.executable, arguments);
}

void ProcessHandler::createAndRunProcessFromTrayCommand(const common::TrayCommand& command) {
    QProcess* newProcess = new QProcess(this);
    
    // Connect all process signals for logging feedback to core
    QObject::connect(newProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
                     this, SLOT(handlerErrorOccurred(QProcess::ProcessError)));
    QObject::connect(newProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                     this, SLOT(handleFinished(int, QProcess::ExitStatus)));
    QObject::connect(newProcess, SIGNAL(readyReadStandardError()),
                     this, SLOT(handleReadyReadStandardError()));
    QObject::connect(newProcess, SIGNAL(readyReadStandardOutput()),
                     this, SLOT(handleReadyReadStandardOutput()));
    QObject::connect(newProcess, SIGNAL(started()),
                     this, SLOT(handleStarted()));
    
    // Run the process with the command
    runProcessWithTrayCommand(newProcess, command);

    
    // Insert command and process into out lists
    _processes.insert(std::pair<QString, QProcess*>(command.identifier, newProcess));
    _processIdToTrayId.insert(std::pair<qint64, QString>(newProcess->processId(), command.identifier));
}



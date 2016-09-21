#include "processhandler.h"

#include <genericmessage.h>
#include <trayprocesslogmessage.h>
#include <trayprocessstatus.h>

#include <QDebug>

#include <QJsonDocument>

ProcessHandler::ProcessHandler() {}

ProcessHandler::~ProcessHandler() {}

void ProcessHandler::handleSocketMessage(QString message) {
    // Create new traycommand
    QJsonDocument messageDoc = QJsonDocument::fromJson(message.toUtf8());
    common::TrayCommand command(messageDoc);
    
    qDebug() << "Received TrayCommand";
    qDebug() << "Executable: " << command.executable;
    qDebug() << "Identifier: " << command.identifier;
    
    
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

void ProcessHandler::handlerErrorOccurred(QProcess::ProcessError error) {
    qDebug() << "Error occurred: " << error;
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        common::TrayProcessStatus ps;
        ps.identifier = p2T->second;
        bool sendError = true;
        switch (error) {
            case QProcess::FailedToStart:
                ps.status = "FailedToStart";
                break;
            case QProcess::Timedout:
                ps.status = "Timedout";
                break;
            case QProcess::WriteError:
                ps.status = "WriteError";
                break;
            case QProcess::ReadError:
                ps.status = "ReadError";
                break;
            case QProcess::UnknownError:
                ps.status = "UnknownError";
                break;
            default:
                sendError = false;
                break;
        }
        if(sendError) {
            // Send out the TrayProcessStatus with the error/status string
            common::GenericMessage msg;
            msg.type = common::TrayProcessStatus::Type;
            msg.payload = ps.toJson().object();
            emit sendSocketMessage(msg.toJson().toJson());
        }
    }
}

void ProcessHandler::handleStarted(){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        // Send out the TrayProcessStatus with the status string
        common::TrayProcessStatus ps;
        ps.identifier = p2T->second;
        ps.status = "Running";
        common::GenericMessage msg;
        msg.type = common::TrayProcessStatus::Type;
        msg.payload = ps.toJson().object();
        emit sendSocketMessage(msg.toJson().toJson());
    }
}

void ProcessHandler::handleFinished(int exitCode, QProcess::ExitStatus exitStatus){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        common::TrayProcessStatus ps;
        ps.identifier = p2T->second;
        switch (exitStatus) {
            case QProcess::NormalExit:
                ps.status = "NormalExit";
                break;
            case QProcess::CrashExit:
                ps.status = "CrashExit";
                break;
            default:
                break;
        }
        // Send out the TrayProcessStatus with the error/status string
        common::GenericMessage msg;
        msg.type = common::TrayProcessStatus::Type;
        msg.payload = ps.toJson().object();
        emit sendSocketMessage(msg.toJson().toJson());
    }
}

void ProcessHandler::handleReadyReadStandardError(){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        // Send out the TrayProcessLogMessage with the stderror key
        common::TrayProcessLogMessage pm;
        pm.identifier = p2T->second;
        pm.stdOutLog = "";
        pm.stdErrorLog = QString::fromUtf8(process->readAllStandardError());
        common::GenericMessage msg;
        msg.type = common::TrayProcessLogMessage::Type;
        msg.payload = pm.toJson().object();
        emit sendSocketMessage(msg.toJson().toJson());
    }
}

void ProcessHandler::handleReadyReadStandardOutput(){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    std::map<qint64, QString>::iterator p2T = _processIdToTrayId.find(process->processId());
    if (p2T != _processIdToTrayId.end() ) {
        common::TrayProcessLogMessage pm;
        pm.identifier = p2T->second;
        pm.stdOutLog = QString::fromUtf8(process->readAllStandardOutput());
        pm.stdErrorLog = "";
        common::GenericMessage msg;
        msg.type = common::TrayProcessLogMessage::Type;
        msg.payload = pm.toJson().object();
        emit sendSocketMessage(msg.toJson().toJson());
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



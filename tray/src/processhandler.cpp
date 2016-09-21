#include "processhandler.h"
#include <QProcess>
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

void ProcessHandler::runProcessWithTrayCommand(QProcess* process, const common::TrayCommand& command) {
    QStringList arguments;
    arguments << command.commandlineParameters;
    
    process->start(command.executable, arguments);
}

void ProcessHandler::createAndRunProcessFromTrayCommand(const common::TrayCommand& command) {
    QProcess* newProcess = new QProcess(this);
    
    runProcessWithTrayCommand(newProcess, command);
    
    // Insert command and process into out lists
    _processes.insert(std::pair<QString, QProcess*>(command.identifier, newProcess));
}



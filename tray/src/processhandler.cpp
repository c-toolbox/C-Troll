#include "processhandler.h"
#include <QProcess>
#include <QJsonDocument>

ProcessHandler::ProcessHandler() {}

ProcessHandler::~ProcessHandler() {}

void ProcessHandler::createProcessFromTrayCommand(common::TrayCommand command) {    
    QStringList arguments;
    arguments << command.commandlineParameters;
    
    QProcess* newProcess = new QProcess(this);
    newProcess->start(command.executable, arguments);
    
    // Insert command and process into out lists
    _processes.insert(std::pair<QString, QProcess*>(command.identifier, newProcess));
    _commands.insert(std::pair<QString, common::TrayCommand>(command.identifier, command));
}

void ProcessHandler::handleMessage(std::string message){
    // Create new traycommand
    QJsonDocument messageDoc = QJsonDocument::fromJson(QByteArray::fromStdString(message));
    common::TrayCommand newTrayCommand(messageDoc);
    
    // Check if identifer of traycommand already exists
    if (_commands.find(newTrayCommand.identifier) == _commands.end() ) {
        // Not Found
    } else {
        // Found
    }
    
}

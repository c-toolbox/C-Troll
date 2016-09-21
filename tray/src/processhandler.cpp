#include <QProcess>
#include "processhandler.h"

ProcessHandler::ProcessHandler() {}

ProcessHandler::~ProcessHandler() {}

void ProcessHandler::createProcessFromTrayCommand(common::TrayCommand command) {
    QString id = "...";
    
    QStringList arguments;
    arguments << command.commandlineParameters;
    
    QProcess* newProcess = new QProcess(this);
    newProcess->start(command.executable, arguments);
    
    // Insert command and process into out lists
    _processes.insert(std::pair<QString, QProcess*>(id, newProcess));
    _commands.insert(std::pair<QString, common::TrayCommand>(id, command));
}
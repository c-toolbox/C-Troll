#include "processhandler.h"

#include <genericmessage.h>
#include <trayprocesslogmessage.h>
#include <trayprocessstatus.h>

#include <QDebug>

#include <QJsonDocument>

template<class T>
struct map_data_compare : public std::binary_function<typename T::value_type,
typename T::mapped_type,
bool>
{
public:
    bool operator() (typename T::value_type &pair,
                     typename T::mapped_type i) const
    {
        return pair.second == i;
    }
};

ProcessHandler::ProcessHandler() {}

ProcessHandler::~ProcessHandler() {}

void ProcessHandler::handleSocketMessage(QString message) {
    // Create new traycommand
    QJsonDocument messageDoc = QJsonDocument::fromJson(message.toUtf8());
    // qDebug() << messageDoc;
    common::TrayCommand command(messageDoc);
    
    qDebug() << "Received TrayCommand";
    qDebug() << "Command: " << command.command;
    qDebug() << "Identifier: " << command.identifier;
    qDebug() << "Executable: " << command.executable;
    qDebug() << "CommandLineParameters: " << command.commandlineParameters;
    qDebug() << "BaseDirectory: " << command.baseDirectory;
    qDebug() << "CurrentWorkingDirectory: " << command.currentWorkingDirectory;
    
    // Check if identifer of traycommand already is tied to a process
    // We don't allow the same identifier for multiple processes
    std::map<QString, QProcess*>::iterator p = _processes.find(command.identifier);
    if (p == _processes.end() ) {
        // Not Found, create and run a process with it
        createAndRunProcessFromTrayCommand(command);
    } else {
        // Found
        executeProcessWithTrayCommand(p->second, command);
    }
    
}

void ProcessHandler::handlerErrorOccurred(QProcess::ProcessError error) {
    qDebug() << "Error occurred: " << error;
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(_processes.begin(), _processes.end(), std::bind2nd(map_data_compare<ProcessMap>(), process) );
    
    if (p2T != _processes.end() ) {
        common::TrayProcessStatus ps;
        ps.identifier = p2T->first;
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
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(_processes.begin(), _processes.end(), std::bind2nd(map_data_compare<ProcessMap>(), process) );
    
    if (p2T != _processes.end() ) {
        // Send out the TrayProcessStatus with the status string
        common::TrayProcessStatus ps;
        ps.identifier = p2T->first;
        ps.status = "Running";
        common::GenericMessage msg;
        msg.type = common::TrayProcessStatus::Type;
        msg.payload = ps.toJson().object();
        emit sendSocketMessage(msg.toJson().toJson());
    }
}

void ProcessHandler::handleFinished(int exitCode){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    handleFinished(exitCode, process->exitStatus());
}

void ProcessHandler::handleFinished(int exitCode, QProcess::ExitStatus exitStatus){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(_processes.begin(), _processes.end(), std::bind2nd(map_data_compare<ProcessMap>(), process) );
    
    if (p2T != _processes.end() ) {
        common::TrayProcessStatus ps;
        ps.identifier = p2T->first;
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
        
        // Remove this process from the list as we consider it finsihed
        _processes.erase(p2T);
    }
}

void ProcessHandler::handleReadyReadStandardError(){
    QProcess* process = qobject_cast<QProcess*>(QObject::sender());
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(_processes.begin(), _processes.end(), std::bind2nd(map_data_compare<ProcessMap>(), process) );
    
    if (p2T != _processes.end() ) {
        // Send out the TrayProcessLogMessage with the stderror key
        common::TrayProcessLogMessage pm;
        pm.identifier = p2T->first;
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
    
    // Find specifc value in process map i.e. process
    ProcessMap::iterator p2T = std::find_if(_processes.begin(), _processes.end(), std::bind2nd(map_data_compare<ProcessMap>(), process) );
    
    if (p2T != _processes.end() ) {
        common::TrayProcessLogMessage pm;
        pm.identifier = p2T->first;
        pm.stdOutLog = QString::fromUtf8(process->readAllStandardOutput());
        pm.stdErrorLog = "";
        common::GenericMessage msg;
        msg.type = common::TrayProcessLogMessage::Type;
        msg.payload = pm.toJson().object();
        emit sendSocketMessage(msg.toJson().toJson());
    }
}

void ProcessHandler::executeProcessWithTrayCommand(QProcess* process, const common::TrayCommand& command) {    
    if(command.command == "start"){
        if(!command.currentWorkingDirectory.isEmpty()){
            process->setWorkingDirectory(command.currentWorkingDirectory);
        }
        else if(!command.baseDirectory.isEmpty()){
            process->setWorkingDirectory(command.baseDirectory);
        }
        
        if(command.commandlineParameters.isEmpty()){
            process->start(command.executable);
        }
        else {
            process->start(command.executable + " " + command.commandlineParameters);
        }
    }
    else if(command.command == "kill"){
        process->kill();
    }
    else if(command.command == "exit"){
        process->terminate();
    }
}

void ProcessHandler::createAndRunProcessFromTrayCommand(const common::TrayCommand& command) {
    QProcess* newProcess = new QProcess(this);
    
    // Connect all process signals for logging feedback to core
    QObject::connect(newProcess, SIGNAL(errorOccurred(QProcess::ProcessError)),
                     this, SLOT(handlerErrorOccurred(QProcess::ProcessError)));
    QObject::connect(newProcess, SIGNAL(finished(int)),
                     this, SLOT(handleFinished(int)));
    QObject::connect(newProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                     this, SLOT(handleFinished(int, QProcess::ExitStatus)));
    QObject::connect(newProcess, SIGNAL(readyReadStandardError()),
                     this, SLOT(handleReadyReadStandardError()));
    QObject::connect(newProcess, SIGNAL(readyReadStandardOutput()),
                     this, SLOT(handleReadyReadStandardOutput()));
    QObject::connect(newProcess, SIGNAL(started()),
                     this, SLOT(handleStarted()));
    
    // Insert command identifier and process into out lists
    _processes.insert(std::pair<QString, QProcess*>(command.identifier, newProcess));
    
    // Run the process with the command
    executeProcessWithTrayCommand(newProcess, command);
}



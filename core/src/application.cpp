#include "application.h"

#include "command.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QObject>
#include <QThread>

#include <assert.h>

namespace {
    const QString KeyApplicationPath = "applicationPath";
    const QString KeyClusterPath = "clusterPath";
    const QString KeyListeningPort = "listeningPort";
}

Application::Application(QString configurationFile) {
    QFile f(configurationFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject jsonObject = d.object();

    QString programPath = jsonObject.value("applicationPath").toString();
    QString clusterPath = jsonObject.value("clusterPath").toString();
    int listeningPort = jsonObject.value("listeningPort").toInt();

    // Load all program descriptions from the path provided by the configuration file
    _programs = loadProgramsFromDirectory(programPath);
    // Load all cluster descriptions from the path provided by the configuration file
    _clusters = loadClustersFromDirectory(clusterPath);

    // The incoming socket handler takes care of messages from the GUI
    _incomingSocketHandler.initialize(listeningPort);
    
    // The outgoing socket handler takes care of messages to the Tray
    _outgoingSocketHandler.initialize(_clusters);

    connect(
        &_incomingSocketHandler, &IncomingSocketHandler::messageReceived,
        this, &Application::incomingMessage
    );
    
    connect(
            &_incomingSocketHandler, &IncomingSocketHandler::newConnectionEstablished,
            this, &Application::sendInitializationInformation
    );
}

void Application::handleIncomingCommand(CoreCommand cmd) {
    qDebug() << "\tCommand: " << cmd.command;
    qDebug() << "\tApplication: " << cmd.applicationId;
    qDebug() << "\tConfiguration: " << cmd.configurationId;
    qDebug() << "\tCluster: " << cmd.clusterId;
    
    if (cmd.command == "Start") {
        // Find the correct program to start
        auto iProgram = std::find_if(
            _programs.begin(),
            _programs.end(),
             [&](const Program& p) {
                 return p.id() == cmd.application;
             }
        );
        
        if (iProgram == _programs.end() {
            // We didn't find the program you were looking for
            // TODO(alex): Signal this back to the GUI
            qDebug() << "Could not find application id '" << cmd.Application << "'";
            return;
        }
    
        // Get the correct Cluster
        auto iCluster = std::find_if(
            _clusters.begin(),
            _clusters.end(),
            [&](const Cluster& c) {
                return c.name() == cmd.cluster;
            }
        );
            
        if (iCluster == _clusters.end() {
            // We didn't find the cluster you were looking for
            // TODO(alex): Signal this back to the GUI
            qDebug() << "Could not find cluster id '" << cmd.cluster << "'";
            return;
        }
        
        bool clusterIsValidForApplication =
            iProgram->clusters().empty() ||
            std::find_if(
                iProgram->clusters().begin(),
                iProgram->clusters().end(),
                [&](const QString& s) { return iCluster->name() == s; }
             ) != iProgram->clusters().end();

        if (!clusterIsValidForApplication) {
            // We tried to start an application on a cluster for which the application
            // is not configured
            // TODO(alex): Signal this back to the GUI
            qDebug() << "Application id '" << cmd.application << "' cannot be "
                << "started on cluster id '" << cmd.cluster << "'";
            return;
        }
            
        // Get the correct configuration, if it exists
        if (cmd.configuration.isEmpty()) {
            sendMessage(*iCluster, programToTrayCommand(*iProgram));
        }
        else {
            auto iConfiguration = std::find_if(
                iProgram->configurations().begin(),
                iProgram->configurations().end(),
                [&](const Program::Configuration& c) {
                   return c.identifier == cmd.configuration;
                }
            );
            
            if (iConfiguration == iProgram->configurations().end() {
                // The requested configuration does not exist for the application
                // TODO(alex): Signal this back to the GUI
                qDebug() << "The configuration '" << cmd.configuration << "' does "
                << "not exist for the application id '" << cmd.application << "'";
                return;
                
            }
            sendMessage(
                *iCluster,
                programToTrayCommand(*iProgram, iConfiguration->commandlineParameters)
            );
        }
    }
}

void Application::incomingMessage(QString message) {
    // The message contains a JSON object of the GenericMessage
    GenericMessage msg(QJsonDocument(message));
    
    qDebug() << "Received message of type '" << msg.type << "'";

    if (msg.type == CoreCommand::Type) {
        // We have received a message from the GUI to start a new application
        handleIncomingCommand(CoreCommand(msg.payload));
    }
}
                
void Application::sendInitializationInformation(QTcpSocket* socket) {
    common::GenericMessage msg;
    msg.type = commmon::GuiInitialization::Type;
    
    common::GuiInitialization initMsg;
    for (const Program& p : _programs) {
        initMsg.applications.push_back(programToGuiInitializationApplication(p));
    }
    
    for (const Cluster& c : _clusters) {
        initMsg.clusters.push_back(c.name());
    }
    
    msg.payload = initMsg;
    _incomingSocketHandler.sendMessage(socket, msg);
}

void Application::sendMessage(const Cluster& cluster, common::TrayCommand command) {
    // We have to wrap the TrayCommand into a GenericMessage first
    common::GenericMessage msg;
    msg.type = common::TrayCommand::Type;
    msg.payload = command.toJson();
    
    _outgoingSocketHandler.sendMessage(cluster, msg.toJson().toJson());
}

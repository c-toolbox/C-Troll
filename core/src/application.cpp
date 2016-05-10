#include "application.h"

#include "command.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QObject>
#include <QThread>

#include <assert.h>

Application::Application(QString configurationFile) {
    QFile f(configurationFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject jsonObject = d.object();

    QString programPath = jsonObject.value("applicationPath").toString();
    QString clusterPath = jsonObject.value("clusterPath").toString();
    int listeningPort = jsonObject.value("listeningPort").toInt();

    _programHandler.loadFromDirectory(programPath);
    _clusterHandler.loadFromDirectory(clusterPath);
    _incomingSocketHandler.initialize(listeningPort);

    connect(
        &_incomingSocketHandler, &IncomingSocketHandler::messageReceived,
        this, &Application::incomingMessage
    );

}

void Application::incomingMessage(QString message) {
    //Command cmd(message);
    // This is for testing purposes ---abock
    Command cmd;
    cmd.application = "identifier2";
    cmd.cluster = "Office Computer Alex";
    cmd.configuration = "";

    // Get the correct program
    auto iProgram = std::find_if(
        _programHandler.programs().begin(),
        _programHandler.programs().end(),
        [&](const Program& p) {
        return p.id() == cmd.application;
    }
    );
    // At the moment, we just crash if we can't find the program
    assert(iProgram != _programHandler.programs().end());

    // Get the correct Cluster
    auto iCluster = std::find_if(
        _clusterHandler.clusters().begin(),
        _clusterHandler.clusters().end(),
        [&](const Cluster& c) {
        return c.name() == cmd.cluster;
    }
    );
    // At the moment, we just crash if we can't find the cluster
    assert(iCluster != _clusterHandler.clusters().end());

    // Check if the program is valid for the selected cluster
    // At the moment, we just crash if the cluster is not valid
    assert(iProgram->clusters().empty() ||
           std::find_if(
               iProgram->clusters().begin(),
               iProgram->clusters().end(),
               [&](const QString& s) { return iCluster->name() == s; }
           ) != iProgram->clusters().end()
    );

    // Get the correct configuration, if it exists
    if (cmd.configuration.isEmpty()) {
        sendMessage(TrayCommand(*iProgram), *iCluster);
    }
    else {
        auto iConfiguration = std::find_if(
            iProgram->configurations().begin(),
            iProgram->configurations().end(),
            [&](const Program::Configuration& c) {
                return c.identifier == cmd.configuration;
            }
        );
        // At the moment, we just crash if the configuration does not exist
        assert(iConfiguration != iProgram->configurations().end());

        sendMessage(TrayCommand(*iProgram, iConfiguration->commandlineParameters), *iCluster);
    }
    
}

void Application::sendMessage(TrayCommand command, const Cluster& cluster) {
    for (const Cluster::Node& node : cluster.nodes()) {
        QTcpSocket socket;
        socket.connectToHost(node.ipAddress, node.port);
        bool success = socket.waitForConnected();
        QString message = command.json();
        socket.write(message.toUtf8());

        socket.flush();
    }
}

#include <QCoreApplication>

#include <QTCPSocket>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include <QDebug>

#include <cassert>

#include "application/application.h"
#include "application/applicationhandler.h"

#include "cluster/clusterhandler.h"

namespace {
    const QString HostAddress = "10.7.38.89";
    const int Port = 5000;
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    
    ApplicationHandler appHandler;
    appHandler.loadFromDirectory("/Users/alex/Development/C-Troll/example/application");
    
    for (Application a : appHandler.applications())
        qDebug() << a;
    
    ClusterHandler cluHandler;
    cluHandler.loadFromDirectory("/Users/alex/Development/C-Troll/example/cluster");
    
    for (Cluster c : cluHandler.clusters())
        qDebug() << c;
    
    
    QTcpSocket socket;
    socket.connectToHost(HostAddress, Port);
    
    bool success = socket.waitForConnected();
    std::cout << success << std::endl;
    
    QString s("&Fooar");
    socket.write(s.toUtf8());
    
    socket.close();
    
    app.exec();
}
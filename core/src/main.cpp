#include <QCoreApplication>
#include <iostream>
#include <QFileInfo>
#include <QTCPSocket>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QDir>
#include <QJsonObject>
#include <iostream>
#include <QDebug>
#include <QThread>
#include <cassert>

#include "program/program.h"
#include "handler/mainhandler.h"
#include "configuration.h"

#define DEVELOP

namespace {
    const QString HostAddress = "130.236.145.183";
    const int Port = 5000;
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    
//    qDebug() << QFileInfo(".").absolutePath();
//    qDebug() << QFileInfo("../..").absolutePath();
//    qDebug() << QFileInfo("../../example").absolutePath();

#ifdef DEVELOP
    QDir current = QDir::current();
#ifndef WIN32
    current.cdUp();
#endif
    current.cdUp();
    current.cdUp();
    current.cd("example");
    QDir::setCurrent(current.absolutePath());
#endif
    
    
    // Load configuration file
    QString configurationFile = QDir::current().relativeFilePath("config.json");
    qDebug() << configurationFile;
    if (argc == 2) {
        configurationFile = QString::fromLatin1(argv[1]);
    }
    
    if (!QFileInfo(configurationFile).exists()) {
        std::cerr << "Could not find configuration file '" <<
            QFileInfo(configurationFile).absolutePath().toStdString() << "'" << std::endl;
        exit(EXIT_FAILURE);
    }



    
    Configuration config(configurationFile);
    qDebug() << "Application Path: " << config.applicationPath;
    qDebug() << "Cluster Path: " << config.clusterPath;
    
    ProgramHandler appHandler;
    appHandler.loadFromDirectory(config.applicationPath);
    
    //for (Application a : appHandler.applications())
    //    qDebug() << a;
    
    ClusterHandler clusterHandler;
    clusterHandler.loadFromDirectory(config.clusterPath);
    
    //for (Cluster c : cluHandler.clusters())
    //    qDebug() << c;
    
    
    QTcpSocket socket;
    socket.connectToHost(HostAddress, Port);
    
    bool success = socket.waitForConnected();
    std::cout << success << std::endl;
    
    QString s("&Fooar");
    socket.write(s.toUtf8());
    
    QObject().thread()->usleep(1000*1000*2);
    
    bool bb = socket.waitForReadyRead();
    QByteArray b = socket.readAll();
    qDebug() << QString::fromLatin1(b);
    
    socket.close();
    
    app.exec();
}
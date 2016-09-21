#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include <iostream>

#include "application.h"
#include <logging.h>

#define DEVELOP

int main(int argc, char** argv) {
    QCoreApplication application(argc, argv);
    
    common::Log::initialize("core");
    
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
    if (argc == 2) {
        configurationFile = QString::fromLatin1(argv[1]);
    }
    
    if (!QFileInfo(configurationFile).exists()) {
        std::cerr << "Could not find configuration file '" <<
            QFileInfo(configurationFile).absolutePath().toStdString() << "'" << std::endl;
        exit(EXIT_FAILURE);
    }

    Application app(configurationFile);
    
    //QTcpSocket socket;
    //socket.connectToHost(HostAddress, Port);
    //
    //bool success = socket.waitForConnected();
    //std::cout << success << std::endl;
    //
    //QString s("&Fooar");
    //socket.write(s.toUtf8());
    //
    //QObject().thread()->usleep(1000*1000*2);
    //
    //bool bb = socket.waitForReadyRead();
    //QByteArray b = socket.readAll();
    //qDebug() << QString::fromLatin1(b);
    //
    //socket.close();
    
    application.exec();
}

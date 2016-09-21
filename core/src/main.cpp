#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

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
        // If someone passed us a second argument, we assume this to be the configuration
        configurationFile = QString::fromLatin1(argv[1]);
    }

    QFileInfo info(configurationFile);
    if (!info.exists()) {
        Log(
            "Could not find configuration file " + info.absolutePath()
        );
        exit(EXIT_FAILURE);
    }

    Application app(configurationFile);
    application.exec();
}

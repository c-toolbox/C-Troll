#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QDebug>
#include <QJsonObject>
#include <QList>
#include <QString>

class Application {
public:
    Application(const QJsonObject& jsonObject);
    
    struct Configuration {
        QString identifier;
        QString commandlineParameters;
    };

private:
    friend QDebug operator<<(QDebug debug, const Application& application);

    QString id; // Application ID (name of the file + directory)
    QString name; // Application Name
    QString executable; // Executable name
    QString baseDirectory; // Base Directory
    bool fileSynchronization; // File synchronization
    QString commandlineParameters; // General Comandline Parameters
    QString currentWorkingDirectory; // Current working directory
    QList<QString> tags; // Tags

    QList<Configuration> configurations; // List of all configurations
};

QDebug operator<<(QDebug debug, const Application& application);

QDebug operator<<(QDebug debug, const Application::Configuration& configuration);

#endif // __APPLICATION_H__


#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <QDebug>
#include <QJsonObject>
#include <QList>
#include <QString>

class Program {
public:
    Program(const QJsonObject& jsonObject);
    
    struct Configuration {
        QString identifier;
        QString commandlineParameters;
    };

private:
    friend QDebug operator<<(QDebug debug, const Program& application);

    QString id; // Program ID (name of the file + directory)
    QString name; // Program Name
    QString executable; // Executable name
    QString baseDirectory; // Base Directory
    bool fileSynchronization; // File synchronization
    QString commandlineParameters; // General Comandline Parameters
    QString currentWorkingDirectory; // Current working directory
    QList<QString> tags; // Tags

    QList<Configuration> configurations; // List of all configurations
};

QDebug operator<<(QDebug debug, const Program& application);

QDebug operator<<(QDebug debug, const Program::Configuration& configuration);

#endif // __PROGRAM_H__

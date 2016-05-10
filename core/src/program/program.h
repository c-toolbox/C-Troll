#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <QDebug>
#include <QJsonObject>
#include <QList>
#include <QString>

#include "traycommand.h"

class Program {
public:
    Program(const QJsonObject& jsonObject);
    
    struct Configuration {
        QString identifier;
        QString commandlineParameters;
    };

    const QString& id() const;
    const QString& name() const;
    const QString& executable() const;
    const QString& baseDirectory() const;
    bool fileSynchronization() const;
    const QString& commandlineParameters() const;
    const QString& currentWorkingDirectory() const;
    const QList<QString>& tags() const;
    const QList<QString>& clusters() const;
    const QList<Configuration>& configurations() const;

private:
    friend QDebug operator<<(QDebug debug, const Program& application);

    QString _id; // Program ID (name of the file + directory)
    QString _name; // Program Name
    QString _executable; // Executable name
    QString _baseDirectory; // Base Directory
    bool _fileSynchronization; // File synchronization
    QString _commandlineParameters; // General Comandline Parameters
    QString _currentWorkingDirectory; // Current working directory
    QList<QString> _tags; // Tags

    QList<QString> _clusters; // Supported clusters

    QList<Configuration> _configurations; // List of all configurations
};

QDebug operator<<(QDebug debug, const Program& application);
QDebug operator<<(QDebug debug, const Program::Configuration& configuration);

TrayCommand programToTrayCommand(const Program& program, QString configuration = "");

#endif // __PROGRAM_H__

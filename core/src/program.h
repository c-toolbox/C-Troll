#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <QDebug>
#include <QJsonObject>
#include <QList>
#include <QString>

#include "traycommand.h"

class Program {
public:
    Program() = default;
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

TrayCommand programToTrayCommand(const Program& program, QString configuration = "");

using Programs = QVector<Program>;

Programs loadProgramsFromDirectory(QString directory);

#endif // __PROGRAM_H__

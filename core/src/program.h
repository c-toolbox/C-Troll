#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <QJsonObject>
#include <QList>
#include <QString>
#include <QVector>

#include "guiinitialization.h"
#include "traycommand.h"

class Program {
public:
    Program() = default;
    Program(const QJsonObject& jsonObject);
    
    struct Configuration {
        QString name;
        QString identifier;
        QString commandlineParameters;
    };

    QString id() const;
    QString name() const;
    QString executable() const;
    QString baseDirectory() const;
    QString commandlineParameters() const;
    QString currentWorkingDirectory() const;
    QStringList tags() const;
    QStringList clusters() const;
    QList<Configuration> configurations() const;

private:
    QString _id; // Program ID (name of the file + directory)
    QString _name; // Program Name
    QString _executable; // Executable name
    QString _baseDirectory; // Base Directory
    QString _commandlineParameters; // General Comandline Parameters
    QString _currentWorkingDirectory; // Current working directory
    QList<QString> _tags; // Tags

    QList<QString> _clusters; // Supported clusters

    QList<Configuration> _configurations; // List of all configurations
};

common::TrayCommand programToTrayCommand(const Program& program, QString configuration = "");

common::GuiInitialization::Application programToGuiInitializationApplication(const Program& program);

using Programs = QVector<Program>;

Programs loadProgramsFromDirectory(QString directory);

#endif // __PROGRAM_H__

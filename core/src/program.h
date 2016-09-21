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

    /**
     * Returns the unique identifier for this Program.
     * \return The unique identifier for this Program
     */
    QString id() const;
    
    /**
     * Returns the human readable name for this Program.
     * \return The human readable name for this Program
     */
    QString name() const;
    
    /**
     * Returns the full path to the executable for this Program.
     * \return The full path to the executable for this Program
     */
    QString executable() const;
    
    /**
     * Returns the base directory for this Program.
     * \return The base directory for this Program
     */
    QString baseDirectory() const;
    
    /**
     * Returns the commandline parameters for this Program.
     * \return The commandline parameters for this Program
     */
    QString commandlineParameters() const;
    
    /**
     * Returns the current working directory from which the application will be 
     * started.
     * \return The application's working directory
     */
    QString currentWorkingDirectory() const;
    
    /**
     * Returns a (potentially empty) list of relevant tags for this application.
     * \return A (potentially empty) list of relevant tags for this application.
     */
    QStringList tags() const;
    
    /**
     * Returns a list of clusters on which this application is available. This list
     * might be empty, which corresponds to no limitation as to where the Program can
     * be executed.
     * \return A list of clusters on which this application is available
     */
    QStringList clusters() const;
    
    /**
     * Returns A list of (potentially empty) configurations for this Program. A 
     * Configuration is a set of extra commandline arguments that can toggle parts of 
     * the program as the configurator desires.
     * \return A list of (potentially empty) configuratins for this Program
     */
    QList<Configuration> configurations() const;

private:
    /// A unique identifier
    QString _id;
    /// A human readable name for this Program
    QString _name;
    /// The full path to the executable
    QString _executable;
    /// The base directory of the application
    QString _baseDirectory;
    /// A fixed set of commandline parameters
    QString _commandlineParameters;
    /// The current working directory from which the Program is started
    QString _currentWorkingDirectory;
    /// A list of tags that are associated with this Program
    QList<QString> _tags;

    QList<QString> _clusters; // Supported clusters

    QList<Configuration> _configurations; // List of all configurations
};

common::TrayCommand programToTrayCommand(const Program& program, QString configuration = "");

common::GuiInitialization::Application programToGuiInitializationApplication(const Program& program);

using Programs = QVector<Program>;

Programs loadProgramsFromDirectory(QString directory);

#endif // __PROGRAM_H__

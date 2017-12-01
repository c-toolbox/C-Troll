/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 * of conditions and the following disclaimer.                                           *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 * list of conditions and the following disclaimer in the documentation and/or other     *
 * materials provided with the distribution.                                             *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 * used to endorse or promote products derived from this software without specific prior *
 * written permission.                                                                   *
 *                                                                                       *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY   *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT   *
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,        *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  *
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR    *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      *
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    *
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                                               *
 *                                                                                       *
 ****************************************************************************************/

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <QJsonObject>
#include <QList>
#include <QString>
#include <QVector>
#include <QMap>

#include <memory>

#include "guiinitialization.h"
#include "traycommand.h"

class Process;

class Program {
public:
    Program() = default;
    Program(const QJsonObject& jsonObject);
    ~Program();
    
    struct Configuration {
        QString id;
        QString name;
        QMap<QString, QString> clusterCommanlineParameters;
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
     * Returns A list of (potentially empty) configurations for this Program. A 
     * Configuration is a set of extra commandline arguments that can toggle parts of 
     * the program as the configurator desires.
     * \return A list of (potentially empty) configuratins for this Program
     */
    QList<Configuration> configurations() const;

    common::GuiInitialization::Application toGuiInitializationApplication() const;

    /**
    * Return a JSON string that represents this program.
    */
    QJsonObject toJson() const;

    /**
    * Return a unique hash that represents this program configuration.
    */
    QByteArray hash() const;

    static std::unique_ptr<std::vector<std::unique_ptr<Program>>> loadProgramsFromDirectory(QString directory);

    static std::unique_ptr<Program> loadProgram(QString jsonFile, QString baseDirectory);

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
    // List of all configurations
    QList<Configuration> _configurations;
    // Default configuration id
    QString _defaultConfiguration;
    // Default cluster id
    QString _defaultCluster;
    /// A vector of processes that derive from this program.
    QVector<Process*> _processes;
};


#endif // __PROGRAM_H__

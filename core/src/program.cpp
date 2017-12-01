/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
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

#include "program.h"

#include <QDebug>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonParseError>
#include <QCryptographicHash>

#include <cassert>
#include <logging.h>

#include "jsonsupport.h"

namespace {
    const QString KeyId = "id";
    const QString KeyName = "name";
    const QString KeyExecutable = "executable";
    const QString KeyBaseDirectory = "baseDirectory";
    const QString KeyCommandlineParameters = "commandlineParameters";
    const QString KeyCurrentWorkingDirectory = "currentWorkingDirectory";
    const QString KeyClusterCommandlineParameters = "clusters";
    const QString KeyTags = "tags";
    const QString KeyConfigurations = "configurations";

    const QString KeyConfigurationName = "name";
    const QString KeyConfigurationIdentifier = "identifier";
    const QString KeyConfigurationParameters = "commandlineParameters";
}

std::unique_ptr<Program> Program::loadProgram(QString jsonFile, QString baseDirectory) {
    QString identifier = QDir(baseDirectory).relativeFilePath(jsonFile);
    
    // relativeFilePath will have the baseDirectory in the beginning of the relative path
    // and we want to remove it:  baseDirectory.length() + 1
    // then, we want to remove the extension of 5 characters (.json)
    // So we take the middle part of the string:
    identifier = identifier.mid(
        // length of the base directory + '/'
        baseDirectory.length() + 1,
        // total length - (stuff we removed in the beginning) - length('.json')
        identifier.size() - (baseDirectory.length() + 1) - 5
    );

    QFile f(jsonFile);
    f.open(QFile::ReadOnly);

    QJsonParseError err;
    QJsonDocument d = QJsonDocument::fromJson(f.readAll(), &err);

    if (d.isEmpty()) {
        throw std::runtime_error(
            std::to_string(err.offset) + ": " +
            err.errorString().toStdString()
        );
    }
    
    QJsonObject obj = d.object();
    obj["id"] = identifier;
    return std::make_unique<Program>(obj);
}

common::GuiInitialization::Application Program::toGuiInitializationApplication() const {
    common::GuiInitialization::Application app;
    app.name = name();
    app.id = id();
    app.tags = tags();

    for (const Program::Configuration& conf : configurations()) {
        common::GuiInitialization::Application::Configuration c;
        c.name = conf.name;
        c.id = conf.id;
        c.clusters = conf.clusterCommanlineParameters.keys();

        app.configurations.push_back(c);
    }
    
    return app;
}

std::unique_ptr<std::vector<std::unique_ptr<Program>>> Program::loadProgramsFromDirectory(QString directory) {
    std::unique_ptr<std::vector<std::unique_ptr<Program>>> programs
        = std::make_unique<std::vector<std::unique_ptr<Program>>>();
    // First, get all the *.json files from the directory and subdirectories
    QDirIterator it(
        directory,
        QStringList() << "*.json",
        QDir::Files,
        QDirIterator::Subdirectories
    );
    while (it.hasNext()) {
        QString file = it.next();
        Log("Loading application file " + file);
        try {
            std::unique_ptr<Program> program = loadProgram(file, directory);
            programs->push_back(std::move(program));
        } catch (const std::runtime_error& e) {
            Log("Failed to load application file " + file + ". " + e.what());
        }
    }
    return std::move(programs);
}

Program::Program(const QJsonObject& jsonObject) {
    _id = common::testAndReturnString(jsonObject, KeyId);
    _name = common::testAndReturnString(jsonObject, KeyName);
    _executable = common::testAndReturnString(jsonObject, KeyExecutable);
    _baseDirectory = common::testAndReturnString(
        jsonObject, KeyBaseDirectory, Optional::Yes
    );
    _commandlineParameters = common::testAndReturnString(
        jsonObject, KeyCommandlineParameters, Optional::Yes
    );
    _currentWorkingDirectory = common::testAndReturnString(
        jsonObject, KeyCurrentWorkingDirectory,
        Optional::Yes, _baseDirectory
    );
    
    _tags = common::testAndReturnStringList(jsonObject, KeyTags, Optional::Yes);
    
    QJsonObject configurationObject = common::testAndReturnObject(
        jsonObject, KeyConfigurations, Optional::Yes
    );

    for (auto it = configurationObject.begin();
        it != configurationObject.end();
        it++)
    {
        Configuration conf;
        conf.id = it.key();

        QJsonValueRef value = it.value();
        
        if (!value.isObject()) {
            continue;
        }
        QJsonObject obj = value.toObject();
        conf.name = common::testAndReturnString(obj, KeyConfigurationName);

        QJsonObject clusters = common::testAndReturnObject(obj, KeyClusterCommandlineParameters);

        for (QString key : clusters.keys()) {
            conf.clusterCommanlineParameters[key] = common::testAndReturnString(clusters, key);
        }

        _configurations.push_back(conf);
    }
}

Program::~Program() {
    assert(_processes.empty());
}

QString Program::id() const {
    return _id;
}

QString Program::name() const {
    return _name;
}

QString Program::executable() const {
    return _executable;
}

QString Program::baseDirectory() const {
    return _baseDirectory;
}

QString Program::commandlineParameters() const {
    return _commandlineParameters;
}

QString Program::currentWorkingDirectory() const {
    return _currentWorkingDirectory;
}

QStringList Program::tags() const {
    return _tags;
}

QList<Program::Configuration> Program::configurations() const {
    return _configurations;
}

QJsonObject Program::toJson() const {
    QJsonObject program;

    program[KeyId] = _id;
    program[KeyName] = _name;
    program[KeyExecutable] = _executable;
    if (_baseDirectory != "") {
        program[KeyBaseDirectory] = _baseDirectory;
    }
    if (_commandlineParameters != "") {
        program[KeyCommandlineParameters] = _commandlineParameters;
    }
    if (_currentWorkingDirectory != "") {
        program[KeyCurrentWorkingDirectory] = _currentWorkingDirectory;
    }

    if (_tags.size() != 0) {
        QJsonArray tags;
        for (const auto& tag : _tags) {
            tags.push_back(tag);
        }
        program[KeyTags] = tags;
    }
    
    if (_configurations.size() != 0) {
        QJsonObject configurations;
        for (const auto& configuration : _configurations) {
            QJsonObject configurationObject;
            configurationObject[KeyConfigurationName] = configuration.name;

            QJsonObject clusterParams;
            for (const auto& cluster : configuration.clusterCommanlineParameters.keys()) {
                QString params = configuration.clusterCommanlineParameters[cluster];
                clusterParams[cluster] = params;
            }
            configurationObject[KeyClusterCommandlineParameters] = clusterParams;

            configurations[configuration.id] = configurationObject;
        }
        program[KeyConfigurations] = configurations;
    }
    return program;
}

QByteArray Program::hash() const {
    QJsonDocument doc(toJson());
    QString input = doc.toJson();
    return QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha1);
}
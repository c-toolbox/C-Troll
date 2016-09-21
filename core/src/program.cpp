#include "program.h"

#include <QDebug>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonParseError>

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
    const QString KeyTags = "tags";
    const QString KeyClusters = "clusters";
    const QString KeyConfigurations = "configurations";

    const QString KeyConfigurationName = "name";
    const QString KeyConfigurationIdentifier = "identifier";
    const QString KeyConfigurationParameters = "commandlineParameters";
}

Program loadProgram(QString jsonFile, QString baseDirectory) {
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
    return Program(obj);
}


common::TrayCommand programToTrayCommand(const Program& program, QString configuration) {
    common::TrayCommand t;
    t.executable = program.executable();
    t.baseDirectory = program.baseDirectory();
    t.currentWorkingDirectory = program.currentWorkingDirectory();
    
    if (program.commandlineParameters().isEmpty() && configuration.isEmpty()) {
        t.commandlineParameters = "";
    }
    else {
        t.commandlineParameters = program.commandlineParameters() + " " + configuration;
    }

    return t;
}

common::GuiInitialization::Application programToGuiInitializationApplication(
    const Program& program)
{
    common::GuiInitialization::Application app;
    app.name = program.name();
    app.identifier = program.id();
    app.tags = program.tags();
    app.clusters = program.clusters();
    
    for (const Program::Configuration& conf : program.configurations()) {
        common::GuiInitialization::Application::Configuration c;
        c.name = conf.name;
        c.identifier = conf.identifier;
        app.configurations.push_back(c);
    }
    
    return app;
}

Programs loadProgramsFromDirectory(QString directory) {
    Programs programs;
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
        programs.push_back(loadProgram(file, directory));
    }
    return programs;
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
    _clusters = common::testAndReturnStringList(jsonObject, KeyClusters, Optional::Yes);
    
    QJsonArray configurationArray = common::testAndReturnArray(
        jsonObject, KeyConfigurations, Optional::Yes
    );
    _configurations.clear();
    for (const QJsonValue& v : configurationArray) {
        Configuration conf;
        QJsonObject obj = v.toObject();
        
        conf.name = common::testAndReturnString(obj, KeyConfigurationName);
        
        conf.identifier = common::testAndReturnString(
            obj, KeyConfigurationIdentifier
        );
        conf.commandlineParameters = common::testAndReturnString(
            obj, KeyConfigurationParameters
        );
        
        _configurations.push_back(conf);
    }
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

QStringList Program::clusters() const {
    return _clusters;
}

QList<Program::Configuration> Program::configurations() const {
    return _configurations;
}

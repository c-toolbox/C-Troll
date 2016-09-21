#include "program.h"

#include <QDirIterator>
#include <QJsonArray>
#include <cassert>

#include "jsonsupport.h"

namespace {
    const QString KeyId = "id";
    const QString KeyName = "name";
    const QString KeyExecutable = "executable";
    const QString KeyBaseDirectory = "baseDirectory";
    const QString KeyFileSynchronization = "fileSynchronization";
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
    QDir dir(baseDirectory);
    dir.cdUp();

    QString identifier = dir.relativeFilePath(jsonFile);
    // The extension is only 5 characters ('.json')
    // so we can just remove it
    identifier = identifier.left(identifier.size() - 5);

    QFile f(jsonFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject obj = d.object();
    obj["id"] = identifier;
    return Program(d.object());
}


common::TrayCommand programToTrayCommand(const Program& program, QString configuration) {
    common::TrayCommand t;
    t.executable = program.executable();
    t.baseDirectory = program.baseDirectory();
    t.commandlineParameters = program.commandlineParameters() + " " + configuration;

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
        Program a = loadProgram(it.next(), directory);
        programs.push_back(a);
    }
    return programs;
}

Program::Program(const QJsonObject& jsonObject) {
    _id = common::testAndReturnString(jsonObject, KeyId);
    _name = common::testAndReturnString(jsonObject, KeyName);
    _executable = common::testAndReturnString(jsonObject, KeyExecutable);
    _baseDirectory = common::testAndReturnString(jsonObject, KeyBaseDirectory);
    _fileSynchronization = common::testAndReturnBool(jsonObject, KeyFileSynchronization);
    _commandlineParameters = common::testAndReturnString(
        jsonObject, KeyCommandlineParameters
    );
    _currentWorkingDirectory = common::testAndReturnString(
        jsonObject, KeyCurrentWorkingDirectory
    );
    
    QJsonArray tagsArray = common::testAndReturnArray(jsonObject, KeyTags);
    _tags.clear();
    for (const QJsonValue& v : tagsArray) {
        _tags.push_back(v.toString());
    }

    QJsonValue v = jsonObject.value(KeyClusters);
    if (v.isArray()) {
        QJsonArray clusterArray = v.toArray();
        for (const QJsonValue& v : clusterArray) {
            _clusters.push_back(v.toString());
        }
    }
    
    QJsonArray configurationArray = common::testAndReturnArray(
        jsonObject, KeyConfigurations
    );
    _configurations.clear();
    for (const QJsonValue& v : configurationArray) {
        Configuration conf;
        QJsonObject obj = v.toObject();
        assert(obj.size() == 3);
        
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

const QString& Program::id() const {
    return _id;
}

const QString& Program::name() const {
    return _name;
}

const QString& Program::executable() const {
    return _executable;
}

const QString& Program::baseDirectory() const {
    return _baseDirectory;
}

bool Program::fileSynchronization() const {
    return _fileSynchronization;
}

const QString& Program::commandlineParameters() const {
    return _commandlineParameters;
}

const QString& Program::currentWorkingDirectory() const {
    return _currentWorkingDirectory;
}

const QStringList& Program::tags() const {
    return _tags;
}

const QStringList& Program::clusters() const {
    return _clusters;
}

const QList<Program::Configuration>& Program::configurations() const {
    return _configurations;
}

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


TrayCommand programToTrayCommand(const Program& program, QString configuration) {
    TrayCommand t;
    t.executable = program.executable();
    t.baseDirectory = program.baseDirectory();
    t.commandlineParameters = program.commandlineParameters() + " " + configuration;

    return t;
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
    _id = json::testAndReturnString(jsonObject, KeyId);
    _name = json::testAndReturnString(jsonObject, KeyName);
    _executable = json::testAndReturnString(jsonObject, KeyExecutable);
    _baseDirectory = json::testAndReturnString(jsonObject, KeyBaseDirectory);
    _fileSynchronization = json::testAndReturnBool(jsonObject, KeyFileSynchronization);
    _commandlineParameters = json::testAndReturnString(
        jsonObject, KeyCommandlineParameters
    );
    _currentWorkingDirectory = json::testAndReturnString(
        jsonObject, KeyCurrentWorkingDirectory
    );
    
    QJsonArray tagsArray = json::testAndReturnArray(jsonObject, KeyTags);
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
    
    QJsonArray configurationArray = json::testAndReturnArray(
        jsonObject, KeyConfigurations
    );
    _configurations.clear();
    for (const QJsonValue& v : configurationArray) {
        Configuration conf;
        QJsonObject obj = v.toObject();
        assert(obj.size() == 2);
        
        conf.identifier = json::testAndReturnString(
            obj, KeyConfigurationIdentifier
        );
        conf.commandlineParameters = json::testAndReturnString(
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

const QList<QString>& Program::tags() const {
    return _tags;
}

const QList<QString>& Program::clusters() const {
    return _clusters;
}

const QList<Program::Configuration>& Program::configurations() const {
    return _configurations;
}

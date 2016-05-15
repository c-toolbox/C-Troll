#include "program.h"

#include <QDirIterator>
#include <QJsonArray>
#include <cassert>


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
    // jsonObject.contains(...) -> bool
    _id = jsonObject.value("id").toString();
    _name = jsonObject.value("name").toString();
    _executable = jsonObject.value("executable").toString();
    _baseDirectory = jsonObject.value("baseDirectory").toString();
    _fileSynchronization = jsonObject.value("fileSynchronization").toBool();
    _commandlineParameters = jsonObject.value("commandlineParameters").toString();
    _currentWorkingDirectory = jsonObject.value("currentWorkingDirectory").toString();
    
    QJsonArray tagsArray = jsonObject.value("tags").toArray();
    _tags.clear();
    for (const QJsonValue& v : tagsArray) {
        _tags.push_back(v.toString());
    }

    QJsonValue v = jsonObject.value("cluster");
    if (v.isArray()) {
        QJsonArray clusterArray = v.toArray();
        for (const QJsonValue& v : clusterArray) {
            _clusters.push_back(v.toString());
        }
    }
    
    QJsonArray configurationArray = jsonObject.value("configurations").toArray();
    _configurations.clear();
    for (const QJsonValue& v : configurationArray) {
        Configuration conf;
        QJsonObject a = v.toObject();
        assert(a.size() == 2);
        
        conf.identifier = a.value("identifier").toString();
        conf.commandlineParameters = a.value("commandlineParamters").toString();
        
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

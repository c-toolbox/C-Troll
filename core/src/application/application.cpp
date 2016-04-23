#include "application.h"

#include <QJsonArray>
#include <cassert>
#include <iostream>

Application::Application(const QJsonObject& jsonObject) {
    // jsonObject.contains(...) -> bool
    id = jsonObject.value("id").toString();
    name = jsonObject.value("name").toString();
    executable = jsonObject.value("executable").toString();
    baseDirectory = jsonObject.value("baseDirectory").toString();
    fileSynchronization = jsonObject.value("fileSynchronization").toBool();
    commandlineParameters = jsonObject.value("commandlineParameters").toString();
    currentWorkingDirectory = jsonObject.value("currentWorkingDirectory").toString();
    
    QJsonArray tagsArray = jsonObject.value("tags").toArray();
    tags.clear();
    for (const QJsonValue& v : tagsArray) {
        tags.push_back(v.toString());
    }
    
    QJsonArray configurationArray = jsonObject.value("configurations").toArray();
    configurations.clear();
    for (const QJsonValue& v : configurationArray) {
        Configuration conf;
        QJsonObject a = v.toObject();
        assert(a.size() == 2);
        
        conf.identifier = a.value("identifier").toString();
        conf.commandlineParameters = a.value("commandlineParamters").toString();
        
        configurations.push_back(conf);
    }
}

QDebug operator<<(QDebug debug, const Application& application) {
    debug << "Application\n";
    debug << "===========\n";
    debug << "ID: " << application.id << "\n";
    debug << "Name: " << application.name << "\n";
    debug << "Executable: " << application.executable << "\n";
    debug << "Base Directory: " << application.baseDirectory << "\n";
    debug << "File Sync: " << application.fileSynchronization << "\n";
    debug << "Params: " << application.commandlineParameters << "\n";
    debug << "Working Dir: " << application.currentWorkingDirectory << "\n";
    debug << "Tags: " << application.tags << "\n";
    debug << "Configs: " << application.configurations << "\n";
    debug << "\n";
    return debug;
}

QDebug operator<<(QDebug debug, const Application::Configuration& configuration) {
    debug << "[";
    debug << configuration.identifier;
    debug << " , ";
    debug << configuration.commandlineParameters;
    debug << "]";
    return debug;
}

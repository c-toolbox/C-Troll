#include "application.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

Application::Application(QString configurationFile) {
    QFile f(configurationFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject jsonObject = d.object();

    QString programPath = jsonObject.value("applicationPath").toString();
    QString clusterPath = jsonObject.value("clusterPath").toString();

    _programHandler.loadFromDirectory(programPath);
    _clusterHandler.loadFromDirectory(clusterPath);
}

ProgramHandler& Application::programHandler() {
    return _programHandler;
}

ClusterHandler& Application::clusterHandler() {
    return _clusterHandler;
}

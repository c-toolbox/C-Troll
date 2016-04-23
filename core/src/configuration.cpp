#include "configuration.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

Configuration::Configuration(QString configurationFile) {
    QFile f(configurationFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject jsonObject = d.object();
    
    applicationPath = jsonObject.value("applicationPath").toString();
    clusterPath = jsonObject.value("clusterPath").toString();
}

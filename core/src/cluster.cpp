#include "cluster.h"

#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>
#include <cassert>

#include "jsonsupport.h"

namespace {
    const QString KeyName = "name";
    const QString KeyIdentifier = "id";
    const QString KeyEnabled = "enabled";
    const QString KeyNodes = "nodes";

    const QString KeyNodeName = "name";
    const QString KeyNodeIpAddress = "ip";
    const QString KeyNodePort = "port";
}

Cluster::Cluster(const QJsonObject& jsonObject) {
    _name = common::testAndReturnString(jsonObject, KeyName);
    _identifier = common::testAndReturnString(jsonObject, KeyIdentifier);
    _enabled = common::testAndReturnBool(jsonObject, KeyEnabled);
    
    QJsonArray nodesArray = common::testAndReturnArray(jsonObject, KeyNodes);
    _nodes.clear();
    for (const QJsonValue& v : nodesArray) {
        QJsonObject obj = v.toObject();
        assert(obj.size() == 3);

        QString name = common::testAndReturnString(obj, KeyNodeName);
        QString ipAddress = common::testAndReturnString(obj, KeyNodeIpAddress);
        int port = common::testAndReturnInt(obj, KeyNodePort);
        
        _nodes.push_back({ name, ipAddress, port });
    }
}

QString Cluster::name() const {
    return _name;
}

QString Cluster::identifier() const {
    return _identifier;
}

bool Cluster::enabled() const {
    return _enabled;
}

QList<Cluster::Node> Cluster::nodes() const {
    return _nodes;
}

Cluster loadCluster(QString jsonFile, QString baseDirectory) {
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
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject obj = d.object();
    obj[KeyIdentifier] = identifier;
    return Cluster(obj);
}

Clusters loadClustersFromDirectory(QString directory) {
    Clusters result;
    // First, get all the *.json files from the directory and subdirectories
    QDirIterator it(
        directory,
        QStringList() << "*.json",
        QDir::Files,
        QDirIterator::Subdirectories
    );
    while (it.hasNext()) {
        QString file = it.next();
        qDebug() << "Loading cluster file " << file;
        Cluster c = loadCluster(file, directory);
        result.push_back(c);
    }

    return result;
}

common::GuiInitialization::Cluster clusterToGuiInitializationCluster(Cluster c) {
    common::GuiInitialization::Cluster cluster;
    cluster.name = c.name();
    cluster.identifier = c.identifier();
    cluster.enabled = c.enabled();
    return cluster;
}


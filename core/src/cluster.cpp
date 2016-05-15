#include "cluster.h"

#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>
#include <cassert>

#include "jsonsupport.h"

namespace {
    const QString KeyName = "name";
    const QString KeyNodes = "nodes";

    const QString KeyNodeName = "name";
    const QString KeyNodeIpAddress = "ip";
    const QString KeyNodePort = "port";
}

Cluster::Cluster(const QJsonObject& jsonObject) {
    _name = common::testAndReturnString(jsonObject, KeyName);
    
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

const QString& Cluster::name() const {
    return _name;
}

const QList<Cluster::Node>& Cluster::nodes() const {
    return _nodes;
}

Cluster loadCluster(QString jsonFile) {
    QFile f(jsonFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject obj = d.object();
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
        Cluster c = loadCluster(it.next());
        result.push_back(c);
    }

    return result;
}
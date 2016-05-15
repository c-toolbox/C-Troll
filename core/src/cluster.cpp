#include "cluster.h"

#include <QDirIterator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVector>
#include <cassert>

Cluster::Cluster(const QJsonObject& jsonObject) {
    // jsonObject.contains(...) -> bool
    _name = jsonObject.value("name").toString();
    
    QJsonArray nodesArray = jsonObject.value("nodes").toArray();
    _nodes.clear();
    for (const QJsonValue& v : nodesArray) {
        QJsonObject a = v.toObject();
        assert(a.size() == 3);
        
        QString name = a.value("name").toString();
        QString ipAddress = a.value("ip").toString();
        int port = a.value("port").toInt();

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
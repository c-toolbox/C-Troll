#include "cluster/clusterhandler.h"

#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>

void ClusterHandler::loadFromDirectory(QString directory) {
    // First, get all the *.json files from the directory and subdirectories
    QDirIterator it(directory, QStringList() << "*.json", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        Cluster c = loadCluster(it.next());
        addCluster(c);
    }
    
}

void ClusterHandler::addCluster(Cluster cluster) {
    _clusters.push_back(std::move(cluster));
}

const QList<Cluster>& ClusterHandler::clusters() const {
    return _clusters;
}

Cluster ClusterHandler::loadCluster(QString jsonFile) {
    QFile f(jsonFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject obj = d.object();
    return Cluster(obj);
}

#include "cluster/cluster.h"

#include <QJsonArray>
#include <QJsonObject>
#include <cassert>

Cluster::Cluster(const QJsonObject& jsonObject) {
    // jsonObject.contains(...) -> bool
    _name = jsonObject.value("name").toString();
    
    QJsonArray nodesArray = jsonObject.value("nodes").toArray();
    _nodes.clear();
    for (const QJsonValue& v : nodesArray) {
        Node node;
        QJsonObject a = v.toObject();
        assert(a.size() == 3);
        
        node.name = a.value("name").toString();
        node.ipAddress = a.value("ip").toString();
        node.port = a.value("port").toInt();
        _nodes.push_back(node);
    }
}

const QString& Cluster::name() const {
    return _name;
}

const QList<Cluster::Node>& Cluster::nodes() const {
    return _nodes;
}

#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#include <QList>
#include <QJsonObject>
#include <QTcpSocket>

#include <memory>

class Cluster {
public:
    Cluster(const QJsonObject& jsonObject);
    
    struct Node {
        QString name;
        QString ipAddress;
        int port;
    };

    const QString& name() const;
    const QList<Node>& nodes() const;
    
private:
    QString _name;
    QList<Node> _nodes;
};

using Clusters = QList<Cluster>;

Clusters loadClustersFromDirectory(QString directory);

#endif // __CLUSTER_H__

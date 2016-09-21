#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#include "guiinitialization.h"

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

    QString name() const;
    QString identifier() const;
    QList<Node> nodes() const;
    
private:
    QString _name;
    QString _identifier;
    QList<Node> _nodes;
};

using Clusters = QList<Cluster>;

Clusters loadClustersFromDirectory(QString directory);

common::GuiInitialization::Cluster clusterToGuiInitializationCluster(Cluster cluster);

#endif // __CLUSTER_H__

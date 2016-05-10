#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#include <QDebug>
#include <QList>

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
    friend QDebug operator<<(QDebug debug, const Cluster& cluster);
    
    QString _name;
    QList<Node> _nodes;
};

#endif // __CLUSTER_H__

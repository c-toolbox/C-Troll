#ifndef __OUTGOINGSOCKETHANDLER_H__
#define __OUTGOINGSOCKETHANDLER_H__

#include <QObject>

#include <QMap>
#include <QTcpSocket>

#include <memory>

#include "cluster.h"

class OutgoingSocketHandler : public QObject {
Q_OBJECT
public:
    void initialize(const QList<Cluster>& clusters);

    void sendMessage(const Cluster& cluster, QString message) const;

private:
    using HashValue = QString;
    
    HashValue hash(const Cluster& cluster, const Cluster::Node& node) const;

    Clusters _clusters;
    std::map<HashValue, std::unique_ptr<QTcpSocket>> _sockets;
 
};

#endif // __OUTGOINGSOCKETHANDLER_H__

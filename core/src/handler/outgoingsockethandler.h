#ifndef __OUTGOINGSOCKETHANDLER_H__
#define __OUTGOINGSOCKETHANDLER_H__

#include <QObject>

#include "cluster.h"

#include <QMap>

#include <map>
#include <memory>

class QTcpSocket;

class OutgoingSocketHandler : public QObject {
Q_OBJECT
public:
    ~OutgoingSocketHandler();

    void initialize(const QList<Cluster>& clusters);

    void sendMessage(const Cluster& cluster, QString message) const;

private:
    using HashValue = QString;
    
    HashValue hash(const Cluster& cluster, const Cluster::Node& node) const;

    QList<Cluster> _clusters;
    std::map<HashValue, std::unique_ptr<QTcpSocket>> _sockets;
 
};

#endif // __OUTGOINGSOCKETHANDLER_H__

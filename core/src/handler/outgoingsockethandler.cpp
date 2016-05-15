#include "outgoingsockethandler.h"

#include <assert.h>


OutgoingSocketHandler::HashValue OutgoingSocketHandler::hash(
    const Cluster& cluster, const Cluster::Node& node) const
{
    return cluster.name() + "::" + node.name;
}


void OutgoingSocketHandler::initialize(const QList<Cluster>& clusters) {
    for (const Cluster& c : clusters) {
        for (const Cluster::Node& n : c.nodes()) {
            HashValue h = hash(c, n);

            std::unique_ptr<QTcpSocket> socket = std::make_unique<QTcpSocket>();
            socket->connectToHost(n.ipAddress, n.port);
            _sockets[h] = std::move(socket);
        }
    }
}

void OutgoingSocketHandler::sendMessage(const Cluster& cluster, QString message) const {
    for (const Cluster::Node& n : cluster.nodes()) {
        HashValue h = hash(cluster, n);

        auto it = _sockets.find(h);
        assert(it != _sockets.end());

        it->second->write(message.toUtf8());
        it->second->flush();
    }
}
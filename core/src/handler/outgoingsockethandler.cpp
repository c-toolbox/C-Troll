#include "outgoingsockethandler.h"

#include <assert.h>


OutgoingSocketHandler::HashValue OutgoingSocketHandler::hash(
    const Cluster& cluster, const Cluster::Node& node) const
{
    return cluster.name() + "::" + node.name;
}

void OutgoingSocketHandler::initialize(const QList<Cluster>& clusters) {
    for (const Cluster& c : clusters) {
        for (const Cluster::Node& node : c.nodes()) {
            HashValue h = hash(c, node);

            // This handler keeps the sockets to the tray applications open
            std::unique_ptr<QTcpSocket> socket = std::make_unique<QTcpSocket>();
            socket->connectToHost(node.ipAddress, node.port);
            _sockets[h] = std::move(socket);
        }
    }
}

void OutgoingSocketHandler::sendMessage(const Cluster& cluster, QString msg) const {
    for (const Cluster::Node& node : cluster.nodes()) {
        qDebug() << "Node: " << node.name << node.ipAddress << node.port;
        HashValue h = hash(cluster, node);

        auto it = _sockets.find(h);
        assert(it != _sockets.end());

        it->second->write(msg.toUtf8());
        it->second->flush();
    }
}

#include "outgoingsockethandler.h"

#include <QTcpSocket>
#include <QTimer>

#include <assert.h>

// This is just here so that we can use a forward declaration in the header file
OutgoingSocketHandler::~OutgoingSocketHandler() = default;

OutgoingSocketHandler::HashValue OutgoingSocketHandler::hash(
    const Cluster& cluster, const Cluster::Node& node) const
{
    return cluster.name() + "::" + node.name;
}

void OutgoingSocketHandler::initialize(const QList<Cluster>& clusters) {
    _clusters = std::move(clusters);
    for (const Cluster& c : _clusters) {
        for (const Cluster::Node& node : c.nodes()) {
            HashValue h = hash(c, node);

            // This handler keeps the sockets to the tray applications open
            std::unique_ptr<QTcpSocket> socket = std::make_unique<QTcpSocket>();
            connect(
                socket.get(), &QAbstractSocket::stateChanged,
                [node](QAbstractSocket::SocketState state) {
                    if (state == QAbstractSocket::SocketState::ConnectedState) {
                        qDebug() << "Socket state change: "
                            << node.ipAddress << node.port << state;
                    }
                    else if (state == QAbstractSocket::SocketState::ClosingState) {
                        qDebug() << "Socket state change: "
                            << node.ipAddress << node.port << state;
   
                    }
                }
            );
            connect(
                socket.get(), &QTcpSocket::readyRead,
                [s = socket.get()]() {
                    QByteArray data = s->readAll();
                    QString message = QString::fromUtf8(data);
                    qDebug() << message;
                }
            );

            
            socket->connectToHost(node.ipAddress, node.port);
            _sockets[h] = std::move(socket);
        }
    }
    
    QTimer* timer = new QTimer(this);
    connect(
        timer, &QTimer::timeout,
        [this](){
            for (const Cluster& c : _clusters) {
                if (c.enabled()) {
                    for (const Cluster::Node& node : c.nodes()) {
                        HashValue h = hash(c, node);

                        auto it = _sockets.find(h);
                        assert(it != _sockets.end());
                               
                        if (it->second->state() == QAbstractSocket::SocketState::UnconnectedState) {
                            qDebug() << "Unconnected: " << node.ipAddress << node.port;
                            it->second->connectToHost(node.ipAddress, node.port);
                        }
                    }
                }
            }
            
        }
    );
    timer->start(2500);
}

void OutgoingSocketHandler::sendMessage(const Cluster& cluster, QString msg) const {
    assert(!msg.isEmpty());

    for (const Cluster::Node& node : cluster.nodes()) {
        qDebug() << "Node: " << node.name << node.ipAddress << node.port;
        HashValue h = hash(cluster, node);

        auto it = _sockets.find(h);
        assert(it != _sockets.end());

        it->second->dumpObjectInfo();

        it->second->write(msg.toUtf8());
        it->second->flush();

    }
}


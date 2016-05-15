#include "incomingsockethandler.h"

#include <QTcpSocket>

#include <assert.h>

void IncomingSocketHandler::initialize(quint16 port) {
    _server.listen(QHostAddress::Any, port);

    connect(
        &_server, &QTcpServer::newConnection,
        this, &IncomingSocketHandler::newConnection
    );
}

void IncomingSocketHandler::newConnection() {
    while (_server.hasPendingConnections()) {
        QTcpSocket* socket = _server.nextPendingConnection();
        connect(
            socket, &QTcpSocket::readyRead,
            [=]() { readyRead(socket); }
        );
        connect(
            socket, &QTcpSocket::disconnected,
            [=]() { disconnectedConnection(socket); }
        );
        _sockets.push_back(socket);
    }
}

void IncomingSocketHandler::readyRead(QTcpSocket* socket) {
    assert(socket);
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    emit messageReceived(message);
}

void IncomingSocketHandler::disconnectedConnection(QTcpSocket* socket) {
    auto it = std::find(_sockets.begin(), _sockets.end(), socket);
    assert(it != _sockets.end());
    _sockets.erase(it);
}
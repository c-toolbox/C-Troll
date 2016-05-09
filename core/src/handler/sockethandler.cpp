#include "sockethandler.h"

#include <QTcpSocket>

#include <assert.h>

SocketHandler::SocketHandler(quint16 port) {
    _server.listen(QHostAddress::Any, port);

    connect(
        &_server, &QTcpServer::newConnection,
        this, &SocketHandler::newConnection
    );
}

void SocketHandler::newConnection() {
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

void SocketHandler::readyRead(QTcpSocket* socket) {
    assert(socket);
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    emit messageReceived(message);
}

void SocketHandler::disconnectedConnection(QTcpSocket* socket) {
    auto it = std::find(_sockets.begin(), _sockets.end(), socket);
    assert(it != _sockets.end());
    _sockets.erase(it);
}

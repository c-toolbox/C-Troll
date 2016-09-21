#include "incomingsockethandler.h"

#include <QTcpSocket>

#include <assert.h>

void IncomingSocketHandler::initialize(quint16 port) {
    _server.listen(QHostAddress::Any, port);

    QObject::connect(
        &_server, &QTcpServer::newConnection,
        [this]() { newConnection(); }
    );
}

void IncomingSocketHandler::newConnection() {
    while (_server.hasPendingConnections()) {
        QTcpSocket* socket = _server.nextPendingConnection();
        QObject::connect(
            socket, &QTcpSocket::readyRead,
            [=]() { readyRead(socket); }
        );
        QObject::connect(
            socket, &QTcpSocket::disconnected,
            [=]() { disconnectedConnection(socket); }
        );
        _sockets.push_back(socket);
        
        emit newConnectionEstablished(socket);
    }
}

void IncomingSocketHandler::readyRead(QTcpSocket* socket) {
    assert(socket);
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    emit messageReceived(message);
}

void IncomingSocketHandler::disconnectedConnection(QTcpSocket* socket) {
    assert(socket);
    auto it = std::find(_sockets.begin(), _sockets.end(), socket);
    assert(it != _sockets.end());
    _sockets.erase(it);
}

void IncomingSocketHandler::sendMessage(QTcpSocket* socket, QString message) {
    assert(socket);
    assert(
        std::find(_sockets.begin(), _sockets.end(), socket) != _sockets.end()
    );
    assert(!message.isEmpty());

    socket->write(message.toUtf8());
    socket->flush();   
}

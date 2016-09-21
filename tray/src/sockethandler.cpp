#include <iostream>
#include <QTcpSocket.h>
#include "sockethandler.h"



SocketHandler::SocketHandler() {}

SocketHandler::~SocketHandler() {}

void SocketHandler::initialize() {
    const int port = 5000;
    _server.listen(QHostAddress::Any, port);
    QObject::connect(&_server, &QTcpServer::newConnection, this, &SocketHandler::newConnection);
}

void SocketHandler::readyRead(QTcpSocket* socket) {
    QByteArray byteArray = socket->readAll();
    QString message = QString::fromUtf8(byteArray);
    emit messageRecieved(message);
}

void SocketHandler::sendMessage(QString message) {
    for (QTcpSocket* socket : _sockets) {
        socket->write(message.toUtf8());
    }
}

void SocketHandler::disconnected(QTcpSocket* socket) {
    auto ptr = std::find(_sockets.begin(), _sockets.end(), socket);
    if (ptr != _sockets.end()) {
        _sockets.erase(ptr);
        std::cout << "Socket disconnected." << std::endl;
    }
}

void SocketHandler::newConnection() {
    while (_server.hasPendingConnections()) {
        QTcpSocket* socket = _server.nextPendingConnection();
        QObject::connect(socket, &QTcpSocket::readyRead, [=]() {
            readyRead(socket);
        });
        QObject::connect(socket, &QTcpSocket::disconnected, [=]() {
            disconnected(socket);
        });
        _sockets.push_back(socket);
        std::cout << "Socket connected." << std::endl;
    }
}

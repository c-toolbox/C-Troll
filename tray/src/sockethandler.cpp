#include <iostream>
#include <QTcpSocket.h>
#include "sockethandler.h"

#include <QDebug>

SocketHandler::SocketHandler() {}

SocketHandler::~SocketHandler() {}

void SocketHandler::initialize() {
    const int port = 5000;
    
    qDebug() << "Listening on port:" << port;
    
    _server.listen(QHostAddress::Any, port);
    QObject::connect(&_server, &QTcpServer::newConnection, this, &SocketHandler::newConnection);
}

void SocketHandler::readyRead(QTcpSocket* socket) {
    QByteArray byteArray = socket->readAll();
    QString message = QString::fromUtf8(byteArray);
    //qDebug() << "Message received: " << message;
    emit messageRecieved(message);
}

void SocketHandler::sendMessage(QString message) {
    qDebug() << "Sending message: " << message;
    for (QTcpSocket* socket : _sockets) {
        qDebug() << socket->localAddress() << " -> " << socket->peerAddress();
        socket->write(message.toUtf8());
    }
}

void SocketHandler::disconnected(QTcpSocket* socket) {
    auto ptr = std::find(_sockets.begin(), _sockets.end(), socket);
    if (ptr != _sockets.end()) {
        _sockets.erase(ptr);
        qDebug() << "Socket disconnected";
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
        qDebug() << "Socket connected";
    }
}

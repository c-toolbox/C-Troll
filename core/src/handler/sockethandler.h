#ifndef __SOCKETHANDLER_H__
#define __SOCKETHANDLER_H__

#include <QObject>
#include <QTcpServer>

class SocketHandler : public QObject {
Q_OBJECT
public:
    void initialize(quint16 port);

signals:
    void messageReceived(QString message);

private:
    void newConnection();
    void readyRead(QTcpSocket* socket);
    void disconnectedConnection(QTcpSocket* socket);

    QTcpServer _server;

    QVector<QTcpSocket*> _sockets;
};

#endif __SOCKETHANDLER_H__

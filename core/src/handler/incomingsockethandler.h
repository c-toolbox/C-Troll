#ifndef __INCOMINGSOCKETHANDLER_H__
#define __INCOMINGSOCKETHANDLER_H__

#include <QObject>
#include <QTcpServer>

class IncomingSocketHandler : public QObject {
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

#endif __INCOMINGSOCKETHANDLER_H__

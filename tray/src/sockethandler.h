#ifndef __SOCKETHANDLER_H__
#define __SOCKETHANDLER_H__

#include <QObject.h>
#include <QTcpServer.h>

class QTcpSocket;

class SocketHandler : public QObject {
    Q_OBJECT
public:
    SocketHandler();
    ~SocketHandler();
    void newConnection();
    void disconnected(QTcpSocket*);
    void readyRead(QTcpSocket*);
    void initialize();
public slots:    
    void sendMessage(QString message);
signals:
    void messageRecieved(QString message);
 private:
    QTcpServer _server;
    std::vector<QTcpSocket*> _sockets;
};

#endif

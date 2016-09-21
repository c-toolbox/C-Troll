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
    void sendMessage(std::string message);
signals:
    void messageRecieved(std::string message);
 private:
    QTcpServer _server;
    std::vector<QTcpSocket*> _sockets;
};

#endif

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QObject>

#include "program/programhandler.h"
#include "cluster/clusterhandler.h"
#include "handler/sockethandler.h"
#include "traycommand.h"

class Application : public QObject {
//Q_OBJECT
public:
    Application(QString configurationFile);

private:
    void incomingMessage(QString message);

    void sendMessage(TrayCommand command, const Cluster& cluster);

    ProgramHandler _programHandler;
    ClusterHandler _clusterHandler;
    SocketHandler _socketHandler;
};

#endif // __APPLICATION_H__

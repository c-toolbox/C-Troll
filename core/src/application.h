#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QObject>

#include "program/programhandler.h"
#include "cluster/clusterhandler.h"
#include "handler/incomingsockethandler.h"
#include "handler/outgoingsockethandler.h"
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
    IncomingSocketHandler _incomingSocketHandler;
    OutgoingSocketHandler _outgoingSocketHandler;
};

#endif // __APPLICATION_H__

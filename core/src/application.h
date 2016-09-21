#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QObject>

#include "cluster.h"
#include "program.h"
#include "handler/incomingsockethandler.h"
#include "handler/outgoingsockethandler.h"
#include "traycommand.h"

namespace common {
    struct CoreCommand;
} // namespace common

class Application : public QObject {
public:
    Application(QString configurationFile);

private:
    void incomingMessage(QString message);
    
    void handleIncomingCommand(common::CoreCommand cmd);
    
    void sendInitializationInformation(QTcpSocket* socket);

    void sendMessage(const Cluster& cluster, common::TrayCommand command);

    Programs _programs;
    Clusters _clusters;
    IncomingSocketHandler _incomingSocketHandler;
    OutgoingSocketHandler _outgoingSocketHandler;
};

#endif // __APPLICATION_H__

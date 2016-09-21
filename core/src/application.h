#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "cluster.h"
#include "program.h"
#include "handler/incomingsockethandler.h"
#include "handler/outgoingsockethandler.h"
#include "traycommand.h"

namespace common {
    struct GuiCommand;
} // namespace common

class Application {
public:
    Application(QString configurationFile);

private:
    void incomingMessage(QString message);
    
    void handleIncomingCommand(common::GuiCommand cmd);
    
    void sendInitializationInformation(QTcpSocket* socket);

    void sendMessage(const Cluster& cluster, common::TrayCommand command, QString cmd);

    Programs _programs;
    QList<Cluster> _clusters;
    IncomingSocketHandler _incomingSocketHandler;
    OutgoingSocketHandler _outgoingSocketHandler;
};

#endif // __APPLICATION_H__

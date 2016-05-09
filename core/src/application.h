#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QObject>

#include "program/programhandler.h"
#include "cluster/clusterhandler.h"

class Application : QObject {
//Q_OBJECT
public:
    Application(QString configurationFile);

    ProgramHandler& programHandler();
    ClusterHandler& clusterHandler();

private:
    ProgramHandler _programHandler;
    ClusterHandler _clusterHandler;
};

#endif // __APPLICATION_H__

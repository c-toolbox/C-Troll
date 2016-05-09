#ifndef __MAINHANDLER_H__
#define __MAINHANDLER_H__

#include <QObject>

#include "program/programhandler.h"
#include "cluster/clusterhandler.h"

class MainHandler : QObject {
Q_OBJECT
public:
    MainHandler();

    ProgramHandler& programHandler();
    ClusterHandler& clusterHandler();

private:
    ProgramHandler _programHandler;
    ClusterHandler _clusterHandler;
};

#endif // __MAINHANDLER_H__

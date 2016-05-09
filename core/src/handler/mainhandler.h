#ifndef __MAINHANDLER_H__
#define __MAINHANDLER_H__

#include <QObject>

#include "application/applicationhandler.h"
#include "cluster/clusterhandler.h"

class MainHandler : QObject {
Q_OBJECT
public:
    MainHandler();

    ApplicationHandler& applicationHandler();
    ClusterHandler& clusterHandler();

private:
    ApplicationHandler _applicationHandler;
    ClusterHandler _clusterHandler;
};

#endif // __MAINHANDLER_H__

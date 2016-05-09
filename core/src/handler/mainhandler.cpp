#include "mainhandler.h"

ApplicationHandler& MainHandler::applicationHandler() {
    return _applicationHandler;
}

ClusterHandler& MainHandler::clusterHandler() {
    return _clusterHandler;
}

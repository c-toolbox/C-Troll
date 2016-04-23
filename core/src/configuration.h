#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <QJsonObject>

struct Configuration {
    Configuration(QString configurationFile);
    
    QString applicationPath;
    QString clusterPath;
};

#endif // __CONFIGURATION_H__

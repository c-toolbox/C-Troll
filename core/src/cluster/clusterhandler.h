#ifndef __CLUSTERHANDLER_H__
#define __CLUSTERHANDLER_H__

#include "cluster.h"

class ClusterHandler {
public:
    ClusterHandler() = default;
    
    void loadFromDirectory(QString directory);
    
    void addCluster(Cluster cluster);
    
    const QList<Cluster>& clusters() const;
    
private:
    Cluster loadCluster(QString jsonFile);
    
    QList<Cluster> _clusters;
};


#endif // __APPLICATIONHANDLER_H__

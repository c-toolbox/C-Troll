CORE
 - if connection to node is lost, need to check coreprocesses to see if that node had a process left
 - Need to update process status in CoreProcess based on feedback from tray
 - Need to check at startup of all clusters for all applications exist.  Problem if an application references a cluster that does not exist
 - Change the ClusterConnectionHandler to only create a single connection per node and group nodes together into clusters
 - Need to check at startup for uniqueness of names or the clusterconnectionhandler hash function is no longer unique
 - Pass over the codebase to unify application <-> program usage
 - Global database struct that stores clusters, applications, and processes and we place all pointer handling with id strings
 - When starting C-Troll with a running Tray, closing the tray, and reopening it, C-Troll does not try to reconnect to the tray application anymore
 - 1. start a program on a cluster, 2. close the tray on that node, 3. start the tray again -> C-Troll still says "Stop: XX"
CORE
 - if connection to node is lost, need to check coreprocesses to see if that node had a process left
 - Need to update process status in CoreProcess based on feedback from tray
 - Need to check at startup of all clusters for all applications exist.  Problem if an application references a cluster that does not exist
 - Change the ClusterConnectionHandler to only create a single connection per node and group nodes together into clusters
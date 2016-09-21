#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#include "guiinitialization.h"

#include <QList>

class JsonObject;

/**
 * This class represents a cluster setup, that is, a collection of computers that are
 * addressed as a unit. Each cluster has a human readable \m _name, a unique
 * \m _identifier, a setting whether or not it is \m _enabled and a list of computer
 * \m _nodes.
 */
class Cluster {
public:
    /**
     * Constructs the Cluster class from a JSON object. The JSON must contain the
     * following values and types:
     * \c name <string> The human-readable name
     * \c id <string> The unique identifier
     * \c enabled <bool, optional> Determining whether or not the cluster is currently
     * enabled. If it is disabled, it will not be included in the Core application's
     * attempt to contact
     * \c nodes <array> An array of objects for the individual
     * Cluster::Nodes, each with the following values:
     * \c name <string> The human-readable name of the computer
     * \c ip <string> The IP address or hostname of the computer
     * \c port <int> The port on which the Tray application on that computer is
     * listening
     * \param jsonObject The JSON object that contains the information for this
     * cluster
     * \throw std::runtime_error If the \p jsonObject does not contain all necessary
     * keys or if at least one of the keys has the wrong type
     */
    Cluster(const QJsonObject& jsonObject);
    
    /**
     * This struct contains information about individual computer nodes of the 
     * cluster. Each node has a human-readable \m name, an \m ipAddress, and a \m port
     * on which the Tray application is listening.
     */
    struct Node {
        /// The human readable name of the computer node
        QString name;
        /// The IP address at which the computer is reachable; this can also be a
        /// hostname
        QString ipAddress;
        /// The port on which the Tray application on that computer is listening
        int port;
    };

    /**
     * Returns the human-readable name of the cluster.
     * \return The human-readable name of the cluster
     */
    QString name() const;
    
    /**
     * Returns the unique identifier of the cluster.
     * \return The unique identifier of the cluster
     */
    QString identifier() const;
    
    /**
     * Returns whether the cluster is currently enabled or disabled. 
     * \return Whether the cluster is currently enabled or disabled
     */
    bool enabled() const;
    
    /**
     * Returns a list of all computer nodes that belong to this Cluster.
     * \return A list of all computer nodes that belong to this Cluster
     */
    QList<Node> nodes() const;
    
private:
    /// The human readable name of this Cluster
    QString _name;
    /// The unique identifier of this Cluster
    QString _identifier;
    /// A flag whether this Cluster is enabled or disabled
    bool _enabled;
    /// A list of all nodes belonging to this cluster
    QList<Node> _nodes;
};

/**
 * This method walks the passed \p directory and looks for all <code>*.json</code>
 * files in it. Any \c JSON file in it will be interpreted as a cluster configuration
 * and returned.
 * \param directory The directory that is walked in search for <code>*.json</code>
 * files
 * \return A list of all Cluster%s that were found by walking the \p directory
 */
QList<Cluster> loadClustersFromDirectory(QString directory);

/**
 * This method converts a Cluster information into the
 * common::GuiInitialization::Cluster format so that it can be send to connected
 * GUIs.
 * \param cluster The Cluster information that is to be converte
 * \return A common::GuiInitialization::Cluster structure that contains all relevant
 * information for the GUI initialization step
 */
common::GuiInitialization::Cluster clusterToGuiInitializationCluster(Cluster cluster);

#endif // __CLUSTER_H__

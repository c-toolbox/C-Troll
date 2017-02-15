/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 * of conditions and the following disclaimer.                                           *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 * list of conditions and the following disclaimer in the documentation and/or other     *
 * materials provided with the distribution.                                             *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 * used to endorse or promote products derived from this software without specific prior *
 * written permission.                                                                   *
 *                                                                                       *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY   *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT   *
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,        *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  *
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR    *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      *
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    *
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                                               *
 *                                                                                       *
 ****************************************************************************************/

#ifndef __GUIINITIALIZATION_H__
#define __GUIINITIALIZATION_H__

#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QString>

namespace common {
    
/// This struct is the data structure that gets send from the Core to the GUI informing
/// the GUI about the available applications, configurations, and clusters
struct GuiInitialization {
    /// The string representing this command type, for usage in the common::GenericMessage
    static const QString Type;
    
    /// This struct represents the information about an application that is registered
    /// with the Core library
    struct Application {
        /// Default constructor
        Application() = default;
        
        /**
         * Creates an Application from the passed \p application. The \p application must
         * contain all of the following keys with the associated types:
         * \c name The name of the application (String)
         * \c id The identifier of the application (String)
         * \c tags The tags that the application is associated with (Array of Strings)
         * \c clusters The clusters that this application is supporting (Array of 
         * Strings)
         * \c configurations The configurations that are supported by this application 
         * (Array of pairs of strings, \c name and \c id)
         * \param application The JSON object that contains all of the values for this
         * application
         * \throw std::runtime_error If one of the keys is missing or has the wrong type
         */
         Application(QJsonObject application);
        
        /**
         * Returns the JSON object representation of this application. See the constructor
         * for a list of keys and their types.
         * \return The JSON object representation of this application
         */
        QJsonObject toJson() const;
        
        QString name;
        QString id;
        QStringList tags;
        QStringList clusters;
        
        struct Configuration {
            QString name;
            QString id;
        };
        QList<Configuration> configurations;
    };
    
    /// This struct represents the information about the clusters that are registered with
    /// the Core library
    struct Cluster {
        /// Default constructor
        Cluster() = default;
        
        /**
         * Creates a Cluster from the passed \p cluster. The \p cluster must contain both
         * of the following keys; both of type string:
         * \c name The human-readable name of the cluster
         * \c id The identifier used for this cluster
         * \param cluster The JSON object that contains all of the values for this cluster
         * \throw std::runtime_error If one of the keys is missing or has the wrong type
         */
        Cluster(QJsonObject cluster);
        
        /**
         * Returns the JSON object representation of this cluster. See the constructor
         * for a list of keys and their types.
         * \return The JSON object representation of this cluster
         */
        QJsonObject toJson() const;
        
        /// The human-readable name of the cluster
        QString name;
        /// The unique identifier for the cluster
        QString id;
        /// Whether the cluster is currently enabled or not
        bool enabled;
        /// Whether the cluster is currently connected or not
        bool connected;
    };

    /// This struct represents the information about the processes that are registered with
    /// the Core library
    struct Process {

        struct NodeStatus {
            /**
             * Creates a NodeStatus from the passed node status. The object must contain
             * all of the following keys.
             * \c status The node status (string)
             * \c time The time when the status was set (double)
             */
            NodeStatus() = default;
            NodeStatus(QJsonObject nodeStatus);

            /**
            * Returns the JSON object representation of this NodeStatus. See the constructor
            * for a list of keys and their types.
            * \return The JSON object representation of this NodeStatus
            */

            QString node;
            QJsonObject toJson() const;
            QString status;
            double time;
        };

        /// Default constructor
        Process() = default;

        /**
        * Creates a Process from the passed \p process. The \p process must contain all
        * of the following keys
        * \c id The identifier used for this process (int)
        * \c applicationId The application id associated with this process (string)
        * \c configurationId The configuration id of the application (string)
        * \c clusterId The cluster id associated with this process (string)
        * \param process The JSON object that contains all of the values for this process
        * \throw std::runtime_error If one of the keys is missing or has the wrong type
        */
        Process(QJsonObject process);

        /**
        * Returns the JSON object representation of this process. See the constructor
        * for a list of keys and their types.
        * \return The JSON object representation of this process
        */
        QJsonObject toJson() const;

        /// The unique identifier for the process
        int id;

        QString applicationId;

        QString configurationId;

        QString clusterId;

        QString clusterStatus;

        double clusterStatusTime;

        QList<GuiInitialization::Process::NodeStatus> nodeStatusHistory;

    };
    
    /// Default constructor
    GuiInitialization() = default;

    /**
     * Creates a GuiInitialization from the passed \p document. The \p document must
     * contain all of the following keys with the associated types:
     * \c applications of type \c array containing GuiInitialization::Application%s
     * \c cluster of type \c array containing GuiInitialization::Cluster%s
     * \param document The QJsonDocument that contains the information about this
     * GuiInitialization
     * \throws std::runtime_error If one of the required keys were not present or of the
     * wrong type
     */
    GuiInitialization(const QJsonDocument& document);

    /**
     * Converts the GuiInitialization into a valid QJsonDocument object and returns it.
     * \return the QJsonDocument representing this GuiInitialization
     */
    QJsonDocument toJson() const;

    /// All applications that are registered with the Core
    QList<Application> applications;
    /// All clusters that the Core is responsible for
    QList<Cluster> clusters;
    /// All current processes
    QList<Process> processes;
};

} // namespace

#endif // __CORECOMMAND_H__

#ifndef __GUIINITIALIZATION_H__
#define __GUIINITIALIZATION_H__

#include <QJsonDocument>
#include <QList>
#include <QString>

namespace common {
    
/// This struct is the data structure that gets send from the Core to the GUI
/// informing the GUI about the available applications, configurations, and clusters
struct GuiInitialization {
    static const QString Type;
    
    /// This struct represents the information about an application that is registered
    /// with the Core library
    struct Application {
        /// Default constructor
        Application() = default;
        
        /**
         * Creates an Application from the passed \p application. The \p application
         * must contain all of the following keys with the associated types:
         * \c name The name of the application (String)
         * \c identifier The identifier of the application (String)
         * \c tags The tags that the application is associated with (Array of Strings)
         * \c clusters The clusters that this application is supporting (Array of 
         * Strings)
         * \c configurations The configurations that are supported by this application 
         * (Array of pairs of strings, \c name and \c identifier)
         * \param application The JSON object that contains all of the values for this
         * application
         * \throw std::runtime_error If one of the keys is missing or has the wrong
         * type
         */
         Application(QJsonObject application);
        
        /**
         * Returns the JSON object representation of this application. See the 
         * constructor for a list of keys and their types.
         * \return The JSON object representation of this application
         */
        QJsonObject toJson() const;
        
        QString name;
        QString identifier;
        QStringList tags;
        QStringList clusters;
        
        struct Configuration {
            QString name;
            QString identifier;
        };
        QList<Configuration> configurations;
    };
    
    /// This struct represents the information about the clusters that are registered
    /// with the Core library
    struct Cluster {
        /// Default constructor
        Cluster() = default;
        
        /**
         * Creates a Cluster from the passed \p cluster. The \p cluster must contain 
         * both of the following keys; both of type string:
         * \c name The human-readable name of the cluster
         * \c identifier The identifier used for this cluster
         * \param cluster The JSON object that contains all of the values for this
         * cluster
         * \throw std::runtime_error If one of the keys is missing or has the wrong
         * type
         */
        Cluster(QJsonObject cluster);
        
        /**
         * Returns the JSON object representation of this application. See the 
         * constructor for a list of keys and their types.
         * \return The JSON object representation of this application
         */
        QJsonObject toJson() const;
        
        QString name;
        QString identifier;
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
     * \throws std::runtime_error If one of the required keys were not present or of
     * the wrong type
     */
    GuiInitialization(const QJsonDocument& document);

    /**
     * Converts the GuiInitialization into a valid QJsonDocument object and returns
     * it.
     * \return the QJsonDocument representing this GuiInitialization
     */
    QJsonDocument toJson() const;

    /// All applications that are registered with the Core
    QList<Application> applications;
    /// All clusters that the Core is responsible for
    QList<Cluster> clusters;
};

} // namespace

#endif // __CORECOMMAND_H__

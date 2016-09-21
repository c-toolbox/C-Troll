#ifndef __GUICOMMAND_H__
#define __GUICOMMAND_H__

#include <QJsonDocument>
#include <QString>

namespace common {

/// This struct is the data structure that gets send from the GUI to the Core
/// to signal that the Core should perform a task for a specific cluster
struct GuiCommand {
    static const QString Type;
    
    /// Default constructor
    GuiCommand() = default;

    /**
     * Creates a GuiCommand from the passed \p document. The \p document must contain
     * all of the following keys, all of type string:
     * \c command
     * \c application_identifier
     * \c configuration_identifier
     * \c cluster_identifier
     * \param document The QJsonDocument that contains the information about this
     * CoreCommand
     * \throws std::runtime_error If one of the required keys were not present or of the
     * wrong type
     */
    GuiCommand(const QJsonDocument& document);

    /**
     * Converts the GuiCommand into a valid QJsonDocument object and returns it.
     * \return the QJsonDocument representing this GuiCommand
     */
    QJsonDocument toJson() const;

    /// The kind of command that is to be executed
    QString command;
    /// The unique identifier of the application that is to be started
    QString applicationId;
    /// The identifier of the application's configuration that is to be started
    QString configurationId;
    /// The identifier of the cluster on which the application is to be started
    QString clusterId;
};

} // namespace

#endif // __GUICOMMAND_H__

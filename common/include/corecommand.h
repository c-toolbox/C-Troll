#ifndef __CORECOMMAND_H__
#define __CORECOMMAND_H__

#include <QJsonDocument>
#include <QString>

namespace common {

/// This struct is the data structure that gets send from the GUI to the Core
/// to signal that the Core should perform a task for a specific cluster
struct CoreCommand {
    /// Default constructor
    CoreCommand() = default;

    /**
     * Creates a CoreCommand from the passed \p document.
     * \param document The QJsonDocument that contains the information about this
     * CoreCommand
     * \throws std::runtime_error If one of the required keys were not present or of the
     * wrong type
     */
    CoreCommand(const QJsonDocument& document);

    /**
     * Converts the CoreCommand into a valid QJsonDocument object and returns it.
     * \return the QJsonDocument representing this CoreCommand
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

#endif // __CORECOMMAND_H__

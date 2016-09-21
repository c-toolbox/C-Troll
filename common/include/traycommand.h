#ifndef __TRAYCOMMAND_H__
#define __TRAYCOMMAND_H__

#include <QJsonDocument>
#include <QString>

namespace common {

/// This struct is the data structure that gets send from the Core to the Tray to signal
/// that the Tray should perform a task
struct TrayCommand {
    /// The string representing this command type, for usage in the common::GenericMessage
    static const QString Type;
    
    /// Default constructor
    TrayCommand() = default;

    /**
     * Creates a TrayCommand from the passed \p document. The \p document must  contain
     * the following keys, all of type string:
     * \c identifier
     * \c command
     * \c executable
     * \c baseDirectory
     * \c currentWorkingDirectory
     * \c commandlineArguments
     * \param document The QJsonDocument that contains the information about this
     * TrayCommand
     * \throws std::runtime_error If one of the required keys were not present or of the
     * wrong type
     */
    TrayCommand(const QJsonDocument& document);

    /**
     * Converts the TrayCommand into a valid QJsonDocument object and returns it.
     * \return the QJsonDocument representing this TrayCommand
     */
    QJsonDocument toJson() const;

    /// The unique identifier for the process that will be created
    QString identifier;
    /// The kind of command that is to be executed
    QString command;
    /// The name of the executable
    QString executable;
    /// The directory in which the executable is located
    QString baseDirectory;
    /// The location that should be set as the working directory prior to execution
    QString currentWorkingDirectory;
    /// The list of commandline parameters to be passed to the executable
    QString commandlineParameters;
};

} // namespace commmon

#endif // __TRAYCOMMAND_H__

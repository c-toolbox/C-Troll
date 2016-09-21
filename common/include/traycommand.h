#ifndef __TRAYCOMMAND_H__
#define __TRAYCOMMAND_H__

#include <QJsonDocument>
#include <QString>

namespace common {

struct TrayCommand {
    /// Default constructor
    TrayCommand() = default;

    /**
     * Creates a TrayCommand from the passed \p document.
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
    /// The unique identifier for the process that will be created
    QString identifier;
};

} // namespace

#endif // __TRAYCOMMAND_H__

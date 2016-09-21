#ifndef __TRAYPROCESSLOGMESSAGE_H__
#define __TRAYPROCESSLOGMESSAGE_H__

#include <QJsonDocument>
#include <QString>

namespace common {
    
/// This struct is the data structure that gets send from the Core to the Tray
/// to signal that the Tray should perform a task
struct TrayProcessLogMessage {
    static const QString Type;
    
    /// Default constructor
    TrayProcessLogMessage() = default;
    
    /**
     * Creates a TrayProcessLogMessage from the passed \p document. The \p document must
     * contain the following keys, all of type string:
     * \c identifier
     * \c stdOutLog
     * \c stdErrorLog
     * \param document The QJsonDocument that contains the information about this
     * TrayProcessLogMessage
     * \throws std::runtime_error If one of the required keys were not present or of the
     * wrong type
     */
    TrayProcessLogMessage(const QJsonDocument& document);
    
    /**
     * Converts the TrayProcessLogMessage into a valid QJsonDocument object and returns it.
     * \return The QJsonDocument representing this TrayProcessLogMessage
     */
    QJsonDocument toJson() const;
    
    /// The unique identifier for the process that will be created
    QString identifier;
    /// The process stdout line
    QString stdOutLog;
    /// The process stderror line
    QString stdErrorLog;
};
    
} // namespace common

#endif // __TRAYPROCESSLOGMESSAGE_H__

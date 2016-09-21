#ifndef __TRAYPROCESSSTATUS_H__
#define __TRAYPROCESSSTATUS_H__

#include <QJsonDocument>
#include <QString>

namespace common {
    
/// This struct is the data structure that gets send from the Core to the Tray
/// to signal that the Tray should perform a task
struct TrayProcessStatus {
    static const QString Type;
    
    /// Default constructor
    TrayProcessStatus() = default;
    
    /**
     * Creates a TrayProcessStatus from the passed \p document. The \p document must
     * contain the following keys, all of type string:
     * \c identifier
     * \c status
     * \param document The QJsonDocument that contains the information about this
     * TrayProcessStatus
     * \throws std::runtime_error If one of the required keys were not present or of the
     * wrong type
     */
    TrayProcessStatus(const QJsonDocument& document);
    
    /**
     * Converts the TrayProcessLogMessage into a valid QJsonDocument object and returns it.
     * \return The QJsonDocument representing this TrayProcessLogMessage
     */
    QJsonDocument toJson() const;
    
    /// The unique identifier for the process that will be created
    QString identifier;
    /// The process status
    QString status;
};
    
} // namespace common

#endif // __TRAYPROCESSSTATUS_H__
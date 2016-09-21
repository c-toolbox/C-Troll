#include "trayprocesslogmessage.h"

#include "jsonsupport.h"

#include <QJsonObject>

namespace {
    const QString KeyIdentifier = "identifier";
    const QString KeyStdOut = "stdout";
    const QString KeyStdError = "stderror";
}

namespace common {
    
const QString TrayProcessLogMessage::Type = "TrayProcessLogMessage";

TrayProcessLogMessage::TrayProcessLogMessage(const QJsonDocument& document) {
    QJsonObject obj = document.object();
    
    identifier = common::testAndReturnString(obj, KeyIdentifier);
    stdOutLog = common::testAndReturnString(obj, KeyStdOut);
    stdErrorLog = common::testAndReturnString(obj, KeyStdError);
}

QJsonDocument TrayProcessLogMessage::toJson() const {
    QJsonObject obj;
    obj[KeyIdentifier] = identifier;
    obj[KeyStdOut] = stdOutLog;
    obj[KeyStdError] = stdErrorLog;
    
    return QJsonDocument(obj);
    
}
    
} // namespace common

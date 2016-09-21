#include "trayprocessstatus.h"

#include "jsonsupport.h"

#include <QJsonObject>

namespace {
    const QString KeyIdentifier = "identifier";
    const QString KeyStatus = "status";
}

namespace common {
    
const QString TrayProcessStatus::Type = "TrayProcessStatus";

TrayProcessStatus::TrayProcessStatus(const QJsonDocument& document) {
    QJsonObject obj = document.object();
    
    identifier = common::testAndReturnString(obj, KeyIdentifier);
    status = common::testAndReturnString(obj, KeyStatus);
}

QJsonDocument TrayProcessStatus::toJson() const {
    QJsonObject obj;
    obj[KeyIdentifier] = identifier;
    obj[KeyStatus] = status;
    
    return QJsonDocument(obj);
    
}
    
} // namespace common

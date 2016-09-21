#include "genericmessage.h"

#include "jsonsupport.h"

#include <QJsonObject>

namespace {
    const QString KeyType = "type";
    const QString KeyPayload = "payload";
}

namespace common {

GenericMessage::GenericMessage(const QJsonDocument& document) {
    QJsonObject obj = document.object();
    
    type = common::testAndReturnString(obj, KeyType);
    payload = common::testAndReturnObject(obj, KeyPayload);
}
    
QJsonDocument GenericMessage::toJson() const {
    QJsonObject obj;
    obj[KeyType] = type;
    obj[KeyPayload] = payload;
    
    return QJsonDocument(obj);
}
    
} // namespace common

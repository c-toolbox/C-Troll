#include "jsonsupport.h"

#include <QJsonArray>

namespace common {

QString testAndReturnString(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key)) {
        throw std::runtime_error(
            "Missing key '" + key.toStdString() + "' in TrayCommand"
        );
    }
    if (!obj[key].isString()) {
        throw std::runtime_error(
            "Key '" + key.toStdString() + "' in TrayCommand is not a string"
        );
    }

    return obj[key].toString();
}

int testAndReturnInt(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key)) {
        throw std::runtime_error(
            "Missing key '" + key.toStdString() + "' in TrayCommand"
        );
    }
    if (!obj[key].isDouble()) {
        throw std::runtime_error(
            "Key '" + key.toStdString() + "' in TrayCommand is not a string"
        );
    }

    return obj[key].toInt();
}

QJsonArray testAndReturnArray(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key)) {
        throw std::runtime_error(
            "Missing key '" + key.toStdString() + "' in TrayCommand"
        );
    }
    if (!obj[key].isArray()) {
        throw std::runtime_error(
            "Key '" + key.toStdString() + "' in TrayCommand is not a string"
        );
    }

    return obj[key].toArray();
}

bool testAndReturnBool(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key)) {
        throw std::runtime_error(
            "Missing key '" + key.toStdString() + "' in TrayCommand"
        );
    }
    if (!obj[key].isBool()) {
        throw std::runtime_error(
            "Key '" + key.toStdString() + "' in TrayCommand is not a string"
        );
    }

    return obj[key].toBool();
}

} // namespace common

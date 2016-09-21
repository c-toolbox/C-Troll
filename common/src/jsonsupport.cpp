#include "jsonsupport.h"

#include <QJsonArray>

namespace common {

QString testAndReturnString(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key)) {
        throw std::runtime_error(
            "Missing key '" + key.toStdString() + "'"
        );
    }
    if (!obj[key].isString()) {
        throw std::runtime_error(
            "Key '" + key.toStdString() + "' is not a string"
        );
    }

    return obj[key].toString();
}

int testAndReturnInt(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key)) {
        throw std::runtime_error(
            "Missing key '" + key.toStdString() + "'"
        );
    }
    if (!obj[key].isDouble()) {
        throw std::runtime_error(
            "Key '" + key.toStdString() + "' is not an integer"
        );
    }

    return obj[key].toInt();
}

QJsonArray testAndReturnArray(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key)) {
        throw std::runtime_error(
            "Missing key '" + key.toStdString() + "'"
        );
    }
    if (!obj[key].isArray()) {
        throw std::runtime_error(
            "Key '" + key.toStdString() + "' is not an array"
        );
    }

    return obj[key].toArray();
}

QJsonObject testAndReturnObject(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key)) {
        throw std::runtime_error(
            "Missing key '" + key.toStdString() + "'"
                                 );
    }
    if (!obj[key].isObject()) {
        throw std::runtime_error(
            "Key '" + key.toStdString() + "' is not an object"
        );
    }
    
    return obj[key].toObject();
};

bool testAndReturnBool(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key)) {
        throw std::runtime_error(
            "Missing key '" + key.toStdString() + "'"
        );
    }
    if (!obj[key].isBool()) {
        throw std::runtime_error(
            "Key '" + key.toStdString() + "' is not a boolean"
        );
    }

    return obj[key].toBool();
}

} // namespace common

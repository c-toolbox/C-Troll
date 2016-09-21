#include "jsonsupport.h"

#include <QJsonArray>

namespace common {

QString testAndReturnString(const QJsonObject& obj, const QString& key,
                            Optional optional, QString defaultValue)
{
    if (optional == Optional::Yes && !obj.contains(key)) {
        return defaultValue;
    }
    
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

int testAndReturnInt(const QJsonObject& obj, const QString& key,
                     Optional optional, int defaultValue)
{
    if (optional == Optional::Yes && !obj.contains(key)) {
        return defaultValue;
    }
    
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

QJsonArray testAndReturnArray(const QJsonObject& obj, const QString& key,
                              Optional optional, QJsonArray defaultValue)
{
    if (optional == Optional::Yes && !obj.contains(key)) {
        return defaultValue;
    }
    
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
    
QStringList testAndReturnStringList(const QJsonObject& obj, const QString& key,
                                    Optional optional, QStringList defaultValue)
{
    if (optional == Optional::Yes && !obj.contains(key)) {
        return defaultValue;
    }
    
    QStringList result;
    QJsonArray array = testAndReturnArray(obj, key);
    for (const QJsonValue& value : array) {
        if (!value.isString()) {
            throw std::runtime_error(
                "Array '" + key.toStdString() + "' is not an array of strings"
            );
        }
        result.push_back(value.toString());
    }
    return result;
}

QJsonObject testAndReturnObject(const QJsonObject& obj, const QString& key,
                                Optional optional, QJsonObject defaultValue)
{
    if (optional == Optional::Yes && !obj.contains(key)) {
        return defaultValue;
    }
    
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

bool testAndReturnBool(const QJsonObject& obj, const QString& key,
                       Optional optional, bool defaultValue)
{
    if (optional == Optional::Yes && !obj.contains(key)) {
        return defaultValue;
    }
    
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

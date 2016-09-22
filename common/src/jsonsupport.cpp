/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 * of conditions and the following disclaimer.                                           *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 * list of conditions and the following disclaimer in the documentation and/or other     *
 * materials provided with the distribution.                                             *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 * used to endorse or promote products derived from this software without specific prior *
 * written permission.                                                                   *
 *                                                                                       *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY   *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT   *
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,        *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  *
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR    *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      *
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    *
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                                               *
 *                                                                                       *
 ****************************************************************************************/

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

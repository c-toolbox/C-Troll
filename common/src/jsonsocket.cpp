/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
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

#include <jsonsocket.h>

namespace common {

JsonSocket::JsonSocket(std::unique_ptr<QTcpSocket> socket, QObject *parent)
    : QObject(parent)
    , _socket(std::move(socket))
    , _payloadSize(-1)
{

    QObject::connect(_socket.get(), &QTcpSocket::readyRead,
        [this]() { readToBuffer(); });
}

JsonSocket::~JsonSocket() {}

void JsonSocket::write(QJsonDocument jsonDocument) {
    QByteArray json = jsonDocument.toJson();
    QByteArray length = QString::number(json.size()).toUtf8();
    _socket->write(length);
    _socket->write("#");
    _socket->write(json);
    _socket->flush();
}

void JsonSocket::readToBuffer() {
    QByteArray incomingData = _socket->readAll();
    std::copy(incomingData.begin(), incomingData.end(), std::back_inserter(_buffer));
    if (_payloadSize == -1) {
        auto it = std::find(_buffer.begin(), _buffer.end(), '#');
        if (it != _buffer.end()) {
            std::string sizeString;
            std::copy(_buffer.begin(), it, std::back_inserter(sizeString));
            _payloadSize = QString::fromStdString(sizeString).toInt();
            _buffer.erase(_buffer.begin(), it + 1);
        }
    }

    if (_payloadSize > 0 && _payloadSize <= _buffer.size()) {
        emit readyRead();
    }
}

QJsonDocument JsonSocket::read() {
    if (_payloadSize <= _buffer.size()) {
        std::vector<char> data(_buffer.begin(), _buffer.begin() + _payloadSize);
        QByteArray json(data.data(), _payloadSize);       
        _buffer.erase(_buffer.begin(), _buffer.begin() + _payloadSize);
        _payloadSize = -1;
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        if (error.error != QJsonParseError::NoError) {
            qDebug() << "\nJson parse error:\n" << error.errorString() << "\nJson string:\n" << json;
        }
        return doc;
    }
    else {
        return QJsonDocument::fromJson("");
    }
}

QTcpSocket* JsonSocket::socket() {
    return _socket.get();
}

}
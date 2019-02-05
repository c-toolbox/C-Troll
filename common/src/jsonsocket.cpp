/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
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

#include "jsonsocket.h"

#include <QNetworkProxy>

namespace common {

JsonSocket::JsonSocket(std::unique_ptr<QTcpSocket> socket)
    : QObject()
    , _socket(std::move(socket))
{
    connect(_socket.get(), &QTcpSocket::readyRead, this, &JsonSocket::readToBuffer);
    connect(_socket.get(), &QTcpSocket::disconnected, this, &JsonSocket::disconnected);
    _socket->setProxy(QNetworkProxy::NoProxy);
}
void JsonSocket::connectToHost(const std::string& host, int port) {
    _socket->connectToHost(QString::fromStdString(host), port);
}

QTcpSocket::SocketState JsonSocket::state() const {
    return _socket->state();
}

void JsonSocket::write(nlohmann::json jsonDocument) {
    std::string jsonText = jsonDocument.dump();
    std::string length = std::to_string(jsonText.size());
    std::string msg = length + '#' + jsonText;

    _socket->write(msg.c_str());
    _socket->flush();
}

void JsonSocket::readToBuffer() {
    QByteArray incomingData = _socket->readAll();
    _buffer.resize(incomingData.size());
    std::copy(incomingData.begin(), incomingData.end(), _buffer.begin());

    // If it is the first package to arrive, we extract the expected length of the message
    if (_payloadSize == -1) {
        auto it = std::find(_buffer.begin(), _buffer.end(), '#');
        if (it != _buffer.end()) {
            std::string sizeString(_buffer.begin(), it);
            _payloadSize = std::stoi(sizeString);
            _buffer.erase(_buffer.begin(), it + 1);
        }
    }

    if (_payloadSize > 0 && (_payloadSize <= _buffer.size())) {
        emit readyRead();
        readToBuffer();
    }
}

nlohmann::json JsonSocket::read() {
    if (_payloadSize > _buffer.size()) {
        return nlohmann::json();
    }

    std::vector<char> data(_buffer.begin(), _buffer.begin() + _payloadSize);
    std::string json(data.data(), _payloadSize);
    _buffer.erase(_buffer.begin(), _buffer.begin() + _payloadSize);
    _payloadSize = -1;

    return nlohmann::json::parse(json);
}

std::string JsonSocket::localAddress() const {
    return _socket->localAddress().toString().toStdString();

}

std::string JsonSocket::peerAddress() const {
    return _socket->peerAddress().toString().toStdString();
}

} // namespace common

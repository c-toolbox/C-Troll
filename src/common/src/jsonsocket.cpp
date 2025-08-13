/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2025                                                               *
 * Alexander Bock                                                                        *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 *    of conditions and the following disclaimer.                                        *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 *    list of conditions and the following disclaimer in the documentation and/or other  *
 *    materials provided with the distribution.                                          *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 *    used to endorse or promote products derived from this software without specific    *
 *    prior written permission.                                                          *
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

#include "logging.h"
#include <QCryptographicHash>
#include <QNetworkProxy>

namespace {
    void Debug(std::string msg) {
        ::Debug("JsonSocket", std::move(msg));
    }
} // namespace

namespace common {

JsonSocket::JsonSocket(std::unique_ptr<QTcpSocket> socket, std::string secret)
    : QObject()
    , _socket(std::move(socket))
{
    if (!secret.empty()) {
        QByteArray secretHash = QCryptographicHash::hash(
            QString::fromStdString(secret).toUtf8(),
            QCryptographicHash::Sha1
        );

        quint64 key = secretHash.toULongLong();
        _crypto = SimpleCrypt(key);
    }

    connect(_socket.get(), &QTcpSocket::readyRead, this, &JsonSocket::readToBuffer);
    connect(_socket.get(), &QTcpSocket::disconnected, this, &JsonSocket::disconnected);
    _socket->setProxy(QNetworkProxy::NoProxy);
}

void JsonSocket::connectToHost(const std::string& host, int port) {
    Debug(std::format("Connecting to {}:{}", host, port));
    _socket->connectToHost(QString::fromStdString(host), static_cast<quint16>(port));
}

QTcpSocket::SocketState JsonSocket::state() const {
    return _socket->state();
}

void JsonSocket::write(const nlohmann::json& jsonDocument) {
    std::string jsonText = jsonDocument.dump();
    std::string length = std::to_string(jsonText.size());
    std::string msg = length + '#' + jsonText;

    bool success;
    if (_crypto.has_value()) {
        QByteArray data = _crypto->encryptToByteArray(QString::fromStdString(msg));
        qint64 res = _socket->write(data);
        success = (data.size() == res);
    }
    else {
        const size_t messageSize = msg.size();
        qint64 res = _socket->write(msg.c_str());
        success = (static_cast<qint64>(messageSize) == res);
    }
    if (!success) {
        ::Log("JsonSocket", std::format("Error writing message: {})", msg));
    }
    _socket->flush();
}

void JsonSocket::readToBuffer() {
    try {
        QByteArray incomingData = _socket->readAll();

        if (_crypto.has_value()) {
            QByteArray payload = _crypto->decryptToByteArray(incomingData);
            incomingData = payload;
        }

        _buffer.resize(static_cast<size_t>(incomingData.size()));
        std::copy(incomingData.begin(), incomingData.end(), _buffer.begin());

        parseBuffer();
    }
    catch (const std::exception&) {
        ::Log("JsonSocket::readToBuffer", "Caught exception when trying to read buffer");
        ::Log("JsonSocket::readToBuffer (Buffer Size", std::to_string(_buffer.size()));
        ::Log(
            "JsonSocket::readToBuffer (Buffer Contents)",
            std::string(_buffer.begin(), _buffer.end())
        );
        ::Log("JsonSocket::readToBuffer (payload size)", std::to_string(_payloadSize));

        _payloadSize = -1;
        _buffer.clear();
    }
}

void JsonSocket::parseBuffer() {
    // If it is the first package to arrive, we extract the expected length of the message
    if (_payloadSize == -1) {
        const auto it = std::find(_buffer.cbegin(), _buffer.cend(), '#');
        if (it != _buffer.end()) {
            std::string sizeString(_buffer.cbegin(), it);
            _payloadSize = std::stoi(sizeString);
            _buffer.erase(_buffer.begin(), it + 1);
        }
    }

    if (_payloadSize > 0 && (_payloadSize <= static_cast<int>(_buffer.size()))) {
        std::vector<char> data(_buffer.begin(), _buffer.begin() + _payloadSize);
        std::string json(data.data(), static_cast<size_t>(_payloadSize));
        _buffer.erase(_buffer.begin(), _buffer.begin() + _payloadSize);
        _payloadSize = -1;

        nlohmann::json message = nlohmann::json::parse(json);
        emit messageReceived(message);

        if (!_buffer.empty()) {
            // This can only happen if we get one TCP package with multiple messages in it
            parseBuffer();
        }
    }
}

std::string JsonSocket::localAddress() const {
    return _socket->localAddress().toString().toLocal8Bit().constData();
}

std::string JsonSocket::peerAddress() const {
    return _socket->peerAddress().toString().toLocal8Bit().constData();
}

} // namespace common

/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2026                                                               *
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
#include <QNetworkProxy>

namespace {
    void Debug(std::string msg) {
        ::Debug("JsonSocket", std::move(msg));
    }
} // namespace

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
    Debug(std::format("Connecting to {}:{}", host, port));
    _socket->connectToHost(QString::fromStdString(host), static_cast<quint16>(port));
}

void JsonSocket::disconnectFromHost() {
    _socket->disconnectFromHost();
}

QTcpSocket::SocketState JsonSocket::state() const {
    return _socket->state();
}
void JsonSocket::write(const nlohmann::json& jsonDocument) {
    const std::string jsonText = jsonDocument.dump();
    const std::string msg = std::format("{}#{}", jsonText.size(), jsonText);

    const qint64 res = _socket->write(msg.data(), static_cast<qint64>(msg.size()));
    if (res != static_cast<qint64>(msg.size())) {
        ::Log("JsonSocket", std::format("Error writing message: {})", msg));
    }
    _socket->flush();
}

void JsonSocket::readToBuffer() {
    try {
        const QByteArray incomingData = _socket->readAll();

        // A message can be split across multiple packages, so the new data has to be
        // appended to whatever is left over from the previous read
        _buffer.insert(_buffer.end(), incomingData.begin(), incomingData.end());

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
        const auto it = std::find(_buffer.begin(), _buffer.end(), '#');
        if (it != _buffer.end()) {
            std::string sizeString = std::string(_buffer.begin(), it);
            _payloadSize = std::stoi(sizeString);
            _buffer.erase(_buffer.begin(), it + 1);
        }
    }

    if (_payloadSize > 0 && (_payloadSize <= static_cast<int>(_buffer.size()))) {
        std::vector<char> data = std::vector<char>(
            _buffer.begin(),
            _buffer.begin() + _payloadSize
        );
        std::string json = std::string(data.data(), static_cast<size_t>(_payloadSize));
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

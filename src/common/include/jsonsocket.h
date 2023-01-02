/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2023                                                             *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
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

#ifndef __COMMON__JSONSOCKET_H__
#define __COMMON__JSONSOCKET_H__

#include <QObject>

#include <QFloat16>
#include <QTcpSocket>
#include <nlohmann/json.hpp>
#include <simplecrypt/simplecrypt.h>
#include <memory>
#include <optional>
#include <vector>

namespace common {

/**
 * This socket handles connections that transmit entire JSON messages. Individual packages
 * are cached. When a complete JSON object is received the readyRead signal is emitted,
 * after which the JSON object is available through the read method. Similarly, the
 * write method will transmit a JSON object through the socket to the receiver.
 */
class JsonSocket : public QObject {
Q_OBJECT
public:
    JsonSocket(std::unique_ptr<QTcpSocket> socket, std::string secret);
    virtual ~JsonSocket() = default;

    void connectToHost(const std::string& host, int port);
    QTcpSocket::SocketState state() const;

    void write(nlohmann::json json);

    std::string localAddress() const;
    std::string peerAddress() const;

signals:
    void messageReceived(nlohmann::json message);
    void disconnected();

private:
    void readToBuffer();
    void parseBuffer();

    std::unique_ptr<QTcpSocket> _socket;
    std::optional<SimpleCrypt> _crypto;
    std::vector<char> _buffer;
    int _payloadSize = -1;
};

} // namespace common

#endif // __COMMON__JSONSOCKET_H__

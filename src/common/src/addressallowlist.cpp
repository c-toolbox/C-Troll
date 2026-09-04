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

#include "addressallowlist.h"

namespace {
    // A dual-stack listening socket reports IPv4 peers as ::ffff:a.b.c.d, which would not
    // match an entry that was written as a plain IPv4 address
    QHostAddress normalized(const QHostAddress& address) {
        bool isIpv4 = false;
        const quint32 ipv4 = address.toIPv4Address(&isIpv4);
        return isIpv4 ? QHostAddress(ipv4) : address;
    }
} // namespace

namespace common {

AddressAllowList::AddressAllowList(const std::vector<std::string>& entries) {
    for (const std::string& entry : entries) {
        const QString value = QString::fromStdString(entry).trimmed();
        if (value.isEmpty()) {
            continue;
        }

        if (value.compare(QLatin1String("localhost"), Qt::CaseInsensitive) == 0) {
            _subnets.emplace_back(QHostAddress(QHostAddress::LocalHost), 32);
            _subnets.emplace_back(QHostAddress(QHostAddress::LocalHostIPv6), 128);
            continue;
        }

        // Handles both literal addresses and CIDR notation; -1 signals a parsing failure
        const QPair<QHostAddress, int> subnet = QHostAddress::parseSubnet(value);
        if (subnet.second == -1) {
            _invalidEntries.push_back(entry);
            continue;
        }
        _subnets.emplace_back(subnet.first, subnet.second);
    }
}

bool AddressAllowList::isEmpty() const {
    return _subnets.empty();
}

bool AddressAllowList::contains(const QHostAddress& address) const {
    if (address.isNull()) {
        return false;
    }

    const QHostAddress addr = normalized(address);
    for (const std::pair<QHostAddress, int>& subnet : _subnets) {
        if (addr.isInSubnet(subnet.first, subnet.second)) {
            return true;
        }
    }
    return false;
}

bool AddressAllowList::contains(const std::string& address) const {
    return contains(QHostAddress(QString::fromStdString(address)));
}

const std::vector<std::string>& AddressAllowList::invalidEntries() const {
    return _invalidEntries;
}

} // namespace common

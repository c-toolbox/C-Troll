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

#ifndef __COMMON__ADDRESSALLOWLIST_H__
#define __COMMON__ADDRESSALLOWLIST_H__

#include <QHostAddress>
#include <string>
#include <utility>
#include <vector>

namespace common {

/**
 * Matches the address of a connecting peer against a list of entries that were provided
 * in a configuration file. Each entry is either a literal IPv4/IPv6 address, a subnet in
 * CIDR notation (`192.168.0.0/24`), or the special value `localhost`. Entries that cannot
 * be parsed are collected in \m invalidEntries so that the caller can report them.
 *
 * An instance that was created from an empty list matches nothing. Deciding whether that
 * should mean "reject everything" or "accept everything" is left to the caller.
 */
class AddressAllowList {
public:
    AddressAllowList() = default;
    explicit AddressAllowList(const std::vector<std::string>& entries);

    /// Returns \c true if no usable entry was provided
    bool isEmpty() const;

    bool contains(const QHostAddress& address) const;
    bool contains(const std::string& address) const;

    /// The entries that were neither a valid address nor a valid subnet
    const std::vector<std::string>& invalidEntries() const;

private:
    std::vector<std::pair<QHostAddress, int>> _subnets;
    std::vector<std::string> _invalidEntries;
};

} // namespace common

#endif // __COMMON__ADDRESSALLOWLIST_H__

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

#ifndef __COMMON__COMMANDLINEPARSING_H__
#define __COMMON__COMMANDLINEPARSING_H__

#include <optional>
#include <string>
#include <vector>

namespace common {

/**
 * This function returns \c true iff the commandline `--debug` was passed through the list
 * of commandline arguments.  If the parameter does not occur, \c false is returned.
 *
 * \param The list of commandline arguments. The best way to generate this from the
 *        standard argc+argv construct is through: `{ argv, argv + argc }`
 * \return \c true if `--debug` occurred in the commandline arguments, \c false otherwise
 */
bool parseDebugCommandlineArgument(std::vector<std::string> args);

/**
 * This function returns the desired window position if `--pos` was provided to the
 * application, or `std::nullopt` if the argument was not present.
 *
 * \param The list of commandline arguments. The best way to generate this from the
 *        standard argc+argv construct is through: `{ argv, argv + argc }`
 * \return Either `std::nullopt` if the commandline flag was not found or the pixel
 *         position provided as arguments
 */
std::optional<std::pair<int, int>> parseLocationArgument(std::vector<std::string> args);

} // namespace common

#endif // __COMMON__COMMANDLINEPARSING_H__

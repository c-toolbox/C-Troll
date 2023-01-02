/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                               *
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

#include "jsonvalidation.h"

#include <nlohmann/json-schema.hpp>
#include <QFile>

namespace {
    //nlohmann::json_schema::json_validator ApplicationCTroll
} // namespace

namespace validation {

void ErrorHandler::error(const nlohmann::json::json_pointer& ptr,
                         const nlohmann::json& instance, const std::string& msg)
{
    nlohmann::json_schema::basic_error_handler::error(ptr, instance, msg);
    path = ptr.to_string();
    if (path.empty()) {
        path = "/";
    }
    message = msg;
}

JsonError::JsonError(ErrorHandler handler)
    : std::runtime_error(fmt::format("Error at {}: {}", handler.path, handler.message))
    , path(std::move(handler.path))
    , message(std::move(handler.message))
{}

nlohmann::json_schema::json_validator loadValidator(std::string path) {
    QFile file = QFile(QString::fromStdString(path));
    file.open(QFile::ReadOnly | QFile::Text);
    QByteArray ba = file.readAll();
    std::string content = std::string(ba.constData(), ba.length());
    nlohmann::json schema = nlohmann::json::parse(content);
    return nlohmann::json_schema::json_validator(schema);
}

} // namespace validation

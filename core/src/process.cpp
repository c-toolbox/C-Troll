/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sund�n, Emil Axelsson                                            *
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

#include "process.h"

#include "database.h"
#include "program.h"
#include "logging.h"
#include <assert.h>

common::CommandMessage startProcessCommand(const Process& process) {
    Program* program = data::findProgram(process.programId);
    const Program::Configuration& configuration = data::findConfigurationForProgram(
        *program, process.configurationId
    );

    common::CommandMessage t;
    t.id = process.id.v;
    t.executable = program->executable;
    t.workingDirectory = program->workingDirectory;
    t.command = common::CommandMessage::Command::Start;

    t.commandlineParameters =
        program->commandlineParameters + ' ' + configuration.parameters;

    return t;
}

common::CommandMessage exitProcessCommand(const Process& process) {
    common::CommandMessage t;
    t.id = process.id.v;
    t.command = common::CommandMessage::Command::Exit;
    return t;
}

int Process::nextId = 0;

Process::Process(Program::ID programId, Program::Configuration::ID configurationId,
                 Cluster::ID clusterId, Node::ID nodeId)
    : id{ nextId++ }
    , programId(programId)
    , configurationId(configurationId)
    , clusterId(clusterId)
    , nodeId(nodeId)
    , status(common::ProcessStatusMessage::Status::Unknown)
{}
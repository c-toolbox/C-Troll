/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
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

#include "process.h"

#include "database.h"
#include "program.h"
#include "logging.h"
#include <assert.h>

common::StartCommandMessage startProcessCommand(const Process& process) {
    const Program* program = data::findProgram(process.programId);
    assert(program);
    const Program::Configuration* configuration = data::findConfigurationForProgram(
        *program,
        process.configurationId
    );
    assert(configuration);

    common::StartCommandMessage t;
    t.id = process.id.v;
    t.executable = program->executable;
    t.workingDirectory = program->workingDirectory;

    if (!program->commandlineParameters.empty()) {
        t.commandlineParameters = program->commandlineParameters;
    }
    if (!configuration->parameters.empty()) {
        t.commandlineParameters += ' ';
        t.commandlineParameters += configuration->parameters;
    }

    t.programId = process.programId.v;
    t.configurationId = process.configurationId.v;
    t.clusterId = process.clusterId.v;
    t.nodeId = process.nodeId.v;
    t.forwardStdOutStdErr = program->shouldForwardMessages;
    t.dataHash = data::dataHash();

    return t;
}


//////////////////////////////////////////////////////////////////////////////////////////


common::ExitCommandMessage exitProcessCommand(const Process& process) {
    common::ExitCommandMessage t;
    t.id = process.id.v;
    return t;
}


//////////////////////////////////////////////////////////////////////////////////////////


int Process::nextId = 0;

Process::Process(Program::ID programId_, Program::Configuration::ID configurationId_,
                 Cluster::ID clusterId_, Node::ID nodeId_)
    : id(nextId++)
    , programId(programId_)
    , configurationId(configurationId_)
    , clusterId(clusterId_)
    , nodeId(nodeId_)
    , status(common::ProcessStatusMessage::Status::Unknown)
{}

Process::Process(ID id_, Program::ID programId_,
                 Program::Configuration::ID configurationId_, Cluster::ID clusterId_,
                 Node::ID nodeId_)
    : id(id_)
    , programId(programId_)
    , configurationId(configurationId_)
    , clusterId(clusterId_)
    , nodeId(nodeId_)
    , status(common::ProcessStatusMessage::Status::Unknown)
{}

void Process::setNextIdIfHigher(int id) {
    nextId = std::max(nextId, id);
}


/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                             *
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
#include <fmt/format.h>
#include <assert.h>

common::StartCommandMessage startProcessCommand(const Process& process) {
    const Program* program = data::findProgram(process.programId);
    assert(program);
    const Program& prg = *program;

    const Program::Configuration* configuration = data::findConfigurationForProgram(
        prg,
        process.configurationId
    );
    assert(configuration);
    const Program::Configuration& conf = *configuration;

    const Cluster* cluster = data::findCluster(process.clusterId);
    assert(cluster);
    auto it = std::find_if(
        program->clusters.begin(),
        program->clusters.end(),
        [cluster](const Program::Cluster& c) { return c.name == cluster->name; }
    );
    assert(it != program->clusters.end());

    common::StartCommandMessage t;
    t.id = process.id.v;
    t.executable = prg.executable;
    t.workingDirectory = prg.workingDirectory;
    t.commandlineParameters = fmt::format(
        "{} {} {}", prg.commandlineParameters, conf.parameters, it->parameters
    );
    t.programId = process.programId.v;
    t.configurationId = process.configurationId.v;
    t.clusterId = process.clusterId.v;
    t.nodeId = process.nodeId.v;
    t.forwardStdOutStdErr = prg.shouldForwardMessages;
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

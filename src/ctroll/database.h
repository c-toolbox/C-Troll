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

#ifndef __CTROLL__DATABASE_H__
#define __CTROLL__DATABASE_H__

#include "cluster.h"
#include "color.h"
#include "node.h"
#include "process.h"
#include "program.h"
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace data {

std::vector<const Cluster*> clusters();
std::vector<const Node*> nodes();
std::vector<const Program*> programs();
std::vector<const Process*> processes();

const Cluster* findCluster(Cluster::ID id);
const Cluster* findCluster(std::string_view name);
std::vector<const Cluster*> findClustersForProgram(const Program& program);
std::vector<const Cluster*> findClusterForNode(const Node& node);

const Node* findNode(Node::ID id);
const Node* findNode(std::string_view name);
std::vector<const Node*> findNodesForCluster(const Cluster& cluster);
void setNodeConnecting(Node::ID id, bool connected);
void setNodeConnected(Node::ID id, bool connected);
void setNodeDisconnecting(Node::ID id);

const Program* findProgram(Program::ID id);
const Program* findProgram(std::string_view name);

const Program::Configuration* findConfigurationForProgram(const Program& program,
    Program::Configuration::ID id);
const Program::Configuration* findConfigurationForProgram(const Program& program,
    std::string_view name);

bool hasTag(Program::ID id, const std::vector<std::string>& tags);
std::set<std::string> findTags();

const Process* findProcess(Process::ID id);
void addProcess(std::unique_ptr<Process> process);
void setProcessStatus(Process::ID id, common::ProcessStatusMessage::Status status);

Color colorForTag(std::string_view tag);
void setTagColors(std::vector<Color> colors);

[[nodiscard]] bool loadData(std::string_view programPath, std::string_view clusterPath,
    std::string_view nodePath);

std::size_t dataHash();

} // namespace data

#endif // __CTROLL__DATABASE_H__

/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
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

#ifndef __CORE__DATABASE_H__
#define __CORE__DATABASE_H__

#include "cluster.h"
#include "configuration.h"
#include "node.h"
#include "process.h"
#include "program.h"
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace data {
    
std::vector<Cluster*> clusters();
std::vector<Node*> nodes();
std::vector<Program*> programs();
std::vector<Process*> processes();

Cluster* findCluster(Cluster::ID id);
Cluster* findCluster(const std::string& name);
std::vector<Cluster*> findClustersForProgram(const Program& program);
std::vector<Cluster*> findClusterForNode(const Node& node);

Node* findNode(Node::ID id);
Node* findNode(const std::string& name);
std::vector<Node*> findNodesForCluster(const Cluster& cluster);

Program* findProgram(Program::ID id);
const Program::Configuration& findConfigurationForProgram(const Program& program, 
    Program::Configuration::ID id);
bool hasTag(Program::ID id, const std::vector<std::string>& tags);
std::set<std::string> findTags();

Process* findProcess(Process::ID id);
void addProcess(std::unique_ptr<Process> process);

Color colorForTag(const std::string& tag);
void setTagColors(std::vector<Color> colors);

void loadData(const std::string& programPath, const std::string& clusterPath,
    const std::string& nodePath);

std::size_t dataHash();

} // namespace data

#endif // __CORE__DATABASE_H__

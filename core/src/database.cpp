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

#include "database.h"

#include "cluster.h"
#include "program.h"
#include "process.h"

namespace {

std::vector<std::unique_ptr<Cluster>> gClusters;
std::vector<std::unique_ptr<Program>> gPrograms;
std::vector<std::unique_ptr<Process>> gProcesses;

} // namespace

namespace data {

std::vector<Cluster*> clusters() {
    std::vector<Cluster*> clusters;
    for (const std::unique_ptr<Cluster>& c : gClusters) {
        clusters.push_back(c.get());
    }
    return clusters;
}

std::vector<Program*> programs() {
    std::vector<Program*> programs;
    for (const std::unique_ptr<Program>& c : gPrograms) {
        programs.push_back(c.get());
    }
    return programs;
}

std::vector<Process*> processes() {
    std::vector<Process*> processes;
    for (const std::unique_ptr<Process>& c : gProcesses) {
        processes.push_back(c.get());
    }
    return processes;
}


//////////////////////////////////////////////////////////////////////////////////////////


void addProcess(std::unique_ptr<Process> process) {
    gProcesses.push_back(std::move(process));
}

Process* findProcess(int id) {
    const auto it = std::find_if(
        gProcesses.begin(), gProcesses.end(),
        [id](const std::unique_ptr<Process>& p) {
            return p->id == id;
        }
    );
    return it != gProcesses.end() ? it->get() : nullptr;
}


//////////////////////////////////////////////////////////////////////////////////////////


void loadPrograms(const std::string& path) {
    gPrograms.clear();

    std::vector<Program> programs = loadProgramsFromDirectory(path);
    for (Program& program : programs) {
        std::unique_ptr<Program> p = std::make_unique<Program>(std::move(program));
        gPrograms.push_back(std::move(p));
    }
}

void loadClusters(const std::string& path) {
    gClusters.clear();

    std::vector<Cluster> clusters = loadClustersFromDirectory(path);
    for (Cluster& cluster : clusters) {
        std::unique_ptr<Cluster> c = std::make_unique<Cluster>(std::move(cluster));
        gClusters.push_back(std::move(c));
    }
}


} // namespace data

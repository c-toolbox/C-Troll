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

#ifndef __CORE__CLUSTERWIDGET_H__
#define __CORE__CLUSTERWIDGET_H__

#include <QWidget>

#include <QLabel>
#include "cluster.h"
#include <vector>

class ClusterWidget : public QWidget {
Q_OBJECT
public:
    ClusterWidget(const Cluster& cluster);

    void updateConnectionStatus(const Cluster::Node& node);

private:
    const Cluster& _cluster;
    QLabel* _connectionLabel;
    std::vector<QLabel*> _nodeConnectionLabels;
};


//////////////////////////////////////////////////////////////////////////////////////////


class ClustersWidget : public QWidget {
    Q_OBJECT
public:
    ClustersWidget(const std::vector<Cluster>& clusters);

public slots:
    void connectedStatusChanged(const Cluster& cluster, const Cluster::Node& node);

private:
    const std::vector<Cluster>& _clusters;
    std::vector<ClusterWidget*> _clusterWidgets;
};

#endif // __CORE__CLUSTERWIDGET_H__

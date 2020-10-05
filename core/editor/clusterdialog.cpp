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

#include "clusterdialog.h"

#include "cluster.h"
#include "jsonload.h"
#include "node.h"
#include <QCheckBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <filesystem>

ClusterDialog::ClusterDialog(QWidget* parent, std::string clusterPath,
                             std::string nodePath)
    : QDialog(parent)
    , _clusterPath(std::move(clusterPath))
    , _nodePath(std::move(nodePath))
{
    assert(!_clusterPath.empty());

    setWindowTitle(QString::fromStdString("Cluster: " + _clusterPath));

    QBoxLayout* layout = new QVBoxLayout(this);

    {
        QWidget* edit = new QWidget;
        QGridLayout* editLayout = new QGridLayout(edit);
        editLayout->setMargin(0);

        editLayout->addWidget(new QLabel("Name:"), 0, 0);
        _name = new QLineEdit;
        editLayout->addWidget(_name, 0, 1);

        editLayout->addWidget(new QLabel("Enabled:"), 1, 0);
        _enabled = new QCheckBox;
        editLayout->addWidget(_enabled, 1, 1);

        QWidget* spacer = new QWidget;
        spacer->setObjectName("spacer");
        editLayout->addWidget(spacer, 2, 0);

        editLayout->addWidget(new QLabel("Nodes"), 3, 0);

        QPushButton* newNode = new QPushButton("+");
        newNode->setObjectName("add");
        connect(
            newNode, &QPushButton::clicked,
            [this]() {
                std::string name = selectNode();
                if (!name.empty()) {
                    addNode(name);
                }
            }
        );
        editLayout->addWidget(newNode, 3, 1, Qt::AlignRight);

        QScrollArea* area = new QScrollArea;
        area->setWidgetResizable(true);
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        
        QWidget* nodeContainer = new QWidget;
        area->setWidget(nodeContainer);

        _nodeLayout = new QVBoxLayout(nodeContainer);
        _nodeLayout->setAlignment(Qt::AlignTop);
        _nodeLayout->setMargin(0);
        _nodeLayout->setContentsMargins(0, 0, 0, 0);
        _nodeLayout->setSpacing(0);
        editLayout->addWidget(area, 4, 0, 1, 2);
        editLayout->setRowStretch(4, 1);

        layout->addWidget(edit);
    }

    {
        QWidget* control = new QWidget;
        QBoxLayout* controlLayout = new QHBoxLayout(control);

        controlLayout->addStretch(5);

        QPushButton* save = new QPushButton("Save");
        connect(save, &QPushButton::clicked, this, &ClusterDialog::save);
        controlLayout->addWidget(save);

        QPushButton* cancel = new QPushButton("Cancel");
        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
        controlLayout->addWidget(cancel);

        layout->addWidget(control);
    }

    if (std::filesystem::exists(_clusterPath)) {
        Cluster cluster = common::loadFromJson<Cluster>(_clusterPath);

        _name->setText(QString::fromStdString(cluster.name));
        _enabled->setChecked(cluster.isEnabled);
        for (const std::string& node : cluster.nodes) {
            addNode(node);
        }
    }
}

void ClusterDialog::save() {
    if (_name->text().isEmpty()) {
        QMessageBox::critical(this, "Error", "Name must not be empty");
        return;
    }

    if (_nodes.empty()) {
        QMessageBox::critical(this, "Error", "Need at least one node");
        return;
    }

    for (QLabel* node : _nodes) {
        if (node->text().isEmpty()) {
            QMessageBox::critical(this, "Error", "Node name must not be empty");
            return;
        }
    }


    Cluster cluster;
    cluster.name = _name->text().toStdString();
    cluster.isEnabled = _enabled->isChecked();
    for (QLabel* node : _nodes) {
        cluster.nodes.push_back(node->text().toStdString());
    }

    if (std::filesystem::path(_clusterPath).extension().empty()) {
        common::saveToJson(_clusterPath + ".json", cluster);
    }
    else {
        common::saveToJson(_clusterPath, cluster);
    }

    accept();
}

std::string ClusterDialog::selectNode() {
    std::vector<Node> nodes = common::loadJsonFromDirectory<Node>(_nodePath);
    QStringList list;
    for (const Node& node : nodes) {
        list.push_back(QString::fromStdString(node.name));
    }

    QString selected = QInputDialog::getItem(
        this,
        "Add Node",
        "Select the node to add",
        list
    );

    return !selected.isEmpty() ? selected.toStdString() : "";
}

QLabel* ClusterDialog::addNode(std::string name) {
    QWidget* container = new QWidget;
    QBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(10, 5, 10, 5);

    QLabel* node = new QLabel(QString::fromStdString(name));
    node->setTextInteractionFlags(Qt::TextSelectableByMouse);
    node->setCursor(QCursor(Qt::IBeamCursor));
    layout->addWidget(node);

    QPushButton* remove = new QPushButton("X");
    remove->setObjectName("remove");
    connect(remove, &QPushButton::clicked, [this, node]() { removeNode(node); });
    layout->addWidget(remove);

    _nodeLayout->addWidget(container);
    _nodes.push_back(node);
    return node;
}

void ClusterDialog::removeNode(QLabel* sender) {
    const auto it = std::find(_nodes.cbegin(), _nodes.cend(), sender);
    assert(it != _nodes.cend());

    _nodes.erase(it);
    _nodeLayout->removeWidget(sender->parentWidget());
    sender->deleteLater();
}

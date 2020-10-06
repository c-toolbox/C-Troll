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

#include "addbutton.h"
#include "cluster.h"
#include "jsonload.h"
#include "node.h"
#include "removebutton.h"
#include "spacer.h"
#include <QCheckBox>
#include <QDialogButtonBox>
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
        _name->setToolTip("The name of the cluster defined in this file");
        connect(_name, &QLineEdit::textChanged, this, &ClusterDialog::updateSaveButton);
        editLayout->addWidget(_name, 0, 1);

        editLayout->addWidget(new QLabel("Enabled:"), 1, 0);
        _enabled = new QCheckBox;
        _enabled->setToolTip("Determines whether this cluster is currenty used");
        editLayout->addWidget(_enabled, 1, 1);

        editLayout->addWidget(new Spacer, 2, 0);

        editLayout->addWidget(new QLabel("Nodes"), 3, 0);

        QPushButton* newNode = new AddButton;
        connect(newNode, &QPushButton::clicked, this, &ClusterDialog::addNewNode);
        editLayout->addWidget(newNode, 3, 1, Qt::AlignRight);

        _nodes = new DynamicList;
        _nodes->setToolTip("The nodes that belong to this cluster");
        connect(
            _nodes, &DynamicList::updated,
            this, &ClusterDialog::updateSaveButton
        );
        editLayout->addWidget(_nodes, 4, 0, 1, 2);
        editLayout->setRowStretch(4, 1);

        layout->addWidget(edit);
    }

    QDialogButtonBox* box = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel
    );
    _saveButton = box->button(QDialogButtonBox::Save);
    connect(box, &QDialogButtonBox::accepted, this, &ClusterDialog::save);
    connect(box, &QDialogButtonBox::rejected, this, &ClusterDialog::reject);
    layout->addWidget(box, 0, Qt::AlignRight);


    if (std::filesystem::exists(_clusterPath)) {
        Cluster cluster = common::loadFromJson<Cluster>(_clusterPath);
        std::vector<Node> nodes = common::loadJsonFromDirectory<Node>(_nodePath);

        _name->setText(QString::fromStdString(cluster.name));
        _enabled->setChecked(cluster.isEnabled);
        for (const std::string& node : cluster.nodes) {
            QLabel* nodeLabel = new QLabel(QString::fromStdString(node));
            _nodes->addItem(nodeLabel);

            const auto it = std::find_if(
                nodes.cbegin(), nodes.cend(),
                [node](const Node& n) { return n.name == node; }
            );
            if (it == nodes.cend()) {
                nodeLabel->setObjectName("invalid");
                nodeLabel->setToolTip("Could not find node in nodes folder");
            }
        }
    }

    updateSaveButton();
}

void ClusterDialog::save() {
    Cluster cluster;
    cluster.name = _name->text().toStdString();
    cluster.isEnabled = _enabled->isChecked();
    for (QLabel* node : _nodes->items<QLabel>()) {
        cluster.nodes.push_back(node->text().toStdString());
    }
    common::saveToJson(_clusterPath, cluster);
    accept();
}

void ClusterDialog::updateSaveButton() {
    const bool hasItems = !_nodes->items<QLabel>().empty();
    _saveButton->setEnabled(!_name->text().isEmpty() && hasItems);
}

void ClusterDialog::addNewNode() {
    std::vector<Node> nodes = common::loadJsonFromDirectory<Node>(_nodePath);

    // Remove the nodes that have already been added
    std::vector<QLabel*> currentNodes = _nodes->items<QLabel>();
    nodes.erase(
        std::remove_if(
            nodes.begin(), nodes.end(),
            [&currentNodes](const Node& n) {
                const auto it = std::find_if(
                    currentNodes.cbegin(), currentNodes.cend(),
                    [n](QLabel* l) { return l->text().toStdString() == n.name; }
                );
                return it != currentNodes.cend();
            }
        ),
        nodes.end()
    );

    if (nodes.empty()) {
        QMessageBox::information(this, "Add node", "No available nodes left to add");
        return;
    }


    QStringList list;
    for (const Node& node : nodes) {
        list.push_back(QString::fromStdString(node.name));
    }

    QString n = QInputDialog::getItem(this, "Add Node", "Select the node to add", list);

    if (!n.isEmpty()) {
        _nodes->addItem(new QLabel(n));
        updateSaveButton();
    }
}

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

#include "clusterdialog.h"

#include "addbutton.h"
#include "cluster.h"
#include "jsonload.h"
#include "node.h"
#include "removebutton.h"
#include "spacer.h"
#include <fmt/format.h>
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

bool operator==(const Node& node, const std::string& name) {
    return node.name == name;
}

ClusterDialog::ClusterDialog(QWidget* parent, std::string clusterPath,
                             std::string nodePath)
    : QDialog(parent)
    , _clusterPath(std::move(clusterPath))
    , _nodePath(std::move(nodePath))
{
    assert(!_clusterPath.empty());

    setWindowTitle(QString::fromStdString(fmt::format("Cluster: {}", _clusterPath)));

    QBoxLayout* layout = new QVBoxLayout(this);

    {
        QWidget* edit = new QWidget;
        QGridLayout* editLayout = new QGridLayout(edit);
        editLayout->setContentsMargins(0, 0, 0, 0);

        editLayout->addWidget(new QLabel("Name:"), 0, 0);
        _name = new QLineEdit;
        _name->setToolTip("The name of the cluster defined in this file");
        connect(_name, &QLineEdit::textChanged, this, &ClusterDialog::updateSaveButton);
        editLayout->addWidget(_name, 0, 1);

        editLayout->addWidget(new QLabel("Enabled:"), 1, 0);
        _enabled = new QCheckBox;
        _enabled->setToolTip("Determines whether this cluster is currently used");
        editLayout->addWidget(_enabled, 1, 1);

        editLayout->addWidget(new QLabel("Description:"), 2, 0);
        _description = new QLineEdit;
        _description->setToolTip("Additional information for the user about the cluster");
        _description->setPlaceholderText("optional");
        editLayout->addWidget(_description, 2, 1);

        
        editLayout->addWidget(new Spacer, 3, 0, 1, 2);


        editLayout->addWidget(new QLabel("Nodes"), 4, 0);

        QPushButton* newNode = new AddButton;
        connect(newNode, &QPushButton::clicked, this, &ClusterDialog::addNewNode);
        editLayout->addWidget(newNode, 4, 1, Qt::AlignRight);

        _nodes = new DynamicList;
        _nodes->setToolTip("The nodes that belong to this cluster");
        connect(_nodes, &DynamicList::updated, this, &ClusterDialog::updateSaveButton);
        editLayout->addWidget(_nodes, 5, 0, 1, 2);
        editLayout->setRowStretch(5, 1);

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
        std::pair<std::vector<Node>, bool> nodes =
            common::loadJsonFromDirectory<Node>(_nodePath);

        _name->setText(QString::fromStdString(cluster.name));
        _name->setCursorPosition(0);
        _enabled->setChecked(cluster.isEnabled);
        _description->setText(QString::fromStdString(cluster.description));
        _description->setCursorPosition(0);
        for (const std::string& node : cluster.nodes) {
            QLabel* nodeLabel = new QLabel(QString::fromStdString(node));
            _nodes->addItem(nodeLabel);

            const auto it = std::find(nodes.first.cbegin(), nodes.first.cend(), node);
            if (it == nodes.first.cend()) {
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
    cluster.description = _description->text().toStdString();
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

bool operator==(QLabel* name, const Node& node) {
    return node.name == name->text().toStdString();
}

void ClusterDialog::addNewNode() {
    std::pair<std::vector<Node>, bool> nodes =
        common::loadJsonFromDirectory<Node>(_nodePath);

    // Remove the nodes that have already been added
    std::vector<QLabel*> currentNodes = _nodes->items<QLabel>();
    nodes.first.erase(
        std::remove_if(
            nodes.first.begin(), nodes.first.end(),
            [&currentNodes](const Node& n) {
                const auto it = std::find(currentNodes.cbegin(), currentNodes.cend(), n);
                return it != currentNodes.cend();
            }
        ),
        nodes.first.end()
    );

    if (nodes.first.empty()) {
        QMessageBox::information(this, "Add node", "No available nodes left to add");
        return;
    }


    QStringList list;
    for (const Node& node : nodes.first) {
        list.push_back(QString::fromStdString(node.name));
    }

    bool ok;
    QString n = QInputDialog::getItem(
        this,
        "Add Node",
        "Select the node to add",
        list,
        0,
        true,
        &ok
    );

    if (ok) {
        _nodes->addItem(new QLabel(n));
        updateSaveButton();
    }
}

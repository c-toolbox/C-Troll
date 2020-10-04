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
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <filesystem>

ClusterDialog::ClusterDialog(QWidget* parent, std::string path)
    : QDialog(parent)
    , _path(std::move(path))
{
    assert(!_path.empty());

    setWindowTitle(QString::fromStdString("Cluster: " + _path));

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

        editLayout->addWidget(new QLabel("Nodes"), 2, 0);

        QPushButton* newNode = new QPushButton("+");
        connect(newNode, &QPushButton::clicked, this, &ClusterDialog::addNode);
        editLayout->addWidget(newNode, 2, 1);

        QScrollArea* area = new QScrollArea;
        area->setWidgetResizable(true);
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        
        QWidget* nodeContainer = new QWidget;
        area->setWidget(nodeContainer);

        _nodeLayout = new QVBoxLayout(nodeContainer);
        _nodeLayout->setAlignment(Qt::AlignTop);
        editLayout->addWidget(area, 3, 0, 1, 2);

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

    if (std::filesystem::exists(_path)) {
        Cluster cluster = common::loadFromJson<Cluster>(_path);

        _name->setText(QString::fromStdString(cluster.name));
        _enabled->setChecked(cluster.isEnabled);
        for (const std::string& node : cluster.nodes) {
            QLineEdit* line = addNode();
            line->setText(QString::fromStdString(node));
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

    for (QLineEdit* node : _nodes) {
        if (node->text().isEmpty()) {
            QMessageBox::critical(this, "Error", "Node name must not be empty");
            return;
        }
    }


    Cluster cluster;
    cluster.name = _name->text().toStdString();
    cluster.isEnabled = _enabled->isChecked();
    for (QLineEdit* node : _nodes) {
        cluster.nodes.push_back(node->text().toStdString());
    }

    if (std::filesystem::path(_path).extension().empty()) {
        common::saveToJson(_path + ".json", cluster);
    }
    else {
        common::saveToJson(_path, cluster);
    }

    accept();
}

QLineEdit* ClusterDialog::addNode() {
    QWidget* container = new QWidget;
    QBoxLayout* layout = new QHBoxLayout(container);

    QLineEdit* node = new QLineEdit;
    layout->addWidget(node);

    QPushButton* remove = new QPushButton("X");
    connect(remove, &QPushButton::clicked, [this, node]() { removeNode(node); });
    layout->addWidget(remove);

    _nodeLayout->addWidget(container);
    _nodes.push_back(node);
    return node;
}

void ClusterDialog::removeNode(QLineEdit* sender) {
    const auto it = std::find(_nodes.cbegin(), _nodes.cend(), sender);
    assert(it != _nodes.cend());

    _nodes.erase(it);
    _nodeLayout->removeWidget(sender->parentWidget());
    sender->deleteLater();
}

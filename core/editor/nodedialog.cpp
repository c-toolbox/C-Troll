/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sund�n, Emil Axelsson                                            *
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

#include "nodedialog.h"

#include "jsonload.h"
#include "node.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <filesystem>

NodeDialog::NodeDialog(QWidget* parent, std::string path)
    : QDialog(parent)
    , _path(std::move(path))
{
    assert(!_path.empty());

    setWindowTitle(QString::fromStdString("Node: " + _path));

    QBoxLayout* layout = new QVBoxLayout(this);

    {
        QWidget* edit = new QWidget;
        QGridLayout* editLayout = new QGridLayout(edit);
        editLayout->setMargin(0);

        editLayout->addWidget(new QLabel("Name:"), 0, 0);
        _name = new QLineEdit;
        editLayout->addWidget(_name, 0, 1);

        editLayout->addWidget(new QLabel("IP:"), 1, 0);
        _ip = new QLineEdit;
        editLayout->addWidget(_ip, 1, 1);

        editLayout->addWidget(new QLabel("Port:"), 2, 0);
        _port = new QSpinBox;
        _port->setMinimum(0);
        _port->setMaximum(std::numeric_limits<int>::max());
        editLayout->addWidget(_port, 2, 1);

        editLayout->addWidget(new QLabel("Secret"), 3, 0);
        _secret = new QLineEdit;
        editLayout->addWidget(_secret, 3, 1);

        layout->addWidget(edit);
    }

    {
        QWidget* control = new QWidget;
        QBoxLayout* controlLayout = new QHBoxLayout(control);

        QPushButton* save = new QPushButton("Save");
        connect(save, &QPushButton::clicked, this, &NodeDialog::save);
        controlLayout->addWidget(save);

        QPushButton* cancel = new QPushButton("Cancel");
        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
        controlLayout->addWidget(cancel);


        layout->addWidget(control, 0, Qt::AlignRight);
    }

    if (std::filesystem::exists(_path)) {
        Node node = common::loadFromJson<Node>(_path);

        _name->setText(QString::fromStdString(node.name));
        _ip->setText(QString::fromStdString(node.ipAddress));
        _port->setValue(node.port);
        _secret->setText(QString::fromStdString(node.secret));
    }
}

void NodeDialog::save() {
    if (_name->text().isEmpty()) {
        QMessageBox::critical(this, "Error", "Name must not be empty");
        return;
    }

    if (_ip->text().isEmpty()) {
        QMessageBox::critical(this, "Error", "IP must not be empty");
        return;
    }

    Node node;
    node.name = _name->text().toStdString();
    node.ipAddress = _ip->text().toStdString();
    node.port = _port->value();
    node.secret = _secret->text().toStdString();

    if (std::filesystem::path(_path).extension().empty()) {
        common::saveToJson(_path + ".json", node);
    }
    else {
        common::saveToJson(_path, node);
    }

    accept();
}

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

#include "dynamiclist.h"

#include "removebutton.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

DynamicList::DynamicList() {
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* container = new QWidget;
    setWidget(container);
    //widget()->layout()

    _layout = new QVBoxLayout(container);
    _layout->setAlignment(Qt::AlignTop);
    _layout->setMargin(0);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
}

QLabel* DynamicList::addItem(std::string name) {
    QWidget* container = new QWidget;
    QBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(10, 5, 10, 5);

    QLabel* node = new QLabel(QString::fromStdString(name));
    node->setTextInteractionFlags(Qt::TextSelectableByMouse);
    node->setCursor(QCursor(Qt::IBeamCursor));
    layout->addWidget(node);

    QPushButton* remove = new RemoveButton;
    connect(
        remove, &QPushButton::clicked,
        [this, node]() { removeItem(node); }
    );
    layout->addWidget(remove);

    _layout->addWidget(container);
    _items.push_back(node);
    return node;
}

std::vector<std::string> DynamicList::items() const {
    std::vector<std::string> res;
    res.reserve(_items.size());
    for (QLabel* label : _items) {
        res.push_back(label->text().toStdString());
    }
    return res;
}

bool DynamicList::empty() const {
    return _items.empty();
}

void DynamicList::removeItem(QLabel* sender) {
    const auto it = std::find(_items.cbegin(), _items.cend(), sender);
    assert(it != _items.cend());

    _items.erase(it);
    _layout->removeWidget(sender->parentWidget());
    sender->parent()->deleteLater();

    emit updated();
}

/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2023                                                             *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
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

#include "mainwindow.h"

#include "clusterdialog.h"
#include "nodedialog.h"
#include "programdialog.h"
#include "version.h"
#include <QFileDialog>
#include <QFileSystemModel>
#include <QGridLayout>
#include <QLabel>
#include <QMimeData>
#include <QPushButton>
#include <QTreeView>
#include <QWidget>
#include <fmt/format.h>
#include <filesystem>
#include <functional>

#include <Windows.h>
#include <iostream>

namespace {
    QTreeView* createTreeView(const std::string& folder,
                              std::function<void(const std::string&)> editFunction)
    {
        QFileSystemModel* model = new QFileSystemModel;
        model->setRootPath(QString::fromStdString(folder));

        QTreeView* tree = new QTreeView;
        tree->setModel(model);
        tree->setAnimated(true);
        tree->setHeaderHidden(true);
        tree->setIndentation(20);
        tree->setColumnHidden(1, true);
        tree->setColumnHidden(2, true);
        tree->setColumnHidden(3, true);
        tree->setRootIndex(model->index(QString::fromStdString(folder)));

        QObject::connect(
            tree, &QTreeView::doubleClicked,
            [editFunction](const QModelIndex& idx) {
                const QFileSystemModel* m = dynamic_cast<const QFileSystemModel*>(
                    idx.model()
                );
                assert(m);

                if (!m->isDir(idx)) {
                    std::string path = m->filePath(idx).toStdString();
                    editFunction(path);
                }
            }
        );
        return tree;
    }
} // namespace

MainWindow::MainWindow(std::string applicationPath, std::string clusterPath,
                       std::string nodePath)
    : _applicationPath(std::move(applicationPath))
    , _clusterPath(std::move(clusterPath))
    , _nodePath(std::move(nodePath))
{
    setWindowTitle("Editor");
    setAcceptDrops(true);

    QWidget* center = new QWidget;
    QGridLayout* layout = new QGridLayout(center);
    setCentralWidget(center);

    {
        // Nodes
        layout->addWidget(new QLabel("Nodes"), 0, 0, Qt::AlignCenter);
        QTreeView* nodesTree = createTreeView(
            _nodePath,
            [this](const std::string& path) { editNode(path); }
        );

        layout->addWidget(nodesTree, 1, 0);
        QPushButton* newNode = new QPushButton("Add new Node");
        connect(
            newNode, &QPushButton::clicked,
            [this]() {
                std::string path = newFilePath(_nodePath);
                if (!path.empty()) {
                    editNode(path);
                }
            }
        );
        layout->addWidget(newNode, 2, 0);
    }

    {
        // Clusters
        layout->addWidget(new QLabel("Clusters"), 0, 1, Qt::AlignCenter);
        QTreeView* clusterTree = createTreeView(
            _clusterPath,
            [this](const std::string& path) { editCluster(path); }
        );
        layout->addWidget(clusterTree, 1, 1);
        QPushButton* newCluster = new QPushButton("Add new Cluster");
        connect(
            newCluster, &QPushButton::clicked,
            [this]() {
                std::string path = newFilePath(_clusterPath);
                if (!path.empty()) {
                    editCluster(path);
                }
            }
        );
        layout->addWidget(newCluster, 2, 1);
    }

    {
        // Application
        layout->addWidget(new QLabel("Application"), 0, 2, Qt::AlignCenter);
        QTreeView* applicationTree = createTreeView(
            _applicationPath,
            [this](const std::string& path) { editProgram(path); }
        );
        layout->addWidget(applicationTree, 1, 2);
        QPushButton* newApplication = new QPushButton("Add new Application");
        connect(
            newApplication, &QPushButton::clicked,
            [this]() {
                std::string path = newFilePath(_applicationPath);
                if (!path.empty()) {
                    editProgram(path);
                }
            }
        );
        layout->addWidget(newApplication, 2, 2);
    }

    {
        QLabel* version = new QLabel(QString::fromStdString(
            fmt::format("Version: {}", application::Version))
        );
        version->setObjectName("version");
        layout->addWidget(version, 3, 0, 1, 3, Qt::AlignLeft);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    std::filesystem::path p = event->mimeData()->text().toStdString();

    if (p.extension() == ".exe") {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event) {
    std::filesystem::path p = event->mimeData()->text().toStdString();
    std::filesystem::path configPath = _applicationPath / p.filename();
    configPath.replace_extension(".json");

    ProgramDialog dialog = ProgramDialog(this, configPath.string(), _clusterPath);
    
    std::string exe = p.string();
    if (exe.starts_with("file:///")) {
        using namespace std::string_view_literals;
        exe = exe.substr("file:///"sv.size());
    }
    dialog.setExecutableInformation(exe);
    
    dialog.exec();
}

std::string MainWindow::newFilePath(const std::string& path) {
    QString file = QFileDialog::getSaveFileName(
        this,
        "Save File",
        QString::fromStdString(path),
        "JSON file (*.json)"
    );
    if (file.isEmpty()) {
        return "";
    }

    return file.toStdString();
}

void MainWindow::editNode(std::string path) {
    NodeDialog(this, path).exec();
}

void MainWindow::editCluster(std::string path) {
    ClusterDialog(this, path, _nodePath).exec();
}

void MainWindow::editProgram(std::string path) {
    ProgramDialog(this, path, _clusterPath).exec();
}

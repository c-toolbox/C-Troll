#include "program/programhandler.h"

#include <QDirIterator>
#include <QFile>
#include <QJsonDocument>
#include <QStringList>

void ProgramHandler::loadFromDirectory(QString directory) {
    qDebug() << directory;
    // First, get all the *.json files from the directory and subdirectories
    QDirIterator it(directory, QStringList() << "*.json", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        Program a = loadProgram(it.next(), directory);
        addProgram(a);
    }
}

void ProgramHandler::addProgram(Program program) {
    _programs.push_back(std::move(program));
}

const QList<Program>& ProgramHandler::programs() const {
    return _programs;
}

Program ProgramHandler::loadProgram(QString jsonFile, QString baseDirectory) {
    QDir dir(baseDirectory);
    dir.cdUp();
    
    QString identifier = dir.relativeFilePath(jsonFile);
    // The extension is only 5 characters ('.json')
    // so we can just remove it
    identifier = identifier.left(identifier.size() - 5);
    
    QFile f(jsonFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject obj = d.object();
    obj["id"] = identifier;
    return Program(d.object());
}

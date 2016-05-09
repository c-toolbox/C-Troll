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
        Program a = loadApplication(it.next(), directory);
        addApplication(a);
    }
}

void ProgramHandler::addApplication(Program application) {
    _applications.push_back(std::move(application));
}

const QList<Program>& ProgramHandler::applications() const {
    return _applications;
}

Program ProgramHandler::loadApplication(QString jsonFile, QString baseDirectory)
{
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

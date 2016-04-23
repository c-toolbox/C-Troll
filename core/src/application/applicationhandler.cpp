#include "applicationhandler.h"

#include <QDirIterator>
#include <QFile>
#include <QJsonDocument>
#include <QStringList>

void ApplicationHandler::loadFromDirectory(QString directory) {
    qDebug() << directory;
    // First, get all the *.json files from the directory and subdirectories
    QDirIterator it(directory, QStringList() << "*.json", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        Application a = loadApplication(it.next(), directory);
        addApplication(a);
    }
}

void ApplicationHandler::addApplication(Application application) {
    _applications.push_back(std::move(application));
}

const QList<Application>& ApplicationHandler::applications() const {
    return _applications;
}

Application ApplicationHandler::loadApplication(QString jsonFile, QString baseDirectory)
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
    return Application(d.object());
}

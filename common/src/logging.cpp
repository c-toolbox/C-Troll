#include "logging.h"

#include <QDebug>

#include <assert.h>

namespace {
    const QString BaseName = "log_";
}

namespace common {

Log* Log::_log;
    
void Log::initialize(QString application) {
    assert(!application.isEmpty());
    _log = new Log(std::move(application));
}
    
Log& Log::ref() {
    assert(_log);
    return *_log;
}
    
Log::Log(QString componentName)
    : _file(BaseName + componentName + ".txt")
{
    assert(!componentName.isEmpty());
    _file.open(QIODevice::WriteOnly);
}
    
Log::~Log() {
    _file.close();
}
    
void Log::logMessage(QString message) {
    _file.write(message.toUtf8());
    _file.write("\n");
    _file.flush();
    qDebug() << message;
}
    
} // namespace common

void Log(QString msg) {
    common::Log::ref().logMessage(std::move(msg));
}

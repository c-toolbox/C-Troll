#include "logging.h"

#include <QDebug>

#include <assert.h>

#ifdef WIN32
#include <Windows.h>
#endif

namespace {
    const QString LogPrefix = "log_";
    const QString LogPostfix = ".txt";
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
    : _file(LogPrefix + componentName + LogPostfix)
{
    assert(!componentName.isEmpty());
    _file.open(QIODevice::WriteOnly);
}
    
Log::~Log() {
    _file.close();
}
    
void Log::logMessage(QString message) {
    // First the file
    _file.write(message.toUtf8());
    _file.write("\n");
    _file.flush();

    // Then the console
    qDebug() << message;

    // And if we are running in Visual Studio, this output, too
#ifdef WIN32
    message = message + "\n";
    OutputDebugString(message.toStdString().c_str());
#endif
}
    
} // namespace common

void Log(QString msg) {
    common::Log::ref().logMessage(std::move(msg));
}

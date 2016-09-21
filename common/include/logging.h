#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <QFile>
#include <QString>

namespace common {
    
/**
 * This static class provides the ability to log information to both the console and a
 * persistent file. The file will be created in working directory of the application will
 * is partially determined by the \c application variable that is passed to the
 * initialize function. Every following call to logMessage will be relegated to both a
 * file that is called <code>log_{application}.txt</code.> as well as the console. Every
 * time the log file is created, the old contents will be silently overwritten.
 */
class Log {
public:
    /**
     * Initializes the static Log and opens the log file for reading.
     * \param application The name of the application. This is used for creating
     * unique(ish) names for the log.
     */
    static void initialize(QString application);
    
    /**
     * Returns the static reference to the Log instance.
     * \return The static reference to the Log instance
     * \pre Log::initialize needs to be called before the first call to Log::ref
     */
    static Log& ref();
 
    /**
     * Logs a message with the Log. This message is both logged to the log file as well
     * as to the console using the \c qDebug macro. Every content to the file is flushed
     * immediately.
     * \param message The message that is to be logged
     */
    void logMessage(QString message);
  
private:
    /**
     * Constructs a Log and opens the file for reading, overwriting any old content that
     * was in the file previously.
     * \param application The name of the application that requested the log file
     */
     Log(QString application);
    
    /// Destructor the will close the file.
     ~Log();

    // The static Log that is returned in the Log::ref method.
    static Log* _log;
    
    /// The log file to which all messages from the logMessage method get logged
    QFile _file;
};
    
} // namespace common

/**
 * This method is a shortcut for a more convenient logging. Calling this function is
 * equivalent to calling <code>common::Log::ref().logMessage(msg)</code>.
 * \param message The message that is to be logged and passed to the Log::logMessage
 * function
 */
void Log(QString message);

#endif // __LOGGING_H__

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <QString>

struct Command {
    Command() = default; // temporary for texting ---abock
    Command(QString command);

    QString cluster;
    QString application;
    QString configuration;
};

#endif // __COMMAND_H__

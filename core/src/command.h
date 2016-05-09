#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <QString>

struct Command {
    Command(QString command);

    QString cluster;
    QString application;
};

#endif // __COMMAND_H__

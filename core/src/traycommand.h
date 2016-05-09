#ifndef __TRAYCOMMAND_H__
#define __TRAYCOMMAND_H__

#include "program\program.h"

struct TrayCommand {
    TrayCommand(const Program& program, QString configuration = "");

    QString json() const;

    QString executable;
    QString baseDirectory;
    QString commandlineParameters;
};

#endif // __TRAYCOMMAND_H__

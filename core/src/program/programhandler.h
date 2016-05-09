#ifndef __PROGRAMHANDLER_H__
#define __PROGRAMHANDLER_H__

#include "program/program.h"

class ProgramHandler {
public:
    ProgramHandler() = default;
    
    void loadFromDirectory(QString directory);
    
    void addApplication(Program application);
    
    const QList<Program>& applications() const;
    
private:
    Program loadApplication(QString jsonFile, QString baseDirectory);
    
    QList<Program> _applications;
};


#endif // __PROGRAMHANDLER_H__

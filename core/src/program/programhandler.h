#ifndef __PROGRAMHANDLER_H__
#define __PROGRAMHANDLER_H__

#include "program/program.h"

class ProgramHandler {
public:
    ProgramHandler() = default;
    
    void loadFromDirectory(QString directory);
    
    void addProgram(Program application);
    
    const QList<Program>& programs() const;
    
private:
    Program loadProgram(QString jsonFile, QString baseDirectory);
    
    QList<Program> _programs;
};


#endif // __PROGRAMHANDLER_H__

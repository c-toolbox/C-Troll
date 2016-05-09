#ifndef __APPLICATIONHANDLER_H__
#define __APPLICATIONHANDLER_H__

#include "application.h"

class ApplicationHandler {
public:
    ApplicationHandler() = default;
    
    void loadFromDirectory(QString directory);
    
    void addApplication(Application application);
    
    const QList<Application>& applications() const;
    
private:
    Application loadApplication(QString jsonFile, QString baseDirectory);
    
    QList<Application> _applications;
};


#endif // __APPLICATIONHANDLER_H__

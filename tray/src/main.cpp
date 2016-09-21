#include <iostream>

#include "sockethandler.h"
#include "processhandler.h"
#include <QCoreApplication>


int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    
    SocketHandler socketHandler;
    socketHandler.initialize();
    
    ProcessHandler processHandler;
    
    // Connect the sockethandler and the processhandler
    QObject::connect(&socketHandler, SIGNAL(messageRecieved(std::string)),
                     &processHandler, SLOT(handleSocketMessage(std::string)));
    QObject::connect(&processHandler, SIGNAL(sendSocketMessage(std::string)),
                     &socketHandler, SLOT(sendMessage(std::string)));
    
    /*QObject::connect(&socketHandler, &SocketHandler::messageRecieved, [&](std::string message) {
        std::cout << "message recieved: " << message;
        socketHandler.sendMessage("I feel you bro. I also say " + message + " all the time.");
    });*/

    app.exec();
}

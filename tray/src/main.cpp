#include <iostream>

#include "sockethandler.h"
#include <QCoreApplication>


int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    SocketHandler handler;
    handler.initialize();
    
    QObject::connect(&handler, &SocketHandler::messageRecieved, [&](std::string message) {
        std::cout << "message recieved: " << message;
        handler.sendMessage("I feel you bro. I also say " + message + " all the time.");
    });

    app.exec();
}

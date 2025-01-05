
// main.cpp

#include "include/Server.hpp"
#include "include/Logging/Logger.hpp"

#define PORT_IN 42068 // port for outgoung traffic (speak)
#define PORT_OUT 42069 // port for incoming traffic (listen)


int main(int argc, char *argv[]) {

    if (!SDLUtils::initialize()) {
        return 1;
    }

    try {
        Server::Setup(PORT_IN, PORT_OUT);
    }
    catch(std::runtime_error &e) {
        Logger::error(e.what());
        SDLUtils::cleanup();

        return 1;
    }

    Server::Run();
    Server::Cleanup();


    return 0;
}
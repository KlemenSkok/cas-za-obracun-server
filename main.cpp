
// main.cpp

#include "include/Server.hpp"
#include "include/Logging/Logger.hpp"
#include "include/Utilities/Utility.hpp"

#define PORT_OUT 42068 // port for outgoing traffic (speak)
#define PORT_IN 42069 // port for incoming traffic (listen)


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
        Server::Cleanup();

        return 1;
    }

    Server::Run();
    
    Server::Cleanup();
    SDLUtils::cleanup();


    return 0;
}
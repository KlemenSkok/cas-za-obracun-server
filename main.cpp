
// main.cpp

#include "include/Server.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

#define PORT_IN 42068 // port for incoming traffic
#define PORT_OUT 42069 // port for outgoung traffic


int main(int argc, char *argv[]) {

    if (!SDLUtils::initialize()) {
        return 1;
    }

    try {
        Server::Setup(42069, 42068);
    }
    catch(std::runtime_error &e) {
        spdlog::error(e.what());
        SDLUtils::cleanup();

        return 1;
    }


    Server::Run();
    Server::Cleanup();


    
    return 0;
}
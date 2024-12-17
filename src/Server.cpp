
// Server.cpp

#include "../include/Server.hpp"
#include "../include/SocketComm.hpp"
#include <SDL2/SDL.h>


int Server::Setup() {
    return 1;
}

int Server::Run() {
    //std::thread listenerThr(SocketListener::Listen, PORT_IN);
    //listenerThr = std::make_unique<std::thread>()
    return 0;
}

int Server::Cleanup() {
    return 0;
}
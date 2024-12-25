
// Server.cpp

#include "../include/Server.hpp"
#include "../include/SocketComm.hpp"

#include <SDL2/SDL.h>
#include <iostream>
#include <stdexcept>
#include <cstring>

/**
 * @param i port to listen on
 * @param o port to send from
 */
int Server::Setup(uint16_t port_in, uint16_t port_out) {
    try {
        SocketListener::Start(port_in);
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start listening thread: ") + e.what());
    }

    try {
        // todo
        //SocketSpeaker::Start(port_out);
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start speaking thread: ") + e.what());
    }

    return 0;
}

int Server::Run() {

    std::string str;
    bool quit = false;

    while(!quit) {
        std::cin >> str;

        if(str == "exit")
            quit = true;
    }

    return 0;
}

int Server::Cleanup() {

    SocketListener::Stop();
    //SocketSpeaker::Stop();    

    return 0;
}
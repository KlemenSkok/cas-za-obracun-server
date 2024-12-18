
// Server.cpp

#include "../include/Server.hpp"
#include "../include/SocketComm.hpp"

#include <SDL2/SDL.h>
#include <iostream>


int Server::Setup(uint16_t port_in, uint16_t port_out) {
    SocketListener::Start(port_in);
    //SocketSpeaker::Start(port_out);

    if(!SocketListener::_running /*|| !SocketSpeaker::_running*/) {
        // error opening ports
        // todo: throw an exception instead of returning sth
        return -1;
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
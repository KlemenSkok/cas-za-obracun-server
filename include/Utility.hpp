
// Utility.hpp

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>


// ID generator for all server objects
class IDgenerator {
public:
    static int addPlayer();
    static int addGame();
};


// namespace for dealing with SDL initialization and quitting
namespace SDLUtils {
    // tuki samo deklaracije
    bool initialize();
    void cleanup();
}



std::string formatIP(Uint32 ip);
void dumpPacket(UDPpacket *packet);
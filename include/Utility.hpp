
// Utility.hpp

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>


// ID generator for all server objects
class IDGenerator {
public:
    static int GetNextID() {
        static int current_id = 1;
        return current_id++;
    }
};


// namespace for dealing with SDL initialization and quitting
namespace SDLUtils {
    // tuki samo deklaracije
    bool initialize();
    void cleanup();
}



std::string formatIP(Uint32 ip);
void dumpPacket(UDPpacket *packet);
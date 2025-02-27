
// Utility.hpp

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <string>


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

// game info constants
enum class GameState: uint8_t {
    WAITING_FOR_PLAYERS = 0,
    WAITING_NEXT_ROUND,
    ROUND_RUNNING,
    ROUND_ENDING,
    GAME_FINISHED
};

Uint32 getGameStateDuration(GameState);


std::string formatIP(Uint32 ip);
void dumpPacket(UDPpacket *packet);
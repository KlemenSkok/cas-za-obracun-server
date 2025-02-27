
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
    WAITING_FOR_PLAYERS = 0,    // as long as needed
    ROUND_ENDING = 3,           // 3s
    WAITING_NEXT_ROUND = 10,    // 10s
    ROUND_RUNNING,              // as long as needed
    GAME_FINISHED = 30          // 30s
};

Uint32 getGameStateDuration(GameState);


std::string formatIP(Uint32 ip);
void dumpPacket(UDPpacket *packet);
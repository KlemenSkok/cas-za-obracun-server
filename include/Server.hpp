
// Server.hpp

#pragma once

#include <cstdint>
//#include <SDL2/SDL_net.h>

#include "GameSession.hpp"
//#include "SocketListener.hpp"

#define MAX_SESSIONS 255
#define PORT_IN 42068 // port for incoming traffic
#define PORT_OUT 42069 // port for outgoung traffic


// TODO: namespace to initialize SDL!!!!


class Server {
    std::map<uint8_t, GameSession*> activeSessions;

public:
    static int Setup(uint16_t i, uint16_t o);
    static int Run();
    static int Cleanup();

private:
};

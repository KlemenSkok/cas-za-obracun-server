
// Server.hpp

#pragma once

#include "GameSession.hpp"
#include "SocketComm.hpp"
#include "Structs.hpp"


#define MAX_SESSIONS 256


class Server {
    std::map<uint8_t, GameSession*> activeSessions;

public:
    static void Setup(uint16_t i, uint16_t o);
    static void Run();
    static void Cleanup();

private:
};

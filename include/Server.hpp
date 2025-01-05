
// Server.hpp

#pragma once

#include "GameSession.hpp"
#include "Communication/SocketListener.hpp"
#include "Communication/SocketSpeaker.hpp"
#include "Containers.hpp"


#define MAX_SESSIONS 256


class Server {
    std::map<uint8_t, GameSession*> activeSessions;

public:
    static void Setup(uint16_t i, uint16_t o);
    static void Run();
    static void Cleanup();
    
    static void processNewPackets();
};


// Server.hpp

#pragma once

#include "GameSession.hpp"
#include "Communication/SocketListener.hpp"
#include "Communication/SocketSpeaker.hpp"
#include "Containers.hpp"

#include <set>

#define MAX_SESSIONS 256


class Server {
    static std::map<uint8_t, std::unique_ptr<GameSession>> _sessions;
    static std::set<uint16_t> _free_client_ids;
    static std::set<uint8_t> _free_session_ids;

public:
    static void Setup(uint16_t i, uint16_t o);
    static void Run();
    static void Cleanup();
    
    static void processNewPackets();

    static int addClient(IPaddress ip); // returns client_id or -1 on failure
    static void removeClient(uint16_t c_id);
    static void removeClient(uint16_t c_id, uint8_t s_id);
    static int queryClient(uint16_t c_id); // returns session_id of the queried client
    static int queryAddress(IPaddress ip); // returns channel of passed IP or -1

    static int addSession(); // returns new session id
    static void removeSession(uint8_t  id);
};


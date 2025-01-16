
// GameSession.hpp

#pragma once

#include <cstdint> // za playerid
#include <map>
#include <memory>

#include "Client.hpp"
#include "Logging/Logger.hpp"

#define MAX_PLAYERS 4 // per session


class GameSession {
private:
    std::map<uint16_t, std::unique_ptr<Client>> clients;
    uint8_t id;

public: 
    GameSession(int id) : id(id) {}
    ~GameSession() = default;

    uint8_t get_id();

    bool isFull();
    bool acceptsPlayers();
    short size(); // == number of clients

    void addClient(uint16_t c_id, IPaddress ip);
    void removeClient(uint16_t c_id, UDPsocket);
    bool hasClient(uint16_t c_id);
    int queryAddress(IPaddress ip);

    void Stop(UDPsocket);



};
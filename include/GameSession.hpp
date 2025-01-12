
// GameSession.hpp

#pragma once

#include <cstdint> // za playerid
#include <map>
#include <memory>

#include "Client.hpp"

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
    void addClient(uint16_t id);



};
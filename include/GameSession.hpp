
// GameSession.hpp

#pragma once

#include <cstdint> // za playerid
#include <map>
#include <unordered_map>
#include <memory>

#include "Client.hpp"
#include "Player.hpp"
#include "Logging/Logger.hpp"

#define MAX_PLAYERS 4 // per session


class GameSession {
private:
    std::unordered_map<uint16_t, std::shared_ptr<Client>> clients;
    std::unordered_map<uint16_t, std::shared_ptr<Player>> players;
    uint8_t id;

public: 
    GameSession(int id) : id(id) {
        //std::cout << "Session created. ID: " << id << '\n';
    }
    ~GameSession() {
        //std::cout << "Session destructor called. ID: " << id << '\n';
    }

    uint8_t get_id();

    bool isFull();
    bool acceptsPlayers();
    uint8_t size(); // == number of clients

    void addClient(uint16_t c_id, IPaddress ip);
    void removeClient(uint16_t c_id);
    bool hasClient(uint16_t c_id);
    int queryAddress(IPaddress ip);
    IPaddress getClientAddr(uint16_t c_id);

    void processPacket();

    void Stop(UDPsocket);

    std::weak_ptr<Client> getClient(uint16_t c_id);



};
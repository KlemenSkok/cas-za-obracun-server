
// GameSession.hpp

#pragma once

#include <cstdint> // za playerid
#include <map>
#include <unordered_map>
#include <memory>
#include <vector>

#include "Client.hpp"
#include "Player.hpp"
#include "Logging/Logger.hpp"
#include "Containers.hpp"

#define MAX_PLAYERS 4 // per session


class GameSession {
private:
    std::unordered_map<uint16_t, std::shared_ptr<Client>> clients;
    std::unordered_map<uint16_t, std::shared_ptr<Player>> players;
    uint8_t id;

    Uint32 lastUpdate;

public: 
    static std::vector<std::unique_ptr<UDPmessage>> pending_msgs;
    

    GameSession(int id) : id(id), lastUpdate(SDL_GetTicks()) {
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
    std::weak_ptr<Client> getClient(uint16_t c_id);
    int queryAddress(IPaddress ip);
    IPaddress getClientAddr(uint16_t c_id);

    void processPacket(PacketData data);
    void manageSession();
    std::vector<uint16_t> checkClientInactivity();

    void sendGameUpdatesToClient(uint16_t c_id); // 

    void Stop(UDPsocket);




};
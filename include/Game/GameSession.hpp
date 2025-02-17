
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
#include "Projectile.hpp"

#define MAX_PLAYERS 4 // per session


class GameSession {
private:
    std::unordered_map<uint16_t, std::shared_ptr<Client>> clients;
    std::unordered_map<uint16_t, std::shared_ptr<Player>> players;
    std::unordered_map<uint16_t, std::shared_ptr<Projectile>> projectiles;

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

    // utility functions
    bool isFull();
    bool acceptsPlayers();
    uint8_t size(); // == number of clients
    void Stop(UDPsocket);
    
    // session main loop
    void manageSession();
    // main loop components
    void updateEverything(float);
    void checkCollisions();
    void broadcastUpdates();

    // functions for dealing with clients
    void addClient(uint16_t c_id, IPaddress ip);
    void removeClient(uint16_t c_id);
    bool hasClient(uint16_t c_id);
    std::weak_ptr<Client> getClient(uint16_t c_id);
    int queryAddress(IPaddress ip);
    IPaddress getClientAddr(uint16_t c_id);

    // functions for packet processing
    void processPacket(PacketData data);
    void processPlayerUpdates(PacketData data);
    std::vector<uint16_t> getInactiveClients();

    // functions for sending out packets
    void sendGameUpdatesToClient(uint16_t c_id);
    void sendPlayerStatesToClient(uint16_t c_id);
    void sendProjectileStatesToClient(uint16_t c_id);


};
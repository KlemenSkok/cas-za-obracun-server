
// GameSession.hpp

#pragma once

#include <cstdint> // za playerid
#include <map>
#include <unordered_map>
#include <memory>
#include <vector>

#include "Communication/Client.hpp"
#include "Player.hpp"
#include "Logging/Logger.hpp"
#include "Containers.hpp"
#include "Projectile.hpp"
#include "Flag.hpp"
#include "Site.hpp"
#include "Utilities/Utility.hpp"

#define MAX_PLAYERS 4 // per session


class GameSession {
private:
    std::unordered_map<uint16_t, std::shared_ptr<Client>> clients;
    std::unordered_map<uint16_t, std::shared_ptr<Player>> players;
    std::unordered_map<uint16_t, std::shared_ptr<Projectile>> projectiles;
    std::unordered_map<uint8_t, std::shared_ptr<Site>> sites;
    std::shared_ptr<Flag> flag;

    uint8_t id;

    Uint32 lastUpdate;

    // game state tracking
    GameState currentState;
    Uint32 currentStateStartTime; // [ms] time since the start of this state

    uint8_t currentRound;
    std::vector<uint8_t> score;

public: 
    static std::vector<std::unique_ptr<UDPmessage>> pending_msgs;


    GameSession(int id) : 
        id(id), 
        lastUpdate(SDL_GetTicks()), 
        currentState(GameState::WAITING_FOR_PLAYERS), 
        currentRound(0)
    {
        this->flag = std::make_shared<Flag>(GAME_FLAG_HOME_POS_X, GAME_FLAG_HOME_POS_X);
        this->score = std::vector<uint8_t>(2, 0);
        
        this->sites[1] = std::make_shared<Site>(1);
        this->sites[2] = std::make_shared<Site>(2);

        std::cout << "Waiting for players...\n";

    }
    ~GameSession() = default;

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
    void checkGameState();

    // functions for game state managing
    void resetRound();
    void startRound();
    void endRound(uint8_t);
    void startWaitingNextRound();
    void finishGame();

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
    void sendFlagStateToClient(uint16_t c_id);
    void sendPlayerStatesToClient(uint16_t c_id);
    void sendProjectileStatesToClient(uint16_t c_id);

};
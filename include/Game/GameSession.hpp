
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
    std::shared_ptr<Flag> flag;

    uint8_t id;

    Uint32 lastUpdate;

    // game state tracking
    GameState currentState;
    Uint32 currentStateStartTime; // [ms] time since the start of this state
    Uint32 lastStateUpdateTime; // [ms]

    uint8_t currentRound;
    std::vector<uint8_t> score;

public: 
    static std::vector<std::unique_ptr<UDPmessage>> pending_msgs;


    GameSession(int id) : 
        id(id), 
        lastUpdate(SDL_GetTicks()), 
        currentState(GameState::WAITING_FOR_PLAYERS), 
        currentRound(0),
        lastStateUpdateTime(SDL_GetTicks())
    {
        this->initialize();
    }
    ~GameSession() = default;

    uint8_t get_id();

    // utility functions
    bool isFull() const;
    bool isEnding() const;
    bool acceptsPlayers() const;
    uint8_t size() const; // == number of clients
    void Stop(UDPsocket);
    void initialize();

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
    void sendGameStateToClient(uint16_t c_id);

    void forceGameStateUpdates();

    data_packets::GameStateData generateGameStateData();

};

// GameSession.cpp

#include "Game/GameSession.hpp"
#include "Game/Map/MapData.hpp"
#include "Utilities/Constants.hpp"
#include "Communication/PacketTypes.hpp"

#include <vector>


std::vector<std::unique_ptr<UDPmessage>> GameSession::pending_msgs;


void GameSession::initialize() {
    this->flag = std::make_shared<Flag>(GAME_FLAG_HOME_POS_X, GAME_FLAG_HOME_POS_X);
    this->score = std::vector<uint8_t>(2, 0);
    
    this->sites[1] = std::make_shared<Site>(1);
    this->sites[1]->setPosition(TEAM_SITE_1_POSITION);
    this->sites[1]->setSize(TEAM_SITE_1_SIZE);
    this->sites[2] = std::make_shared<Site>(2);
    this->sites[2]->setPosition(TEAM_SITE_2_POSITION);
    this->sites[2]->setSize(TEAM_SITE_2_SIZE);

    std::cout << "Waiting for players...\n";
}

bool GameSession::isFull() const {
    return clients.size() >= MAX_PLAYERS;
}

bool GameSession::isEnding() const {
    return this->currentState == GameState::GAME_FINISHED;
}

bool GameSession::acceptsPlayers() const {
    // zaenkrat gledamo samo na stevilo igralcev v igri
    return !this->isFull();
}

uint8_t GameSession::size() const {
    return clients.size();
}

uint8_t GameSession::get_id() {
    return this->id;
}

void GameSession::addClient(uint16_t id, IPaddress ip) {
    
    // assign to a team
    uint8_t teamsCnt[2] = {0, 0};
    for(const auto& [pid, pl] : players) {
        teamsCnt[pl->getTeam()-1]++;
    }
    uint8_t teamNumber = (teamsCnt[0] > teamsCnt[1]) ? 2 : 1;

    clients[id] = std::make_shared<Client>(id, ip);
    players[id] = std::make_shared<Player>(id);

    auto p = players[id];
    p->setTeam(teamNumber);

    if(this->players.size() == MAX_PLAYERS) {
        // start the round
        this->startWaitingNextRound();
    }

}

void GameSession::removeClient(uint16_t c_id) {
    
    if(clients.find(c_id) != clients.end()) {
        //Logger::info(("Removed client. ID: " + std::to_string(c_id)).c_str());
    }

    if(this->flag->getCarrierID() == c_id) {
        this->flag->dropFlag();
    }

    clients.erase(c_id);
    players.erase(c_id);
}

bool GameSession::hasClient(uint16_t c_id) {
    return (clients.find(c_id) != clients.end());
}

void GameSession::Stop(UDPsocket socket) {
    // unbind all clients from the UDP socket
    for(auto &c : clients) {
        SDLNet_UDP_Unbind(socket, c.second->get_id());
    }
    // ostalo se spuca samo od sebe (zaradi shared_ptr)
}

int GameSession::queryAddress(IPaddress ip) {
    for(auto &c : clients) {
        if(c.second->get_ip().host == ip.host && c.second->get_ip().port == ip.port) {
            return c.first;
        }
    }
    return -1;
}

IPaddress GameSession::getClientAddr(uint16_t c_id) {
    for(auto &c : clients) {
        if(c.first == c_id) {
            return c.second->get_ip();
        }
    }
    return IPaddress();
}


std::weak_ptr<Client> GameSession::getClient(uint16_t c_id) {
    return this->clients[c_id];
}

/**
 * Checks for inactive clients
 * @return an `std::vector` containing inactive clients' ids (`uint16_t`)
 */
std::vector<uint16_t> GameSession::getInactiveClients() {
    std::vector<uint16_t> client_list;
    for(auto& c : clients) {
        if(c.second->checkTimeout()) {
            client_list.push_back(c.first);
        }
    }
    return client_list;
}

void GameSession::processPacket(PacketData data) {
    uint32_t packet_id;
    uint16_t client_id;
    uint8_t packet_type;
    try {
        data.getByOffset(client_id, sizeof(uint16_t), OFFSET_CLIENT_ID);
        data.getByOffset(packet_id, sizeof(uint32_t), OFFSET_PACKET_ID);
        data.getByOffset(packet_type, sizeof(uint8_t), OFFSET_PACKET_TYPE);
    }
    catch(std::exception& e) {
        Logger::warn(e.what());
        return;
    }

    // ensure duplicate packet handling
    if(packet_id <= this->clients[client_id]->getLastRecvPacketID()) {
        // this is a duplicate packet, ignore it
        return;
    }

    // now we can process the packet
    this->clients[client_id]->updateLastRecvPacketID(packet_id);
    this->clients[client_id]->updateLastRecvPacketTime();

    // todo
    switch((PacketType)packet_type) {
        case PacketType::PLAYER_UPDATES:
            // process players in range
            GameSession::processPlayerUpdates(data);
            break;
        default:
            Logger::warn("Unknown packet type.");
    }
    
}

void GameSession::processPlayerUpdates(PacketData data) {
    using namespace data_packets;
    PlayerKeyStates pks;
    uint16_t c_id;
    float direction;

    data.getByOffset(c_id, sizeof(uint16_t), OFFSET_CLIENT_ID);
    pks.deserialize(data, OFFSET_DATA);
    data.getByOffset(direction, sizeof(float), OFFSET_DATA + PlayerKeyStates::size());
    
    auto p = players[c_id];
    p->importUpdates(pks, direction);

    // shooting and flag are only enabled during the round
    if(this->currentState == GameState::ROUND_RUNNING) {

        // check if the player shot a projectile
        // player cant shoot while carrying the flag
        if(p->shotProjectile() && (p->get_id() != this->flag->getCarrierID())) {
            std::shared_ptr<Projectile> pr = std::make_shared<Projectile>(p->position.x, p->position.y, p->direction, p->get_id(), p->getTeam());
            this->projectiles[pr->get_id()] = pr;
        }
        
        // check if player is trying to interact with the flag
        if(p->isInteracting()) {
            if(this->flag->isCaptured()) {
                if(this->flag->getCarrierID() == p->get_id()) {
                    // release the flag
                    this->flag->dropFlag();
                    p->dropFlag();
                }
            }
            else if(!p->isPostureBroken()) { 
                // check if the flag is in range to pick up
                int dx = p->getPosition().x - this->flag->getPosition().x;
                int dy = p->getPosition().y - this->flag->getPosition().y;
                if((dx*dx + dy*dy) < GAME_FLAG_PICKUP_RANGE*GAME_FLAG_PICKUP_RANGE) {
                    // pick up the flag
                    this->flag->capture(p->get_id());
                    p->captureFlag();
                }
            }
        }
    }
}

/**
 @brief GameSession main loop.
 */
void GameSession::manageSession() {
    // update the session, check for collisions etc.
    // main loop for sessions
    // 60Hz server tick rate
    Uint32 now = SDL_GetTicks();
    if(now - this->lastUpdate < SESSION_UPDATE_DELAY) {
        return;
    }
    int deltaTime = now - this->lastUpdate;
    lastUpdate = now;


    // update everithing (using deltaTime)
    this->updateEverything(deltaTime / 1000.0f);

    this->checkCollisions();

    this->checkGameState();

    this->broadcastUpdates();

}

void GameSession::sendGameUpdatesToClient(uint16_t c_id) {
    
    // send data about states for players 
    GameSession::sendPlayerStatesToClient(c_id);
    GameSession::sendFlagStateToClient(c_id);
    GameSession::sendProjectileStatesToClient(c_id);

}

/**
 * @brief Force immediate sending game state updates to clients. Occours on state changes
 */
void GameSession::forceGameStateUpdates() {
    this->lastStateUpdateTime = SDL_GetTicks();

    for(auto& p : players) {
        GameSession::sendGameStateToClient(p.first);
    }
}

void GameSession::sendPlayerStatesToClient(uint16_t c_id) {

    PacketData d(true);
    d.flags() |= (1 << FLAG_DATA);                      // 1 B
    d.append(this->id);                                 // 1 B
    d.append(c_id);                                     // 2 B
    d.append(clients[c_id]->getLastSentPacketID());     // 4 B
    d.append((uint8_t)PacketType::PLAYERS_IN_RANGE);    // 1 B

    // the target player is always first
    players[c_id]->dumpMovement().serialize(d);         // 28 B
    
    // append other players
    for (auto& p : players) {
        if(p.first != c_id) {
            p.second->dumpMovement().serialize(d);      // 28 B (each player)
        }
    }
    
    std::unique_ptr<UDPmessage> msg = std::make_unique<UDPmessage>();
    msg->ip = std::make_unique<IPaddress>(clients[c_id]->get_ip());
    msg->data = d.getRawData();
    msg->len = d.size();

    GameSession::pending_msgs.push_back(std::move(msg));

    // expected packet size: 37 B (+ 28 B for each added player)

}

void GameSession::sendProjectileStatesToClient(uint16_t c_id) {
    PacketData d(true);
    d.flags() |= (1 << FLAG_DATA);                      // 1 B
    d.append(this->id);                                 // 1 B
    d.append(c_id);                                     // 2 B
    d.append(clients[c_id]->getLastSentPacketID());     // 4 B
    d.append((uint8_t)PacketType::PROJECTILES_IN_RANGE);// 1 B

    for(auto& pr : projectiles) {
        pr.second->dumpData().serialize(d);
    }

    std::unique_ptr<UDPmessage> msg = std::make_unique<UDPmessage>();
    msg->ip = std::make_unique<IPaddress>(clients[c_id]->get_ip());
    msg->data = d.getRawData();
    msg->len = d.size();

    GameSession::pending_msgs.push_back(std::move(msg));

    // expected packet size: 9 B (+ 18 B for each projectile)

}

void GameSession::sendFlagStateToClient(uint16_t c_id) {
    PacketData d(true);
    d.flags() |= (1 << FLAG_DATA);                      // 1 B
    d.append(this->id);                                 // 1 B
    d.append(c_id);                                     // 2 B
    d.append(clients[c_id]->getLastSentPacketID());     // 4 B
    d.append((uint8_t)PacketType::FLAG_STATE);          // 1 B

    // the target player is always first
    this->flag->dumpData().serialize(d);                // 15 B

    
    std::unique_ptr<UDPmessage> msg = std::make_unique<UDPmessage>();
    msg->ip = std::make_unique<IPaddress>(clients[c_id]->get_ip());
    msg->data = d.getRawData();
    msg->len = d.size();

    GameSession::pending_msgs.push_back(std::move(msg));

    // expected packet size: 24 B

}

void GameSession::sendGameStateToClient(uint16_t c_id) {
    PacketData d(true);
    d.flags() |= (1 << FLAG_DATA);                      // 1 B
    d.append(this->id);                                 // 1 B
    d.append(c_id);                                     // 2 B
    d.append(clients[c_id]->getLastSentPacketID());     // 4 B
    d.append((uint8_t)PacketType::GAME_STATE);          // 1 B

    GameSession::generateGameStateData().serialize(d);  // 10 B

    std::unique_ptr<UDPmessage> msg = std::make_unique<UDPmessage>();
    msg->ip = std::make_unique<IPaddress>(clients[c_id]->get_ip());
    msg->data = d.getRawData();
    msg->len = d.size();

    GameSession::pending_msgs.push_back(std::move(msg));

    // expected packet size: 19 B

}

//
// MAIN LOOP COMPONENTS
//

// sending game state updates to clients
void GameSession::broadcastUpdates() {

    // send out data to players
    for(auto& p : players) {
        GameSession::sendGameUpdatesToClient(p.first);
    }

    // send games state updates
    if(SDL_GetTicks() - this->lastStateUpdateTime > GAME_STATE_UPDATE_PERIOD) {
        this->lastStateUpdateTime = SDL_GetTicks();
        
        for(auto& p : players) {
            GameSession::sendGameStateToClient(p.first);
        }

    }

}

/**
 * @brief Updating the game state
 * @param deltaTime Time difference in `ms`
 */
void GameSession::updateEverything(float deltaTime) {
    // update players
    for(auto& p : players) {
        p.second->update(deltaTime);
    }

    // update projeciles
    for(auto it = projectiles.begin(); it != projectiles.end();  ) {
        it->second->update(deltaTime);
        if(it->second->isOutOfRange()) {
            it = projectiles.erase(it);
        }
        else it++;
    }

    // update flag
    if(this->flag->isCaptured()) {
        uint16_t carrierID = this->flag->getCarrierID();
        this->flag->updatePosition(this->players[carrierID]->getPosition());
    }

}

// Checking for collisions
void GameSession::checkCollisions() {

    // first check for any projectile hits (players)
    for(auto& p : this->players) {
        for(auto it = this->projectiles.begin(); it != this->projectiles.end();  ) {
            int dx = it->second->position.x - p.second->position.x;
            int dy = it->second->position.y - p.second->position.y;
            if(it->second->get_parent_team() != p.second->getTeam() && (dx*dx + dy*dy) < (PLAYER_RADIUS*PLAYER_RADIUS + PROJECTILE_RADIUS*PROJECTILE_RADIUS)) {
                // destroy the projectile
                it = this->projectiles.erase(it);
                // todo: deal posture damage to player
                p.second->dealPostureDamage();

                // player drops the flag on posture break
                if(p.second->isPostureBroken()) {
                    if(p.first == this->flag->getCarrierID()) {
                        this->flag->dropFlag();
                        p.second->dropFlag();
                    }
                }
            }
            else ++it;
        }
    }

    // check for projectile collisions with map barriers
    for(auto it = this->projectiles.begin(); it != this->projectiles.end();  ) {
        if(MapData::CheckCollision(*it->second.get(), it->second->position)) {
            it = this->projectiles.erase(it);
            //std::cout << "Collision detected.\n";
        }
        else ++it;
    }

    // check if the flag is fully inside the carrier's team site
    if(this->flag->isCaptured()) {
        for(auto& [sid, s] : this->sites) {
            if(this->players[this->flag->getCarrierID()]->getTeam() == sid) {
                if(s->checkFlagCollision(this->flag->getPosition(), this->flag->getSize())) {
                    // team ${sid} won the round
                    this->endRound(sid);
                    break;
                }
            }
        }
    }

}

/**
 * @brief Manage the game state
 */
void GameSession::checkGameState() {

    Uint32 elapsedTime = SDL_GetTicks() - this->currentStateStartTime;

    if(elapsedTime >= getGameStateDuration(this->currentState)) { // the current state should end
        // switch between game states
        switch(this->currentState) {
            case GameState::WAITING_NEXT_ROUND:
                this->startRound();
                // todo: send out an update packet
                break;
            case GameState::ROUND_ENDING:
                this->startWaitingNextRound();
                break;
            case GameState::GAME_FINISHED:
                // cleanup, kick all clients, session closes automatically
                this->forceGameStateUpdates();

                std::cout << "The game has finished!\n";
                break;
        }
    }

}


//
// GAME STATE MANAGING
//

/**
 * @brief Reset the round before the start of a round
 */
void GameSession::resetRound() {
    
    std::vector<std::vector<PointF>> start_positions = {
        { PLAYERS_START_POS_T1 },
        { PLAYERS_START_POS_T2 }
    };
    
    // determine starting positions
    for(auto& [pid, p] : this->players) {
        p->reset();
        if(!start_positions[p->getTeam() - 1].empty()) {
            PointF start_pos = start_positions[p->getTeam() - 1].back();
            start_positions[p->getTeam() - 1].pop_back();

            p->setPosition(start_pos);
            p->freezeControls();
        }
        
    }

    this->projectiles.clear();
    this->flag->reset();
    this->flag->updatePosition(GAME_FLAG_HOME_POS);

}

/**
 * @brief Start the 10s waiting period before a round
 */
void GameSession::startWaitingNextRound() {

    // 10s pause before the start of the round. Players are freezed in place
    this->currentState = GameState::WAITING_NEXT_ROUND;
    this->currentStateStartTime = SDL_GetTicks();
    this->forceGameStateUpdates();
    this->resetRound();

    std::cout << "Waiting for next round...\n";

}


void GameSession::finishGame() {

    // Announce the winner, wait for players to leave.
    this->currentState = GameState::GAME_FINISHED;
    this->currentStateStartTime = SDL_GetTicks();
    this->forceGameStateUpdates();

    // every 1 second send out a new packet called data_packet::GameResults containing the winning team
    // lasts 30s

    int winner = (this->score[0] > this->score[1]) ? 1 : 2;
    std::cout << "Team " << winner << " has won the game!\n";

}

void GameSession::endRound(uint8_t winner) {

    this->score[winner - 1]++;

    std::cout << "The round was won by team " << (int)winner << '\n';
    
    if(this->currentRound == NUMBER_OF_ROUNDS) {
        this->finishGame();
    }
    else {
        this->currentStateStartTime = SDL_GetTicks();
        this->currentState = GameState::ROUND_ENDING;
        this->forceGameStateUpdates();
    }

    
}

void GameSession::startRound() {

    this->currentState = GameState::ROUND_RUNNING;
    this->forceGameStateUpdates();

    // unfreeze players
    for(auto [pid, p] : players) {
        p->unfreezeControls();
    }

    // start a new round
    this->currentRound++;
    this->currentStateStartTime = SDL_GetTicks();
    std::cout << "Starting round " << (int)this->currentRound << '\n';

}

data_packets::GameStateData GameSession::generateGameStateData() {
    using namespace data_packets;

    GameStateData data;
    data.elapsedTime = this->currentStateStartTime;
    data.gameState = this->currentState;
    
    data.teamScores = 0;
    data.teamScores |= (this->score[0] & 0xF) << 4;
    data.teamScores |= this->score[1] & 0xF;

    return data;
}
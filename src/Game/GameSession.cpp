
// GameSession.cpp

#include "Game/GameSession.hpp"
#include "Game/Map/MapData.hpp"
#include "Utilities/Constants.hpp"
#include "Communication/PacketTypes.hpp"

#include <vector>


std::vector<std::unique_ptr<UDPmessage>> GameSession::pending_msgs;


bool GameSession::isFull() {
    return clients.size() >= MAX_PLAYERS;
}

bool GameSession::acceptsPlayers() {
    // zaenkrat gledamo samo na stevilo igralcev v igri
    return !this->isFull();
}

uint8_t GameSession::size() {
    return clients.size();
}

uint8_t GameSession::get_id() {
    return this->id;
}

void GameSession::addClient(uint16_t id, IPaddress ip) {
    clients[id] = std::make_shared<Client>(id, ip);
    players[id] = std::make_shared<Player>(id);
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

    // check if the player shot a projectile
    // player cant shoot while carrying the flag
    if(p->shotProjectile() && (p->get_id() != this->flag->getCarrierID())) {
        std::shared_ptr<Projectile> pr = std::make_shared<Projectile>(p->position.x, p->position.y, p->direction, p->get_id());
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
    
    this->broadcastUpdates();

}

void GameSession::sendGameUpdatesToClient(uint16_t c_id) {
    
    // send data about states for players 
    GameSession::sendPlayerStatesToClient(c_id);
    GameSession::sendFlagStateToClient(c_id);
    GameSession::sendProjectileStatesToClient(c_id);

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

//
// MAIN LOOP COMPONENTS
//

// sending game state updates to clients
void GameSession::broadcastUpdates() {
    // send out data to players
    for(auto& p : players) {
        GameSession::sendGameUpdatesToClient(p.first);
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
            if(it->second->get_parent_id() != p.second->get_id() && (dx*dx + dy*dy) < (PLAYER_RADIUS*PLAYER_RADIUS + PROJECTILE_RADIUS*PROJECTILE_RADIUS)) {
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

}
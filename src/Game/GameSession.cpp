
// GameSession.cpp

#include "Game/GameSession.hpp"
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
std::vector<uint16_t> GameSession::checkClientInactivity() {
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
    if(p->shotProjectile()) {
        std::shared_ptr<Projectile> pr = std::make_shared<Projectile>(p->position.x, p->position.y, p->direction);
        this->projectiles[pr->get_id()] = pr;
    }
}

/**
 * @brief GameSession main loop.
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
    // todo: check for collisions
    for(auto& p : players) {
        p.second->update(deltaTime / 1000.0f);
    }
    for(auto it = projectiles.begin(); it != projectiles.end();) {
        it->second->update(deltaTime / 1000.0f);
        if(it->second->isOutOfRange()) {
            it = projectiles.erase(it);
        }
        else it++;
    }

    // send out data to players
    for(auto& p : players) {
        GameSession::sendGameUpdatesToClient(p.first);
    }

}

void GameSession::sendGameUpdatesToClient(uint16_t c_id) {
    
    // send data about states for players 
    GameSession::sendPlayerStatesToClient(c_id);
    GameSession::sendProjectileStatesToClient(c_id);
    // ! todo
    

}

void GameSession::sendPlayerStatesToClient(uint16_t c_id) {

    PacketData d(true);
    d.flags() |= (1 << FLAG_DATA);                      // 1 B
    d.append(this->id);                                 // 1 B
    d.append(c_id);                                     // 2 B
    d.append(clients[c_id]->getLastSentPacketID());     // 4 B
    d.append((uint8_t)PacketType::PLAYERS_IN_RANGE);    // 1 B

    // the target player is always first
    players[c_id]->dumpMovement().serialize(d);         // 27 B
    
    // append other players
    for (auto& p : players) {
        if(p.first != c_id) {
            p.second->dumpMovement().serialize(d);      // 27 B (each player)
        }
    }
    
    std::unique_ptr<UDPmessage> msg = std::make_unique<UDPmessage>();
    msg->ip = std::make_unique<IPaddress>(clients[c_id]->get_ip());
    msg->data = d.getRawData();
    msg->len = d.size();

    GameSession::pending_msgs.push_back(std::move(msg));

    // expected packet size: 36 B (+ 27 B for each added player)

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
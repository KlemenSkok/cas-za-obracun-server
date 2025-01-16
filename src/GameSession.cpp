
// GameSession.cpp

#include "../include/GameSession.hpp"


bool GameSession::isFull() {
    return clients.size() >= MAX_PLAYERS;
}

bool GameSession::acceptsPlayers() {
    // zaenkrat gledamo samo na stevilo igralcev v igri
    return !this->isFull();
}

short GameSession::size() {
    return clients.size();
}

uint8_t GameSession::get_id() {
    return this->id;
}

void GameSession::addClient(uint16_t id, IPaddress ip) {

    clients[id] = std::make_unique<Client>(id, ip);

    // todo
}

void GameSession::removeClient(uint16_t c_id, UDPsocket socket) {
    SDLNet_UDP_Unbind(socket, c_id);
    Logger::info("TUKI!!");
    
    if(clients.find(c_id) != clients.end()) {
        Logger::info("Client je bil najden tuki.\n");
    }
    
    clients.erase(c_id);
    Logger::info(("Removed client. id=" + std::to_string(c_id)).c_str());
}

bool GameSession::hasClient(uint16_t c_id) {
    return (clients.find(c_id) != clients.end());
}

void GameSession::Stop(UDPsocket socket) {
    // unbind all clients from the UDP socket
    for(auto &c : clients) {
        SDLNet_UDP_Unbind(socket, c.second->get_id());
    }
    // ostalo se spuca samo od sebe (zaradi unique_ptr)
}

int GameSession::queryAddress(IPaddress ip) {
    for(auto &c : clients) {
        if(c.second->get_ip().host == ip.host && c.second->get_ip().port == ip.port) {
            return c.first;
        }
    }
    return -1;
}
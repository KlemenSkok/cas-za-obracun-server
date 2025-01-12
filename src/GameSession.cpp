
// GameSession.cpp

#include "../include/GameSession.hpp"


bool GameSession::isFull() {
    return clients.size() >= MAX_PLAYERS;
}

bool GameSession::acceptsPlayers() {
    // zaenkrat gledamo samo na stevilo igralcev v igri
    return !this->isFull();
}

uint8_t GameSession::get_id() {
    return this->id;
}

void GameSession::addClient(uint16_t id) {

    clients[id] = std::make_unique<Client>(id);


    // todo
}



// GameSession.cpp

#include "../include/GameSession.hpp"


bool GameSession::isFull() {
    return (clients.size() >= 4) ? true : false;
}

int GameSession::addClient() {
    if(this->isFull())
        return -1;
    return 0;
    // TODO
}
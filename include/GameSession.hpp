
// GameSession.hpp

#pragma once

#include <cstdint> // za uint8_t
#include <map>

#include "Base.hpp"
#include "Client.hpp"

#define MAX_PLAYERS 4 // per session


class GameSession : public BaseServerObject {
private:
    std::map<uint8_t, Client*> clients;

public: 
    GameSession();
    ~GameSession();

    bool isFull();
    int addClient();

};
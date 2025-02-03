
// Player.hpp

#pragma once

#include "Communication/PacketTypes.hpp"
#include "Containers.hpp"

#include <cstdint>

class Player {

    uint16_t id;

public:

    struct { float x, y; } position;
    struct { float x, y; } velocity;
    KeyStates keyStates;
    float direction;


    Player(uint16_t id) : id(id) {}
    ~Player() = default;

    data_packets::PlayerData dumpMovement(); // dump all data necessary for player movement
    void importChanges(data_packets::PlayerData);
};
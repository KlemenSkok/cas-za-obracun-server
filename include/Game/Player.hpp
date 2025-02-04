
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
    struct { float x, y; } acceleration;
    
    KeyStates keyStates;
    float direction;


    Player(uint16_t id) : id(id) {
        position.x = position.y = 0.0f;
        velocity.x = velocity.y = 0.0f;
        acceleration.x = acceleration.y = 0.0f;
    }
    ~Player() = default;

    void update(float deltaTime);

    data_packets::PlayerData dumpMovement(); // dump all data necessary for player movement
    void importKeyStates(data_packets::PlayerKeyStates);
};
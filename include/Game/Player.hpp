
// Player.hpp

#pragma once

#include "Communication/PacketTypes.hpp"
#include "Containers.hpp"

#include <cstdint>

class Player {

    uint16_t id;
    bool projectileTriggered;

public:

    struct { float x, y; } position;
    struct { float x, y; } velocity;
    struct { float x, y; } acceleration;

    KeyStates keyStates;
    float direction;
    Uint32 lastProjectileTime; // timestamp of last projectile throw (for cooldown)

    Player(uint16_t id) : id(id), projectileTriggered(false) {
        position.x = position.y = 0.0f;
        velocity.x = velocity.y = 0.0f;
        acceleration.x = acceleration.y = 0.0f;
    }
    ~Player() = default;
    uint16_t get_id();

    void update(float deltaTime);

    data_packets::PlayerData dumpMovement(); // dump all data necessary for player movement
    void importUpdates(data_packets::PlayerKeyStates, float);
    bool shotProjectile();
};
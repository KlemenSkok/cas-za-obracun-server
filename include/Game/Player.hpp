
// Player.hpp

#pragma once

#include "Communication/PacketTypes.hpp"
#include "Containers.hpp"

#include <cstdint>

class Player {

    uint16_t id;
    bool projectileTriggered;
    
    // Posture [0, 100]. When it runs out, player gets concussed (slower movement)
    int8_t posture;
    // Timestamp of last time when damage was taken. Used for healing timing
    Uint32 lastDamageTime;
    Uint32 lastHealTime;
    // Concussion occours on posture break. It lasts until posture equals 0
    bool isConcussed;

public:

    Point position;
    Point velocity;
    Point acceleration;

    KeyStates keyStates;
    float direction;
    Uint32 lastProjectileTime; // timestamp of last projectile throw (for cooldown)

    Player(uint16_t id) : id(id), projectileTriggered(false) {
        position.x = position.y = 0.0f;
        velocity.x = velocity.y = 0.0f;
        acceleration.x = acceleration.y = 0.0f;
        lastDamageTime = lastHealTime = SDL_GetTicks();
        posture = 100;
    }
    ~Player() = default;
    uint16_t get_id();

    void update(float deltaTime);
    void dealPostureDamage();

    data_packets::PlayerData dumpMovement(); // dump all data necessary for player movement
    void importUpdates(data_packets::PlayerKeyStates, float);
    bool shotProjectile();

    Point getPosition() const { return position; }
};
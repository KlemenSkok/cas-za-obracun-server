
// Player.hpp

#pragma once

#include "Communication/PacketTypes.hpp"
#include "Containers.hpp"
#include "Utilities/Constants.hpp"

#include <cstdint>

class Player {

    uint16_t id;
    bool projectileTriggered;
    bool interactionTriggered;
    
    // Posture [0, 100]. When it runs out, player gets concussed (slower movement)
    int8_t posture;
    // Timestamp of last time when damage was taken. Used for healing timing
    Uint32 lastDamageTime;
    Uint32 lastHealTime;
    // Concussion occours on posture break. It lasts until posture equals 0
    bool isConcussed;
    bool hasFlag;

    // players are split in two teams
    uint8_t teamNumber;

    bool controlsFreezed;

    // dynamic movement settings
    float nextFriction_k;
    float nextAcceleration_k;
    float nextSpeedCap;

public:

    PointF position;
    PointF velocity;
    PointF acceleration;

    KeyStates keyStates;
    float direction;
    Uint32 lastProjectileTime; // timestamp of last projectile throw (for cooldown)

    Player(uint16_t id) : 
        id(id), 
        projectileTriggered(false),
        controlsFreezed(false),
        posture(100),
        hasFlag(false),
        isConcussed(false),
        interactionTriggered(false),
        keyStates(KeyStates()),
        direction(0.0f),
        teamNumber(0),
        nextSpeedCap(PLAYER_MAX_SPEED),
        nextFriction_k(PLAYER_FRICTION),
        nextAcceleration_k(PLAYER_ACCELERATION)
    {
        position.x = 1534.0f;
        position.y = 228.0f;
        velocity.x = velocity.y = 0.0f;
        acceleration.x = acceleration.y = 0.0f;
        lastDamageTime = lastHealTime = lastProjectileTime = SDL_GetTicks();
    }
    ~Player() = default;
    uint16_t get_id();

    void update(float deltaTime);
    void dealPostureDamage();
    void reset();

    data_packets::PlayerData dumpMovement(); // dump all data necessary for player movement
    void importUpdates(data_packets::PlayerKeyStates, float);
    bool shotProjectile();
    bool isInteracting();
    bool isPostureBroken() const;

    void captureFlag();
    void dropFlag();

    uint8_t getTeam() const { return teamNumber; }
    void setTeam(uint8_t t) { teamNumber = t; }

    PointF getPosition() const { return position; }
    void setPosition(const PointF& p) { position = p; }

    void freezeControls();
    void unfreezeControls();

    void setNextFriction(float);
    void setNextAcceleration(float);
    void setNextSpeedCap(float);

};
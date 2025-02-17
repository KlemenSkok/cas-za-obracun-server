
// Projectile.hpp

#pragma once

#include "Communication/PacketTypes.hpp"

#include <cstdint>


class Projectile {
    
    uint16_t id;
    uint16_t parent_id;
    float angle;
    
public:

    struct { float x, y; } start;
    struct { float x, y; } position;
    struct { float x, y; } velocity;

    Projectile(float x, float y, float angle, uint16_t);
    ~Projectile() = default;
    
    uint16_t get_id();
    uint16_t get_parent_id();
    void update(float t);
    data_packets::ProjectileData dumpData();
    bool isOutOfRange();
};
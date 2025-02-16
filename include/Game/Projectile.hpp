
// Projectile.hpp

#pragma once

#include "Communication/PacketTypes.hpp"

#include <cstdint>


class Projectile {
    const int radius = 10;
    float angle;
    struct { float x, y; } position;
    struct { float x, y; } velocity;

    uint16_t id;
    
public:
    Projectile(float x, float y, float angle);
    ~Projectile() = default;
    
    void update(float t);
    data_packets::ProjectileData dumpData();

};
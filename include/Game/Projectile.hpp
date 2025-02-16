
// Projectile.hpp

#pragma once

#include "Communication/PacketTypes.hpp"

#include <cstdint>


class Projectile {
    const int radius = 10;
    float angle;
    
    struct { float x, y; } start;
    struct { float x, y; } position;
    struct { float x, y; } velocity;
    
    uint16_t id;
    
public:
    Projectile(float x, float y, float angle);
    ~Projectile() {
        std::cout << "Projectile destructor called.\n";
    }
    
    uint16_t get_id();
    void update(float t);
    data_packets::ProjectileData dumpData();
    bool isOutOfRange();
};
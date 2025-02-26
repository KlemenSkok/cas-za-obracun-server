
// Projectile.hpp

#pragma once

#include "Communication/PacketTypes.hpp"
#include "Containers.hpp"

#include <cstdint>


class Projectile {
    
    uint16_t id;
    uint16_t parent_id;
    float angle;
    uint8_t parent_team;
    
public:

    PointF start;
    PointF position;
    PointF velocity;

    Projectile(float x, float y, float angle, uint16_t, uint8_t);
    ~Projectile() = default;
    
    uint16_t get_id() const;
    uint16_t get_parent_id() const;
    uint8_t get_parent_team() const;

    void update(float t);
    data_packets::ProjectileData dumpData();
    bool isOutOfRange();
};
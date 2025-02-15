
// Projectile.hpp

#pragma once


class Projectile {
    const int radius = 10;
    float angle;
    struct { float x, y; } position;
    struct { float x, y; } velocity;
    
public:
    Projectile(float x, float y, float angle);
    ~Projectile() = default;
    
    void update(float t);
};
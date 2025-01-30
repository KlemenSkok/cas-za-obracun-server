
// Player.hpp

#pragma once

#include <cstdint>


class Player {

    uint16_t id;

public:

    struct {
        float x, y;
    } pos;
    float velocityX, velocityY;

    Player(uint16_t id) : id(id) {}
    ~Player() = default;

};
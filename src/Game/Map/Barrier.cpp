
// Barrier.cpp

#include "Game/Map/Barrier.hpp"



void Barrier::setPosition(float x, float y) {
    this->position.x = x;
    this->position.y = y;
}

void Barrier::setDimensions(int w, int h) {
    this->width = w;
    this->height = h;
}

void Barrier::setTexture(int id) {
    this->texture_id = id;
}

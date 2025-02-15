
// Projectile.cpp

#include "Game/Projectile.hpp"
#include "Utilities/Constants.hpp"

#include <cmath>


Projectile::Projectile(float x, float y, float angle) {
    this->position = { x, y };
    this->velocity.x = cosf(angle) * PROJECTILE_VELOCITY;
    this->velocity.y = sinf(angle) * PROJECTILE_VELOCITY;
}

void Projectile::update(float deltaTime) {
    this->position.x += this->velocity.x * deltaTime;
    this->position.y += this->velocity.y * deltaTime;
}
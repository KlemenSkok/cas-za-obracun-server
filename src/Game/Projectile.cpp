
// Projectile.cpp

#include "Game/Projectile.hpp"
#include "Utilities/Constants.hpp"

#include <cmath>


Projectile::Projectile(float x, float y, float angle) {

    static uint16_t id_counter = 0;
    this->id = id_counter++;

    this->position = { x, y };
    this->velocity.x = cosf(angle) * PROJECTILE_VELOCITY;
    this->velocity.y = sinf(angle) * PROJECTILE_VELOCITY;
}

void Projectile::update(float deltaTime) {
    this->position.x += this->velocity.x * deltaTime;
    this->position.y += this->velocity.y * deltaTime;
}

data_packets::ProjectileData Projectile::dumpData() {
    data_packets::ProjectileData data;

    data.id = this->id;
    // position
    data.position.x = this->position.x;
    data.position.y = this->position.y;
    // velocity
    data.velocity.x = this->velocity.x;
    data.velocity.y = this->velocity.y;

    return data;
}
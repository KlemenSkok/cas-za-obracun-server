
// Projectile.cpp

#include "Game/Projectile.hpp"
#include "Utilities/Constants.hpp"

#include <cmath>


uint16_t Projectile::get_id() const {
    return this->id;
}

uint16_t Projectile::get_parent_id() const {
    return this->parent_id;
}

uint8_t Projectile::get_parent_team() const {
    return this->parent_team;
}

Projectile::Projectile(float x, float y, float angle, uint16_t parent_id, uint8_t team) {

    static uint16_t id_counter = 0;
    this->id = id_counter++;
    this->parent_id = parent_id;
    this->parent_team = team;

    // convert angle back to radians
    constexpr float multiplier = M_PI / 180.0f;
    angle *= multiplier;

    this->position = { x, y };
    this->start = { x, y };
    this->velocity.x = -cosf(angle) * PROJECTILE_VELOCITY;
    this->velocity.y = -sinf(angle) * PROJECTILE_VELOCITY;

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
    // parent team
    data.parentTeam = this->parent_team;

    return data;
}

bool Projectile::isOutOfRange() {
    int dx = start.x - position.x;
    int dy = start.y - position.y;
    if((dx*dx + dy*dy) > PROJECTILE_RANGE) {
        return true;
    }
    return false;
}
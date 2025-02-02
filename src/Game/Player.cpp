
// Player.cpp

#include "Game/Player.hpp"


data_packets::PlayerData Player::dumpMovement() {
    data_packets::PlayerData data;

    data.id = this->id;
    // position
    data.position.x = this->position.x;
    data.position.y = this->position.y;
    // velocity
    data.velocity.x = this->velocity.x;
    data.velocity.y = this->velocity.y;
    // key states
    data.keyStates = this->keyStates;
    data.direction = this->direction;

    return data;
}
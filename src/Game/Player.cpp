
// Player.cpp

#include "Game/Player.hpp"
#include "Containers.hpp"


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
    data.keyStates = encodeKeyStates(this->keyStates);
    // direction
    data.direction = this->direction;

    return data;
}

void Player::importChanges(data_packets::PlayerData data) {
    decodeKeyStates(data.keyStates, this->keyStates);

    //! temporary:
    this->position.x = data.position.x;
    this->position.y = data.position.y;
}

// Player.cpp

#include "Game/Player.hpp"
#include "Containers.hpp"
#include "Utilities/Constants.hpp"


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

void Player::importKeyStates(data_packets::PlayerKeyStates data) {
    decodeKeyStates(data.keyStates, this->keyStates);

    // reset acceleration
    this->acceleration.x = 0.0f;
    this->acceleration.y = 0.0f;

    // apply acceleration based on key states
    if(this->keyStates.w) this->acceleration.y -= PLAYER_ACCELERATION;
    if(this->keyStates.s) this->acceleration.y += PLAYER_ACCELERATION;
    if(this->keyStates.a) this->acceleration.x -= PLAYER_ACCELERATION;
    if(this->keyStates.d) this->acceleration.x += PLAYER_ACCELERATION;

}

void Player::update(float deltaTime) {
    // update the player position
    this->velocity.x += this->acceleration.x * deltaTime;
    this->velocity.y += this->acceleration.y * deltaTime;

    this->position.x += this->velocity.x * deltaTime;
    this->position.y += this->velocity.y * deltaTime;

    // apply friction if no keys are pressed
    if(!this->keyStates.a && !this->keyStates.d) {
        if(this->velocity.x > 0) {
            this->velocity.x -= PLAYER_FRICTION * deltaTime;
            if(this->velocity.x < 0) this->velocity.x = 0;
        } else if(this->velocity.x < 0) {
            this->velocity.x += PLAYER_FRICTION * deltaTime;
            if(this->velocity.x > 0) this->velocity.x = 0;
        }
    }
    if(!this->keyStates.w && !this->keyStates.s) {
        if(this->velocity.y > 0) {
            this->velocity.y -= PLAYER_FRICTION * deltaTime;
            if(this->velocity.y < 0) this->velocity.y = 0;
        } else if(this->velocity.y < 0) {
            this->velocity.y += PLAYER_FRICTION * deltaTime;
            if(this->velocity.y > 0) this->velocity.y = 0;
        }
    }

    // clamp velocity
    if(this->velocity.x > PLAYER_MAX_SPEED) this->velocity.x = PLAYER_MAX_SPEED;
    if(this->velocity.x < -PLAYER_MAX_SPEED) this->velocity.x = -PLAYER_MAX_SPEED;
    if(this->velocity.y > PLAYER_MAX_SPEED) this->velocity.y = PLAYER_MAX_SPEED;
    if(this->velocity.y < -PLAYER_MAX_SPEED) this->velocity.y = -PLAYER_MAX_SPEED;

    // normalize diagonal velocity
    if(this->velocity.x == PLAYER_MAX_SPEED && this->velocity.y == PLAYER_MAX_SPEED) {
        this->velocity.x = PLAYER_MAX_SPEED / 1.4142f;
        this->velocity.y = PLAYER_MAX_SPEED / 1.4142f;
    }
    if(this->velocity.x == -PLAYER_MAX_SPEED && this->velocity.y == PLAYER_MAX_SPEED) {
        this->velocity.x = -PLAYER_MAX_SPEED / 1.4142f;
        this->velocity.y = PLAYER_MAX_SPEED / 1.4142f;
    }
    if(this->velocity.x == PLAYER_MAX_SPEED && this->velocity.y == -PLAYER_MAX_SPEED) {
        this->velocity.x = PLAYER_MAX_SPEED / 1.4142f;
        this->velocity.y = -PLAYER_MAX_SPEED / 1.4142f;
    }
    if(this->velocity.x == -PLAYER_MAX_SPEED && this->velocity.y == -PLAYER_MAX_SPEED) {
        this->velocity.x = -PLAYER_MAX_SPEED / 1.4142f;
        this->velocity.y = -PLAYER_MAX_SPEED / 1.4142f;
    }

    // update position
    this->position.x += this->velocity.x * deltaTime;
    this->position.y += this->velocity.y * deltaTime;

}
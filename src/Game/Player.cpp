
// Player.cpp

#include "Game/Player.hpp"
#include "Game/Map/MapData.hpp"
#include "Containers.hpp"
#include "Utilities/Constants.hpp"


uint16_t Player::get_id() {
    return this->id;
}

data_packets::PlayerData Player::dumpMovement() {
    data_packets::PlayerData data;

    data.id = this->id;
    // posture
    data.posture = this->posture;
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

void Player::importUpdates(data_packets::PlayerKeyStates data, float direction) {

    KeyStates ks;
    decodeKeyStates(data.keyStates, ks);

    if(ks.left_click && !this->keyStates.left_click && (SDL_GetTicks() - this->lastProjectileTime > PROJECTILE_THROW_COOLDOWN)) {
        this->lastProjectileTime = SDL_GetTicks();
        
        // trigger a projectile deployment (done by GameSession)
        this->projectileTriggered = true;
    }
    if(ks.use_button && !this->keyStates.use_button) {
        // player may interact with flag
        this->interactionTriggered = true;
    }

    this->keyStates = ks;
    this->direction = direction;
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

    // TODO: UPDATE POSTURE
    // raise and event on posture break
    // moves slower when posture == 0
    // starts healing within 3s after last hit (when posture < 100)
    if(this->posture < 100 && (SDL_GetTicks() - this->lastDamageTime > PLAYER_HEAL_DELAY)) {
        if(this->posture == 0) {
            this->posture += PLAYER_HEAL_AMOUNT; // per tick
            this->lastHealTime = SDL_GetTicks();
        }
        else if(SDL_GetTicks() - this->lastHealTime > PLAYER_HEAL_PERIOD) {
            this->posture += PLAYER_HEAL_AMOUNT;
            this->lastHealTime = SDL_GetTicks();
            if(this->posture > 100) {
                this->posture = 100;
            }
        }
    }
    

    PointF newPosition = { this->position.x, this->position.y };

    // update the player position
    this->velocity.x += this->acceleration.x * deltaTime;
    this->velocity.y += this->acceleration.y * deltaTime;

    newPosition.x += this->velocity.x * deltaTime;
    newPosition.y += this->velocity.y * deltaTime;

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

    // determine speed limit (players are slower when concussed)
    float speed_cap = (this->posture == 0) ? PLAYER_MAX_SPEED_SLOWED : (hasFlag) ? PLAYER_MAX_SPEED_CARRYING : PLAYER_MAX_SPEED;

    // clamp velocity
    if(this->velocity.x > speed_cap) this->velocity.x = speed_cap;
    if(this->velocity.x < -speed_cap) this->velocity.x = -speed_cap;
    if(this->velocity.y > speed_cap) this->velocity.y = speed_cap;
    if(this->velocity.y < -speed_cap) this->velocity.y = -speed_cap;

    // normalize diagonal velocity
    if(this->velocity.x == speed_cap && this->velocity.y == speed_cap) {
        this->velocity.x = speed_cap / 1.4142f;
        this->velocity.y = speed_cap / 1.4142f;
    }
    if(this->velocity.x == -speed_cap && this->velocity.y == speed_cap) {
        this->velocity.x = -speed_cap / 1.4142f;
        this->velocity.y = speed_cap / 1.4142f;
    }
    if(this->velocity.x == speed_cap && this->velocity.y == -speed_cap) {
        this->velocity.x = speed_cap / 1.4142f;
        this->velocity.y = -speed_cap / 1.4142f;
    }
    if(this->velocity.x == -speed_cap && this->velocity.y == -speed_cap) {
        this->velocity.x = -speed_cap / 1.4142f;
        this->velocity.y = -speed_cap / 1.4142f;
    }

    // update position
    newPosition.x += this->velocity.x * deltaTime;
    newPosition.y += this->velocity.y * deltaTime;

    if(MapData::CheckCollision(*this, newPosition)) {
        // collision detected; update position
        this->position = newPosition;
    }
    else {
        // no collision; update position
        this->position = newPosition;
    }

}

bool Player::shotProjectile() {
    if(this->projectileTriggered) {
        this->projectileTriggered = false;
        return true;
    }
    return false;
}

bool Player::isInteracting() {
    if(this->interactionTriggered) {
        this->interactionTriggered = false;
        return true;
    }
    return false;
}

void Player::dealPostureDamage() {
    this->posture -= PROJECTILE_POSTURE_DAMAGE;
    if(this->posture <= 0) {
        this->posture = 0;
        this->isConcussed = true;

        // flag dropped
    }

    this->lastDamageTime = SDL_GetTicks();
}

void Player::reset() {
    this->acceleration = { 0.0f, 0.0f };
    this->position = { 0.0f, 0.0f };
    this->direction = 0.0f;
    this->posture = 100;
    this->hasFlag = false;
    this->projectileTriggered = false;
    this->interactionTriggered = false;
    this->isConcussed = false;
    this->keyStates = KeyStates();
}

bool Player::isPostureBroken() const {
    return this->posture == 0;
}

void Player::captureFlag() {
    this->hasFlag = true;
}

void Player::dropFlag() {
    this->hasFlag = false;
}
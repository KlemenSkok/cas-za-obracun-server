
// Trap.cpp

#include "Game/Map/Trap.hpp"
#include "Utilities/Constants.hpp"


Trap::Trap() : 
    MapObject(MapObjType::TRAP) 
{
    this->friction = 1.0f;
    this->acceleration = 1.0f;
    this->max_speed = PLAYER_MAX_SPEED;
}

void Trap::setFriction(float kf) {
    this->friction = kf;
}
void Trap::setAccel(float ka) {
    this->acceleration = ka;
}
void Trap::setSpeed(float s) {
    this->max_speed = s;
}

float Trap::getFrictionCoefficient() const {
    return this->friction;
}
float Trap::getAccelerationCoefficient() const {
    return this->acceleration;
}
float Trap::getMaxSpeed() const {
    return this->max_speed;
}
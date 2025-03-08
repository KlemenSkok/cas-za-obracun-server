
// Trap.cpp

#include "Game/Map/Trap.hpp"


Trap::Trap() {
    this->friction = 1.0f;
    this->acceleration = 1.0f;
}

void Trap::setFriction(float kf) {
    this->friction = kf;
}
void Trap::setAccel(float ka) {
    this->acceleration = ka;
}

float Trap::getFrictionCoefficient() const {
    return this->friction;
}
float Trap::getAccelerationCoefficient() const {
    return this->acceleration;
}

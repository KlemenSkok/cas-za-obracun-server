
// Trap.cpp

#include "Game/Map/Trap.hpp"
#include "Utilities/Constants.hpp"


Trap::Trap() : 
    MapObject(MapObjType::TRAP) 
{
    this->friction_k = 1.0f;
    this->acceleration_k = 1.0f;
    this->max_speed = PLAYER_MAX_SPEED;
}

void Trap::setTrapType(char* t) {

    //std::cout << "Setting trap type: " << t << ".\n";
    if(!strcmp(t, "ice")) {
        this->friction_k = 0.1f;
        this->acceleration_k = 0.1f;
        this->max_speed = PLAYER_MAX_SPEED;
    }
    else if(!strcmp(t, "sand")) {
        this->friction_k = 10.0f;
        this->acceleration_k = 0.5f;
        this->max_speed = PLAYER_MAX_SPEED / 4;
    }

}

void Trap::setFriction(float kf) {
    this->friction_k = kf;
}
void Trap::setAccel(float ka) {
    this->acceleration_k = ka;
}
void Trap::setSpeed(float s) {
    this->max_speed = s;
}

float Trap::getFrictionCoefficient() const {
    return this->friction_k;
}
float Trap::getAccelerationCoefficient() const {
    return this->acceleration_k;
}
float Trap::getMaxSpeed() const {
    return this->max_speed;
}
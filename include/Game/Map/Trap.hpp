
// Trap.hpp

#pragma once

#include "MapObject.hpp"


class Trap : public MapObject {

    float friction_k; // friction coefficient
    float acceleration_k; // accel coefficient
    float max_speed;

public:

    Trap();

    void setTrapType(char*);
    void setFriction(float kf);
    void setAccel(float ka);
    void setSpeed(float s);

    float getFrictionCoefficient() const;
    float getAccelerationCoefficient() const;
    float getMaxSpeed() const;

};
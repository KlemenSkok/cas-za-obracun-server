
// Trap.hpp

#pragma once

#include "MapObject.hpp"


class Trap : public MapObject {

    float friction; // friction coefficient
    float acceleration; // accel coefficient

public:

    Trap();

    void setFriction(float kf);
    void setAccel(float ka);

    float getFrictionCoefficient() const;
    float getAccelerationCoefficient() const;

};
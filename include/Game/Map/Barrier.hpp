
// Barrier.hpp

#pragma once

#include "Containers.hpp"


class Barrier {

    Point position;
    int width, height;

    int texture_id;

public:

    Barrier() = default;

    void setPosition(float x, float y);
    void setDimensions(int w, int h);
    void setTexture(int id);
    Point getPosition() const { return position; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
};
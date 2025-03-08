
// MapObject.hpp

#pragma once

#include "Containers.hpp"


// base class for all static map objects
class MapObject {

protected:

    PointF position;
    Point size;

    int texture_id;

public:

    MapObject() = default;

    virtual void setPosition(float x, float y);
    virtual void setDimensions(int w, int h);
    virtual void setTexture(int id);
    virtual PointF getPosition() const;
    virtual int getWidth() const;
    virtual int getHeight() const;

};


inline void MapObject::setPosition(float x, float y) {
    this->position = {x, y};
}

inline void MapObject::setDimensions(int w, int h) {
    this->size = {w, h};
}

inline void MapObject::setTexture(int id ) {
    this->texture_id = id;
}

inline PointF MapObject::getPosition() const {
    return position;
}

inline int MapObject::getWidth() const {
    return size.x;
}

inline int MapObject::getHeight() const {
    return size.y;
}
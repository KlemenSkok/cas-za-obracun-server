
// Site.hpp

#pragma once


#include "Containers.hpp"


class Site {

    PointF position;
    Point size;
    uint8_t teamNumber;

public:

    Site() :
        position({0.0f, 0.0f}),
        size(Point{ 0, 0 }),
        teamNumber(0) {}

    void setPosition(const PointF&);
    PointF getPosition() const;
    
    void setSize(const Point&);
    Point getSize() const;
    
    void setTeam(uint8_t);
    uint8_t getTeam() const;

    bool checkFlagCollision(const PointF&, const Point&);

};
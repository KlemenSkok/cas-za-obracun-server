
// Site.hpp

#include "Containers.hpp"


class Site {

    PointF position;
    Point size;
    uint8_t teamNumber;
    
public:

    Site(uint8_t t) : 
        teamNumber(t) {}

    void setPosition(const PointF&);
    PointF getPosition() const;
    
    void setSize(const Point&);
    Point getSize() const;
    
    uint8_t getTeam() const;

    bool checkFlagCollision(const PointF&, const Point&);

};
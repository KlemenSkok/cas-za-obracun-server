
// Site.cpp

#include "Game/Site.hpp"



void Site::setPosition(const PointF& p) {
    this->position = p;
}

PointF Site::getPosition() const {
    return this->position;
}

void Site::setSize(const Point& s) {
    this->size = s;
}

Point Site::getSize() const {
    return this->size;
}

uint8_t Site::getTeam() const {
    return this->teamNumber;
}

bool Site::checkFlagCollision(const PointF& f_pos, const Point& f_size) {
    
    // the flag should be entirely inside the site to count as captured
    return  f_pos.x >= this->position.x && 
            f_pos.y >= this->position.y && 
            (f_pos.x + f_size.x) <= (this->position.x + this->size.x) && 
            (f_pos.y + f_size.y) <= (this->position.y + this->size.y);
        
}
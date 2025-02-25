
// Flag.cpp

#include "Game/Flag.hpp"


void Flag::update(float detaTime) {

}

void Flag::capture(uint16_t playerID) {
    this->_isCaptured = true;
    this->_isHome = false;
    this->carrierID = playerID;
}

void Flag::release() {
    this->_isCaptured = false;
    this->_isHome = false;
    this->carrierID = 0;
}

void Flag::reset() {
    this->_isCaptured = false;
    this->_isHome = true;
    this->carrierID = 0;
    this->position = this->homePosition;
}

bool Flag::isCaptured() const {
    return this->_isCaptured;
}

bool Flag::isHome() const {
    return this->_isHome;
}

uint16_t Flag::getCarrierID() const {
    return this->carrierID;
}

PointF Flag::getPosition() const {
    return this->position;
}

// set absolute position (this->x == p.x, this->y == p.y)
void Flag::setPosition(const PointF& p) {
    this->position.x = p.x;
    this->position.y = p.y;
}

// set position relative to carrier's position (flag center == player center)
void Flag::updatePosition(const PointF& p) {
    this->position.x = p.x - this->size.x / 2.0f;
    this->position.y = p.y - this->size.y / 2.0f;
}

void Flag::dropFlag() {
    this->_isCaptured = false;
    this->carrierID = 0;
}


data_packets::FlagData Flag::dumpData() const {
    using namespace data_packets;

    FlagData data;
    data.carrierID = this->carrierID;
    data.position = this->position;
    
    // encode other data

    // ----
    
    return data;

}
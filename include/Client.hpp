
// Client.hpp

#pragma once

#include "../include/Utility.hpp"

#include <set>
#include <cstdint>

class Client {
private:
    uint16_t id; // == UDP channel


public:
    Client(int id) : id(id) {}
    uint16_t get_id();

};
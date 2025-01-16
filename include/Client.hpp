
// Client.hpp

#pragma once

#include "../include/Utility.hpp"

#include <set>
#include <cstdint>
#include <SDL2/SDL_net.h>

class Client {
private:
    uint16_t id; // == UDP channel
    IPaddress ip;


public:
    Client(int id, IPaddress ip) : id(id), ip(ip) {
        //std::cout << "Client created\n";
    }
    ~Client() {
        std::cout << "Client destructor called!\n";
    }
    uint16_t get_id();
    IPaddress get_ip();

};

// Client.hpp

#pragma once

#include <set>
#include <cstdint>
#include <SDL2/SDL_net.h>
#include <chrono>
#include <memory>

class Client {
private:
    uint16_t id; // == UDP channel for SocketListener

    struct {
        IPaddress ip;
        uint32_t lastPacketID;
        std::chrono::steady_clock::time_point lastPacketTime;
    } connection;



public:
    Client(int id, IPaddress ip) : id(id) {
        //std::cout << "Client created\n";
        this->connection.ip = ip;
        this->connection.lastPacketTime = std::chrono::steady_clock::now();
        this->connection.lastPacketID = 0;
    }
    ~Client() {
        //std::cout << "Client destructor called!\n";
    }
    uint16_t get_id();
    IPaddress get_ip();
    bool checkTimeout();
    void updatePacketTime();
    void updatePacketID(uint32_t p_id);
    uint32_t getLastPacketID();

};
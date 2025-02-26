
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
        uint32_t lastRecvPacketID,
                 lastSentPacketID;
        std::chrono::steady_clock::time_point lastRecvPacketTime;
    } connection;



public:
    Client(int id, IPaddress ip) : id(id) {
        //std::cout << "Client created\n";
        this->connection.ip = ip;
        this->connection.lastRecvPacketTime = std::chrono::steady_clock::now();
        this->connection.lastRecvPacketID = 0;
        this->connection.lastSentPacketID = 0;
    }
    ~Client() {
        //std::cout << "Client destructor called!\n";
    }
    uint16_t get_id();
    IPaddress get_ip();
    bool checkTimeout();
    void updateLastRecvPacketTime();
    void updateLastRecvPacketID(uint32_t p_id);
    uint32_t getLastRecvPacketID();
    uint32_t getLastSentPacketID();

};
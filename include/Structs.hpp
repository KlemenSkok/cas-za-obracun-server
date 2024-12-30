
// Structs.hpp

#pragma once

#include <SDL2/SDL_net.h>
#include <memory>
#include <utility>
#include <cstring>

// tukaj so definirane strukture za celoten server


struct UDPmessage {
    int channel; // packet channel
    std::unique_ptr<Uint8[]> data;
    int len;
    std::unique_ptr<IPaddress> ip;

    // kopirni konstruktor, ki prepreci ponavljanje kazalcev na data in ip (error double free)
    UDPmessage(UDPmessage&& other) : 
        channel(other.channel), 
        data(std::move(other.data)), 
        len(other.len), 
        ip(std::move(other.ip)) {}
    // konstruktor direktno iz UDPpacket
    UDPmessage(const UDPpacket *packet) : 
        channel(packet->channel), 
        len(packet->len) 
    {
        data = std::make_unique<Uint8[]>(len);
        std::memcpy(data.get(), packet->data, len);

        if(channel == -1) {
            ip = std::make_unique<IPaddress>(packet->address);
        }
    }
    // se vedno rabmo default konstruktor
    UDPmessage() : channel(-1), len(0) {}
    
    // assigment overload, da se unique pointerji ne ponavljajo. sprejme rvalue
    UDPmessage& operator=(UDPmessage&& other) {
        if(this != &other) { // preprecimo self assigment
            channel = other.channel;
            data = std::move(other.data);
            len = other.len;
            ip = std::move(other.ip);
        }
        return *this;
    }


    // izbrisemo default kopirni konstruktor, da uporabi zgornjega
    UDPmessage(const UDPmessage&) = delete;
    UDPmessage& operator=(const UDPmessage&) = delete;

    // default destructor, ker se pointerji pobrisejo avtomatsko
    ~UDPmessage() = default;

};
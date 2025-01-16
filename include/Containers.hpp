
// Structs.hpp

#pragma once

#include "Logging/Logger.hpp"

#include <SDL2/SDL_net.h>
#include <memory>
#include <utility>
#include <cstring>
#include <vector>

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
        // kopiramo podatke
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


class PacketData {
    std::vector<Uint8> data;

    // zaradi novih clientov
    IPaddress ip;

public:
    PacketData() = default; // prazen paket
    PacketData(const PacketData&) = default; // kopirni konstruktor
    PacketData(bool set_flags) : data(set_flags ? std::vector<Uint8>{0} : std::vector<Uint8>{}) {}
    PacketData(const Uint8* data, int len); // nov paket iz raw sporocila

    std::unique_ptr<Uint8[]> getRawData(); // vrne Uint8* kazalec na podatke
    size_t size() const; // vrne dolzino podatkov
    void clear(); // izbrise vse podatke
    void reset();
    std::string dump();
    
    // adding data
    template<typename T>
    void append(T data);  // doda podatke na konec paketa
    void append(const Uint8* data, int len);

    // accessing data
    Uint8& flags(); // vrne prvi byte (server flags)
    Uint8& operator[](int i); // vrne i-ti byte
    template<typename T>
    void getByOffset(size_t offset, size_t size, T& target); // vrne podatek tipa T na offsetu

};


// template funkcije MORAJO bit v isti datoteki kot class definition, ker mora
// compiler takoj najt definicijo funkcije da lahko nardi potrebne overloade
// 
// pazi: ce je treba meta deklaracije posebi, je treba v .cpp file dodat tole:
// Explicit template instantiations
// 
// template void PacketData::getByOffset<unsigned char>(size_t, size_t, unsigned char&);
// template void PacketData::getByOffset<unsigned short>(size_t, size_t, unsigned short&);

template<typename T>
void PacketData::getByOffset(size_t offset, size_t size, T& target) {
    if(offset + size > data.size()) {
        //Logger::warn("Offset out of bounds - PacketData::getByOffset().");
        return;
    }

    std::memcpy(&target, &data[offset], size);
}


// ta funkcija kontra zapise bajte. todo: fix
template<typename T>
void PacketData::append(T data) {
    // resize the vector and append the data
    size_t current_size = this->data.size();
    this->data.resize(current_size + sizeof(T)); // resize vector
    std::memcpy(&this->data[current_size], &data, sizeof(T)); // append data
}

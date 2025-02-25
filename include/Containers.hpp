
// Structs.hpp

#pragma once

#include "Logging/Logger.hpp"

#include <SDL2/SDL_net.h>
#include <memory>
#include <utility>
#include <cstring>
#include <vector>

// tukaj so definirane strukture za celoten server


// --------------------------------------- //
//                KEY STATES               //
// --------------------------------------- //

// w,a,s,d key states + left mouse button and interaction button
typedef struct {
    uint8_t w = 0, 
            a = 0, 
            s = 0, 
            d = 0,
            left_click = 0,
            use_button = 0;
} KeyStates;
uint8_t encodeKeyStates(const KeyStates& ks);
void decodeKeyStates(const uint8_t& data, KeyStates& ks);

typedef struct {
    float x, y;
} PointF;

typedef struct {
    float x, y;
} Point;

// ---------------------------------------- //
//                UDP MESSAGE               //
// ---------------------------------------- //

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

        ip = std::make_unique<IPaddress>(packet->address);
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

// ----------------------------------------- //
//                PACKET DATA                //
// ----------------------------------------- //

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
    void getByOffset(T& target, size_t size, size_t offset); // vrne podatek tipa T na offsetu

};


// template funkcije MORAJO bit v isti datoteki kot class definition, ker mora
// compiler takoj najt definicijo funkcije da lahko nardi potrebne overloade
// 
// pazi: ce je treba meta deklaracije posebi, je treba v .cpp file dodat tole:
// Explicit template instantiations
// 
// template void PacketData::getByOffset<unsigned char>(size_t, size_t, unsigned char&);
// template void PacketData::getByOffset<unsigned short>(size_t, size_t, unsigned short&);

/**
 * Prebere podatek tipa T iz podatkov na offsetu. 
 * V paketu je podatek zapisan v network order (big endian)
 @attention Ce je offset izven meja, vrze std::out_of_range
 @param target Spremenljivka, v katero prepisemo data
 @param size Velikost podatka [B]
 @param offset Polozaj podatka v paketu (odmik prvega bajta [B])
 */
template<typename T>
void PacketData::getByOffset(T& target, size_t size, size_t offset) {
    static_assert(std::is_integral<T>::value, "Only integral types are supported.");

    if (offset + size > data.size()) {
        throw std::out_of_range("Offset out of bounds in PacketData::getByOffset.");
    }

    if constexpr (sizeof(T) == 1) {
        // For 1-byte types, no need for endianness conversion
        target = static_cast<T>(data[offset]);
    } else if constexpr (sizeof(T) == 2) {
        // Use SDLNet_Read16 for 2-byte types
        Uint16 network_order;
        std::memcpy(&network_order, &data[offset], sizeof(Uint16));
        target = SDLNet_Read16(&network_order);
    } else if constexpr (sizeof(T) == 4) {
        // Use SDLNet_Read32 for 4-byte types
        Uint32 network_order;
        std::memcpy(&network_order, &data[offset], sizeof(Uint32));
        target = SDLNet_Read32(&network_order);
    } else {
        throw std::runtime_error("Unsupported type size for PacketData::getByOffset.");
    }
}

/**
 * Funkcija podatke zapise podatke v formatu BIG ENDIAN (network order) == MSB first
 * (system default je little endian (x86), LSB first)
 * @param data poljuben (naceloma primitiven) podatkovni tip velikosti 1, 2 ali 4B
 */
template<typename T>
void PacketData::append(T data) {
    static_assert(std::is_integral<T>::value, "Only integral types are supported.");

    if constexpr (sizeof(T) == 1) {
        // For 1-byte types, no need for endianness conversion
        this->data.push_back(static_cast<Uint8>(data));
    } else if constexpr (sizeof(T) == 2) {
        // Use SDLNet_Write16 for 2-byte types
        Uint16 network_order;
        SDLNet_Write16(data, &network_order);
        this->data.insert(this->data.end(), reinterpret_cast<Uint8*>(&network_order),
                          reinterpret_cast<Uint8*>(&network_order) + sizeof(Uint16));
    } else if constexpr (sizeof(T) == 4) {
        // Use SDLNet_Write32 for 4-byte types
        Uint32 network_order;
        SDLNet_Write32(data, &network_order);
        this->data.insert(this->data.end(), reinterpret_cast<Uint8*>(&network_order),
                          reinterpret_cast<Uint8*>(&network_order) + sizeof(Uint32));
    } else {
        throw std::runtime_error("Unsupported type size for PacketData::append.");
    }
}

// template overloads for float types
template<>
void PacketData::append(float data);

template<>
void PacketData::getByOffset(float& target, size_t size, size_t offset);

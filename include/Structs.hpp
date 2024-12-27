
// Structs.hpp

#pragma once

#include <SDL2/SDL_net.h>

// tukaj so definirane strukture za celoten server


struct UDPmessage {
    int channel; // packet channel
    Uint8 *data;
    int len;
    IPaddress *ip;

    UDPmessage() {
        data = nullptr;
        ip = nullptr;
    }
    ~UDPmessage() {
        delete[] data;
        delete ip;
    }
};
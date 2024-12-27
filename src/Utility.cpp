
// Utility.cpp

#include "../include/Utility.hpp"

namespace SDLUtils {
    bool initialize() {
        // init SDL
        if(SDL_Init(SDL_INIT_EVENTS) != 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
            return false;    
        }

        // init SDL_net
        if(SDLNet_Init() != 0) {
            std::cerr << "SDLNet_Init Error: " << SDLNet_GetError() << "\n";
            return false;
        }

        return true;
    }

    void cleanup() {
        SDLNet_Quit();
        SDL_Quit();
    }
}

std::string formatIP(Uint32 ip) {
    std::string out;
    for(int i = 0; i < 4; i++) {
        out += std::to_string(((ip & (0xff << i*8)) >> i*8)) + ((i != 3) ? "." : "");
    }
    return out;
}

void dumpPacket(UDPpacket *packet) {
    // dump packet data
    std::cout << "Received packet of size " << packet->len << " bytes.\n";
    std::cout << "Packet content: " << packet->data << "\n";
    std::cout << "Other packet data: \n" << 
        "\tSource address: " << formatIP(packet->address.host) << ":" << packet->address.port << "\n" <<
        "\tChannel: " << packet->channel << "\n" <<
        "\tStatus: " << packet->status << "\n" <<
        "\tMaxLen: " << packet->maxlen << "\n";
    std::cout << "\n";
}
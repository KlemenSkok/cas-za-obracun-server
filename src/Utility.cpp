
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
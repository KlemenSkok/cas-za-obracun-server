
// PacketTypes.hpp

#pragma once

#include "Containers.hpp"

#include <cstdint>
#include <vector>
#include <SDL2/SDL_stdinc.h>

enum class PacketType: uint8_t {
    // client->server
    PLAYER_UPDATES, // packet sent from client to update movement keys states
    // ...

    // server->client
    PLAYERS_IN_RANGE, // info about other players (and self) that are sent periodically
    GAME_STATE // game state, score, time remaining etc.
};


namespace data_packets {
    
    struct PlayerData {
        uint16_t id;
        struct { float x, y; } position;
        struct { float x, y; } velocity;
        uint8_t keyStates;
        float direction;

        static int size();
        void serialize(PacketData&) const;
        void deserialize(PacketData&, size_t);
        PlayerData();
    };

}
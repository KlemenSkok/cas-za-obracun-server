
// PacketTypes.hpp

#pragma once

#include "Containers.hpp"

#include <cstdint>
#include <vector>
#include <SDL2/SDL_stdinc.h>


enum class PacketType: uint8_t {
    // client->server
    PLAYER_UPDATES, // packet sent from client to update movement keys states. a single PlayerKeyStates
    // ...

    // server->client
    PLAYERS_IN_RANGE, // info about other players (and self) that are sent periodically. a list of PlayerData
    GAME_STATE // game state, score, time remaining etc.
    // ...
};


namespace data_packets {
    
    struct PlayerData {
        uint16_t id;
        struct { float x, y; } position;
        struct { float x, y; } velocity;
        uint8_t keyStates;
        float direction;

        Uint32 timestamp; // server timestamp for client side interpolation

        static int size();
        void serialize(PacketData&) const;
        void deserialize(PacketData&, size_t);
        PlayerData& operator=(const PlayerData&);
        PlayerData();
    };

    struct PlayerKeyStates {
        uint8_t keyStates = 0;
    
        static int size();
        void serialize(PacketData&) const;
        void deserialize(PacketData&, size_t);
    };

}
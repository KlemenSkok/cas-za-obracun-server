
// PacketTypes.hpp

#pragma once

#include "Containers.hpp"

#include <cstdint>
#include <vector>
#include <SDL2/SDL_stdinc.h>


enum class PacketType: uint8_t {
    // client->server
    PLAYER_UPDATES,             // packet sent from client to update movement keys states. a single PlayerKeyStates
    // ...

    // server->client
    PLAYERS_IN_RANGE,           // info about other players (and self) that are sent periodically. a list of PlayerData
    PROJECTILES_IN_RANGE,       // info about active projectiles. a list of ProjectileData
    GAME_STATE                  // game state, score, time remaining etc.
    // ...
};


namespace data_packets {
    
    // all data neccesary for clients to render players
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

    // keystate data of clients sent to server
    struct PlayerKeyStates {
        uint8_t keyStates = 0;
    
        static int size();
        void serialize(PacketData&) const;
        void deserialize(PacketData&, size_t);
    };

    // data for client side projectile rendering
    struct ProjectileData {
        uint16_t id;
        struct { float x, y; } position;
        struct { float x, y; } velocity;

        Uint32 timestamp; // server timestamp for client side interpolation

        static int size();
        void serialize(PacketData&) const;
        void deserialize(PacketData&, size_t);
        ProjectileData& operator=(const ProjectileData&);
        ProjectileData();
    };
}
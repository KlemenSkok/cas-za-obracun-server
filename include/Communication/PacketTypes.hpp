
// PacketTypes.hpp

#pragma once

#include "Containers.hpp"
#include "Utilities/Utility.hpp"

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
    GAME_STATE,                 // game state, score, time remaining etc.
    FLAG_STATE,                 // flag state, position, carrier etc.
    // ...
};


namespace data_packets {
    
    // all data neccesary for clients to render players
    struct PlayerData {
        uint16_t id;
        int8_t posture;
        PointF position;
        PointF velocity;
        uint8_t keyStates;
        float direction;
        uint8_t teamNumber;

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
        PointF position;
        PointF velocity;

        Uint32 timestamp; // server timestamp for client side interpolation

        static int size();
        void serialize(PacketData&) const;
        void deserialize(PacketData&, size_t);
        ProjectileData& operator=(const ProjectileData&);
        ProjectileData();
    };

    // flag data
    struct FlagData {
        PointF position;
        uint16_t carrierID;
        uint8_t otherData;

        Uint32 timestamp; // server timestamp for client side interpolation

        static int size();
        void serialize(PacketData&) const;
        void deserialize(PacketData&, size_t);
        FlagData& operator=(const FlagData&);
        FlagData();
    };

    // data about game for clients (enum GAME_STATE)
    struct GameStateData {
        GameState gameState;
        uint8_t teamScores; // first 4 bits are team 1, last 4 bits are team 2 
        Uint32 elapsedTime;

        Uint32 serverTime; // server timestamp for client side interpolation


        static int size();
        void serialize(PacketData&) const;
        void deserialize(PacketData&, size_t);
        GameStateData& operator=(const GameStateData&);
        GameStateData();
    };

}
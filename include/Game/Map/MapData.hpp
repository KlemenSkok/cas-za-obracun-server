
// MapData.hpp

#pragma once

#include "Barrier.hpp"
#include "Game/Site.hpp"
#include "Game/Player.hpp"
#include "Game/Map/Trap.hpp"
#include "Game/Projectile.hpp"
#include "Game/Map/MapObject.hpp"

#include <vector>
#include <unordered_map>
#include <memory>

#define GRID_CELL_SIZE 100 // [px]

class LocalPlayer;
class Projectile;

class MapData {

public:

    static std::unordered_map<uint16_t, std::unordered_map<uint16_t, std::vector<std::shared_ptr<MapObject>>>> grid;
    static std::unordered_map<uint8_t, std::shared_ptr<Site>> sites;


    static void InitializeGrid();
    static void AddBarrier(Barrier&);
    static void AddTrap(Trap&);
    static int LoadMap(const char*);
    static bool CheckCollision(Player&, PointF&);
    static bool CheckCollision(const Projectile&, PointF&);

};

inline int getGridKey(float val) {
    return static_cast<int>(val) / GRID_CELL_SIZE;
}
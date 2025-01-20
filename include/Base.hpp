
// Base.hpp

#pragma once

#include "Utilities/Utility.hpp"


// base object for server
// inherited by server, game sessions, clients...
/*class BaseServerObject {
protected:
    int _id; // object id

public:
    BaseServerObject() : _id(IDGenerator::GetNextID()) {  }
    int GetID() { return this->_id; }
    
    virtual void Init() = 0;
    virtual void Despose() = 0;
};*/


// base game object
// inherited by players, map, tiles and other objects
/* class BaseGameObject {
protected:
    static int BASE_ID;

    typedef struct {
        int x, y;
    } position;

public:
    int base_id;

    BaseGameObject() {
        base_id = BASE_ID;
        BASE_ID++;
    }
    virtual void Init() = 0;
    virtual void Dispose() = 0;
};

int BaseGameObject::BASE_ID = 1; */

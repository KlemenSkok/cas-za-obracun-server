
// Flag.hpp

#include "Containers.hpp"
#include "Utilities/Constants.hpp"
#include "Communication/PacketTypes.hpp"


class Flag  {

    PointF position;
    PointF homePosition;
    Point size;

    uint16_t carrierID;
    bool _isHome;
    bool _isCaptured;

public:
    
    Flag(float x, float y) :
        position({x, y}), 
        homePosition(GAME_FLAG_HOME_POS), 
        size(GAME_FLAG_SIZE), 
        carrierID(0), 
        _isHome(true), 
        _isCaptured(false) {}

    ~Flag() = default;

    void update(float deltaTime);
    
    void capture(uint16_t playerID);
    void release();
    void reset();

    bool isCaptured() const;
    bool isHome() const;
    uint16_t getCarrierID() const;
    PointF getPosition() const;

    void setPosition(const PointF&);
    void updatePosition(const PointF&);
    void dropFlag();

    data_packets::FlagData dumpData() const;

};
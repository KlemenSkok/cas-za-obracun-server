
// PacketTypes.cpp


#include "Communication/PacketTypes.hpp"

using namespace data_packets;



int PlayerData::size() {
    constexpr int size = (sizeof(uint16_t) +    // id
                          sizeof(int8_t) +      // posture
                          sizeof(uint8_t) +     // keyStates
                          5 * sizeof(float)) +  // position + velocity + direction
                          sizeof(Uint32);       // server timestamp
    return size;
}

PlayerData::PlayerData() 
    : id(0), 
    posture(0),
    position{0.0f, 0.0f}, 
    velocity{0.0f, 0.0f}, 
    keyStates(0), 
    direction(0.0f), 
    timestamp(SDL_GetTicks()) {}



// Member function definition
void PlayerData::serialize(PacketData& packet) const {
    packet.append(id);
    packet.append(posture);
    packet.append(position.x);
    packet.append(position.y);
    packet.append(velocity.x);
    packet.append(velocity.y);
    packet.append(keyStates);
    packet.append(direction);
    packet.append(timestamp);
}

void PlayerData::deserialize(PacketData& packet, size_t offset) {
    // id
    packet.getByOffset(id, sizeof(uint16_t), offset);
    offset += sizeof(uint16_t);
    // posture
    packet.getByOffset(posture, sizeof(int8_t), offset);
    offset += sizeof(int8_t);
    // position
    packet.getByOffset(position.x, sizeof(float), offset);
    packet.getByOffset(position.y, sizeof(float), offset + sizeof(float));
    offset += 2 * sizeof(float);
    // velocity
    packet.getByOffset(velocity.x, sizeof(float), offset);
    packet.getByOffset(velocity.y, sizeof(float), offset + sizeof(float));
    offset += 2 * sizeof(float);
    // keyStates
    packet.getByOffset(keyStates, sizeof(uint8_t), offset);
    offset += sizeof(uint8_t);
    // direction
    packet.getByOffset(direction, sizeof(float), offset);
    offset += sizeof(float);
    // timestamp
    packet.getByOffset(timestamp, sizeof(Uint32), offset);
    offset += sizeof(Uint32);
}

PlayerData& PlayerData::operator=(const PlayerData& other) {
    if(this == &other)
        return *this;

    this->id = other.id;
    this->posture = other.posture;
    this->position.x = other.position.x;
    this->position.y = other.position.y;
    this->velocity.x = other.velocity.x;
    this->velocity.y = other.velocity.y;
    this->keyStates = other.keyStates;
    this->direction = other.direction;
    this->timestamp = other.timestamp;

    return *this;
}



// PlayerKeyStates

int PlayerKeyStates::size() {
    return sizeof(uint8_t);
}

void PlayerKeyStates::serialize(PacketData& packet) const {
    packet.append(keyStates);
}

void PlayerKeyStates::deserialize(PacketData& packet, size_t offset) {
    packet.getByOffset(keyStates, sizeof(uint8_t), offset);
}

// ProjectileData

int ProjectileData::size() {
    constexpr int size = sizeof(uint16_t)
                        + sizeof(float) * 4
                        + sizeof(Uint32);
    return size;
}

void ProjectileData::serialize(PacketData& packet) const {
    packet.append(id);
    packet.append(position.x);
    packet.append(position.y);
    packet.append(velocity.x);
    packet.append(velocity.y);
    packet.append(timestamp);
}

void ProjectileData::deserialize(PacketData& packet, size_t offset) {
    // id
    packet.getByOffset(id, sizeof(uint16_t), offset);
    offset += sizeof(uint16_t);
    // position
    packet.getByOffset(position.x, sizeof(float), offset);
    packet.getByOffset(position.y, sizeof(float), offset + sizeof(float));
    offset += sizeof(float) * 2;
    // velocity
    packet.getByOffset(velocity.x, sizeof(float), offset);
    packet.getByOffset(velocity.y, sizeof(float), offset + sizeof(float));
    offset += sizeof(float) * 2;
    // timestamp
    packet.getByOffset(timestamp, sizeof(Uint32), offset);
    offset += sizeof(Uint32);
}

ProjectileData& ProjectileData::operator=(const ProjectileData& other) {
    if(this == &other)
        return *this;

    this->id = other.id;
    this->position.x = other.position.x;
    this->position.y = other.position.y;
    this->velocity.x = other.velocity.x;
    this->velocity.y = other.velocity.y;
    this->timestamp = other.timestamp;

    return *this;
}

ProjectileData::ProjectileData() 
    : id(0), 
    position{0.0f, 0.0f}, 
    velocity{0.0f, 0.0f}, 
    timestamp(SDL_GetTicks()) {}

// FlagData

int FlagData::size() {
    constexpr int size = 2 * sizeof(float) +        // position
                        sizeof(uint16_t) +      // carrierID
                        sizeof(uint8_t) +       // otherData
                        sizeof(Uint32);         // server timestamp
    return size;
}

void FlagData::serialize(PacketData& packet) const {
    packet.append(position.x);
    packet.append(position.y);
    packet.append(carrierID);
    packet.append(otherData);
    packet.append(timestamp);
}

void FlagData::deserialize(PacketData& packet, size_t offset) {
    // position
    packet.getByOffset(position.x, sizeof(float), offset);
    packet.getByOffset(position.y, sizeof(float), offset + sizeof(float));
    offset += 2 * sizeof(float);
    // carrierID
    packet.getByOffset(carrierID, sizeof(uint16_t), offset);
    offset += sizeof(uint16_t);
    // otherData
    packet.getByOffset(otherData, sizeof(uint8_t), offset);
    offset += sizeof(uint8_t);
    // timestamp
    packet.getByOffset(timestamp, sizeof(Uint32), offset);
    offset += sizeof(Uint32);
}

FlagData& FlagData::operator=(const FlagData& other) {
    if(this == &other)
        return *this;

    this->position.x = other.position.x;
    this->position.y = other.position.y;
    this->carrierID = other.carrierID;
    this->otherData = other.otherData;
    this->timestamp = other.timestamp;

    return *this;
}

FlagData::FlagData() 
    : position{0.0f, 0.0f}, 
    carrierID(0), 
    otherData(0), 
    timestamp(SDL_GetTicks()) {}
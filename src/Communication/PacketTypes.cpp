
// PacketTypes.cpp


#include "Communication/PacketTypes.hpp"

using namespace data_packets;



int PlayerData::size() {
    constexpr int size = (sizeof(uint16_t) +    // id
                          sizeof(uint8_t) +     // keyStates
                          5 * sizeof(float));   // position + velocity + direction
    return size;
}

PlayerData::PlayerData() 
    : id(0), position{0.0f, 0.0f}, velocity{0.0f, 0.0f}, keyStates(0), direction(0.0f) {}



// Member function definition
void PlayerData::serialize(PacketData& packet) const {
    packet.append(id);
    packet.append(position.x);
    packet.append(position.y);
    packet.append(velocity.x);
    packet.append(velocity.y);
    packet.append(keyStates);
    packet.append(direction);
}

void PlayerData::deserialize(PacketData& packet, size_t offset) {
    // id
    packet.getByOffset(id, sizeof(uint16_t), offset);
    offset += sizeof(uint16_t);
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
    packet.getByOffset(position.x, sizeof(float), offset);
    offset += sizeof(float);
}
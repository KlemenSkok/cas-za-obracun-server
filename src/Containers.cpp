
// Containers.cpp

#include "Containers.hpp"
#include "Logging/Logger.hpp"

#include <sstream>

// -------------------------------------------------//
//                                                  //
//                  UDP MESSAGE                     //
//                                                  //
// -------------------------------------------------//

PacketData::PacketData(const Uint8* data, int len) {
    if(data == nullptr || len < 1) {
        return; // ignore empty data
    }
    // kopiramo podatke v vektor
    std::copy(data, data + len, std::back_inserter(this->data));

    // ce je dolzina manjsa od 1, dodamo se en byte
    if(this->data.size() < 1) {
        this->data.push_back(0);
    }
}

std::unique_ptr<Uint8[]> PacketData::getRawData() {
    Uint8* out = new Uint8[data.size()];
    std::memcpy(out, data.data(), data.size());
    return std::unique_ptr<Uint8[]>(out);
}

size_t PacketData::size() const {
    return data.size();
}

void PacketData::clear() {
    data.clear();
}

void PacketData::reset() {
    data.clear();
    data.push_back(0); // vedno imamo vsaj en byte
}

Uint8& PacketData::flags() {
    if(data.size() < 0)
        data.push_back(0);
    return data[0];
}

Uint8& PacketData::operator[](int i) {
    return data[i];
}

void PacketData::append(const Uint8* data, int len) {
    this->data.insert(this->data.end(), data, data + len);
}

std::string PacketData::dump() {
    std::ostringstream oss;
    oss << "[";

    for (size_t i = 0; i < data.size(); ++i) {
        oss << static_cast<int>(data.data()[i]); // Cast to int for proper representation
        if (i < data.size() - 1) {
            oss << ", ";
        }
    }

    oss << "]";
    return oss.str();
}

template<>
void PacketData::append<float>(float data) {
    static_assert(sizeof(float) == 4, "Unexpected float size.");

    Uint32 asInt;
    std::memcpy(&asInt, &data, sizeof(float));

    Uint32 network_order;
    SDLNet_Write32(asInt, &network_order);

    this->data.insert(this->data.end(),
                      reinterpret_cast<Uint8*>(&network_order),
                      reinterpret_cast<Uint8*>(&network_order) + sizeof(Uint32));
}

template<>
void PacketData::getByOffset(float& target, size_t size, size_t offset) {
    if (offset + sizeof(float) > data.size()) {
        throw std::out_of_range("Offset out of bounds");
    }

    Uint32 network_order;
    std::memcpy(&network_order, &data[offset], sizeof(Uint32));

    Uint32 host_order = SDLNet_Read32(&network_order);
    std::memcpy(&target, &host_order, sizeof(float)); // Reinterpret as float
}



// template funkcije premaknjene direkt v header
/*
template<typename T>
void PacketData::append(T data) {
    // resize the vector and append the data
    size_t current_size = this->data.size();
    this->data.resize(current_size + sizeof(T)); // resize vector
    std::memcpy(&this->data[current_size], &data, sizeof(T)); // append data
}
*/
/*
template<typename T>
void PacketData::getByOffset(size_t offset, size_t size, T& target) {
    if(offset + size > data.size()) {
        Logger::warn("Offset out of bounds - PacketData::getByOffset().");
        return;
    }

    std::memcpy(&target, &data[offset], size);
}
*/
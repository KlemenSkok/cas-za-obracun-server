
// Client.cpp

#include "Game/Client.hpp"
#include "Utilities/Constants.hpp"


// static members and function definitions

uint16_t Client::get_id() {
    return id;
}

IPaddress Client::get_ip() {
    return this->connection.ip;
}

/**
 * Client, ki bo dolocen cas ne bo poslal sporocila (niti keepalive) bo kickan.
 * Ta funkcija doloci, ali je bil timeout za neaktivnost presezen
 @return true, ce je razlika v casu presegla dolocen timeout
 */
bool Client::checkTimeout() {
    auto now = std::chrono::steady_clock::now();
    auto difference = now - this->connection.lastRecvPacketTime;

    return (difference > std::chrono::seconds(INACTIVITY_TIMEOUT));
}

void Client::updateLastRecvPacketTime() {
    this->connection.lastRecvPacketTime = std::chrono::steady_clock::now();
}

void Client::updateLastRecvPacketID(uint32_t p_id) {
    this->connection.lastRecvPacketID = p_id;
}
uint32_t Client::getLastRecvPacketID() {
    return this->connection.lastRecvPacketID;
}
/**
 * Returns and increments the packet ID
 * @return ID of the packet that will be sent
 */
uint32_t Client::getLastSentPacketID() {
    return ++this->connection.lastSentPacketID;
}
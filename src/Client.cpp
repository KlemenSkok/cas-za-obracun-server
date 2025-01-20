
// Client.cpp

#include "../include/Client.hpp"
#include "../include/Utilities/Constants.hpp"


// static members and function definitions

uint16_t Client::get_id() {
    return id;
}

IPaddress Client::get_ip() {
    return this->connection.ip;
}

/**
 * Client, ki bo dolocen cas ne bo poslal sporocila (vsaj keepalive) bo kickan.
 * Ta funkcija doloci, ali je bil timeout za neaktivnost presezen
 @return true, ce je razlika v casu presegla dolocen timeout
 */
bool Client::checkTimeout() {
    auto now = std::chrono::steady_clock::now();
    auto difference = now - this->connection.lastPacketTime;

    return (difference > std::chrono::seconds(INACTIVITY_TIMEOUT));
}
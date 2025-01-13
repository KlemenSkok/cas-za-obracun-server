
// Client.cpp

#include "../include/Client.hpp"


// static members and function definitions
std::set<uint16_t> free_ids;

uint16_t Client::get_id() {
    return id;
}

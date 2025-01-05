
// Server.cpp

#include "../include/Server.hpp"
#include "../include/Utility.hpp"
#include "../include/Logging/Logger.hpp"

#include <iostream>
#include <stdexcept>



/**
 * @param i port to listen on
 * @param o port to send from
 */
void Server::Setup(uint16_t port_in, uint16_t port_out) {
    try {
        SocketListener::Start(port_in);
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start listening thread: ") + e.what());
    }

    try {
        SocketSpeaker::Start(port_out);
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start speaking thread: ") + e.what());
    }
}

void Server::Run() {
    bool quit = false;

    while(!quit) {
        
        processNewPackets();




        std::this_thread::sleep_for(std::chrono::milliseconds(1));         
    }
}

void Server::Cleanup() {
    Logger::info("Cleaning up...");

    SocketListener::Stop();
    SocketSpeaker::Stop();

}

void Server::processNewPackets() {

    // omejimo stevilo paketov, ki jih obdelamo naenkrat,
    // da ne blokiramo glavne niti
    const uint8_t max_packets = 100;
    uint8_t num_packets = 0;

    // check for recieved messages
    PacketData recv_msg;
    while(getMessageFromQueue(recv_msg) && num_packets < max_packets) {
        
        // handle the message
        // --------------------------------------

        std::cout << "[" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "] " << recv_msg.getRawData().get() << '\n';

        // --------------------------------------

        num_packets++;
    }

}
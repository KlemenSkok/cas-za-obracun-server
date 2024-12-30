
// Server.cpp

#include "../include/Server.hpp"

#include <iostream>
#include <stdexcept>
#include "../include/Utility.hpp"



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
        // todo
        //SocketSpeaker::Start(port_out);
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start speaking thread: ") + e.what());
    }
}

void Server::Run() {

    std::string str;
    bool quit = false;

    while(!quit) {
        //std::cin >> str;
        if(str == "exit")
            quit = true;

        std::this_thread::sleep_for(std::chrono::microseconds(1));

        if(!recievedQueue.empty()) {
            std::lock_guard<std::mutex> lock(recvq_mutex);
            do {
                std::unique_ptr<UDPmessage> msg = std::move(recievedQueue.front());
                std::cout << formatIP(msg->ip->host) << '\n';
                std::cout << msg->data.get() << '\n';
                recievedQueue.pop();

            } while(!recievedQueue.empty());
        }

    }
}

void Server::Cleanup() {

    SocketListener::Stop();
    //SocketSpeaker::Stop();    
}

// SocketListener.cpp

#include <iostream>
#include "../include/SocketComm.hpp"

#define MAX_PACKET_SIZE 512


std::queue<UDPmessage> recievedQueue;
std::mutex recvq_mutex;


// -------------------------------------------------//
//                                                  //
//                  SOCKET LISTENER                 //
//                                                  //
// -------------------------------------------------//
bool SocketListener::_running = false;
std::unique_ptr<std::thread> SocketListener::worker = nullptr;



// open socket and start thread
void SocketListener::Start(uint16_t portNum) {

    // open the passed port
    UDPsocket socket = SDLNet_UDP_Open(portNum);
    if (!socket) {
        throw std::runtime_error(SDLNet_GetError());
    }

    SocketListener::worker = std::make_unique<std::thread>(&SocketListener::Listen, socket);
}

// stop and close thread
void SocketListener::Stop() noexcept {
    std::cout << "Closing listener thread (function Stop())..."  << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count() << '\n';
    SocketListener::_running = false;

    if(SocketListener::worker && SocketListener::worker->joinable()) {
        std::cout << "Joining the thread... (function Stop())..."  << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count() << '\n';
        SocketListener::worker->join();
    }
    std::cout << "Socket listener closed!\n";
}

void SocketListener::Listen(UDPsocket socket) noexcept {

    // allocate a packet
    UDPpacket* packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (!packet) {
        std::cerr << "Failed to allocate packet: " << SDLNet_GetError() << std::endl;
        SocketListener::_running = false;
        return;
    }

    std::cout << "Listening on socket." << "\n";

    // queue za pakete, ki cakajo na push v recievedQueue
    std::queue<UDPmessage> msgBuffer;

    // start after successful initialization
    SocketListener::_running = true;
    while(SocketListener::_running) {

        // empty the buffer if mutex can be locked
        if(!msgBuffer.empty()) {
            if(recvq_mutex.try_lock()) {
                while(!msgBuffer.empty()) {
                    std::cout << "Pusham!\n";
                    recievedQueue.push(msgBuffer.front());
                    msgBuffer.pop();
                }
                recvq_mutex.unlock(); // !
            }
        }
        
        // check for new packets
        int numReceived = SDLNet_UDP_Recv(socket, packet);
        if (numReceived > 0) {
            // copy the packet contents
            UDPmessage msg;
            msg.len = packet->len; // msg length
            msg.data = new Uint8[msg.len]; // msg
            std::memcpy(msg.data, packet->data, msg.len);
            msg.channel = packet->channel; // channel
            if(msg.channel != -1) { // address
                msg.ip = nullptr;
            }
            else {
                msg.ip = new IPaddress(packet->address);
            }
            std::cout << formatIP(msg.ip->host) << '\n';
            std::cout << msg.data << '\n';
            
            if(recvq_mutex.try_lock()) {
                std::cout << "Pusham!\n";
                recievedQueue.push(msg);
                recvq_mutex.unlock(); // ! treba unlockat manually
            }
            else { // dodaj v buffer, da lock ne blocka
                msgBuffer.push(msg);
            }
        }
        else if (numReceived < 0) {
            std::cerr << "SDLNet_UDP_Recv error: " << SDLNet_GetError() << std::endl;
            continue;
        }



        // TODO: packet handling (send to a queue with mutex)
        // TODO: implement logging system (spdlog)

        // sleep to reduce CPU usage (1ms)
        std::this_thread::sleep_for(std::chrono::microseconds(10));

    }

    std::cout << "Exiting Listen loop... "  << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count() << '\n';

    // cleannup
    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(socket);

    std::cout << "Na konc funkcije Listen\n";

}


// -------------------------------------------------//
//                                                  //
//                  SOCKET SPEAKER                  //
//                                                  //
// -------------------------------------------------//

bool SocketSpeaker::_running = false;

std::unique_ptr<std::thread> SocketSpeaker::worker = nullptr;

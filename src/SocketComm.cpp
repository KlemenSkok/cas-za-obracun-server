
// SocketListener.cpp

#include <iostream>
#include "../include/SocketComm.hpp"

#define MAX_PACKET_SIZE 512


std::string formatIP(Uint32 ip) {
    std::string out;
    for(int i = 0; i < 4; i++) {
        out += std::to_string(((ip & (0xff << i*8)) >> i*8)) + ((i != 3) ? "." : "");
    }

    return out;
}


// -------------------------------------------------//
//                                                  //
//                  SOCKET LISTENER                 //
//                                                  //
// -------------------------------------------------//
bool SocketListener::_running = false;
std::unique_ptr<std::thread> SocketListener::workThread = nullptr;



// open socket and start thread
void SocketListener::Start(uint16_t portNum) {

    // open the passed port
    UDPsocket socket = SDLNet_UDP_Open(portNum);
    if (!socket) {
        throw std::runtime_error(SDLNet_GetError());
    }

    SocketListener::workThread = std::make_unique<std::thread>(&SocketListener::Listen, socket);
}

// stop and close thread
void SocketListener::Stop() noexcept {
    std::cout << "Closing listener thread (function Stop())..."  << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count() << '\n';
    SocketListener::_running = false;

    if(SocketListener::workThread && SocketListener::workThread->joinable()) {
        std::cout << "Joining the thread... (function Stop())..."  << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count() << '\n';
        SocketListener::workThread->join();
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

    // start after successful initialization
    SocketListener::_running = true;
    while(SocketListener::_running) {

        int numReceived = SDLNet_UDP_Recv(socket, packet);
        if (numReceived > 0) {
            // dump packet data
            std::cout << "Received packet of size " << packet->len << " bytes.\n";
            std::cout << "Packet content: " << packet->data << "\n";
            std::cout << "Other packet data: \n" << 
                "\tSource address: " << formatIP(packet->address.host) << ":" << packet->address.port << "\n" <<
                "\tChannel: " << packet->channel << "\n" <<
                "\tStatus: " << packet->status << "\n" <<
                "\tMaxLen: " << packet->maxlen << "\n";
            std::cout << "\n";
            std::cout << "EN GAP ME PACKETI\n\n";
        } else if (numReceived < 0) {
            std::cerr << "SDLNet_UDP_Recv error: " << SDLNet_GetError() << std::endl;
            SocketListener::_running = false;  // Stop the loop on error
        }

        // TODO: packet handling (send to a queue with mutex)
        // TODO: implement logging system

        // sleep to reduce CPU usage (1ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

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

std::unique_ptr<std::thread> SocketSpeaker::workThread = nullptr;

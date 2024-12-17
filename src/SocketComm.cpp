
// SocketListener.cpp

#include <iostream>
#include "../include/SocketComm.hpp"

#define MAX_PACKET_SIZE 512



// -------------------------------------------------//
//                                                  //
//                  SOCKET LISTENER                 //
//                                                  //
// -------------------------------------------------//
bool SocketListener::_running = false;
std::unique_ptr<std::thread> SocketListener::workThread = nullptr;



// open socket and start thread
int SocketListener::Start(uint16_t portNum) {
    SocketListener::workThread = std::make_unique<std::thread>(&SocketListener::Listen, portNum);
    return 0;
}

// stop and close thread
void SocketListener::Stop() {
    SocketListener::_running = false;

    if(SocketListener::workThread && SocketListener::workThread->joinable()) {
        SocketListener::workThread->join();
    }
    std::cout << "Socket listener closed!\n";
}

void SocketListener::Listen(uint16_t portNum) {
    // open passed socket
    UDPsocket udpSocket = SDLNet_UDP_Open(portNum);
    if (!udpSocket) {
        std::cerr << "SDLNet_UDP_Open: " << SDLNet_GetError() << std::endl;
        return;
    }
    // allocate a packet
    UDPpacket* packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (!packet) {
        std::cerr << "Failed to allocate packet: " << SDLNet_GetError() << std::endl;
        return;
    }

    std::cout << "Listening on socket " << portNum << "\n";

    // start after successful initialization
    SocketListener::_running = true;
    while(SocketListener::_running) {

        std::cout << "Izpis v loopu\n";

        // TODO: packet handling (send to a queue with mutex)
        // TODO: implement logging system

        // sleep to reduce CPU usage (1ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    }

    SDLNet_FreePacket(packet);

    std::cout << "Na konc funkcije Listen\n";

}


// -------------------------------------------------//
//                                                  //
//                  SOCKET SPEAKER                  //
//                                                  //
// -------------------------------------------------//

bool SocketSpeaker::_running = false;

std::unique_ptr<std::thread> SocketSpeaker::workThread = nullptr;

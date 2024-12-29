
// SocketListener.cpp

#include "../include/SocketComm.hpp"

#define MAX_PACKET_SIZE 512


std::queue<std::unique_ptr<UDPmessage>> recievedQueue;
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

    // start after successful initialization
    SocketListener::_running = true;
    while(SocketListener::_running) {
        
        // check for new packets
        int numReceived = SDLNet_UDP_Recv(socket, packet);
        if (numReceived > 0) {
            // copy the packet contents
            auto msg = std::make_unique<UDPmessage>(packet);

            // add the packet to queue
            {
                std::lock_guard<std::mutex> lock(recvq_mutex); // ta mutex blocka, ampak ni treba bufferja
                recievedQueue.push(std::move(msg));
                //std::cout << "Prejel sem paket!\n";
            }
        }
        else if (numReceived < 0) {
            std::cerr << "SDLNet_UDP_Recv error: " << SDLNet_GetError() << std::endl;
            continue; // skip this loop
        }

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

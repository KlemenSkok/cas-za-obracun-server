
// SocketListener.cpp

#include "../include/Communication/SocketListener.hpp"
#include "../include/Utility.hpp"
#include "../include/Logging/Logger.hpp"

#include <cstring>
#include <utility>
#include <chrono>

#define MAX_PACKET_SIZE 512


// -------------------------------------------------//
//                                                  //
//                  RECIEVED QUEUE                  //
//                                                  //
// -------------------------------------------------//

std::queue<std::unique_ptr<UDPmessage>> recievedQueue;
std::mutex recvq_mutex;


bool getMessageFromQueue(PacketData& data) {
    data.clear();
    if(!recievedQueue.empty()) {
        std::lock_guard<std::mutex> lock(recvq_mutex);

        std::unique_ptr<UDPmessage> msg = std::move(recievedQueue.front());
        recievedQueue.pop();
        data = PacketData(msg->data.get(), msg->len);

        return true;
    }
    return false;
}


// -------------------------------------------------//
//                                                  //
//                  SOCKET LISTENER                 //
//                                                  //
// -------------------------------------------------//

std::atomic<bool> SocketListener::_running = false;
std::atomic<bool> SocketListener::_shutdown = false;
std::unique_ptr<std::thread> SocketListener::worker = nullptr;


// open socket and start thread
void SocketListener::Start(uint16_t port) {

    // open the passed port (0 for dynamic port assignment)
    UDPsocket socket = SDLNet_UDP_Open(port);
    if (!socket) {
        throw std::runtime_error(SDLNet_GetError());
    }

    SocketListener::worker = std::make_unique<std::thread>(&SocketListener::Listen, socket);
}

// stop and close thread
void SocketListener::Stop() noexcept {
    SocketListener::_running = false;
    SocketListener::_shutdown = true;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    if(SocketListener::worker && SocketListener::worker->joinable()) {
        Logger::info("Waiting for socket listener to close...");
        SocketListener::worker->join();
    }
    Logger::info("Socket listener closed.");
}

// listen for new packets
void SocketListener::Listen(UDPsocket socket) noexcept {

    // allocate a packet
    UDPpacket* packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (!packet) {
        Logger::error((std::string("Failed to allocate packet: ") + SDLNet_GetError()).c_str());
        SocketListener::_running = false;
        return;
    }

    Logger::info("Socket listener running.");

    // start after successful initialization
    SocketListener::_running = true;
    while(SocketListener::_running && !SocketListener::_shutdown) {
        
        // check for new packets
        int numReceived = SDLNet_UDP_Recv(socket, packet);
        if (numReceived > 0) {
            // copy the packet contents
            auto msg = std::make_unique<UDPmessage>(packet);

            // add the packet to queue
            {
                std::lock_guard<std::mutex> lock(recvq_mutex); // ta mutex blocka, ampak ni treba bufferja
                recievedQueue.push(std::move(msg));
                //std::cout << " [SocketListener] Prejel sem paket: " << recievedQueue.back()->data.get() << '\n';
                std::cout << "Prejeto: [" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "]\n";
            }
        }
        else if (numReceived < 0) {
            Logger::error((std::string("SDLNet_UDP_Recv error: ") + SDLNet_GetError()).c_str());
            continue; // skip this loop
        }

        if(!SocketListener::_running) {
            break;
        }

        // sleep to reduce CPU usage (1ms)
        std::this_thread::sleep_for(std::chrono::microseconds(LOOP_DELAY));
    }

    // cleannup
    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(socket);
}
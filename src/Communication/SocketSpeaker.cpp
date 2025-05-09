
// SocketSpeaker.cpp

#include "Communication/SocketSpeaker.hpp"
#include "Utilities/Utility.hpp"
#include "Utilities/Constants.hpp"
#include "Logging/Logger.hpp"

#include <cstring>
#include <utility>
#include <chrono>


// -------------------------------------------------//
//                                                  //
//                     SEND QUEUE                   //
//                                                  //
// -------------------------------------------------//

std::queue<std::unique_ptr<UDPmessage>> sendQueue;
std::mutex sendq_mutex;


void addMessageToQueue(PacketData& data, IPaddress ip) {
    std::unique_ptr<UDPmessage> msg = std::make_unique<UDPmessage>();
    //msg->ip = std::make_unique<IPaddress>(server_addr);
    msg->channel = -1;
    msg->len = data.size();
    msg->data = data.getRawData();
    msg->ip = std::make_unique<IPaddress>(ip);

    {
        std::lock_guard<std::mutex> lock(sendq_mutex);
        sendQueue.push(std::move(msg));
    }
}

void addMessageToQueue(std::unique_ptr<UDPmessage> msg) {
    {
        std::lock_guard<std::mutex> lock(sendq_mutex);
        sendQueue.push(std::move(msg));
    }
}

// also empties the vector
void addMessagesToQueue(std::vector<std::unique_ptr<UDPmessage>>& data) {
    {
        std::lock_guard<std::mutex> lock(sendq_mutex);
        for(auto& msg : data) {
            sendQueue.push(std::move(msg));
        }
    }
    data.resize(0);
    data.shrink_to_fit();
}


// -------------------------------------------------//
//                                                  //
//                  SOCKET SPEAKER                  //
//                                                  //
// -------------------------------------------------//

std::atomic<bool> SocketSpeaker::_running = false;
std::atomic<bool> SocketSpeaker::_shutdown = false;
UDPsocket SocketSpeaker::socket = nullptr;
std::unique_ptr<std::thread> SocketSpeaker::worker = nullptr;


// open socket and start thread
void SocketSpeaker::Start(uint16_t port) {

    // open the passed port (0 for dynamic port assignment)
    UDPsocket socket = SDLNet_UDP_Open(port);
    if (!socket) {
        throw std::runtime_error(SDLNet_GetError());
    }
    SocketSpeaker::socket = socket;

    SocketSpeaker::worker = std::make_unique<std::thread>(&SocketSpeaker::Speak, socket);
    SocketSpeaker::worker->detach(); // detach, da ne caka na join pri zapiranju
}

// stop and close thread
void SocketSpeaker::Stop() noexcept {
    SocketSpeaker::_running = false;
    SocketSpeaker::_shutdown = true;

    while(SocketSpeaker::_shutdown);

    Logger::info("Socket speaker closed.");
}

// send pending packets
void SocketSpeaker::Speak(UDPsocket socket) noexcept {

    // allocate a packet
    UDPpacket* packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (!packet) {
        Logger::error((std::string("Failed to allocate packet: ") + SDLNet_GetError()).c_str());
        SocketSpeaker::_running = false;
        return;
    }

    Logger::info("Socket speaker running.");

    std::unique_ptr<UDPmessage> msg = nullptr;
    std::queue<std::unique_ptr<UDPmessage>> msgBuffer;

    // start after successful initialization
    SocketSpeaker::_running = true;
    while(SocketSpeaker::_running && !SocketSpeaker::_shutdown) {
        
        {
            std::lock_guard<std::mutex> lock(sendq_mutex);
            if(!sendQueue.empty()) {
                msgBuffer.push(std::move(sendQueue.front()));
                sendQueue.pop();
            }
        }

        // check for a new packet
        while (!msgBuffer.empty()) {

            msg = std::move(msgBuffer.front());
            msgBuffer.pop();

            // copy the message to the packet
            packet->channel = msg.get()->channel; // server uporablja channele, zato ga je treba poslat zraven
            packet->len = msg.get()->len;
            if(packet->channel == -1) {
                packet->address = *msg.get()->ip.get();
            }

            //std::cout << "Posiljam na channel [" << packet->channel << "]\n";

            // alternativa je std::move na shared pointer
            packet->data = msg.get()->data.release();
            
            // send the packet
            if(SDLNet_UDP_Send(socket, packet->channel, packet) == 0) {
                Logger::error((std::string("SDLNet_UDP_Send error: ") + SDLNet_GetError()).c_str());
            }

            //std::cout << "Vsebina paketa: " << packet->data << "\n\n";
            delete[] packet->data; // ! nujno
            packet->data = nullptr;
            //Logger::info("Poslal brez napak!");

        }

        if(!SocketSpeaker::_running) {
            break;
        }
        
        // sleep to reduce CPU usage (1ms)
        std::this_thread::sleep_for(std::chrono::microseconds(LOOP_DELAY));
    }


    // cleannup
    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(socket);

    SocketSpeaker::_shutdown = false;
}

// return socket, so the clients can be bound to it
UDPsocket SocketSpeaker::getSocket() noexcept {
    return SocketSpeaker::socket;
}
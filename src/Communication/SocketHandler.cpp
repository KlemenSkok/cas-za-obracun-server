
// SocketHandler.cpp

#include "Communication/SocketHandler.hpp"
#include "Utilities/Utility.hpp"
#include "Utilities/Constants.hpp"
#include "Logging/Logger.hpp"

#include <cstring>
#include <utility>
#include <chrono>

#define LOOP_DELAY 1 // delay in ms


// ------------------------------------------------- //
//                                                   //
//                  RECIEVED QUEUE                   //
//                                                   //
// ------------------------------------------------- //

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

bool getMessageFromQueue(std::unique_ptr<UDPmessage>& data) {
    if(!recievedQueue.empty()) {
        std::lock_guard<std::mutex> lock(recvq_mutex);

        data = std::move(recievedQueue.front());
        recievedQueue.pop();

        return true;
    }
    return false;
}



// ------------------------------------------------- //
//                                                   //
//                     SEND QUEUE                    //
//                                                   //
// ------------------------------------------------- //

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

void addMessageToQueue(PacketData& data, int channel) {
    std::unique_ptr<UDPmessage> msg = std::make_unique<UDPmessage>();
    //msg->ip = std::make_unique<IPaddress>(server_addr);
    msg->channel = channel;
    msg->len = data.size();
    msg->data = data.getRawData();

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

void addMessagesToQueue(std::vector<std::unique_ptr<UDPmessage>> &data) {
    {
        std::lock_guard<std::mutex> lock(sendq_mutex);
        for(auto& msg : data) {
            sendQueue.push(std::move(msg));
        }
    }
    data.resize(0);
    data.shrink_to_fit();
}


// ------------------------------------------------- //
//                                                   //
//                  SOCKET HANDLER                   //
//                                                   //
// ------------------------------------------------- //

std::atomic<bool> SocketHandler::_running = false;
std::atomic<bool> SocketHandler::_shutdown = false;
UDPsocket SocketHandler::socket = nullptr;
std::unique_ptr<std::thread> SocketHandler::worker = nullptr;


// open socket and start thread
void SocketHandler::Start(uint16_t port) {

    // open the passed port (0 for dynamic port assignment)
    UDPsocket socket = SDLNet_UDP_Open(port);
    if (!socket) {
        throw std::runtime_error(SDLNet_GetError());
    }
    SocketHandler::socket = socket;
    
    IPaddress *localAddr = SDLNet_UDP_GetPeerAddress(socket, -1);
    Logger::info(("Socket opened on port [" + std::to_string(SDLNet_Read16(&localAddr->port)) + "]").c_str());

    SocketHandler::worker = std::make_unique<std::thread>(&SocketHandler::Work, socket);
    SocketHandler::worker->detach(); // detach, da ne caka na join pri zapiranju
}

// stop and close thread
void SocketHandler::Stop() noexcept {
    SocketHandler::_running = false;
    SocketHandler::_shutdown = true;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

UDPsocket SocketHandler::getSocket() noexcept {
    return SocketHandler::socket;
}

void SocketHandler::Work(UDPsocket socket) noexcept {

    // allocate a packet for recieving
    UDPpacket* recieve_packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (!recieve_packet) {
        Logger::error((std::string("Failed to allocate packet: ") + SDLNet_GetError()).c_str());
        SocketHandler::_running = false;
        return;
    }
    // allocate a packet for sending
    UDPpacket* send_packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (!send_packet) {
        Logger::error((std::string("Failed to allocate packet: ") + SDLNet_GetError()).c_str());
        SocketHandler::_running = false;
        return;
    }

    Logger::info("Socket handler running.");

    // start after successful initialization
    SocketHandler::_running = true;
    Uint32 lastUpdate = SDL_GetTicks();
    while(SocketHandler::_running && !SocketHandler::_shutdown) {
        Uint32 now = SDL_GetTicks();
        if(now - lastUpdate < LOOP_DELAY) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            continue;
        }
        lastUpdate = now;

        // ------------------------------------------------- //
        //                  RECIEVING PACKETS                //
        // ------------------------------------------------- //
        // formerly known as SocketListener
        // check for new packets TO RECIEVE
        int numReceived = SDLNet_UDP_Recv(socket, recieve_packet);
        if (numReceived > 0) {
            // copy the packet contents
            auto msg = std::make_unique<UDPmessage>(recieve_packet);

            //std::cout << "Prejel sem paket: " << msg->data.get() << '\n';

            // add the packet to queue
            {
                std::lock_guard<std::mutex> lock(recvq_mutex); // ta mutex blocka, ampak ni treba bufferja
                recievedQueue.push(std::move(msg));
                //std::cout << " [SocketListener] Prejel sem paket: " << recievedQueue.back()->data.get() << '\n';
                //std::cout << "Prejeto: [" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "]\n";
            }
        }
        else if (numReceived < 0) {
            Logger::error((std::string("SDLNet_UDP_Recv error: ") + SDLNet_GetError()).c_str());
            continue; // skip this loop
        }


        // ------------------------------------------------- //
        //                  SENDING PACKETS                  //
        // ------------------------------------------------- //
        // formerly known as SocketSpeaker
        // check for new packets TO SEND
        std::unique_ptr<UDPmessage> msg = nullptr;

        {
            std::lock_guard<std::mutex> lock(sendq_mutex);
            if(!sendQueue.empty()) {
                msg = std::move(sendQueue.front());
                sendQueue.pop();
            }
        }

        // check for a new packet
        if (msg) {
            // copy the message to the packet
            send_packet->channel = msg.get()->channel; // server uporablja channele, zato ga je treba poslat zraven
            send_packet->len = msg.get()->len;
            if(send_packet->channel == -1) {
                send_packet->address = *msg.get()->ip.get();
            }

            // alternativa je std::move na shared pointer
            send_packet->data = msg.get()->data.release();
            
            // send the packet
            if(SDLNet_UDP_Send(socket, send_packet->channel, send_packet) == 0) {
                Logger::error((std::string("SDLNet_UDP_Send error: ") + SDLNet_GetError()).c_str());
            }

            //std::cout << "Vsebina paketa: " << packet->data << "\n\n";
            delete[] send_packet->data; // ! nujno
            send_packet->data = nullptr;
            //std::cout << "Poslano: [" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "]\n";
        }

        if(!SocketHandler::_running) {
            break;
        }
        
    }

    SDLNet_FreePacket(recieve_packet);
    SDLNet_FreePacket(send_packet);
    SDLNet_UDP_Close(socket);
    socket = nullptr;
    
    Logger::info("Socket handler stopped.");

}
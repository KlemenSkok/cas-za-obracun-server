
// SocketHandler.hpp

#pragma once

#include <cstdint>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <SDL2/SDL_net.h>
#include <atomic>

// za UDPmessage
#include "Containers.hpp"


// ------------------------------------------------- //
//                                                   //
//                  RECIEVED QUEUE                   //
//                                                   //
// ------------------------------------------------- //

// Queue with recieved messages and its mutex
extern std::queue<std::unique_ptr<UDPmessage>> recievedQueue;
extern std::mutex recvq_mutex;

// function that retrieves a message from the recieved queue
bool getMessageFromQueue(PacketData& data);
bool getMessageFromQueue(std::unique_ptr<UDPmessage>& data);


// ------------------------------------------------- //
//                                                   //
//                     SEND QUEUE                    //
//                                                   //
// ------------------------------------------------- //

// Queue with messages that need to be sent and its mutex
extern std::queue<std::unique_ptr<UDPmessage>> sendQueue;
extern std::mutex sendq_mutex;

// function that adds a message to the send queue
void addMessageToQueue(PacketData& data, IPaddress ip);
void addMessageToQueue(PacketData& data, int channel);
void addMessageToQueue(std::unique_ptr<UDPmessage> msg);
// function that adds a vector of messages to the send queue
void addMessagesToQueue(std::vector<std::unique_ptr<UDPmessage>> &data);


// ------------------------------------------------- //
//                                                   //
//                  SOCKET HANDLER                   //
//                                                   //
// ------------------------------------------------- //

class SocketHandler {
    // pointer to the speaker thread
    static std::unique_ptr<std::thread> worker;
    static UDPsocket socket;

public:
    static std::atomic<bool> _shutdown; // atomic za thread safety
    static std::atomic<bool> _running;

    static void Start(uint16_t port);
    static void Work(UDPsocket) noexcept;
    static void Stop() noexcept;
    static UDPsocket getSocket() noexcept; // da se lahko binda na server ip
};
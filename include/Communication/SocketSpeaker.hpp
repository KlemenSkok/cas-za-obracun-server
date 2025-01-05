
// SocketSpeaker.hpp

#pragma once

#include <cstdint>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <SDL2/SDL_net.h>
#include <atomic>

// za UDPmessage
#include "../Containers.hpp"

#define LOOP_DELAY 1000 // 1000us = 1ms


// -------------------------------------------------//
//                                                  //
//                     SEND QUEUE                   //
//                                                  //
// -------------------------------------------------//

// Queue with messages that need to be sent and its mutex
extern std::queue<std::unique_ptr<UDPmessage>> sendQueue;
extern std::mutex sendq_mutex;

// function that add a message to the send queue
void addMessageToQueue(PacketData& data, int channel);


// -------------------------------------------------//
//                                                  //
//                  SOCKET SPEAKER                  //
//                                                  //
// -------------------------------------------------//

// Sending UDP packets
// runs on a separate thread and sends pending UDP packets
class SocketSpeaker {

    // pointer to the speaker thread
    static std::unique_ptr<std::thread> worker;
    static UDPsocket socket;

public:
    static std::atomic<bool> _shutdown;
    static std::atomic<bool> _running;
    
    static void Start(uint16_t port);
    static void Speak(UDPsocket) noexcept;
    static void Stop() noexcept;
    static UDPsocket getSocket() noexcept; // da lahko cliente binda na channele 
};
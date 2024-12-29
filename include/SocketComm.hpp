
// SocketComm.hpp

#pragma once

#include <cstdint>
#include <memory>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <cstring>
#include <iostream>
#include <utility>

#include <SDL2/SDL_net.h>

// za UDPmessage
#include "Structs.hpp"
#include "Utility.hpp"


extern std::queue<std::unique_ptr<UDPmessage>> recievedQueue;
extern std::mutex recvq_mutex;

//extern std::queue<std::unique_ptr<UDPmessage>> sendQueue;
//extern std::mutex sendq_mutex;


// objects for socket communication (in and out) 

// -------------------------------------------------//
//                                                  //
//                  SOCKET LISTENER                 //
//                                                  //
// -------------------------------------------------//
// runs on a separate thread and listens for incoming UDP packets
class SocketListener {

    // pointer to the listener thread
    static std::unique_ptr<std::thread> worker;

public:
    static bool _running;
    static void Start(uint16_t);
    static void Listen(UDPsocket) noexcept;
    static void Stop() noexcept;
};


// -------------------------------------------------//
//                                                  //
//                  SOCKET SPEAKER                  //
//                                                  //
// -------------------------------------------------//
// runs on a separate thread and sends pending UDP packets
class SocketSpeaker {

    // pointer to the speaker thread
    static std::unique_ptr<std::thread> worker;

public:
    static bool _running;
    static int Start(uint16_t);
    static void Speak(uint16_t) noexcept;
    static void Stop() noexcept;
};
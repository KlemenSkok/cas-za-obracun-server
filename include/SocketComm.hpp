
// SocketComm.hpp

#pragma once

#include <cstdint>
#include <memory>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>


//std::queue<UDPpacket> recievedQueue;
//std::mutex recvq_mutex;
//
//std::queue<UDPpacket> sendQueue;
//std::mutex sendq_mutex;


// objects for socket communication (in and out) 

// -------------------------------------------------//
//                                                  //
//                  SOCKET LISTENER                 //
//                                                  //
// -------------------------------------------------//
// runs on a separate thread and listens for incoming UDP packets
class SocketListener {

    // pointer to the listener thread
    static std::unique_ptr<std::thread> workThread;

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
    static std::unique_ptr<std::thread> workThread;

public:
    static bool _running;
    static int Start(uint16_t);
    static void Speak(uint16_t) noexcept;
    static void Stop() noexcept;
};
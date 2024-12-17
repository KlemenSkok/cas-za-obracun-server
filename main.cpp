
// main.cpp

//#include "include/Server.hpp"
//#include "include/Base.hpp"
#include "include/Utility.hpp"
#include "include/SocketComm.hpp"
#include <iostream>


int main(int argc, char *argv[]) {

    if (!SDLUtils::initialize()) {
        return 1;
    }
    std::cout << "Starting Socket Listener...\n";
    SocketListener::Start(42069);

    // Keep main thread alive
    while (SocketListener::_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Main loop exited. Cleaning up...\n";
    SocketListener::Stop();
    SDLUtils::cleanup();


    return 0;
}
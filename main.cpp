
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

    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Main loop exited. Cleaning up... " << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count() << '\n';
                   
    SocketListener::Stop();
    SDLUtils::cleanup();

std::cout << "EXITING MAIN LOOP! " << std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch()).count() << '\n';

    return 0;
}
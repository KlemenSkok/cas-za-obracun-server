
// main.cpp

#include "include/Server.hpp"
#include "include/Logging/Logger.hpp"
#include "include/Utilities/Utility.hpp"

#define COMM_PORT 55000 // port for all traffic


int main(int argc, char *argv[]) {

    if (!SDLUtils::initialize()) {
        return 1;
    }

    try {
        Server::Setup(COMM_PORT);
    }
    catch(std::runtime_error &e) {
        Logger::error(e.what());
        SDLUtils::cleanup();
        Server::Cleanup();

        return 1;
    }

    Server::Run();
    
    Server::Cleanup();
    SDLUtils::cleanup();


    return 0;
}

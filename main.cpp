
// main.cpp

#include "include/Server.hpp"
#include <iostream>


int main(int argc, char *argv[]) {

    if (!SDLUtils::initialize()) {
        return 1;
    }

    Server::Setup(42069, 42068);
    Server::Run();
    Server::Cleanup();


    
    return 0;
}
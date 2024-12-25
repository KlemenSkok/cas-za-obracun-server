
// main.cpp

#include "include/Server.hpp"
#include <iostream>


int main(int argc, char *argv[]) {

    if (!SDLUtils::initialize()) {
        return 1;
    }

    try {
        Server::Setup(42069, 42068);
    }
    catch(std::runtime_error &e) {
        std::cout << e.what() << '\n';
        SDLUtils::cleanup();

        return 1;
    }


    Server::Run();
    Server::Cleanup();


    
    return 0;
}
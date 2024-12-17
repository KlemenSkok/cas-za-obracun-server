
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
    
    SocketListener::Start(42069);


    return 0;
}
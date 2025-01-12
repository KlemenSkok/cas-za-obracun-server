
// Server.cpp

#include "../include/Server.hpp"
#include "../include/Utility.hpp"
#include "../include/Logging/Logger.hpp"

#include <iostream>
#include <stdexcept>

// header flags
#define NUM_FLAGS 8

#define FLAG_ACK 7
#define FLAG_FIN 6
#define FLAG_SYN 5
#define FLAG_KEEPALIVE 4
#define FLAG_DATA 3
#define FLAG_PULL 2
#define FLAG_PUSH 1


// static members
std::map<uint8_t, std::unique_ptr<GameSession>> Server::_sessions;
std::set<uint16_t> Server::_free_client_ids;
std::set<uint8_t> Server::_free_session_ids;


/**
 * @param i port to listen on
 * @param o port to send from
 */
void Server::Setup(uint16_t port_in, uint16_t port_out) {
    try {
        SocketListener::Start(port_in);
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start listening thread: ") + e.what());
    }

    try {
        SocketSpeaker::Start(port_out);
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start speaking thread: ") + e.what());
    }
}

void Server::Run() {
    bool quit = false;

    while(!quit) {
        
        processNewPackets();

        // ostale funkcionalnosti


        // --------------------------------------

        std::this_thread::sleep_for(std::chrono::milliseconds(1));         
    }
}

void Server::Cleanup() {
    Logger::info("Cleaning up...");

    SocketListener::Stop();
    SocketSpeaker::Stop();

}

void Server::processNewPackets() {

    // omejimo stevilo paketov, ki jih obdelamo naenkrat,
    // da ne blokiramo glavne niti
    const uint8_t max_packets = 100;
    uint8_t num_packets = 0;

    // check for recieved messages
    std::unique_ptr<UDPmessage> recv_msg;
    while(getMessageFromQueue(recv_msg) && num_packets < max_packets) {
        
        // handle the message
        // --------------------------------------

        std::cout << "[" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "] -> " << recv_msg->data.get() << '\n';

        // check, if we got a new client
        if(recv_msg->channel == -1) {
            PacketData data(recv_msg->data.get(), recv_msg->len);
            
            if(data.flags() == (1 << FLAG_FIN)+1) { // 00100000
                std::cout << "we got a connection request!\n";
                
                int client_id = Server::addClient();
                
                auto remove_client = [&]() {
                    Server::removeClient(client_id);
                    // send denial to the client
                    recv_msg->data.reset();
                    recv_msg->data = PacketData(true).getRawData();
                    recv_msg->len = 1;
                    addMessageToQueue(std::move(recv_msg));
                };
                
                if(client_id == -1) {
                    remove_client();
                    Logger::warn("Failed to add a new client.");
                    continue;
                }
                if(SDLNet_UDP_Bind(SocketSpeaker::getSocket(), client_id, recv_msg->ip.get()) == -1) {
                    remove_client();
                    Logger::warn("Failed to add a new client.");
                    continue;
                }

                // add a client - done
                // find a free channel and bind the ip to it - done
                // send back either a confirmation or a denial - done
                Logger::info(("A new client was added. client_id=" + std::to_string(client_id)).c_str());
                continue;
            }

            for(int i = NUM_FLAGS-1; i >= 0; i--) {
                if(data.flags() & (1 << i)) {
                    switch(i) {
                        case FLAG_ACK:
                            // look for any awaiting confirmations
                            break;
                        case FLAG_FIN:
                            // terminate connection, send FIN back
                            break;
                        case FLAG_KEEPALIVE:
                            // just update timestamp of the last keepalive message
                            break;
                        case FLAG_DATA:
                            // forward message to game for further processing
                            break;
                        case FLAG_PULL:
                            // send back data about client's game
                            break;
                        default:
                            Logger::info(("A packet was ignored: " + std::string((char*)data.getRawData().get())).c_str());
                            // ignore the packet
                    }
                }
            }
            

        }


        // --------------------------------------

        recv_msg.reset();
        num_packets++;
    }

}


int Server::addClient() {
    static uint16_t id_counter = 0;

    // search for joinable active sessions
    int session_id = -1;
    for(auto &s : _sessions) {
        if(s.second->acceptsPlayers()) {
            session_id = s.first;
        }
    }
    std::cout << "Number of active sessions: " << _sessions.size() << '\n';

    if(session_id == -1) {
        // all sessions are unjoinable, start a new one
        session_id = Server::addSession();

        if(session_id == -1) {
            Logger::warn("Failed to start a new game session");
            return -1; // no place for new sessions
        }
        Logger::info(("Created a new game session. session_id=" + std::to_string(session_id)).c_str());
    }

    // generate a new client id
    int client_id;
    if(!_free_client_ids.empty()) {
        // reuse old ids
        client_id = *_free_client_ids.begin();
        _free_client_ids.erase(client_id);
    }
    else {
        if(id_counter >= MAX_SESSIONS * MAX_PLAYERS) { // (max players per session)
            return -1; // no place for new clients
        }
        else {
            client_id = id_counter++;
        }
    }

    _sessions[session_id]->addClient(client_id);

    return client_id;
}

void Server::removeClient(uint16_t id) {

}


int Server::addSession() {
    static uint8_t id_counter = 0;    
    
    if(_sessions.size() >= MAX_SESSIONS) {
        return -1;
    }

    // generate a new id
    int new_id;
    if(!_free_session_ids.empty()) {
        new_id = *_free_session_ids.begin();
        _free_session_ids.erase(new_id);
    }
    else {
        if(id_counter >= MAX_SESSIONS) { // (max players per session)
            return -1; // no place for new clients
        }
        else {
            new_id = id_counter++;
        }
    }

    // start a new session if everything went okay
    _sessions[new_id] = std::make_unique<GameSession> (new_id);

    return new_id;
}
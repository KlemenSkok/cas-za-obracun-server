
// Server.cpp

#include "../include/Server.hpp"
#include "../include/Utilities/Utility.hpp"
#include "../include/Utilities/Constants.hpp"
#include "../include/Logging/Logger.hpp"

#include <iostream>
#include <stdexcept>



// static members
std::unordered_map<uint8_t, std::unique_ptr<GameSession>> Server::_sessions;
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

void Server::Cleanup() {
    Logger::info("Cleaning up...");

    SocketListener::Stop();
    SocketSpeaker::Stop();

    SDLNet_UDP_Close(SocketSpeaker::getSocket());
    SDLNet_UDP_Close(SocketListener::getSocket());
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
        PacketData data(recv_msg->data.get(), recv_msg->len);

        // check if we got a new client
        if(recv_msg->channel == -1) {
            
            if(data.flags() == (1 << FLAG_SYN)) { // 00100000, za debug das FIN)+1, da vpises 'A'
                int tmp = Server::queryAddress(*recv_msg->ip.get());
                if(tmp != -1) {
                    // duplicated connection request detected, skip
                    //std::cout << "Ta client je ze povezan. njegov id je " << tmp << '\n';
                    continue;
                }
                
                int client_id = Server::addClient(*recv_msg->ip.get());

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
                if(SDLNet_UDP_Bind(SocketListener::getSocket(), client_id, recv_msg->ip.get()) == -1) {
                    remove_client();
                    Logger::warn("Failed to add a new client.");
                    continue;
                }

                // send back a confirmation with the client's new id and session id
                int session_id = Server::queryClient(client_id);

                PacketData d(true);
                d.flags() |= (1 << FLAG_ACK);
                d.flags() |= (1 << FLAG_SYN);
                
                d.append((uint8_t)session_id);
                d.append((uint16_t)client_id);
                
                recv_msg->data.reset();
                recv_msg->len = d.size();
                recv_msg->data = d.getRawData();
                
                addMessageToQueue(std::move(recv_msg));

                Logger::info(("New client connected. Client ID: " + std::to_string(client_id) + ", Session ID: " + std::to_string(session_id)).c_str());

                //if(std::shared_ptr<Client> c_ptr = _sessions[session_id]->getClient(client_id).lock()) {
                //    std::cout << "Last packet time: " << c_ptr->getLastPacketID() << '\n';
                //}

                continue;
            }
        }

        bool finish_packet = false;
        for(int i = NUM_FLAGS-1; i >= 0; i--) {
            if(data.flags() & (1 << i)) {
                switch(i) {
                    case FLAG_ACK:
                        // look for any awaiting confirmations
                        break;
                    case FLAG_FIN:
                        if(Server::queryClient(recv_msg->channel) == -1) {
                            // this is a duplicate message; the client had already been removed
                            break;
                        }

                        // terminate connection, send FIN back
                        uint8_t target_session;
                        uint16_t target_client;
                        try {
                            data.getByOffset(target_session, sizeof(uint8_t), OFFSET_SESSION_ID);
                            data.getByOffset(target_client, sizeof(uint16_t), OFFSET_CLIENT_ID);
                        }
                        catch (std::exception &e) {
                            Logger::warn("Failed to read packet contents.");
                        }
                        Server::removeClient(target_client, target_session);

                        // poslji nazaj FIN
                        {
                            // treba dat v svoj scope zaradi inicializacije PacketData d(true); (error: transfer of control bypasses initialization of...)
                            PacketData d(true);
                            d.flags() |= (1 << FLAG_ACK); // acknowledge the FIN
                            d.flags() |= (1 << FLAG_FIN); // send FIN back
                            recv_msg->data.reset();
                            recv_msg->len = d.size();
                            recv_msg->data = d.getRawData();
                            recv_msg->channel = -1;

                            addMessageToQueue(std::move(recv_msg));
                            finish_packet = true;

                        }

                        break;
                    case FLAG_KEEPALIVE:
                        // just update timestamp of the last keepalive message
                        break;
                    case FLAG_DATA:
                        // forward message to game for further processing
                        // todo: packet id tracking
                        uint8_t s_id;
                        data.getByOffset(s_id, sizeof(uint8_t), OFFSET_DATA);
                        break;
                    case FLAG_PULL:
                        // send back data about client's game
                        break;
                    //default:
                        //Logger::info(("A packet was ignored: " + std::string((char*)data.getRawData().get())).c_str());
                        // ignore the packet
                }
                if(finish_packet) {
                    break; // break from loop checking flags
                }
            }
        }


        // --------------------------------------

        recv_msg.reset();
        num_packets++;
    }

}

// clients
int Server::addClient(IPaddress ip) {
    static uint16_t id_counter = 1;

    // search for joinable active sessions
    int session_id = -1;
    for(auto &s : _sessions) {
        if(s.second->acceptsPlayers()) {
            session_id = s.first;
        }
    }

    if(session_id == -1) {
        // all sessions are unjoinable, start a new one
        session_id = Server::addSession();

        if(session_id == -1) {
            Logger::warn("Failed to start a new game session");
            return -1; // no place for new sessions
        }
        //Logger::info(("Created a new game session. ID: " + std::to_string(session_id)).c_str());
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

    _sessions[session_id]->addClient(client_id, ip);

    return client_id;
}

void Server::removeClient(uint16_t c_id) {
    int session_id = Server::queryClient(c_id);

    if(session_id == -1) {
        Logger::warn("Tried to remove a non-existent client.");
        return;
    }

    SDLNet_UDP_Unbind(SocketListener::getSocket(), c_id); 

    _sessions[session_id]->removeClient(c_id);
    if(_sessions[session_id]->size() == 0) {
        // terminate session if empty
        _sessions.erase(session_id);
    }
}
// also remove client, but the session is already known
void Server::removeClient(uint16_t c_id, uint8_t s_id) {
    if(_sessions.find(s_id) == _sessions.end()) {
        Logger::warn("Tried to access a non-existent session.");
        return;
    }

    SDLNet_UDP_Unbind(SocketListener::getSocket(), c_id);
    _free_client_ids.insert(c_id);
    
    _sessions[s_id]->removeClient(c_id);
    if(_sessions[s_id]->size() == 0) {
        // terminate session if empty
        Server::removeSession(s_id, SocketListener::getSocket());
    }
}

int Server::queryClient(uint16_t c_id) {
    for(auto &s : _sessions) {
        if(s.second->hasClient(c_id)) {
            return s.first;
        }
    }
    return -1;
}

int Server::queryAddress(IPaddress ip) {
    for(auto &s : _sessions) {
        int client_id = s.second->queryAddress(ip);
        if(client_id != -1)
            return client_id;
    }
    return -1;
}

IPaddress Server::getClientAddr(uint16_t c_id) {
    for(auto &s : _sessions) {
        int client_id = s.second->hasClient(c_id);
        if(client_id != -1)
            return s.second->getClientAddr(c_id);
    }
    return IPaddress();
}

// sessions
int Server::addSession() {
    static uint8_t id_counter = 1;    
    
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

void Server::removeSession(uint8_t s_id, UDPsocket socket) {
    //Logger::info(("Stopping session... id=" + std::to_string(s_id)).c_str());

    auto session = _sessions.find(s_id);
    if(session == _sessions.end()) {
        Logger::warn("Tried to stop a non-existent session.");
        return;
    }
    _free_session_ids.insert(session->first);
    // stop session (unbind all clients...)
    session->second->Stop(socket);
    _sessions.erase(session);
}
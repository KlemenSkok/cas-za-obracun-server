
// Constants.hpp

#pragma once


#define SERVER_TICK_RATE 60 // [Hz]
#define SESSION_UPDATE_DELAY 1000 / SERVER_TICK_RATE // [ms]


// communication constants
#define INACTIVITY_TIMEOUT 10 // 60s
#define INACTIVITY_CHECK_INTERVAL 1000 // 1s
#define MAX_PACKET_SIZE 512

// PACKET HEADER CONSTANTS

// header flags
#define NUM_FLAGS 8

#define FLAG_ACK 7
#define FLAG_FIN 6
#define FLAG_SYN 5
#define FLAG_KEEPALIVE 4
#define FLAG_DATA 3
#define FLAG_PULL 2
#define FLAG_PUSH 1

// data offsets in packets [B]
#define OFFSET_FLAGS 0
#define OFFSET_SESSION_ID 1
#define OFFSET_CLIENT_ID 2
#define OFFSET_PACKET_ID 4
#define OFFSET_DATA 8
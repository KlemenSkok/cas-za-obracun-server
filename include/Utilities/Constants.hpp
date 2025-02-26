
// Constants.hpp

#pragma once


#define SERVER_TICK_RATE 30 // [Hz]
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
#define OFFSET_PACKET_TYPE 8
#define OFFSET_DATA 9


// GAMEPLAY CONSTANTS

// -- PLAYERS -- //
// size
#define PLAYER_RADIUS 20 // px
// movement
#define PLAYER_MAX_SPEED 100.0f // [px/s]
#define PLAYER_MAX_SPEED_SLOWED 37.5f // [px/s]
#define PLAYER_MAX_SPEED_CARRYING 50.0f // [px/s]
#define PLAYER_ACCELERATION 1200.0f // [px/s^2]
#define PLAYER_FRICTION 1500.0f // [px/s^2]
// healing
#define PLAYER_HEAL_AMOUNT 12 // posture pts/tick
#define PLAYER_HEAL_DELAY 5000 // on posture broken, wait 3s before healing
#define PLAYER_HEAL_PERIOD 1000 // once healing, only heal every .5s

// -- PROJECTILES -- //
#define PROJECTILE_RADIUS 10 // px
#define PROJECTILE_THROW_COOLDOWN 1000 // [ms]
#define PROJECTILE_VELOCITY 500.0f
#define PROJECTILE_RANGE 1770*1770 // 2500 px in any direction
#define PROJECTILE_POSTURE_DAMAGE 22

// -- FLAGS -- //
#define GAME_FLAG_SIZE {75, 50} // [px] w, h
#define GAME_FLAG_PICKUP_RANGE 100 // [px] from center of the flag
#define GAME_FLAG_HOME_POS_X 400 // [px] x, y
#define GAME_FLAG_HOME_POS_Y 300 // [px] x, y
#define GAME_FLAG_HOME_POS {GAME_FLAG_HOME_POS_X, GAME_FLAG_HOME_POS_Y} // [px] x, y

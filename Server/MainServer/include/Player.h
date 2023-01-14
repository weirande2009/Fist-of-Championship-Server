#ifndef PLAYER_H
#define PLAYER_H
#include "common.h"
#include "Protocols.h"
#include "Database.h"

class Player
{
// Variables
private:
    
    
public:
    enum PlayerState{
        NOT_LOGIN = 0,          // not login
        IN_HALL = 1,            // in the hall
        IN_ROOM = 2,            // in the room
        IN_GAME = 3             // in the game
    };
    std::string player_name;    // player name
    std::string player_id;      // player id
    int player_state;           // player state
    int character_type;         // character type
    std::string room_no;        // room no
    bool ready;                 // ready to start game
    int seat_no;                // seat_no

// Functions
private:

public:
    Player();
    ~Player();
    void ResetRoomInfo();
    void ResetPlayerInfo();
    void ExitRoom();

};





#endif




#ifndef GamePLAYER_H
#define GamePLAYER_H
#include "common.h"
#include "Protocols.h"
#include "Database.h"

class GamePlayer
{
// Variables
private:
    
    
public:
    enum PlayerState{
        EXIT = 0,               // Exit Game
        ALIVE = 1,              // Alive
        DEAD = 2,               // Dead
    };
    std::string player_name;    // player name
    std::string player_id;      // player id
    int player_state;           // player state
    int character_type;         // character type
    int seat_no;                // seat_no
    bool connected;             // Whether connect
    bool load;                  // Whether load
    bool begin;                 // Whether begin
    bool end;                   // Whether end
    bool quit;                  // Whether quit

// Functions
private:

public:
    GamePlayer();
    ~GamePlayer();
    void ResetPlayerInfo();
    void ExitGame();

};





#endif




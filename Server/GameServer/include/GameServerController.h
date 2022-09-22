#ifndef GAMESERVERCONTROLLER_H
#define GAMESERVERCONTROLLER_H
#include "common.h"
#include "GameServer.h"

class GameServerController{

// Variables
private:
    std::string ip;
    int port;
    int total_player_number;
    GameServer *game_server;

public:
    
// Functions
private:

public:
    GameServerController(); 
    GameServerController(int _total_player_number, int _port); 
    ~GameServerController();

    void Start();

};




#endif

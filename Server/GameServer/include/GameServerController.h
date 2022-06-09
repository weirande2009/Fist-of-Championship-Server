#ifndef GAMESERVERCONTROLLER_H
#define GAMESERVERCONTROLLER_H
#include "common.h"
#include "GameServer.h"

class GameServerController{

// Variables
private:
    std::string ip;
    int port;
    GameServer *game_server;

public:
    
// Functions
private:

public:
    GameServerController(); 
    GameServerController(int); 
    ~GameServerController();


};




#endif

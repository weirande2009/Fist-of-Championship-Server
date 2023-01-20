#ifndef GAME_SERVER_PORT_CONTROLLER_H
#define GAME_SERVER_PORT_CONTROLLER_H
#include "utilities/common.h"
#include "utilities/Protocols.h"
#include "game/Room.h"

class GameServerPortController
{
// Variables
private:
    
    
public:
    std::map<int, Room*> port_room_pairs;
    int port_pool[GAME_SERVER_PORT_NUM];
    bool port_used[GAME_SERVER_PORT_NUM];
    

// Functions
private:
    std::mutex port_pool_mutex;

public:
    GameServerPortController();
    ~GameServerPortController();
    int GetNewPort();
    void ReleasePort(int port);

};




#endif

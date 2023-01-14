#ifndef GAMECLIENTPOOL_H
#define GAMECLIENTPOOL_H
#include "common.h"
#include "GameClient.h"

class GameClientPool
{
// Variables
private:
    
public:
    std::vector<GameClient*> clients;               // Contain real client

    int total_player_number;

// Functions
private:

public:
    GameClientPool();
    GameClientPool(int total_player_number);
    ~GameClientPool();
    GameClient* AddClient(sockaddr_in socket_addr);
    GameClient* GetClient(sockaddr_in socket_addr);
    bool ClientEqual(const sockaddr_in &socket_addr_1,const sockaddr_in &socket_addr_2);
    

    bool CheckAllConnect();
    bool CheckAllLoad();
    bool CheckAllQuit();
    bool CheckAllGameOver();
};




#endif

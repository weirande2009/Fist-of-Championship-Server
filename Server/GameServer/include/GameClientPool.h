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
    std::map<int, GameClient*> client_pool;         // A map of fd and client index in client_pool
    std::map<std::string, int> name_fd_map;         // A map of fd and user name

    int total_player_number;

// Functions
private:

public:
    GameClientPool();
    GameClientPool(int total_player_number);
    ~GameClientPool();
    int AddClient(int fd, sockaddr_in socket_addr);
    GameClient* GetClient(int fd);

    bool CheckAllLoad();
    bool CheckAllStart();
    bool CheckAllEnd();
    bool CheckAllQuit();


};




#endif

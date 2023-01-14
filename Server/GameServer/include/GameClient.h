#ifndef GAMECLIENT_H
#define GAMECLIENT_H
#include "common.h"
#include "GamePlayer.h"
#include "GameProtocols.h"

class GameClient
{
// Variables
private:
public:
    /* Socket */
    int no;
    sockaddr_in socket_addr;
    std::string received_string;    // received string

    /* Client State */
    std::mutex send_mutex;          // send mutex
    bool closed;                    // whether the other side is closed

    /* GamePlayer */
    GamePlayer player;

// Functions
private:
    
public:
    GameClient();
    GameClient(sockaddr_in _socket_addr);
    ~GameClient();


};




#endif

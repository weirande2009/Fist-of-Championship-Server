#ifndef GAMECLIENT_H
#define GAMECLIENT_H
#include "common.h"
#include "GamePlayer.h"
#include "GameProtocols.h"

class GameClient
{
// Variables
private:
    int client_no;
    std::string client_ip;          // Server's IP
    int client_port;                // Server's port

public:
    int client_fd;                  // Server's handle
    GamePlayer player;
    int read_length;                // Length of waiting data
    char* received_data;            // received data
    int received_data_size;         // received data size
    std::string received_string;    // received string
    int received_length;            // received length
    bool rd_head;                   // whether read head
    std::mutex send_mutex;          // send mutex

// Functions
private:
    
public:
    GameClient();
    GameClient(int fd, sockaddr_in socket_addr);
    ~GameClient();
    void SetClientNo(int no);
    int GetClientNo();
    


};




#endif

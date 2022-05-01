#ifndef MAINCLIENT_H
#define MAINCLIENT_H
#include "common.h"
#include "Player.h"
#include "Database.h"
#include "Protocols.h"

class MainClient
{
// Variables
private:
    int client_no;
    std::string client_ip;          // Server's IP
    int client_port;                // Server's port

public:
    int client_fd;                  // Server's handle
    Player player;
    int read_length;                // Length of waiting data
    char* received_data;            // received data
    std::string received_string;    // received string
    int received_length;            // received length
    bool rd_head;                   // whether read head
    int client_state;               // client state

// Functions
private:
    
public:
    MainClient();
    MainClient(int fd, sockaddr_in socket_addr);
    ~MainClient();
    void SetClientNo(int no);
    int GetClientNo();

    void InstantiatePlayer(std::string user_name);
    


};




#endif

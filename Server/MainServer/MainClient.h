#ifndef MAINCLIENT_H
#define MAINCLIENT_H
#include "common.h"
#include "../common/Client.h"
#include "Player.h"
#include "Database.h"
#include "Protocols.h"

class MainClient: public Client
{
// Variables
private:
    int client_no;

public:
    Player player;
    int read_length;        // Length of waiting data
    char* received_data;    // received data
    int received_length;    // received length
    bool rd_head;           // whether read head
    int client_state;       // client state

// Functions
private:
    void InstantiatePlayer(std::string user_name);

public:
    MainClient(int fd, sockaddr_in socket_addr);
    ~MainClient();
    void SetClientNo(int no);
    int GetClientNo();

    void InstantiatePlayer(std::string user_name);
    


};




#endif

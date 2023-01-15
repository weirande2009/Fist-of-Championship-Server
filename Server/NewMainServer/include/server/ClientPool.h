#pragma once

#include "utilities/common.h"
#include "server/MainClient.h"

class ClientPool
{
// Variables
private:
    
public:
    std::vector<MainClient*> clients;            // Contain real client
    std::map<int, MainClient*> client_pool;      // A map of fd and client index in client_pool

// Functions
private:

public:
    ClientPool();
    ~ClientPool();
    int AddClient(int fd, sockaddr_in socket_addr);
    MainClient* GetClient(int fd);
    void RemoveClient(int fd);
};


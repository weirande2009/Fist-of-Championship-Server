#ifndef CLIENTPOOL_H
#define CLIENTPOOL_H
#include "common.h"
#include "MainClient.h"

class ClientPool
{
// Variables
private:
    
public:
    std::vector<MainClient*> clients;             // Contain real client
    std::map<int, MainClient*> client_pool;      // A map of fd and client index in client_pool
    std::map<std::string, int> name_fd_map;      // A map of fd and user name

// Functions
private:

public:
    ClientPool();
    ~ClientPool();
    int AddClient(int fd, sockaddr_in socket_addr);
    MainClient* GetClient(int fd);
    std::vector<int> GetOnlineFriendFd(const MainClient& client);
    std::vector<std::string> GetOnlineFriendName(const MainClient& client);
    std::vector<std::string> GetFriendName(const MainClient& client);
    int CheckOnline(std::string user_name);
};




#endif

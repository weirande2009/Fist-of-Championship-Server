#ifndef MAINSERVER_H
#define MAINSERVER_H
#include "../common/common.h"
#include "../common/Server.h"
#include "../common/my_network.h"
#include "ClientPool.h"
#include "Lobby.h"
#include "Database.h"

class MainServer: public Server
{
// Variables
private:
    // constant
    const int EPOLL_EVENT_NUM = 1024;

    // variables
    int server_wait_num = 32;
    int epoll_fd;
    struct epoll_event epoll_ev;
    struct epoll_event* epoll_events;
    ClientPool client_pool;
    Lobby lobby;
    std::vector<void(MainServer::*)(MainClient&)> process_function_pool; 
    

public:


// Functions
private:
    void SetNonBlocking(int fd);  // Set handle to be non-blocking

public:
    MainServer();
    MainServer(std::string _ip, int _port);
    ~MainServer();
    void Start();  // Start listening and wait for connecting
    void MainLoop();  // Main listening loop
    void ProcessData(char* data, int length);  // Process data received from clients
    void SendData(int fd, char* data, int length, int delete_type=0);  // Send data to clients
    void ReadData(int fd);  // Send data to clients
    void FillPlayerInfo(PlayerInfo& player_info,const Player& player);  // Fill the struct of PlayerInfo

    /* Various functions for processing received data */
    void ProcessLogin(MainClient& client);
    void ProcessRegister(MainClient& client);
    void ProcessHallRoom(MainClient& client);
    void ProcessFriend(MainClient& client);
    void ProcessEnterRoom(MainClient& client);
    void ProcessRoomInfo(MainClient& client);
    void ProcessModChar(MainClient& client);
    void ProcessReady(MainClient& client);
    void ProcessCancelReady(MainClient& client);
    void ProcessStartGame(MainClient& client);
    void ProcessExitLogin(MainClient& client);
    void ProcessQuit(MainClient& client);
    void ProcessExitRoom(MainClient& client);
    void ProcessCreateRoom(MainClient& client);
    void ProcessAddFriend(MainClient& client);
    void ProcessReplyFriend(MainClient& client);
};




#endif

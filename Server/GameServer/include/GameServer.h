#ifndef GAMESERVER_H
#define GAMESERVER_H
#include "common.h"
#include "my_network.h"
#include "GameClient.h"
#include "GameClientPool.h"
#include "GameProtocols.h"
#include "GameClientToServer.pb.h"
#include "GameServerToClient.pb.h"
#include "MyLog.h"

class GameServer
{
// Variables
private:
    // constant
    const int EPOLL_EVENT_NUM = 1024;

    // variables
    int server_fd;                      // Server's handle
    std::string server_ip;              // Server's IP
    int server_port;                    // Server's port
    int server_wait_num = 32;
    int epoll_fd;
    struct epoll_event epoll_ev;
    struct epoll_event* epoll_events;
    std::vector<void(GameServer::*)(GameClient*)> process_function_pool; 
    GameClientPool client_pool;         // Client pool
    int total_player_number;            // Total player number in this game

    // multiple thread variables
    char* player_operations;            // players' operations
    std::thread my_thread;              // thread for send operation to players
    std::mutex operation_mutex;         // mutex for operation
    int frame_rate;                     // frame rate
    bool game_over;                     // game over flag
    std::mutex game_over_mutex;         // mutex for operation

public:


// Functions
private:
    void SetNonBlocking(int fd);  // Set handle to be non-blocking

public:
    GameServer();
    GameServer(std::string _ip, int _port, int _total_player_number);
    ~GameServer();
    void Start();  // Start listening and wait for connecting
    void MainLoop();  // Main listening loop
    void ProcessData(char* data, int length);  // Process data received from clients
    void SendData(int fd, const char* data, int length, int cmd);  // Send data to clients
    void ReadData(int fd);  // Send data to clients
    void SendOperation();   // Send Operations to players

    void ProcessConnect(GameClient *client);
    void ProcessLoad(GameClient *client);
    void ProcessExit(GameClient *client);
    void ProcessOperation(GameClient *client);
    void ProcessStart(GameClient *client);
    void ProcessEnd(GameClient *client);

};




#endif

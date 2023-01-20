#ifndef MAINSERVER_H
#define MAINSERVER_H
#include "utilities/common.h"
#include "utilities/my_network.h"
#include "utilities/Database.h"
#include "utilities/MyLog.h"
#include "utilities/Protocols.h"
#include "utilities/GameServerPortController.h"
#include "protobuf/MainProtobuf.pb.h"
#include "protobuf/ServerProtobuf.pb.h"
#include "server/ClientPool.h"
#include "game/Lobby.h"

struct WaitInfo{
    struct timeval start_time;
    MainClient* client;
};

struct SendPackage{
    int fd;
    std::string data;
    int length;
    int cmd;
};

class MainServer
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
    std::unique_ptr<ClientPool> client_pool;
    Lobby lobby;
    std::vector<void(MainServer::*)(MainClient*)> process_function_pool; 
    GameServerPortController game_server_port_controller;

    std::unordered_map<int, WaitInfo> waiting_players;
    std::mutex waiting_player_mutex;
    std::atomic<bool> game_starter_stop;
    std::unique_ptr<std::thread> game_starter;
    std::condition_variable game_starter_condition_variable;

    std::queue<std::unique_ptr<SendPackage>> send_packages;
    std::mutex send_package_mutex;
    std::atomic<bool> send_package_stop;
    std::unique_ptr<std::thread> send_package_thread;
    std::condition_variable send_package_condition_variable;

public:


// Functions
private:
    void SetNonBlocking(int fd);  // Set handle to be non-blocking

public:
    MainServer(std::string _ip, int _port);
    ~MainServer();
    void Start();  // Start listening and wait for connecting
    void MainLoop();  // Main listening loop
    void SendData(int fd, const char* data, int length, int cmd);  // Send data to clients
    void ReadData(int fd);  // Send data to clients

    /* Various functions for processing received data */
    void ProcessParticipate(MainClient* client);
    void ProcessGameServer(MainClient* client);

    /* Game Starter */
    void WaitGameStart();
    void StartGame(std::vector<int> gamers);
    std::vector<int> GeneratePlayList();

    /* Package Sender */
    void PackageSender();
    void SendAPackage(int fd, const char* data, int length, int cmd);

    /* Utilities */
    void RemoveClient(int fd);
    int ComputeInterval(struct timeval& t1, struct timeval& t2);

};




#endif

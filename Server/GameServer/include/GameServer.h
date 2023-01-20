#ifndef GAMESERVER_H
#define GAMESERVER_H
#include "common.h"
#include "my_network.h"
#include "GameClient.h"
#include "GameClientPool.h"
#include "GameProtocols.h"
#include "GameProtobuf.pb.h"
#include "ServerProtobuf.pb.h"
#include "MainProtobuf.pb.h"
#include "MyLog.h"

class GameServer
{
// Variables
private:
    // variables
    int main_server_fd;
    std::string main_server_ip;
    int main_server_port;
    std::string server_ip;              // Server's IP
    int server_udp_fd;                  // Server's udp handle
    int server_udp_port;                // Server's udp port
    int server_wait_num = 32;
    std::vector<void(GameServer::*)(GameClient*)> process_function_pool; 
    GameClientPool client_pool;         // Client pool
    int total_player_number;            // Total player number in this game
    

    // multiple thread variables
    std::vector<ChampionFistGame::OperationFrame> player_operation_vector;
    std::vector<std::vector<ChampionFistGame::OperationFrame>> all_operation_frames;
    int sync_frame_id;
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
    GameServer(std::string _ip, int _udp_port, int _total_player_number);
    ~GameServer();
    void Start();  // Start listening and wait for connecting
    void MainLoop();  // Main listening loop
    void ProcessData(char* data, int length);  // Process data received from clients
    void SendData(int fd, const char* data, int length, int cmd);  // Send data to clients
    void ReadData();            // Send data to clients
    void SendDataToMain(int fd, const char* data, int length, int cmd);
    void SendLogicFrame();      // Send Operations to players

    // Process
    void ProcessConnect(GameClient *client);
    void ProcessLoad(GameClient *client);
    void ProcessExit(GameClient *client);
    void ProcessPlayerFrame(GameClient *client);
    void ProcessDelay(GameClient *client);
    void ProcessGameOver(GameClient *client);

    void ClearPlayerOperation();
    void ExitServer();

};




#endif

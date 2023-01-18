#include "GameServer.h"

void CopyOperation(ChampionFistGame::OperationFrame* des, ChampionFistGame::OperationFrame* src){
    des->set_click_q(src->click_q());
    des->set_click_w(src->click_w());
    des->set_click_e(src->click_e());
    des->set_click_r(src->click_r());
    des->set_click_property(src->click_property());
    des->set_click_mouse(src->click_mouse());
    des->set_change_weapon(src->change_weapon());
    des->set_change_armor(src->change_armor());
    des->set_mouse_pos_x(src->mouse_pos_x());
    des->set_mouse_pos_y(src->mouse_pos_y());
    des->set_armor_no(src->armor_no());
    des->set_low_weapon_no(src->low_weapon_no());
    des->set_middle_weapon_no(src->middle_weapon_no());
    des->set_high_weapon_no(src->high_weapon_no());
}

/******************************************
 * Function: Initialize GameServer
 * Parameters: 2
 * _ip: ip address
 * _port: port no.
 * Return: None
 *****************************************/
GameServer::GameServer(std::string _ip, int _udp_port, int _total_player_number)
{
    // Set basic info
    this->total_player_number = _total_player_number;
    this->server_ip = _ip;
    this->server_udp_port = _udp_port;
    this->server_udp_fd = init_udp_server(this->server_ip, this->server_udp_port, false);
    // Set Clients
    this->client_pool = GameClientPool(_total_player_number);
    // Set process function pool
    process_function_pool.push_back(&GameServer::ProcessConnect);
    process_function_pool.push_back(&GameServer::ProcessLoad);
    process_function_pool.push_back(&GameServer::ProcessExit);
    process_function_pool.push_back(&GameServer::ProcessPlayerFrame);
    process_function_pool.push_back(&GameServer::ProcessDelay);
    process_function_pool.push_back(&GameServer::ProcessGameOver);
    // Set Frame parameter
    this->frame_rate = 15;
    this->game_over = false;
    this->sync_frame_id = -1;
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
GameServer::~GameServer()
{
    
}

/******************************************
 * Function: Start server
 * Parameters: 0
 * Return: None
 *****************************************/
void GameServer::Start()
{
    std::cout << "Start Listening" << std::endl;
    this->MainLoop();
}

/******************************************
 * Function: Main processing loop of server
 * Parameters: 0
 * Return: None
 *****************************************/
void GameServer::MainLoop()
{
    ReadData();
    return;
}

/******************************************
 * Function: Set the given handle to be non-blocking
 * Parameters: 1
 * fd: handle number
 * Return: None
 *****************************************/
void GameServer::SetNonBlocking(int fd)
{
    int old_flags = fcntl(fd, F_GETFL, 0);
    int new_flags = old_flags | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flags);
    return;
}

/******************************************
 * Function: Read data from specific handle
 * Parameters: 1
 * fd: handle number
 * Return: None
 *****************************************/
void GameServer::ReadData()
{
    // read data from client
    int buf_size = 1024;
    char* buf = new char[buf_size];
    struct sockaddr_in client_addr;
    socklen_t sin_size= sizeof(struct sockaddr_in);
    while(1){
        // Check game over
        this->game_over_mutex.lock();
        if(this->game_over){
            // Process Exit
            delete[] buf;
            ExitServer();
            break;
        }
        this->game_over_mutex.unlock();
        // Recv from client
        int rlen = recvfrom(this->server_udp_fd, buf, buf_size, 0, (struct sockaddr*)&client_addr, &sin_size);
        // Get client
        GameClient* client = this->client_pool.GetClient(client_addr);
        std::cout << MyLog::Instance().GetTime() << std::endl;
        std::cout << "Receive Length: " << rlen << " From " << client->no << std::endl;
        if(rlen == 0){
            // client disconnect
            this->ProcessExit(client);
        }
        else if(rlen < 0){
            // something wrong and disconnect
            std::cout << "Receive NEGATIVE!!!" << std::endl;
        }
        else{
            if(rlen < PACKAGE_HEAD_LENGTH){
                continue;
            }
            int package_cmd;
            memcpy(&package_cmd, buf, sizeof(int));
            std::cout << "package_cmd: " << package_cmd << std::endl;
            int package_length;
            memcpy(&package_length, buf+sizeof(int), sizeof(int));
            std::cout << "package_length: " << package_length << std::endl;

            // if receive all the data needed
            if(rlen == package_length){
                // Generate received string
                client->received_string.assign(buf+PACKAGE_HEAD_LENGTH, rlen-PACKAGE_HEAD_LENGTH);
                // Start processing
                (this->*process_function_pool[package_cmd-1])(client);
            }
            else if(rlen < package_length){ 
                std::cout << "package loss" << std::endl;
            }
            else{
                // something wrong
            }
        }
        //std::cout << std::endl;
    }
    
}

/******************************************
 * Function: Send data to specific handle
 * Parameters: 4
 * fd: handle number
 * data: data from Protobuf
 * length: Protobuf data length
 * cmd: command no.
 * Return: None
 *****************************************/
void GameServer::SendData(int no, const char* data, int length, int cmd)
{
    PackageHead package_head;
    package_head.cmd = cmd;
    package_head.length = length+8;
    char* sent_data = new char[package_head.length];
    memcpy(sent_data, (const char*)&package_head, sizeof(PackageHead));
    if(length != 0){
        memcpy(sent_data+sizeof(PackageHead), data, length);
    }
    this->client_pool.clients[no]->send_mutex.lock();
    int len = sendto(this->server_udp_fd, sent_data, package_head.length, MSG_NOSIGNAL, (struct sockaddr*)&(this->client_pool.clients[no]->socket_addr), (socklen_t)sizeof(struct sockaddr_in));
    if(len == -1){
        std::cout << "玩家: " << no << "关闭" << std::endl;
        // Set client close
        this->client_pool.clients[no]->closed = true;
        // Exit client
        this->ProcessExit(this->client_pool.clients[no]);
    }
    this->client_pool.clients[no]->send_mutex.unlock();
    delete[] sent_data;
    std::cout << MyLog::Instance().GetTime() << std::endl;
    std::cout << "Send Length: " << len << std::endl << std::endl;
}

/******************************************
 * Function: Send Operation
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::SendLogicFrame()
{
    std::cout << "开始发送帧线程" << std::endl;
    while(1){
        // Check game over
        this->game_over_mutex.lock();
        if(this->game_over){
            this->game_over_mutex.unlock();
            break;
        }
        this->game_over_mutex.unlock();
        // Send Logic Frame
        this->operation_mutex.lock();
        // Generate new frame
        std::vector<ChampionFistGame::OperationFrame> new_frame;
        bool null_operation = false;
        for(int i=0; i<total_player_number; i++){
            ChampionFistGame::OperationFrame tmp_operation = ChampionFistGame::OperationFrame();
            if(!(&(player_operation_vector[i]))){
                null_operation = true;
                break;
            }
            CopyOperation(&tmp_operation, &(player_operation_vector[i]));
            // tmp_operation.CopyFrom(player_operation_vector[i]);
            new_frame.push_back(tmp_operation);
        }
        if(null_operation){
            this->operation_mutex.unlock();
            break;
        }
        // insert into 
        all_operation_frames.push_back(new_frame);
        this->sync_frame_id++;
        // Send
        std::string send_string;
        for(int i=0; i<this->client_pool.clients.size(); i++){
            if(this->client_pool.clients[i]->player.player_state != GamePlayer::EXIT){
                ChampionFistGame::S_LogicFrame s_logic_frame = ChampionFistGame::S_LogicFrame();
                s_logic_frame.set_frame_id(this->sync_frame_id);
                // Add all unsync frame to logic frame
                for(int j=this->client_pool.clients[i]->player.sync_frame_id+1; j<=this->sync_frame_id; j++){
                    ChampionFistGame::UnsyncFrame* unsync_frame = s_logic_frame.add_unsync_frames();
                    unsync_frame->set_frame_id(j);
                    for(int k=0; k<total_player_number; k++){
                        ChampionFistGame::OperationFrame* tmp_operation = unsync_frame->add_all_players_opt();
                        tmp_operation->CopyFrom(all_operation_frames[j][k]);
                    }
                }
                s_logic_frame.SerializeToString(&send_string);
                this->SendData(this->client_pool.clients[i]->no, send_string.c_str(), send_string.length(), ChampionFistGame::S_LOGIC_FRAME);
            }
        }
        // Reset player_operation
        ClearPlayerOperation();
        this->operation_mutex.unlock();
        usleep(FRAME_INTERVAL);
    }
}

/******************************************
 * Function: Process Connect
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::ProcessConnect(GameClient *client)
{
    std::string send_string;
    ChampionFistGame::C_Connect c_connect;
    c_connect.ParseFromString(client->received_string);
    // Set Client Info
    client->player.player_name = c_connect.name();
    client->player.seat_no = client->no;
    client->player.character_type = c_connect.champion_no();
    client->player.connected = true;
    client->player.player_state = GamePlayer::ALIVE;
    // send received
    SendData(client->no, NULL, 0, ChampionFistGame::C_CONNECT);
    // if all connect, send
    if(this->client_pool.CheckAllConnect()){
        ChampionFistGame::S_Initialize s_initialize;
        // s_connect.set_random_seed(12345);
        srand((unsigned int)time(NULL));
        s_initialize.set_random_seed(rand());
        // Add player info
        for(int i=0; i<this->client_pool.clients.size(); i++){
            ChampionFistGame::GamePlayerInfo *game_player_info = s_initialize.add_game_players_info();
            game_player_info->set_name(this->client_pool.clients[i]->player.player_name);
            game_player_info->set_seat_no(this->client_pool.clients[i]->player.seat_no);
            game_player_info->set_champion_no(this->client_pool.clients[i]->player.character_type);
        }
        // Send to all players
        for(int i=0; i<this->client_pool.clients.size(); i++){
            s_initialize.set_seat_no(i);
            s_initialize.SerializeToString(&send_string);
            this->SendData(this->client_pool.clients[i]->no, send_string.c_str(), send_string.length(), ChampionFistGame::S_INITIALIZE);
        }
    }
}

/******************************************
 * Function: Process Load
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::ProcessLoad(GameClient *client)
{
    std::string send_string;
    // Set Client Info
    client->player.load = true;
    // Send
    this->SendData(client->no, NULL, 0, ChampionFistGame::S_LOAD);
    // if all loaded, start
    if(this->client_pool.CheckAllLoad()){
        // Send Start
        for(int i=0; i<this->client_pool.clients.size(); i++){
            this->SendData(this->client_pool.clients[i]->no, NULL, 0, ChampionFistGame::S_START);
        }
        // Set Operation data
        for(int i=0; i<total_player_number; i++){
            ChampionFistGame::OperationFrame operation_frame = ChampionFistGame::OperationFrame();
            player_operation_vector.push_back(operation_frame);
        }
        this->ClearPlayerOperation();
        // Start thread
        this->my_thread = std::thread(&GameServer::SendLogicFrame, this);
        this->my_thread.detach();
    }
}

/******************************************
 * Function: Process Exit
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::ProcessExit(GameClient *client)
{
    std::string send_string;
    // Set Client Info
    this->operation_mutex.lock();
    client->player.player_state = GamePlayer::EXIT;
    this->operation_mutex.unlock();
    // Send to Other Players
    ChampionFistGame::S_OtherExit s_other_exit;
    s_other_exit.set_seat_no(client->player.seat_no);
    s_other_exit.SerializeToString(&send_string);
    for(int i=0; i<this->client_pool.clients.size(); i++){
        if(!this->client_pool.clients[i]->closed && this->client_pool.clients[i] != client){
            this->SendData(this->client_pool.clients[i]->no, send_string.c_str(), send_string.length(), ChampionFistGame::S_OTHER_EXIT);
        }
    }
    // Send
    this->SendData(client->no, NULL, 0, ChampionFistGame::S_EXIT);
    // Close client

    // If all quit, end game
    if(this->client_pool.CheckAllQuit()){
        // Exit GameServer
        this->game_over_mutex.lock();
        this->game_over = true;
        this->game_over_mutex.unlock();
    }
}

/******************************************
 * Function: Process Operation
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::ProcessPlayerFrame(GameClient *client)
{
    std::string send_string;
    ChampionFistGame::C_PlayerFrame c_player_frame;
    c_player_frame.ParseFromString(client->received_string);
    // Update frame id
    client->player.sync_frame_id = c_player_frame.frame_id() - 1;
    // Set operations
    if(c_player_frame.frame_id() == this->sync_frame_id + 1){
        this->operation_mutex.lock();
        this->player_operation_vector[client->player.seat_no].CopyFrom(c_player_frame.player_opt_frame());
        this->operation_mutex.unlock();
    }
}

/******************************************
 * Function: Process Delay
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::ProcessDelay(GameClient *client)
{
    this->SendData(client->no, NULL, 0, ChampionFistGame::S_DELAY);
}

/******************************************
 * Function: Process GameOver
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::ProcessGameOver(GameClient *client)
{
    client->player.game_over = true;
    if(this->client_pool.CheckAllGameOver()){
        this->game_over_mutex.lock();
        this->game_over = true;
        this->game_over_mutex.unlock();
    }
}

/******************************************
 * Function: Clear player operation
 * Parameters: 0
 * Return: None
 *****************************************/
void GameServer::ClearPlayerOperation()
{
    for(int i=0; i<total_player_number; i++){
        player_operation_vector[i].set_click_q(false);
        player_operation_vector[i].set_click_w(false);
        player_operation_vector[i].set_click_e(false);
        player_operation_vector[i].set_click_r(false);
        player_operation_vector[i].set_click_property(false);
        player_operation_vector[i].set_click_mouse(false);
        player_operation_vector[i].set_change_weapon(false);
        player_operation_vector[i].set_change_armor(false);
        player_operation_vector[i].set_mouse_pos_x(0);
        player_operation_vector[i].set_mouse_pos_y(0);
        player_operation_vector[i].set_armor_no(-1);
        player_operation_vector[i].set_low_weapon_no(-1);
        player_operation_vector[i].set_middle_weapon_no(-1);
        player_operation_vector[i].set_high_weapon_no(-1);
    }
}

/******************************************
 * Function: Exit Server
 * Parameters: 0
 * Return: None
 *****************************************/
void GameServer::ExitServer()
{
    // Clear Client
    this->operation_mutex.lock();
    for(int i=0; i<client_pool.clients.size(); i++){
        if(client_pool.clients[i]->player.player_state != GamePlayer::EXIT){
            client_pool.clients[i]->player.player_state = GamePlayer::EXIT;
        }
        delete client_pool.clients[i];
    }
    this->operation_mutex.unlock();
}





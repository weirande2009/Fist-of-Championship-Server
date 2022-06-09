#include "GameServer.h"

/******************************************
 * Function: Initialize GameServer
 * Parameters: 2
 * _ip: ip address
 * _port: port no.
 * Return: None
 *****************************************/
GameServer::GameServer(std::string _ip, int _port, int _total_player_number)
{
    // Set basic info
    this->total_player_number = _total_player_number;
    this->server_ip = _ip;
    this->server_port = _port;
    this->server_fd = init_server(this->server_ip, this->server_port, this->server_wait_num, true);
    // Set epoll
    this->epoll_events = new struct epoll_event[this->EPOLL_EVENT_NUM];
    this->epoll_fd = epoll_create(this->EPOLL_EVENT_NUM);
    if (this->epoll_fd == -1) {    
        perror("epoll_create");    
        exit(-1);    
    }
    this->epoll_ev.events = EPOLLIN;    
    this->epoll_ev.data.fd = this->server_fd;    
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_fd, &this->epoll_ev) == -1) {    
        perror("epoll_ctl: listen_sock");    
        exit(EXIT_FAILURE);    
    }    
    // Set Clients
    this->client_pool = GameClientPool();
    // Set process function pool
    process_function_pool.push_back(ProcessConnect);
    process_function_pool.push_back(ProcessLoad);
    process_function_pool.push_back(ProcessExit);
    process_function_pool.push_back(ProcessOperation);
    // Set thread
    this->my_thread = std::thread(SendOperation);
    // Set server parameter
    this->frame_rate = 30;
    this->game_over = false;
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
GameServer::~GameServer()
{
    delete[] this->epoll_events;
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
    for (;;) {
        // block waiting
        int nfds = epoll_wait(this->epoll_fd, this->epoll_events, this->EPOLL_EVENT_NUM, -1); 
        if (nfds == -1) {    
            if(errno!=EINTR)
            {
                perror("epoll_pwait");    
                exit(EXIT_FAILURE);   
            }
        }    
        for (int i = 0; i < nfds; ++i) 
        {    
            int fd = this->epoll_events[i].data.fd;   
            int conn_sock; 
            if (fd == this->server_fd)
            {
                while (1)     
                {   
                    struct sockaddr_in remote;
                    socklen_t addrlen = sizeof(remote);
                    conn_sock = accept(this->server_fd, (struct sockaddr *)&remote, &addrlen);
                    if(conn_sock <= 0){
                        break;
                    }
                    this->SetNonBlocking(conn_sock);    
                    this->epoll_ev.events = EPOLLIN | EPOLLET;    
                    this->epoll_ev.data.fd = conn_sock;    
                    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, conn_sock, &this->epoll_ev) == -1)  
                    {    
                        perror("epoll_ctl: add");    
                        exit(-1);    
                    }   
                    // Log
                    MyLog::Instance().LogConnect(conn_sock, remote);
                    // Add new client to pool
                    this->client_pool.AddClient(conn_sock, remote);
                }    
                if (conn_sock == -1) {    
                    if (errno != EAGAIN && errno != ECONNABORTED && errno != EPROTO && errno != EINTR)     
                        perror("accept");    
                }    
                continue;    
            }      
            if (this->epoll_events[i].events & EPOLLIN)
            {    
                this->ReadData(fd);
            }      
        }    
    }
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
void GameServer::ReadData(int fd)
{
    // Get client object
    GameClient* client = this->client_pool.GetClient(fd);
    // read data from client
    while(1){
        int rlen = recv(fd, client->received_data+client->received_length, client->read_length, 0);
        std::cout << MyLog::Instance().GetTime() << std::endl;
        std::cout << "Receive Length: " << rlen << std::endl << std::endl;
        if(rlen == 0){
            // client disconnect
            this->ProcessExit(client);
        }
        else if(rlen < 0){
            // something wrong and disconnect
            break;
        }
        else{
            // store received data
            if(client->read_length >= rlen){
                client->read_length -= rlen;
                client->received_length += rlen;
            }
            else{
                // something wrong
            }
            // judge receive over
            if(client->read_length == 0){
                int package_length;
                memcpy(&package_length, client->received_data+sizeof(int), sizeof(int));
                std::cout << "package_length: " << package_length << std::endl;
                int package_cmd;
                memcpy(&package_cmd, client->received_data, sizeof(int));
                std::cout << "package_cmd: " << package_cmd << std::endl;
                // if receive all the data needed
                if(client->received_length == package_length){
                    // Generate received string
                    client->received_string.assign(client->received_data+PACKAGE_HEAD_LENGTH, package_length-PACKAGE_HEAD_LENGTH);
                    // Start processing
                    (this->*process_function_pool[package_cmd-1])(client);
                    // Set data buffer and received length
                    if(package_cmd != ChampionFist::C_EXIT){
                        client->read_length = PACKAGE_HEAD_LENGTH;
                        client->received_length = 0;
                    }
                    break;
                }
                else if(client->received_length < package_length){  // finish receiving package head
                    if(package_length > client->received_data_size){
                        char* new_data = new char[package_length];
                        memcpy(new_data, client->received_data, client->received_length);
                        delete[] client->received_data;
                        client->received_data = new_data;
                        client->received_data_size = package_length;
                    }
                    client->read_length = package_length - client->received_length;
                }
                else{
                    // something wrong
                }
            }
        }
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
void GameServer::SendData(int fd, const char* data, int length, int cmd)
{
    PackageHead package_head;
    package_head.cmd = cmd;
    package_head.length = length+8;
    char* sent_data = new char[package_head.length];
    memcpy(sent_data, (const char*)&package_head, sizeof(PackageHead));
    memcpy(sent_data+sizeof(PackageHead), data, length);
    this->client_pool.client_pool[fd]->send_mutex.lock();
    int len = send(fd, sent_data, package_head.length, 0);
    this->client_pool.client_pool[fd]->send_mutex.unlock();
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
void GameServer::SendOperation()
{
    while(1){
        // Check game over
        this->game_over_mutex.lock();
        if(this->game_over){
            break;
        }
        this->game_over_mutex.unlock();
        // Send operation
        this->operation_mutex.lock();
        for(int i=0; i<this->client_pool.clients.size(); i++){
            this->client_pool.clients[i]->send_mutex.lock();
            this->SendData(this->client_pool.clients[i]->client_fd, this->player_operations, sizeof(PlayerOperation)*this->total_player_number, ChampionFist::S_OPERATION);
            this->client_pool.clients[i]->send_mutex.unlock();
        }
        this->operation_mutex.unlock();

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
    ChampionFist::C_Connect c_connect;
    c_connect.ParseFromString(client->received_string);
    // Set Client Info
    client->player.player_name = c_connect.name();
    client->player.seat_no = c_connect.seat_no();
    client->player.connected = true;
    // Send
    ChampionFist::S_Connect s_connect;
    s_connect.set_state(client->GetClientNo());
    s_connect.SerializeToString(&send_string);
    this->SendData(client->client_fd, send_string.c_str(), send_string.length(), ChampionFist::S_CONNECT);
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
    ChampionFist::S_Load s_load;
    s_load.set_state(1);
    s_load.SerializeToString(&send_string);
    this->SendData(client->client_fd, send_string.c_str(), send_string.length(), ChampionFist::S_LOAD);
    // if all loaded, send begin
    if(this->client_pool.CheckAllLoad()){
        ChampionFist::S_Begin s_begin;
        s_begin.set_state(1);
        s_begin.SerializeToString(&send_string);
        for(int i=0; i<this->client_pool.clients.size(); i++){
            this->SendData(client->client_fd, send_string.c_str(), send_string.length(), ChampionFist::S_BEGIN);
        }
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
    client->player.quit = true;
    // Send
    ChampionFist::S_Exit s_exit;
    s_exit.set_state(1);
    s_exit.SerializeToString(&send_string);
    this->SendData(client->client_fd, send_string.c_str(), send_string.length(), ChampionFist::S_EXIT);
    // If all quit, end game
    if(this->client_pool.CheckAllQuit()){
        // Exit GameServer
    }
}

/******************************************
 * Function: Process Operation
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::ProcessOperation(GameClient *client)
{
    if(client->received_string.length() == sizeof(PlayerOperation)){
        this->operation_mutex.lock();
        // Copy operations to data
        memcpy(this->player_operations+sizeof(PlayerOperation)*client->GetClientNo(), client->received_string.c_str(), sizeof(PlayerOperation));
        this->operation_mutex.unlock();
    }
}

/******************************************
 * Function: Process Start
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::ProcessStart(GameClient *client)
{
    std::string send_string;
    // Set Client Info
    client->player.begin = true;
    // Check all start
    if(this->client_pool.CheckAllStart()){
        // Set operation data, sorted by client no.
        this->player_operations = new char[sizeof(PlayerOperation)*this->total_player_number];
        // Start thread
        this->my_thread.join();
    }
}

/******************************************
 * Function: Process Start
 * Parameters: 1
 * client: client pointer
 * Return: None
 *****************************************/
void GameServer::ProcessEnd(GameClient *client)
{
    std::string send_string;
    // Set Client Info
    client->player.end = true;
    // Check all start
    if(this->client_pool.CheckAllEnd()){
        ChampionFist::S_End s_end;
        s_end.set_state(1);
        s_end.SerializeToString(&send_string);
        for(int i=0; i<this->client_pool.clients.size(); i++){
            this->SendData(client->client_fd, send_string.c_str(), send_string.length(), ChampionFist::S_END);
        }
        game_over_mutex.lock();
        this->game_over = true;
        game_over_mutex.unlock();
    }
}

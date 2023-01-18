#include "server/MainServer.h"

/******************************************
 * Function: Initialize MainServer
 * Parameters: 2
 * _ip: ip address
 * _port: port no.
 * Return: None
 *****************************************/
MainServer::MainServer(std::string _ip, int _port)
{
    // Set basic info
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
    // Set Client Pool
    this->client_pool = ClientPool();
    // Set Lobby
    this->lobby = Lobby();
    // Set process function pool
    this->process_function_pool.push_back(&MainServer::ProcessParticipate);
    // Set game start thread
    this->game_starter_stop = false;
    this->game_starter = std::make_unique<std::thread>(&MainServer::WaitGameStart, this);
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
MainServer::~MainServer()
{
    delete[] this->epoll_events;
}

/******************************************
 * Function: Start server
 * Parameters: 0
 * Return: None
 *****************************************/
void MainServer::Start()
{
    std::cout << "Start Listening" << std::endl;
    this->MainLoop();
}

/******************************************
 * Function: Main processing loop of server
 * Parameters: 0
 * Return: None
 *****************************************/
void MainServer::MainLoop()
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
void MainServer::SetNonBlocking(int fd)
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
void MainServer::ReadData(int fd)
{
    // Get client object
    MainClient* client = this->client_pool.GetClient(fd);
    // read data from client
    while(1){
        int rlen = recv(fd, client->received_data+client->received_length, client->read_length, 0);
        std::cout << MyLog::Instance().GetTime() << std::endl;
        std::cout << "Receive Length: " << rlen << std::endl << std::endl;
        if(rlen == 0){
            // client disconnect
            this->RemoveClient(fd);
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
                    (this->*process_function_pool[package_cmd])(client);
                    // Set data buffer and received length
                    client->read_length = PACKAGE_HEAD_LENGTH;
                    client->received_length = 0;
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
void MainServer::SendData(int fd, const char* data, int length, int cmd)
{
    PackageHead package_head;
    package_head.cmd = cmd;
    package_head.length = length+8;
    char* sent_data = new char[package_head.length];
    memcpy(sent_data, (const char*)&package_head, sizeof(PackageHead));
    memcpy(sent_data+sizeof(PackageHead), data, length);
    int len = send(fd, sent_data, package_head.length, 0);
    delete[] sent_data;
    std::cout << MyLog::Instance().GetTime() << std::endl;
    std::cout << "Send Length: " << len << std::endl << std::endl;
}

/******************************************
 * Function: Function for game start thread
 * Parameters: 0
 * Return: None
 *****************************************/
void MainServer::WaitGameStart(){
    while (true) {
        std::vector<int> play_list;
        {
            std::unique_lock<std::mutex> lock(waiting_player_mutex);
            if(waiting_players.empty()){  // if empty, wait until there is a package
                game_starter_condition_variable.wait(lock, [this]() { 
                    return game_starter_stop || !waiting_players.empty(); 
                });
            }
            if (game_starter_stop) {
                return;
            }
            // Here store the fd of the selected gamer
            play_list = GeneratePlayList();
        }
        if(play_list.size() > 0){
            StartGame(play_list);
        }
        // remove play list from waiting
        for(int fd: play_list){
            waiting_players.erase(fd);
        }        
    }
}

void MainServer::StartGame(std::vector<int> gamers){
    // get a valid port
    int port = game_server_port_controller.GetNewPort();
    // start the game server
    std::string execute_path = "/root/weiran/Fist-of-Championship/Server/GameServer/main";
    std::string command = execute_path+" "+std::to_string(gamers.size())+" "+std::to_string(port)+"&";
    system(command.c_str());
    // tell gamers to start the game
    ChampionFistMain::S_Start s_start;
    s_start.set_game_server_ip(GAME_SERVER_IP);
    s_start.set_game_server_port(port);
    std::string send_string;
    s_start.SerializeToString(&send_string);
    for(int fd: gamers){
        this->SendData(fd, send_string.c_str(), send_string.length(), ChampionFistMain::S_START+1);
    }
}

std::vector<int> MainServer::GeneratePlayList(){
    // check waiting list
    // strategy:
    // 1. >= 4 people wait at least 2 seconds, get the 4 people
    // 2. 3 people wait at least 5 seconds, get the 3 people
    // 3. 2 people wait at least 60 seconds, get the 2 people
    int n = waiting_players.size();
    std::vector<int> more_than_2;
    std::vector<int> more_than_5;
    std::vector<int> more_than_60;
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    for(auto& info: waiting_players){
        int interval = ComputeInterval(info.second.start_time, current_time);
        if(interval > 60*10e2){
            more_than_60.push_back(info.first);
        }
        if(interval > 5*10e2){
            more_than_5.push_back(info.first);
        }
        if(interval > 2*10e2){
            more_than_2.push_back(info.first);
        }
    }
    std::vector<int> play_list;
    if(more_than_2.size() >= 4){
        play_list.insert(play_list.begin(), more_than_2.begin(), more_than_2.begin()+4);
    }
    else if(more_than_5.size() >= 3){
        play_list.insert(play_list.begin(), more_than_5.begin(), more_than_5.begin()+3);
    }
    else if(more_than_60.size() >= 2){
        play_list.insert(play_list.begin(), more_than_60.begin(), more_than_60.begin()+2);
    }
    return play_list;
}


/******************************************
 * Function: Process Login
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessParticipate(MainClient* client)
{
    std::string send_string;
    ChampionFistMain::C_Participate c_participate;
    c_participate.ParseFromString(client->received_string);
    // add to waiting lists
    {
        std::unique_lock<std::mutex> lock(waiting_player_mutex);
        WaitInfo wait_info;
        gettimeofday(&wait_info.start_time, NULL);
        wait_info.client = client;
        waiting_players[client->client_fd] = wait_info;
        game_starter_condition_variable.notify_one();
    }
}

void MainServer::RemoveClient(int fd){
    client_pool.RemoveClient(fd);
    {
        std::unique_lock<std::mutex> lock(waiting_player_mutex);
        waiting_players.erase(fd);
    }
}

/**
 * Compute the interval of two time in ms
*/
int MainServer::ComputeInterval(struct timeval& t1, struct timeval& t2){
    long mtime, seconds, useconds;
    seconds  = t2.tv_sec  - t1.tv_sec;
    useconds = t2.tv_usec - t1.tv_usec;
    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    return mtime;
}


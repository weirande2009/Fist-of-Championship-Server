#include "MainServer.h"

void TestFunc(MainClient& client);

/******************************************
 * Function: Initialize MainServer
 * Parameters: 2
 * _ip: ip address
 * _port: port no.
 * Return: None
 *****************************************/
MainServer::MainServer(std::string _ip, int _port) : Server()
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
    this->process_function_pool.push_back(&MainServer::ProcessLogin);
    this->process_function_pool.push_back(&MainServer::ProcessRegister);
    this->process_function_pool.push_back(&MainServer::ProcessHallRoom);
    this->process_function_pool.push_back(&MainServer::ProcessFriend);
    this->process_function_pool.push_back(&MainServer::ProcessEnterRoom);
    this->process_function_pool.push_back(&MainServer::ProcessRoomInfo);
    this->process_function_pool.push_back(&MainServer::ProcessModChar);
    this->process_function_pool.push_back(&MainServer::ProcessReady);
    this->process_function_pool.push_back(&MainServer::ProcessCancelReady);
    this->process_function_pool.push_back(&MainServer::ProcessStartGame);
    this->process_function_pool.push_back(&MainServer::ProcessExitLogin);
    this->process_function_pool.push_back(&MainServer::ProcessQuit);
    this->process_function_pool.push_back(&MainServer::ProcessExitRoom);
    this->process_function_pool.push_back(&MainServer::ProcessCreateRoom);
    this->process_function_pool.push_back(&MainServer::ProcessAddFriend);
    this->process_function_pool.push_back(&MainServer::ProcessReplyFriend);
}

/******************************************
 * Function: Start server
 * Parameters: 0
 * Return: None
 *****************************************/
void MainServer::Start()
{
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
            perror("epoll_pwait");    
            exit(EXIT_FAILURE);    
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
    MainClient client = this->client_pool.GetClient(fd);
    char* data = new char[client.read_length];
    // read data from client
    int rlen = recv(fd, data, client.read_length, 0);
    if(rlen == 0){
        // client disconnect
    }
    else if(rlen < 0){
        // something wrong and disconnect
    }
    else{
        // store received data
        if(client.read_length >= rlen){
            // copy received data
            memcpy(client.received_data+client.received_length, data, rlen);
            client.read_length -= rlen;
            client.received_length += rlen;
            delete[] data;
        }
        else{
            // something wrong
        }
        // judge receive over
        if(client.read_length == 0){
            int package_length = int(client.received_data+sizeof(int));
            int package_cmd = int(client.received_data);
            // if receive all the data needed
            if(client.received_length == package_length){
                // Generate received string
                client.received_string.assign(client.received_data+PACKAGE_HEAD_LENGTH, package_length-PACKAGE_HEAD_LENGTH);
                // Start processing
                (this->*process_function_pool[package_cmd])(client);
                // Set data buffer and received length
                if(package_cmd != C_QUIT){
                    delete[] client.received_data;
                    client.received_data = new char[PACKAGE_HEAD_LENGTH];
                    client.read_length = PACKAGE_HEAD_LENGTH;
                    client.received_length = 0;
                }
                
            }
            else if(client.received_length < package_length){  // finish receiving package head
                char* new_data = new char[package_length];
                memcpy(new_data, client.received_data, client.received_length);
                delete[] client.received_data;
                client.received_data = new_data;
                client.read_length = package_length - client.received_length;
            }
            else{
                // something wrong
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
}

/******************************************
 * Function: Process Login
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessLogin(MainClient& client)
{
    std::string send_string;
    CF::C_Login c_login;
    c_login.ParseFromString(client.received_string);
    int ret = Database::Instance().PlayerLogin(c_login.name(), c_login.password_md5());
    if(ret == LOGIN_SUCCESS){
        // Set Client State
        client.client_state = CLIENT_LOGIN;
        // Instantiate Player
        client.InstantiatePlayer(c_login.name());
        // Add fd and user name map
        this->client_pool.name_fd_map[c_login.name()] = client.client_fd;
        // Send online info to friends
        std::vector<int> friends_fd = this->client_pool.GetOnlineFriendFd(client);
        std::vector<std::string> friends_name = this->client_pool.GetOnlineFriendName(client);
        for(int i=0; i<friends_fd.size(); i++){
            CF::S_FriendUpdate s_friend_update = CF::S_FriendUpdate();
            CF::FriendInfo friend_info = CF::FriendInfo();
            friend_info.set_player_name(friends_name[i]);
            friend_info.set_state(PLAYER_ONLINE);
            s_friend_update.set_allocated_friend_info(&friend_info);
            s_friend_update.SerializeToString(&send_string);
            this->SendData(friends_fd[i], send_string.c_str(), send_string.length(), S_FRIEND_UPDATE);
        }
    }
    CF::S_Login s_login = CF::S_Login();
    s_login.set_state(ret);
    s_login.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_LOGIN);
}

/******************************************
 * Function: Process Register
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessRegister(MainClient& client)
{
    std::string send_string;
    CF::C_Register c_register;
    c_register.ParseFromString(client.received_string);
    int ret = Database::Instance().PlayerRegister(c_register.name(), c_register.password_md5());
    CF::S_Register s_register = CF::S_Register();
    s_register.set_state(ret);
    s_register.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_REGISTER);
}

/******************************************
 * Function: Process HallRoom
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessHallRoom(MainClient& client)
{
    std::string send_string;
    CF::C_HallRoom c_hall_room;
    c_hall_room.ParseFromString(client.received_string);
    CF::S_HallRoom s_hall_room;
    std::map<std::string, Room>::iterator it;
    int i;
    for(it=this->lobby.rooms.begin(), i=0; i<c_hall_room.page()*MAX_ROOM_NUMBER_IN_PAGE; i++, it++){
        ;
    }
    i=0;
    while(1){
        if(i == MAX_ROOM_NUMBER_IN_PAGE || (i+c_hall_room.page()*MAX_ROOM_NUMBER_IN_PAGE) == this->lobby.rooms.size()){
            break;
        }
        CF::RoomInfo *room_info = s_hall_room.add_room_info();
        room_info->set_player_number(it->second.player_num);
        room_info->set_room_name(it->second.room_name);
        room_info->set_room_no(it->second.room_no);
        room_info->set_state(it->second.room_state);
        i++;
        it++;
    }
    s_hall_room.set_total_room_num(this->lobby.rooms.size());
    s_hall_room.set_page_room_num(i);
    s_hall_room.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_HALL_ROOM);
}

/******************************************
 * Function: Process Friend
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessFriend(MainClient& client)
{
    std::string send_string;
    // Get Friend Info
    std::vector<std::string> online_friends_name = this->client_pool.GetOnlineFriendName(client);
    std::vector<std::string> friends_name = this->client_pool.GetFriendName(client);
    CF::S_Friend s_friend;
    for(int i=0; i<friends_name.size(); i++){
        CF::FriendInfo *friend_info = s_friend.add_friend_info();
        friend_info->set_player_name(friends_name[i]);
        friend_info->set_state(PLAYER_OFFLINE);
        for(int j=0; j<online_friends_name.size(); j++){
            if(online_friends_name[j] == friends_name[i]){
                friend_info->set_state(PLAYER_ONLINE);
            }        
        }  
    }
    s_friend.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_FRIEND);
    // Send Other Add Friend
    std::vector<std::string> wait_friends_names = Database::Instance().QueryWaitFriendName(client.player.player_id);
    for(int i=0; i<wait_friends_names.size(); i++){
        CF::S_OtherAddFriend s_other_add_friend;
        CF::AddFriendInfo add_friend_info;
        add_friend_info.set_friend_name(wait_friends_names[i]);
        s_other_add_friend.set_allocated_friend_info(&add_friend_info);
        s_friend.SerializeToString(&send_string);
        this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_OTHER_ADD_FRIEND);
    }
}

/******************************************
 * Function: Process EnterRoom
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessEnterRoom(MainClient& client)
{
    std::string send_string;
    CF::C_EnterRoom c_enter_room;
    c_enter_room.ParseFromString(client.received_string);
    int a;
    int seat_no = this->lobby.rooms[c_enter_room.room_no()].EnterRoom(&client);
    if(seat_no != NO_AVAILABLE_SEAT){  // if success, tell others in room
        for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
            if(this->lobby.rooms[c_enter_room.room_no()].clients[i]){
                CF::S_UpdateRoom s_update_room;
                CF::SeatInfo seat_info;
                seat_info.set_empty(SEAT_OCCUPIED);
                seat_info.set_seat_no(seat_no);
                CF::PlayerInfo player_info;
                this->FillPlayerInfo(player_info, client.player);
                seat_info.set_allocated_player_info(&player_info);
                s_update_room.set_allocated_seat_info(&seat_info);
                s_update_room.SerializeToString(&send_string);
                this->SendData(this->lobby.rooms[c_enter_room.room_no()].clients[i]->client_fd, send_string.c_str(), send_string.length(), S_UPDATE_ROOM);
            }
        }
    }
    CF::S_EnterRoom s_enter_room;
    s_enter_room.set_state((seat_no != NO_AVAILABLE_SEAT) ? 1 : 0);
    s_enter_room.set_seat_no(seat_no);
    s_enter_room.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_ENTER_ROOM);
}

/******************************************
 * Function: Process RoomInfo
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessRoomInfo(MainClient& client)
{
    std::string send_string;
    CF::C_RoomInfo c_room_info;
    c_room_info.ParseFromString(client.received_string);
    Room room = this->lobby.rooms[c_room_info.room_no()];
    CF::S_RoomInfo s_room_info;
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        CF::SeatInfo *seat_info = s_room_info.add_seat_info();
        if(!room.clients[i]){
            seat_info->set_empty(SEAT_EMPTY);
        }
        else{
            seat_info->set_empty(SEAT_OCCUPIED);
            seat_info->set_seat_no(i);
            CF::PlayerInfo player_info;
            this->FillPlayerInfo(player_info, room.clients[i]->player);
            seat_info->set_allocated_player_info(&player_info);
        }
    }
    s_room_info.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_ROOM_INFO);
}

/******************************************
 * Function: Process ModChar
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessModChar(MainClient& client)
{
    std::string send_string;
    CF::C_ModChar c_mod_char;
    c_mod_char.ParseFromString(client.received_string);
    // change character type
    client.player.character_type = c_mod_char.char_type();
    // tell others in room
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->lobby.rooms[client.player.room_no].clients[i]){
            CF::S_UpdateRoom s_update_room;
            CF::SeatInfo seat_info;
            seat_info.set_empty(SEAT_OCCUPIED);
            seat_info.set_seat_no(client.player.seat_no);\
            CF::PlayerInfo player_info;
            this->FillPlayerInfo(player_info, client.player);
            seat_info.set_allocated_player_info(&player_info);
            s_update_room.SerializeToString(&send_string);
            this->SendData(this->lobby.rooms[client.player.room_no].clients[i]->client_fd, send_string.c_str(), send_string.length(), S_UPDATE_ROOM);
        }
    }
    CF::S_ModChar s_mod_char;
    s_mod_char.set_state(1);
    s_mod_char.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_MODIFY_CHAR);
}

/******************************************
 * Function: Process Ready
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessReady(MainClient& client)
{
    std::string send_string;
    // change ready state
    client.player.ready = PLAYER_READY;
    // tell others in room
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->lobby.rooms[client.player.room_no].clients[i]){
            CF::S_UpdateRoom s_update_room;
            CF::SeatInfo seat_info;
            seat_info.set_empty(SEAT_OCCUPIED);
            seat_info.set_seat_no(client.player.seat_no);\
            CF::PlayerInfo player_info;
            this->FillPlayerInfo(player_info, client.player);
            seat_info.set_allocated_player_info(&player_info);
            s_update_room.SerializeToString(&send_string);
            this->SendData(this->lobby.rooms[client.player.room_no].clients[i]->client_fd, send_string.c_str(), send_string.length(), S_UPDATE_ROOM);
        }
    }
    CF::S_Ready s_ready;
    s_ready.set_state(1);
    s_ready.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_READY);
}

/******************************************
 * Function: Process CancelReady
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessCancelReady(MainClient& client)
{
    std::string send_string;
    // change ready state
    client.player.ready = PLAYER_NO_READY;
    // tell others in room
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->lobby.rooms[client.player.room_no].clients[i]){
            CF::S_UpdateRoom s_update_room;
            CF::SeatInfo seat_info;
            seat_info.set_empty(SEAT_OCCUPIED);
            seat_info.set_seat_no(client.player.seat_no);\
            CF::PlayerInfo player_info;
            this->FillPlayerInfo(player_info, client.player);
            seat_info.set_allocated_player_info(&player_info);
            s_update_room.SerializeToString(&send_string);
            this->SendData(this->lobby.rooms[client.player.room_no].clients[i]->client_fd, send_string.c_str(), send_string.length(), S_UPDATE_ROOM);
        }
    }
    CF::S_CancelReady s_cancel_ready;
    s_cancel_ready.set_state(1);
    s_cancel_ready.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_CANCEL_READY);
}

/******************************************
 * Function: Process StartGame
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessStartGame(MainClient& client)
{
    std::string send_string;
    // Check all ready
    if(this->lobby.rooms[client.player.room_no].CheckAllReady()){
        // Execute GameServer to process game
                    /* do something */
        // tell all players in room
        CF::S_StartGame s_start_game;
        s_start_game.set_state(GAME_START);
        Room room = this->lobby.rooms[client.player.room_no];
        s_start_game.SerializeToString(&send_string);
        for(int i=0; i<MAX_PLAYER_NUM_IN_ROOM; i++){
            if(room.clients[i]){
                this->SendData(room.clients[i]->client_fd, send_string.c_str(), send_string.length(), S_START_GAME);
            }
        }
    }
    else{
        CF::S_StartGame s_start_game;
        s_start_game.set_state(GAME_NO_START);
        s_start_game.SerializeToString(&send_string);
        this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_START_GAME);
    }

}

/******************************************
 * Function: Process ExitLogin
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessExitLogin(MainClient& client)
{
    std::string send_string;
    // Tell other friends
    std::vector<std::string> online_friends_name = this->client_pool.GetOnlineFriendName(client);
    CF::S_FriendUpdate s_friend_update;
    CF::FriendInfo friend_info;
    for(int i=0; i<online_friends_name.size(); i++){
        friend_info.set_player_name(client.player.player_name);
        friend_info.set_state(PLAYER_OFFLINE);
        s_friend_update.set_allocated_friend_info(&friend_info);
        s_friend_update.SerializeToString(&send_string);
        this->SendData(this->client_pool.name_fd_map[online_friends_name[i]], send_string.c_str(), send_string.length(), S_FRIEND_UPDATE);
    }
    // Set client state
    client.client_state = CLIENT_NO_LOGIN;
    // Reset player object
    client.player = Player();
    CF::S_ExitLogin s_exit_login;
    s_exit_login.set_state(1);
    s_exit_login.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_EXIT_LOGIN);
}

/******************************************
 * Function: Process Quit
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessQuit(MainClient& client)
{
    std::string send_string;
    // if in room
    if(client.player.player_state == Player::IN_ROOM){
        this->ProcessExitRoom(client);
    }
    // if in hall
    if(client.player.player_state == Player::IN_HALL){
        this->ProcessExitLogin(client);
    }
    // delete received data
    delete []client.received_data;
    // delete from name_fd_map
    int fd = client.client_fd;
    this->client_pool.name_fd_map.erase(client.player.player_name);
    this->client_pool.client_pool.erase(client.client_fd);
    CF::S_Quit s_quit;
    s_quit.set_state(1);
    s_quit.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_QUIT);
}

/******************************************
 * Function: Process ExitRoom
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessExitRoom(MainClient& client)
{
    std::string send_string;
    // Tell others
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->lobby.rooms[client.player.room_no].clients[i]){
            if(this->lobby.rooms[client.player.room_no].clients[i] != &client){
                CF::S_UpdateRoom s_update_room;
                CF::SeatInfo seat_info;
                seat_info.set_empty(SEAT_EMPTY);
                seat_info.set_seat_no(client.player.seat_no);
                s_update_room.set_allocated_seat_info(&seat_info);
                s_update_room.SerializeToString(&send_string);
                this->SendData(this->lobby.rooms[client.player.room_no].clients[i]->client_fd, send_string.c_str(), send_string.length(), S_UPDATE_ROOM);
            }   
        }
    }
    // Set room info
    this->lobby.rooms[client.player.room_no].ExitRoom(&client);
    // Set player info
    client.player.ExitRoom();
    CF::S_ExitRoom s_exit_room;
    s_exit_room.set_state(1);
    s_exit_room.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_EXIT_ROOM);
}

/******************************************
 * Function: Process Create Room
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessCreateRoom(MainClient& client)
{
    std::string send_string;
    CF::C_CreateRoom c_create_room;
    c_create_room.ParseFromString(client.received_string);
    this->lobby.CreateRoom(c_create_room.room_name());

    CF::S_CreateRoom s_create_room;
    s_create_room.set_state(1);
    s_create_room.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_CREATE_ROOM);
}

/******************************************
 * Function: Process Add Friend
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessAddFriend(MainClient& client)
{
    std::string send_string;
    CF::C_AddFriend c_add_friend;
    c_add_friend.ParseFromString(client.received_string);

    CF::S_AddFriend s_add_friend;
    // Check username exist
    if(Database::Instance().CheckUserName(c_add_friend.friend_name())){
        // Insert into database
        Database::Instance().InsertWaitFriend(client.player.player_id, Database::Instance().GetUserId(c_add_friend.friend_name()));
        // if online, send
        if(this->client_pool.CheckOnline(c_add_friend.friend_name())){
            CF::S_OtherAddFriend s_other_add_friend;
            CF::AddFriendInfo add_friend_info;
            add_friend_info.set_friend_name(client.player.player_name);
            s_other_add_friend.set_allocated_friend_info(&add_friend_info);
            s_other_add_friend.SerializeToString(&send_string);
            this->SendData(this->client_pool.client_pool[this->client_pool.name_fd_map[c_add_friend.friend_name()]].client_fd, send_string.c_str(), send_string.length(), S_OTHER_ADD_FRIEND);
        }
        s_add_friend.set_state(SEND_FRIEND_OK);
    }
    else{
        s_add_friend.set_state(SEND_FRIEND_FAIL);
    }
    s_add_friend.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_ADD_FRIEND);
}

/******************************************
 * Function: Process Reply Friend
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessReplyFriend(MainClient& client)
{
    std::string send_string;

    CF::C_ReplyFriend c_reply_friend;
    c_reply_friend.ParseFromString(client.received_string);

    CF::S_AddFriend s_add_friend;
    bool friend_online=false;
    if(c_reply_friend.state() == REPLY_FRIEND_OK){
        Database::Instance().InsertFriend(Database::Instance().GetUserId(c_reply_friend.friend_name()), client.player.player_id);
        Database::Instance().DeleteWaitFriend(Database::Instance().GetUserId(c_reply_friend.friend_name()), client.player.player_id);
        // if online, send
        if(this->client_pool.CheckOnline(c_reply_friend.friend_name())){
            // Set flag true
            friend_online = true;
            s_add_friend.set_state(REPLY_FRIEND_OK);
            s_add_friend.SerializeToString(&send_string);
            this->SendData(this->client_pool.client_pool[this->client_pool.name_fd_map[c_reply_friend.friend_name()]].client_fd, send_string.c_str(), send_string.length(), S_ADD_FRIEND);
            CF::S_FriendUpdate s_friend_update;
            CF::FriendInfo friend_info;
            friend_info.set_player_name(client.player.player_name);
            friend_info.set_state(PLAYER_ONLINE);
            s_friend_update.set_allocated_friend_info(&friend_info);
            s_add_friend.SerializeToString(&send_string);
            this->SendData(this->client_pool.client_pool[this->client_pool.name_fd_map[c_reply_friend.friend_name()]].client_fd, send_string.c_str(), send_string.length(), S_FRIEND_UPDATE);
        }
        s_add_friend.set_state(SEND_FRIEND_OK);
    }
    else{
        Database::Instance().DeleteWaitFriend(Database::Instance().GetUserId(c_reply_friend.friend_name()), client.player.player_id);
        // if online, send
        if(this->client_pool.CheckOnline(c_reply_friend.friend_name())){
            s_add_friend.set_state(REPLY_FRIEND_FAIL);
            s_add_friend.SerializeToString(&send_string);
            this->SendData(this->client_pool.client_pool[this->client_pool.name_fd_map[c_reply_friend.friend_name()]].client_fd, send_string.c_str(), send_string.length(), S_ADD_FRIEND);
        }
        s_add_friend.set_state(SEND_FRIEND_OK);
    }
    s_add_friend.SerializeToString(&send_string);
    this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_ADD_FRIEND);
    // Send friend online info
    if(c_reply_friend.state() == REPLY_FRIEND_OK){
        CF::S_FriendUpdate s_friend_update;
        CF::FriendInfo friend_info;
        friend_info.set_player_name(c_reply_friend.friend_name());
        if(friend_online){
            friend_info.set_state(PLAYER_ONLINE);
        }
        else{
            friend_info.set_state(PLAYER_OFFLINE);
        }
        s_friend_update.set_allocated_friend_info(&friend_info);
        s_add_friend.SerializeToString(&send_string);
        this->SendData(client.client_fd, send_string.c_str(), send_string.length(), S_FRIEND_UPDATE);
    }
}

/******************************************
 * Function: Fill struct of PlayerInfo
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::FillPlayerInfo(CF::PlayerInfo& player_info,const Player& player)
{
    player_info.set_player_name(player.player_name);
    player_info.set_char_type(player.character_type);
    player_info.set_ready(player.ready);
    if(this->lobby.rooms[player.room_no].room_owner == player.seat_no){
        player_info.set_owner(PLAYER_READY);
    }
    else{
        player_info.set_owner(PLAYER_NO_READY);
    }
}




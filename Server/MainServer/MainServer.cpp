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
    this->process_function_pool.push_back(&MainServer::CreateRoom);
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
 * data: sent data
 * length: sent data length
 * delete_type: 0->delete, 1->delete[], default 0, 2->no delete
 * Return: None
 *****************************************/
void MainServer::SendData(int fd, char* data, int length, int delete_type)
{
    int len = send(fd, data, length, 0);
    if(delete_type == 0)
        delete data;
    else if(delete_type == 1)
        delete[] data;
}

/******************************************
 * Function: Process Login
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessLogin(MainClient& client)
{
    C_Login *c_login = (C_Login*)client.received_data;
    std::string user_name = c_login->name;
    int ret = Database::Instance().PlayerLogin(user_name, c_login->password_md5);
    if(ret == LOGIN_SUCCESS){
        // Set Client State
        client.client_state = CLIENT_LOGIN;
        // Instantiate Player
        client.InstantiatePlayer(user_name);
        // Add fd and user name map
        this->client_pool.name_fd_map[user_name] = client.client_fd;
        // Send online info to friends
        std::vector<int> friends_fd = this->client_pool.GetOnlineFriendFd(client);
        std::vector<std::string> friends_name = this->client_pool.GetOnlineFriendName(client);
        for(int i=0; i<friends_fd.size(); i++){
            S_FriendUpdate *s_friend_update = new S_FriendUpdate;
            strcpy(s_friend_update->friend_info.player_name, friends_name[i].c_str());
            s_friend_update->friend_info.state = PLAYER_ONLINE;
            this->SendData(friends_fd[i], (char*)s_friend_update, sizeof(S_FriendUpdate));
        }
    }
    S_Login *s_login = new S_Login;
    s_login->state = ret;
    this->SendData(client.client_fd, (char*)s_login, sizeof(S_Login));
}

/******************************************
 * Function: Process Register
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessRegister(MainClient& client)
{
    C_Register *c_register = (C_Register*)client.received_data;
    std::string user_name = c_register->name;
    int ret = Database::Instance().PlayerRegister(user_name, c_register->password_md5);
    S_Register *s_register = new S_Register;
    s_register->state = ret;
    this->SendData(client.client_fd, (char*)s_register, sizeof(S_Register));
}

/******************************************
 * Function: Process HallRoom
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessHallRoom(MainClient& client)
{
    C_HallRoom *c_hall_room = (C_HallRoom*)client.received_data;
    S_HallRoom *s_hall_room = new S_HallRoom;
    std::map<std::string, Room>::iterator it;
    int i;
    for(it=this->lobby.rooms.begin(), i=0; i<c_hall_room->page*MAX_ROOM_NUMBER_IN_PAGE; i++, it++){
        ;
    }
    i=0;
    while(1){
        if(i == MAX_ROOM_NUMBER_IN_PAGE || (i+c_hall_room->page*MAX_ROOM_NUMBER_IN_PAGE) == this->lobby.rooms.size()){
            break;
        }
        s_hall_room->room_info[i].player_number = it->second.player_num;
        strcpy(s_hall_room->room_info[i].room_name, it->second.room_name.c_str());
        strcpy(s_hall_room->room_info[i].room_no, it->second.room_no.c_str());
        s_hall_room->room_info[i].state = it->second.room_state;
        i++;
        it++;
    }
    s_hall_room->total_room_num = this->lobby.rooms.size();
    s_hall_room->page_room_num = i;
    this->SendData(client.client_fd, (char*)s_hall_room, sizeof(S_HallRoom));
}

/******************************************
 * Function: Process Friend
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessFriend(MainClient& client)
{
    // Get Friend Info
    std::vector<std::string> online_friends_name = this->client_pool.GetOnlineFriendName(client);
    std::vector<std::string> friends_name = this->client_pool.GetFriendName(client);
    S_Friend *s_friend = new S_Friend;
    s_friend->friend_info = new FriendInfo[friends_name.size()];
    for(int i=0; i<friends_name.size(); i++){
        strcpy(s_friend->friend_info[i].player_name, friends_name[i].c_str());
        s_friend->friend_info[i].state = PLAYER_OFFLINE;
        for(int j=0; j<online_friends_name.size(); j++){
            if(online_friends_name[j] == friends_name[i]){
                s_friend->friend_info[i].state = PLAYER_ONLINE;
            }        
        }  
    }
    // Build data
    char* new_data = new char[sizeof(S_Friend)+friends_name.size()*sizeof(FriendInfo)];
    memcpy(new_data, s_friend, sizeof(S_Friend));
    memcpy(new_data+sizeof(S_Friend), s_friend->friend_info, friends_name.size()*sizeof(FriendInfo));
    delete[] s_friend->friend_info;
    delete s_friend;
    // Send data and delete in type of delete[]
    this->SendData(client.client_fd, new_data, sizeof(S_Friend)+friends_name.size()*sizeof(FriendInfo), 1);
}

/******************************************
 * Function: Process EnterRoom
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessEnterRoom(MainClient& client)
{
    C_EnterRoom *c_enter_room = (C_EnterRoom*)client.received_data;
    int a;
    int seat_no = this->lobby.rooms[std::string(c_enter_room->room_no)].EnterRoom(&client);
    if(seat_no != NO_AVAILABLE_SEAT){  // if success, tell others in room
        for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
            if(this->lobby.rooms[std::string(c_enter_room->room_no)].clients[i]){
                S_UpdateRoom *s_update_room = new S_UpdateRoom;
                s_update_room->seat_info.empty = SEAT_OCCUPIED;
                s_update_room->seat_info.seat_no = seat_no;
                this->FillPlayerInfo(s_update_room->seat_info.player_info, client.player);
                this->SendData(this->lobby.rooms[std::string(c_enter_room->room_no)].clients[i]->client_fd, (char*)s_update_room, sizeof(S_UpdateRoom));
            }
        }
    }
    S_EnterRoom *s_enter_room = new S_EnterRoom;
    s_enter_room->state = (seat_no != NO_AVAILABLE_SEAT) ? 1 : 0;
    s_enter_room->seat_no = seat_no;
    this->SendData(client.client_fd, (char*)s_enter_room, sizeof(S_EnterRoom));
}

/******************************************
 * Function: Process RoomInfo
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessRoomInfo(MainClient& client)
{
    C_RoomInfo *c_room_info = (C_RoomInfo*)client.received_data;
    Room room = this->lobby.rooms[std::string(c_room_info->room_no)];
    S_RoomInfo *s_room_info = new S_RoomInfo;
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(!room.clients[i]){
            s_room_info->seat_info[i].empty = SEAT_EMPTY;
        }
        else{
            s_room_info->seat_info[i].empty = SEAT_OCCUPIED;
            s_room_info->seat_info[i].seat_no = i;
            this->FillPlayerInfo(s_room_info->seat_info[i].player_info, room.clients[i]->player);
        }
    }
    this->SendData(client.client_fd, (char*)s_room_info, sizeof(S_RoomInfo));
}

/******************************************
 * Function: Process ModChar
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessModChar(MainClient& client)
{
    C_ModChar *c_mod_char = (C_ModChar*)client.received_data;
    // change character type
    client.player.character_type = c_mod_char->char_type;
    // tell others in room
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->lobby.rooms[client.player.room_no].clients[i]){
            S_UpdateRoom *s_update_room = new S_UpdateRoom;
            s_update_room->seat_info.empty = SEAT_OCCUPIED;
            s_update_room->seat_info.seat_no = client.player.seat_no;
            this->FillPlayerInfo(s_update_room->seat_info.player_info, client.player);
            this->SendData(this->lobby.rooms[client.player.room_no].clients[i]->client_fd, (char*)s_update_room, sizeof(S_UpdateRoom));
        }
    }
    S_ModChar *s_mod_char = new S_ModChar;
    s_mod_char->state = 1;
    this->SendData(client.client_fd, (char*)s_mod_char, sizeof(S_ModChar));
}

/******************************************
 * Function: Process Ready
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessReady(MainClient& client)
{
    // change ready state
    client.player.ready = PLAYER_READY;
    // tell others in room
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->lobby.rooms[client.player.room_no].clients[i]){
            S_UpdateRoom *s_update_room = new S_UpdateRoom;
            s_update_room->seat_info.empty = SEAT_OCCUPIED;
            s_update_room->seat_info.seat_no = client.player.seat_no;
            this->FillPlayerInfo(s_update_room->seat_info.player_info, client.player);
            this->SendData(this->lobby.rooms[client.player.room_no].clients[i]->client_fd, (char*)s_update_room, sizeof(S_UpdateRoom));
        }
    }
    S_Ready *s_ready = new S_Ready;
    s_ready->state = 1;
    this->SendData(client.client_fd, (char*)s_ready, sizeof(S_Ready));
}

/******************************************
 * Function: Process CancelReady
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessCancelReady(MainClient& client)
{
    // change ready state
    client.player.ready = PLAYER_NO_READY;
    // tell others in room
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->lobby.rooms[client.player.room_no].clients[i]){
            S_UpdateRoom *s_update_room = new S_UpdateRoom;
            s_update_room->seat_info.empty = SEAT_OCCUPIED;
            s_update_room->seat_info.seat_no = client.player.seat_no;
            this->FillPlayerInfo(s_update_room->seat_info.player_info, client.player);
            this->SendData(this->lobby.rooms[client.player.room_no].clients[i]->client_fd, (char*)s_update_room, sizeof(S_UpdateRoom));
        }
    }
    S_CancelReady *s_cancel_ready = new S_CancelReady;
    s_cancel_ready->state = 1;
    this->SendData(client.client_fd, (char*)s_cancel_ready, sizeof(S_CancelReady));
}

/******************************************
 * Function: Process StartGame
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessStartGame(MainClient& client)
{
    // Check all ready
    if(this->lobby.rooms[client.player.room_no].CheckAllReady()){
        // Execute GameServer to process game
                    /* do something */
        // tell all players in room
        S_StartGame *s_start_game = new S_StartGame;
        s_start_game->state = GAME_START;
        Room room = this->lobby.rooms[client.player.room_no];
        for(int i=0; i<MAX_PLAYER_NUM_IN_ROOM; i++){
            if(room.clients[i]){
                this->SendData(room.clients[i]->client_fd, (char*)s_start_game, sizeof(S_StartGame), 2);
            }
        }
        delete s_start_game;
    }
    else{
        S_StartGame *s_start_game = new S_StartGame;
        s_start_game->state = GAME_NO_START;
        this->SendData(client.client_fd, (char*)s_start_game, sizeof(S_StartGame));
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
    // Tell other friends
    std::vector<std::string> online_friends_name = this->client_pool.GetOnlineFriendName(client);
    S_FriendUpdate *s_friend_update = new S_FriendUpdate;
    for(int i=0; i<online_friends_name.size(); i++){
        strcpy(s_friend_update->friend_info.player_name, client.player.player_name.c_str());
        s_friend_update->friend_info.state = PLAYER_OFFLINE;
        this->SendData(this->client_pool.name_fd_map[online_friends_name[i]], (char*)s_friend_update, sizeof(S_FriendUpdate), 2);
    }
    delete s_friend_update;
    // Set client state
    client.client_state = CLIENT_NO_LOGIN;
    // Reset player object
    client.player = Player();
    S_ExitLogin *s_exit_login = new S_ExitLogin;
    s_exit_login->state = 1;
    this->SendData(client.client_fd, (char*)s_exit_login, sizeof(S_ExitLogin));
}

/******************************************
 * Function: Process Quit
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessQuit(MainClient& client)
{
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
    S_Quit *s_quit = new S_Quit;
    s_quit->state = 1;
    this->SendData(fd, (char*)s_quit, sizeof(S_Quit));
}

/******************************************
 * Function: Process ExitRoom
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessExitRoom(MainClient& client)
{
    // Tell others
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->lobby.rooms[client.player.room_no].clients[i]){
            if(this->lobby.rooms[client.player.room_no].clients[i] != &client){
                S_UpdateRoom *s_update_room = new S_UpdateRoom;
                s_update_room->seat_info.empty = SEAT_EMPTY;
                s_update_room->seat_info.seat_no = client.player.seat_no;
                this->SendData(this->lobby.rooms[client.player.room_no].clients[i]->client_fd, (char*)s_update_room, sizeof(S_UpdateRoom));
            }   
        }
    }
    // Set room info
    this->lobby.rooms[client.player.room_no].ExitRoom(&client);
    // Set player info
    client.player.ExitRoom();
    S_ExitRoom *s_exit_room = new S_ExitRoom;
    s_exit_room->state = 1;
    this->SendData(client.client_fd, (char*)s_exit_room, sizeof(S_ExitRoom));
}

/******************************************
 * Function: Process Create Room
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessCreateRoom(MainClient& client)
{
    C_CreateRoom *c_create_room = (C_CreateRoom*)client.received_data;
    this->lobby.CreateRoom(std::string(c_create_room->room_name));
    S_CreateRoom *s_create_room = new S_CreateRoom;
    s_create_room->state = 1;
    this->SendData(client.client_fd, (char*)s_create_room, sizeof(S_CreateRoom));
}

/******************************************
 * Function: Process Add Friend
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::ProcessAddFriend(MainClient& client)
{
    C_AddFriend *c_add_friend = (C_AddFriend*)client.received_data;
    S_AddFriend *s_add_friend = new S_AddFriend;
    // Check username exist
    if(Database::Instance().CheckUserName(c_add_friend->friend_name)){
        // Insert into database
        Database::Instance().InsertWaitFriend(client.player.player_id, Database::Instance().GetUserId(c_add_friend->friend_name));
        // if online, send
        if(this->client_pool.CheckOnline(c_add_friend->friend_name)){
            S_OtherAddFriend *s_other_add_friend = new S_OtherAddFriend;
            strcpy(s_other_add_friend->friend_info.friend_name, client.player.player_name.c_str());
            this->SendData(this->client_pool.client_pool[this->client_pool.name_fd_map[c_add_friend->friend_name]].client_fd, (char*)s_other_add_friend, sizeof(S_OtherAddFriend));
        }
        s_add_friend->state = 1;
    }
    else{
        s_add_friend->state = 0;
    }
    this->SendData(client.client_fd, (char*)s_add_friend, sizeof(S_AddFriend));
}

/******************************************
 * Function: Fill struct of PlayerInfo
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
void MainServer::FillPlayerInfo(PlayerInfo& player_info,const Player& player)
{
    strcpy(player_info.player_name, player.player_name.c_str());
    player_info.char_type = player.character_type;
    player_info.ready = player.ready;
    if(this->lobby.rooms[player.room_no].room_owner == player.seat_no){
        player_info.owner = PLAYER_READY;
    }
    else{
        player_info.owner = PLAYER_NO_READY;
    }
}


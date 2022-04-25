#include "ClientPool.h"

/******************************************
 * Function: Initialize ClientPool
 * Parameters: 0
 * Return: None
 *****************************************/
ClientPool::ClientPool(){

}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
ClientPool::~ClientPool(){

}

/******************************************
 * Function: Add client to client pool
 * Parameters: 0
 * Return: None
 *****************************************/
int ClientPool::AddClient(int fd, sockaddr_in socket_addr){
    // Instantiate a MainClient object
    MainClient main_client = MainClient(fd, socket_addr);
    // Set client no
    main_client.SetClientNo(this->client_pool.size());
    // Add client to the pool
    this->client_pool[fd] = main_client;
    return 0;
}

/******************************************
 * Function: Get client object
 * Parameters: 0
 * Return: None
 *****************************************/
MainClient& ClientPool::GetClient(int fd){
    return this->client_pool[fd];
}

/******************************************
 * Function: Get Online Friend Fd
 * Parameters: 1
 * client: client
 * Return: None
 *****************************************/
std::vector<int> ClientPool::GetOnlineFriendFd(MainClient& client)
{
    std::vector<std::string> friend_names = Database::Instance().QueryFriendName(client.player.player_id);
    std::vector<int> friend_fds;
    for(int i=0; i<friend_names.size(); i++){
        if(this->name_fd_map.count(friend_names[i])){
            friend_fds.push_back(this->name_fd_map[friend_names[i]]);
        }
    }
    return friend_fds;
}


/******************************************
 * Function: Get Online Friend Name
 * Parameters: 0
 * client: client
 * Return: None
 *****************************************/
std::vector<std::string> ClientPool::GetOnlineFriendName(MainClient& client)
{
    std::vector<std::string> friend_names = Database::Instance().QueryFriendName(client.player.player_id);
    std::vector<std::string> online_friend_names;
    for(int i=0; i<friend_names.size(); i++){
        if(this->name_fd_map.count(friend_names[i])){
            online_friend_names.push_back(friend_names[i]);
        }
    }
    return online_friend_names;
}

/******************************************
 * Function: Get All Friend Name
 * Parameters: 0
 * client: client
 * Return: None
 *****************************************/
std::vector<std::string> ClientPool::GetFriendName(MainClient& client)
{
    return Database::Instance().QueryFriendName(client.player.player_id);;
}

/******************************************
 * Function: Check Online
 * Parameters: 0
 * client: client
 * Return: None
 *****************************************/
int ClientPool::CheckOnline(std::string user_name)
{
    return this->name_fd_map.count(user_name);
}
    

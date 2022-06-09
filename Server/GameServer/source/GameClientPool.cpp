#include "GameClientPool.h"

/******************************************
 * Function: Initialize GameClientPool
 * Parameters: 0
 * Return: None
 *****************************************/
GameClientPool::GameClientPool(){

}

/******************************************
 * Function: Initialize GameClientPool
 * Parameters: 0
 * Return: None
 *****************************************/
GameClientPool::GameClientPool(int _total_player_number){
    this->total_player_number = _total_player_number;
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
GameClientPool::~GameClientPool(){

}

/******************************************
 * Function: Add client to client pool
 * Parameters: 0
 * Return: None
 *****************************************/
int GameClientPool::AddClient(int fd, sockaddr_in socket_addr){
    // Instantiate a MainClient object
    GameClient *game_client = new GameClient(fd, socket_addr);
    // Set client no
    game_client->SetClientNo(this->clients.size());
    // Add client to clients
    this->clients.push_back(game_client);
    // Add client to the pool
    this->client_pool[fd] = game_client;
    return 0;
}

/******************************************
 * Function: Get client object
 * Parameters: 0
 * Return: None
 *****************************************/
GameClient* GameClientPool::GetClient(int fd){
    return this->client_pool[fd];
}

/******************************************
 * Function: Check all player loaded
 * Parameters: 0
 * Return: None
 *****************************************/
bool GameClientPool::CheckAllLoad()
{
    if(clients.size() != this->total_player_number){  // All players should connect
        return false;
    }
    for(int i=0; i<clients.size(); i++){
        if(!clients[i]->player.load){
            return false;
        }
    }
    return true;
}

/******************************************
 * Function: Check all player start
 * Parameters: 0
 * Return: None
 *****************************************/
bool GameClientPool::CheckAllStart()
{
    if(clients.size() != this->total_player_number){  // All players should connect
        return false;
    }
    for(int i=0; i<clients.size(); i++){
        if(!clients[i]->player.begin){
            return false;
        }
    }
    return true;
}

/******************************************
 * Function: Check all player end
 * Parameters: 0
 * Return: None
 *****************************************/
bool GameClientPool::CheckAllEnd()
{
    if(clients.size() != this->total_player_number){  // All players should connect
        return false;
    }
    for(int i=0; i<clients.size(); i++){
        if(!clients[i]->player.end){
            return false;
        }
    }
    return true;
}

/******************************************
 * Function: Check all player quit
 * Parameters: 0
 * Return: None
 *****************************************/
bool GameClientPool::CheckAllQuit()
{
    if(clients.size() != this->total_player_number){  // All players should connect
        return false;
    }
    for(int i=0; i<clients.size(); i++){
        if(!clients[i]->player.quit){
            return false;
        }
    }
    return true;
}


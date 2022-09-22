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
 * Return: the new client point
 *****************************************/
GameClient* GameClientPool::AddClient(sockaddr_in socket_addr){
    // Instantiate a MainClient object
    GameClient *game_client = new GameClient(socket_addr);
    // Set client no
    game_client->no = this->clients.size();
    // Add client to clients
    this->clients.push_back(game_client);
    return game_client;
}

/******************************************
 * Function: Get client object
 * Parameters: 0
 * Return: None
 *****************************************/
GameClient* GameClientPool::GetClient(sockaddr_in socket_addr){
    bool flag = true;
    for(int i=0; i<clients.size(); i++){
        if(this->ClientEqual(clients[i]->socket_addr, socket_addr)){
            flag = false;
            return clients[i];
        }
    }
    // If there is no, create a new client and return
    GameClient* newClient;
    if(flag){
        newClient = this->AddClient(socket_addr);
    }
    return newClient;
}

/******************************************
 * Function: Check whether the two clients are the same
 * Parameters: 0
 * Return: None
 *****************************************/
bool GameClientPool::ClientEqual(const sockaddr_in &socket_addr_1,const sockaddr_in &socket_addr_2){
    if(!strcmp(inet_ntoa(socket_addr_1.sin_addr), inet_ntoa(socket_addr_2.sin_addr))){
        if(socket_addr_1.sin_port == socket_addr_2.sin_port){
            return true;
        }
    }
    return false;
}

/******************************************
 * Function: Check all player connected
 * Parameters: 0
 * Return: None
 *****************************************/
bool GameClientPool::CheckAllConnect()
{
    if(clients.size() != this->total_player_number){  // All players should connect
        return false;
    }
    for(int i=0; i<clients.size(); i++){
        if(!clients[i]->player.connected){
            return false;
        }
    }
    return true;
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
        if(clients[i]->player.player_state != GamePlayer::EXIT){
            return false;
        }
    }
    return true;
}

/******************************************
 * Function: Check all player game over
 * Parameters: 0
 * Return: None
 *****************************************/
bool GameClientPool::CheckAllGameOver()
{
    if(clients.size() != this->total_player_number){  // All players should connect
        return false;
    }
    for(int i=0; i<clients.size(); i++){
        if(!clients[i]->player.game_over){
            return false;
        }
    }
    return true;
}


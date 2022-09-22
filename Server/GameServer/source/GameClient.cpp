#include "GameClient.h"

/******************************************
 * Function: Initialize MainClient
 * Parameters: 0
 * Return: None
 *****************************************/
GameClient::GameClient(){
    
}

/******************************************
 * Function: Initialize MainClient
 * Parameters: 0
 * Return: None
 *****************************************/
GameClient::GameClient(sockaddr_in _socket_addr){
    this->socket_addr = _socket_addr;
    // Init Socket Receiver
    this->player = GamePlayer();
    this->closed = false;
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
GameClient::~GameClient(){

}


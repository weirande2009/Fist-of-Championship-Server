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
GameClient::GameClient(int fd, sockaddr_in socket_addr){
    // Set basic info
    this->client_fd = fd;
    this->client_port = socket_addr.sin_port;
    this->client_ip = inet_ntoa(socket_addr.sin_addr);
    this->received_data_size = 1024;
    this->received_data = new char[this->received_data_size];
    this->read_length = PACKAGE_HEAD_LENGTH;
    this->received_length = 0;
    this->rd_head = true;
    this->player = GamePlayer();
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
GameClient::~GameClient(){

}

/******************************************
 * Function: Set number in ClientPool
 * Parameters: 1
 * no: client no in ClientPool
 * Return: None
 *****************************************/
void GameClient::SetClientNo(int no){
    this->client_no = no;
}

/******************************************
 * Function: Get number in ClientPool
 * Parameters: 0
 * Return: Client no
 *****************************************/
int GameClient::GetClientNo(){
    return this->client_no;
}


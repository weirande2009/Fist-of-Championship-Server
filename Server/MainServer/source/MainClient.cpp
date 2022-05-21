#include "MainClient.h"

/******************************************
 * Function: Initialize MainClient
 * Parameters: 0
 * Return: None
 *****************************************/
MainClient::MainClient(){
    
}

/******************************************
 * Function: Initialize MainClient
 * Parameters: 0
 * Return: None
 *****************************************/
MainClient::MainClient(int fd, sockaddr_in socket_addr){
    // Set basic info
    this->client_fd = fd;
    this->client_port = socket_addr.sin_port;
    this->client_ip = inet_ntoa(socket_addr.sin_addr);
    this->received_data_size = 1024;
    this->received_data = new char[this->received_data_size];
    this->read_length = PACKAGE_HEAD_LENGTH;
    this->received_length = 0;
    this->rd_head = true;
    this->client_state = CLIENT_NO_LOGIN;
    this->player = Player();
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
MainClient::~MainClient(){

}

/******************************************
 * Function: Set number in ClientPool
 * Parameters: 1
 * no: client no in ClientPool
 * Return: None
 *****************************************/
void MainClient::SetClientNo(int no){
    this->client_no = no;
}

/******************************************
 * Function: Get number in ClientPool
 * Parameters: 0
 * Return: Client no
 *****************************************/
int MainClient::GetClientNo(){
    return this->client_no;
}

/******************************************
 * Function: Instantiate Player
 * Parameters: 1
 * user_name: user name
 * Return: None
 *****************************************/
void MainClient::InstantiatePlayer(std::string user_name){
    this->player = Player();
    player.player_name = user_name;
    player.player_id = Database::Instance().GetUserId(user_name);
    player.player_state = Player::IN_HALL;
    player.room_no = "";
    player.character_type = 0;
    player.ready = false;
}

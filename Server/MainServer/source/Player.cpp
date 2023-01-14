#include "Player.h"

/******************************************
 * Function: Initialize Player
 * Parameters: 0
 * Return: None
 *****************************************/
Player::Player(){
    this->ResetPlayerInfo();
    this->ResetRoomInfo();
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
Player::~Player(){
    
}

/******************************************
 * Function: Clear player info
 * Parameters: 0
 * Return: None
 *****************************************/
void Player::ResetPlayerInfo(){
    this->player_name = "";
    this->player_id = "";
    this->player_state = NOT_LOGIN;
}


/******************************************
 * Function: Clear player info in room
 * Parameters: 0
 * Return: None
 *****************************************/
void Player::ResetRoomInfo(){
    this->room_no = "";
    this->ready = 0;
    this->character_type = 0;
    this->seat_no = -1;
}

/******************************************
 * Function: Reset player info
 * Parameters: 0
 * Return: None
 *****************************************/
void Player::ExitRoom(){
    this->ResetRoomInfo();
    this->player_state = this->IN_HALL;
}




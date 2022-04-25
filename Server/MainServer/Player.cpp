#include "Player.h"

/******************************************
 * Function: Initialize Player
 * Parameters: 0
 * Return: None
 *****************************************/
Player::Player(){
    
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
Player::~Player(){
    
}

/******************************************
 * Function: Reset player info
 * Parameters: 0
 * Return: None
 *****************************************/
void Player::ExitRoom(){
    this->room_no = "";
    this->ready = 0;
    this->player_state = this->IN_HALL;
    this->character_type = -1;
    this->seat_no = -1;
}




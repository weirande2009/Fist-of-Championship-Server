#include "GamePlayer.h"

/******************************************
 * Function: Initialize Player
 * Parameters: 0
 * Return: None
 *****************************************/
GamePlayer::GamePlayer(){
    this->ResetPlayerInfo();
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
GamePlayer::~GamePlayer(){
    
}

/******************************************
 * Function: Reset player info
 * Parameters: 0
 * Return: None
 *****************************************/
void GamePlayer::ResetPlayerInfo(){
    this->player_name = "";
    this->player_id = "";
    this->sync_frame_id = -1;
    this->character_type = 0;
    this->seat_no = 0;
    this->player_state = EXIT;
    this->connected = false;
    this->load = false;
    this->game_over = false;
}


/******************************************
 * Function: Player exit room
 * Parameters: 0
 * Return: None
 *****************************************/
void GamePlayer::ExitGame(){
    this->ResetPlayerInfo();
}




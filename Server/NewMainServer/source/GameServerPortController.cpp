#include "GameServerPortController.h"

/******************************************
 * Function: Initialize Lobby
 * Parameters: 0
 * Return: None
 *****************************************/
GameServerPortController::GameServerPortController(){
    for(int i=0; i<GAME_SERVER_PORT_NUM; i++){
        port_pool[i] = GAME_SERVER_PORT_DOWN + i;
        port_used[i] = false;
    }
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
GameServerPortController::~GameServerPortController(){
    
}

/******************************************
 * Function: Get a new port
 * Parameters: 0
 * Return: RoomNo
 *****************************************/
int GameServerPortController::GetNewPort(){
    // Get a new port
    for(int i=0; i<GAME_SERVER_PORT_NUM; i++){
        if(!port_used[i]){
            port_used[i] = true;
            return port_pool[i];
        }
    }
    return -1;
}

/******************************************
 * Function: Release Port
 * Parameters: 0
 * Return: None
 *****************************************/
void GameServerPortController::ReleasePort(int port){
    port_used[port-GAME_SERVER_PORT_DOWN] = false;
}





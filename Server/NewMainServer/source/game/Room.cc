#include "game/Room.h"

/******************************************
 * Function: Initialize Room
 * Parameters: 0
 * Return: None
 *****************************************/
Room::Room(){
    
}


/******************************************
 * Function: Initialize Room
 * Parameters: 0
 * Return: None
 *****************************************/
Room::Room(int room_no){
    this->room_no = room_no;
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
Room::~Room(){
    
}

/******************************************
 * Function: Enter room
 * Parameters: 0
 * Return: index of seat no
 *****************************************/
int Room::EnterRoom(MainClient* client){
    clients.push_back(client);
}

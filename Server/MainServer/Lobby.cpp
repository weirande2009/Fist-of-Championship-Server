#include "Lobby.h"

/******************************************
 * Function: Initialize Lobby
 * Parameters: 0
 * Return: None
 *****************************************/
Lobby::Lobby(){
    
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
Lobby::~Lobby(){
    
}

/******************************************
 * Function: Create Room
 * Parameters: 0
 * Return: None
 *****************************************/
void Lobby::CreateRoom(std::string room_name){
    // Set room no
    int room_num = this->rooms.size();
    std::string room_no = std::to_string(room_num);
    Room room = Room(room_no, room_name);
    rooms.insert(std::pair<std::string, Room>(room_no, room));
}

/******************************************
 * Function: Remove Room
 * Parameters: 0
 * Return: None
 *****************************************/
void Lobby::RemoveRoom(std::string room_no){
    // Set room no
    rooms.erase(room_no);
}





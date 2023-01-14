#include "Lobby.h"

/******************************************
 * Function: Initialize Lobby
 * Parameters: 0
 * Return: None
 *****************************************/
Lobby::Lobby(){
    this->CreateRoom("Test Room1");
    this->CreateRoom("Test Room2");
    this->CreateRoom("Test Room3");
    this->CreateRoom("Test Room4");
    this->CreateRoom("Test Room5");
    this->CreateRoom("Test Room6");
    this->CreateRoom("Test Room7");
    this->CreateRoom("Test Room8");
    this->CreateRoom("Test Room9");
    this->CreateRoom("Test Room10");
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
 * Return: RoomNo
 *****************************************/
std::string Lobby::CreateRoom(std::string room_name){
    // Set room no
    int room_num = this->rooms.size();
    std::string room_no = std::to_string(room_num);
    Room room = Room(room_no, room_name);
    rooms.insert(std::pair<std::string, Room>(room_no, room));
    return room_no;
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





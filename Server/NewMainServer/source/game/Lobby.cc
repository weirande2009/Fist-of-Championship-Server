#include "game/Lobby.h"

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
 * Return: RoomNo
 *****************************************/
int Lobby::CreateRoom(){
    // Set room no
    int room_no = -1;
    for(int i=0; i<MAX_ROOM_NUM; i++){
        if(rooms.find(i) == rooms.end()){
            room_no = i;
            break;
        }
    }
    if(room_no != -1){
        rooms[room_no] = Room(room_no);
    }
    return room_no;
}

/******************************************
 * Function: Remove Room
 * Parameters: 0
 * Return: None
 *****************************************/
int Lobby::RemoveRoom(int room_no){
    // Set room no
    if(rooms.find(room_no) != rooms.end()){
        rooms.erase(room_no);
        return 0;
    }
    else{
        return -1;
    }
}

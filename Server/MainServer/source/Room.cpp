#include "Room.h"

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
Room::Room(std::string _room_no, std::string _room_name){
    this->room_no = _room_no;
    this->room_name = _room_name;
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        this->clients[i] = NULL;
    }
    room_owner = -1;
    room_state = ROOM_NOT_IN_GAME;
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
    int pos = this->FindAvailableSeat();
    if(pos != NO_AVAILABLE_SEAT){
        if(pos == 0 && this->CheckEmpty()){
            this->room_owner = pos;
        }
        // add client
        this->clients[pos] = client;
        player_num++;
        // Set player info
        client->player.player_state = Player::IN_ROOM;
        client->player.room_no = this->room_no;
        client->player.seat_no = pos;
        return pos;
    }
    return NO_AVAILABLE_SEAT;
}

/******************************************
 * Function: Exit room
 * Parameters: 0
 * Return: state of exit room
 *****************************************/
int Room::ExitRoom(MainClient* client){
    int index = this->Find(client);
    if(index != -1){
        this->clients[index] = NULL;
        player_num--;
        // Unset player info
        client->player.ExitRoom();
        if(room_owner == index){  // if owner leave, owner will change
            int new_owner = this->FindFirstOccupiedSeat();
            if(new_owner != ROOM_EMPTY){  // if still player in room
                // Set first player as owner
                this->room_owner = new_owner;
            }
            else{
                // noone in the room
                return ROOM_EMPTY;
            }
        }
        return EXIT_ROOM_OK;
    }
    return EXIT_ROOM_FAIL;
}


/******************************************
 * Function: Check whether all players in the room get ready
 * Parameters: 0
 * Return: bool
 *****************************************/
bool Room::CheckAllReady(){
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(!this->clients[i]->player.ready){
            return false;
        }
    }
    return true;
}

/******************************************
 * Function: Check whether room is full
 * Parameters: 0
 * Return: bool
 *****************************************/
bool Room::CheckFull(){
    if(player_num == MAX_PLAYER_NUM_IN_ROOM){
        return true;
    }
    return false;
}

/******************************************
 * Function: Check whether room is empty
 * Parameters: 0
 * Return: bool
 *****************************************/
bool Room::CheckEmpty(){
    if(player_num == 0){
        return true;
    }
    return false;
}

/******************************************
 * Function: Find the position of a player
 * Parameters: 0
 * Return: index or -1->not in
 *****************************************/
int Room::Find(MainClient* client){
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(!(this->clients[i] == client)){
            return i;
        }
    }
    return -1;
}

/******************************************
 * Function: Find the first available seat
 * Parameters: 0
 * Return: seat index
 *****************************************/
int Room::FindAvailableSeat(){
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->clients[i] == NULL){
            return i;
        }
    }
    return NO_AVAILABLE_SEAT;
}

/******************************************
 * Function: Find the first available seat
 * Parameters: 0
 * Return: seat index
 *****************************************/
int Room::FindFirstOccupiedSeat(){
    for(int i=0; i<SEAT_NUM_IN_ROOM; i++){
        if(this->clients[i] != NULL){
            return i;
        }
    }
    return ROOM_EMPTY;
}


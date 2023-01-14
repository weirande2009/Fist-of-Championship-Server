#ifndef ROOM_H
#define ROOM_H
#include "common.h"
#include "MainClient.h"
#include "Protocols.h"

class Room
{
// Variables
private:
    

public:
    MainClient* clients[SEAT_NUM_IN_ROOM];
    int player_num;
    std::string room_no;        // room no in lobby
    std::string room_name;      // room name
    int room_owner;             // room owner, the index in the room
    int room_state;             // room state, in game or not

// Functions
private:

public:
    Room();
    Room(std::string _room_no, std::string _room_name);
    ~Room();
    int EnterRoom(MainClient* client);
    int ExitRoom(MainClient* client);
    bool CheckAllReady();
    bool CheckFull();
    bool CheckEmpty();
    int Find(MainClient* client);
    int FindAvailableSeat();
    int FindFirstOccupiedSeat();            

};




#endif










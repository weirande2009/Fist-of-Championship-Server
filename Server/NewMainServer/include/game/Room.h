#ifndef ROOM_H
#define ROOM_H

#include "utilities/common.h"
#include "utilities/Protocols.h"
#include "server/MainClient.h"

class Room
{
// Variables
private:
    

public:
    std::vector<MainClient*> clients;
    int player_num;
    int room_no;        // room no

// Functions
private:

public:
    Room();
    Room(int room_no);
    ~Room();
    int EnterRoom(MainClient* client);     

};




#endif










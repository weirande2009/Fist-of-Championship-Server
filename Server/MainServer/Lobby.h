#ifndef LOBBY_H
#define LOBBY_H
#include "../common/common.h"
#include "Protocols.h"
#include "Room.h"

class Lobby
{
// Variables
private:
    
    
public:
    std::map<std::string, Room> rooms;
    

// Functions
private:

public:
    Lobby();
    ~Lobby();
    void CreateRoom(std::string room_name);
    void RemoveRoom(std::string room_no);

};




#endif

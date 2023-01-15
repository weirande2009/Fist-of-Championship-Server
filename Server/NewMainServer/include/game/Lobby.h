#pragma once

#include "utilities/common.h"
#include "utilities/Protocols.h"
#include "game/Room.h"

class Lobby
{
// Variables
private:
    static const int MAX_ROOM_NUM = 1000;
    
public:
    std::unordered_map<int, Room> rooms;

// Functions
private:

public:
    Lobby();
    ~Lobby();
    int CreateRoom();
    int RemoveRoom(int room_no);

};

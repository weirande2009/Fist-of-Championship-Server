#ifndef TABLES_h
#define TABLES_h
#include "utilities/common.h"
struct UserTable{
    std::string table = "Users";
    std::string userid = "userid";
    std::string username = "username";
    std::string passwd = "passwd";
};

struct FriendsTable{
    std::string table = "Friends";
    std::string user1_id = "user1_id";
    std::string user2_id = "user2_id";
};

struct WaitFriendsTable{
    std::string table = "WaitFriends";
    std::string user1_id = "user1_id";
    std::string user2_id = "user2_id";
};

#endif
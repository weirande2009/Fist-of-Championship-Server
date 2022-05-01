#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "ClientToServer.pb.h"
#include "ServerToClient.pb.h"
#include <mysql/mysql.h>
using namespace std;

int main()
{
    int a = ChampionFist::C_ADD_FRIEND;
    ChampionFist::C_AddFriend c_add_friend;
    c_add_friend.set_friend_name("abcd");
    cout << a << endl;
    cout << c_add_friend.friend_name() << endl;
    return 0;
} 
#ifndef DATABASE_H
#define DATABASE_H
#include <mysql/mysql.h>
#include "common.h"
#include "Protocols.h"
#include "Tables.h"

class Database
{
// Variables
private:    
    static Database instance;       // single instance
    struct UserTable user_table;
    struct FriendsTable friends_table;
    struct WaitFriendsTable wait_friends_table;

    MYSQL *connection;                 // Mysql object
    MYSQL_RES *result;
	MYSQL_ROW row;
    
    std::string user;           // user id
    int port;                   // database port
    std::string password;       // database password
    std::string database_name;  // database name
    std::string host_name;      // host name
    std::string config_file_name = "../config/database.cfg";
    

public:
    
public:
    static Database& Instance();  // get instance of Database

// Functions
private:
    void ReadConfig();
    void ConnectMysql();

public:
    Database();
    ~Database();

    /* Basic Database Functions */
    int ExecuteSql(std::string sql);
    std::string GenerateNewUserId();

    /* General Functions */
    std::string GetUserId(std::string user_name);
    std::string GetUserName(std::string user_id);
    int CheckUserName(std::string user_name);
    
    /* functions for login */
    int PlayerLogin(std::string user_name, std::string password_md5);
    int PlayerRegister(std::string user_name, std::string password_md5);

    /* functions for friends */
    std::vector<std::string> QueryFriendName(std::string user_id);
    int InsertFriend(std::string user1_id, std::string user2_id);
    std::vector<std::string> QueryWaitFriendName(std::string user_id);
    int InsertWaitFriend(std::string user1_id, std::string user2_id);
    int DeleteWaitFriend(std::string user1_id, std::string user2_id);
};


#endif

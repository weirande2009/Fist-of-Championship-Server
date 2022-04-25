#ifndef DATABASE_H
#define DATABASE_H
#include <mysql/mysql.h>
#include "../common/common.h"
#include "Protocols.h"

class Database
{
// Variables
private:
    static Database instance;     // single instance

public:
    
    std::string user;           // user id
    int port;                   // database port
    std::string password;       // database password
    std::string database_name;  // database name
    std::string host_name;      // host name
    std::string config_file_name = "../Server/config/database.cfg";
    MYSQL *connection;                 // Mysql object
    MYSQL_RES *result;
	MYSQL_ROW row;
    
public:
    static Database& Instance();  // get instance of Database

// Functions
private:
    void ReadConfig();
    void ConnectMysql();

public:
    Database();
    ~Database();

    /* General Functions */
    std::string GetUserId(std::string user_name);
    int CheckUserName(std::string user_name);
    
    /* functions for login */
    int PlayerLogin(std::string user_name, char password_md5[LENGTH_MD5]);
    int PlayerRegister(std::string user_name, char password_md5[LENGTH_MD5]);

    /* functions for friends */
    std::vector<std::string> QueryFriendName(std::string user_id);
    int InsertFriend(std::string user1_id, std::string user2_id);
    std::vector<std::string> QueryWaitFriendName(std::string user_id);
    int InsertWaitFriend(std::string user1_id, std::string user2_id);

};


#endif

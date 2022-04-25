#include "Database.h"

Database Database::instance;

/******************************************
 * Function: Initialize Database
 * Parameters: 0
 * Return: None
 *****************************************/
Database::Database(){
    // read config
    this->ReadConfig();
    // connect database
    this->ConnectMysql();
}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
Database::~Database(){
    // Initial user and password

}

/******************************************
 * Function: Read config file
 * Parameters: 0
 * Return: None
 *****************************************/
void Database::ReadConfig(){
    std::ifstream config_file_name(this->config_file_name, std::ios::in);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    // read database host name
    std::string host_line;
    config_file_name >> host_line;
    int colon_pos = host_line.find(":");
    this->host_name = host_line.substr(colon_pos+1, host_line.size()-colon_pos-1);
    // read database port
    std::string port_line;
    config_file_name >> port_line;
    colon_pos = port_line.find(":");
    this->port = atoi(port_line.substr(colon_pos+1, port_line.size()-colon_pos-1).c_str());
    // read database user name
    std::string user_line;
    config_file_name >> user_line;
    colon_pos = user_line.find(":");
    this->user = user_line.substr(colon_pos+1, user_line.size()-colon_pos-1);
    // read database password
    std::string passwd_line;
    config_file_name >> passwd_line;
    colon_pos = passwd_line.find(":");
    this->password = passwd_line.substr(colon_pos+1, passwd_line.size()-colon_pos-1);
    // read database name
    std::string dbname_line;
    config_file_name >> dbname_line;
    colon_pos = dbname_line.find(":");
    this->database_name = dbname_line.substr(colon_pos+1, dbname_line.size()-colon_pos-1);

    std::cout << this->host_name << std::endl;
    std::cout << this->user << std::endl;
    std::cout << this->password << std::endl;
    std::cout << this->database_name << std::endl;
}

/******************************************
 * Function: Connect Mysql
 * Parameters: 0
 * Return: None
 *****************************************/
void Database::ConnectMysql(){
    // create mysql object
    this->connection = nullptr;
    this->connection = mysql_init(this->connection);
    if (!this->connection) {
        std::cout << "MySql init error!" << std::endl;
    }
    // connect mysql
    this->connection = mysql_real_connect(this->connection, this->host_name.c_str(), this->user.c_str(), this->password.c_str(), this->database_name.c_str(), this->port, nullptr, 0);
    if (!this->connection) {
        std::cout << "MySql Connect error!" << std::endl;
    }

    if(mysql_query(this->connection, "select * from Users"))
	{
		std::cout << "Query Error:" << mysql_error(connection) << std::endl;;
		exit(1);
	}
	else
	{
		this->result = mysql_use_result(connection); // 获取结果集
		// mysql_field_count()返回connection查询的列数
		for(int i=0; i < mysql_field_count(connection); ++i)
		{
			// 获取下一行
			this->row = mysql_fetch_row(this->result);
			if(this->row <= 0)
			{
				break;
			}
			// mysql_num_fields()返回结果集中的字段数
			for(int j=0; j < mysql_num_fields(this->result); ++j)
			{
				std::cout << this->row[j] << " ";
			}
			std::cout << std::endl;
		}
		// 释放结果集的内存
		mysql_free_result(this->result);
	}
}

/******************************************
 * Function: Connect Mysql
 * Parameters: 0
 * Return: The single instance of Database
 *****************************************/
Database& Database::Instance()
{
    return Database::instance;
}

/******************************************
 * Function: Check player login
 * Parameters: 2
 * user_name: login user name
 * password: login password, md5 form
 * Return: 0->success, -1->fail
 *****************************************/
int Database::PlayerLogin(std::string user_name, char password_md5[LENGTH_MD5])
{
    
    return 0;
}

/******************************************
 * Function: Check player Register
 * Parameters: 2
 * user_name: login user name
 * password: login password, md5 form
 * Return: 0->success, -1->fail
 *****************************************/
int Database::PlayerRegister(std::string user_name, char password_md5[LENGTH_MD5])
{
    
    return 0;
}

/******************************************
 * Function: Get user_id
 * Parameters: 2
 * user_name: user name
 * Return: user_id
 *****************************************/
std::string Database::GetUserId(std::string user_name)
{
    
    
}

/******************************************
 * Function: Check username
 * Parameters: 2
 * user_name: user name
 * Return: true or false
 *****************************************/
int Database::CheckUserName(std::string user_name)
{
    
    
}

/******************************************
 * Function: Query friends
 * Parameters: 2
 * user_id: user id
 * Return: user_name of friends
 *****************************************/
std::vector<std::string> Database::QueryFriendName(std::string user_id)
{
    
    
}

/******************************************
 * Function: Add friend connection
 * Parameters: 2
 * user1_id: user1 id
 * user2_id: user2 id
 * Return: user_name of friends
 *****************************************/
int Database::InsertFriend(std::string user1_id, std::string user2_id)
{
    
    
}

/******************************************
 * Function: Query friends
 * Parameters: 2
 * user_id: user id
 * Return: user_name of friends
 *****************************************/
std::vector<std::string> Database::QueryWaitFriendName(std::string user_id)
{
    
    
}

/******************************************
 * Function: Add friend connection
 * Parameters: 2
 * user1_id: user1 id
 * user2_id: user2 id
 * Return: user_name of friends
 *****************************************/
int Database::InsertWaitFriend(std::string user1_id, std::string user2_id)
{
    
    
}


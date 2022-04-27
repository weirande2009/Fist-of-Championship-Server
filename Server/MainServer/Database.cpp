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
 * Function: Execute sql code
 * Parameters: 0
 * Return: execution state
 *****************************************/
int Database::ExecuteSql(std::string sql)
{
    if(mysql_query(this->connection, "select * from Users")){
		std::cout << "Query Error:" << mysql_error(connection) << std::endl;
        return -1;
	}
    return 0;
}

/******************************************
 * Function: Check player login
 * Parameters: 2
 * user_name: login user name
 * password: login password, md5 form
 * Return: 0->success, -1->fail
 *****************************************/
int Database::PlayerLogin(std::string user_name, std::string password_md5)
{
    // Check user name and password are matched
    std::string sql = "select ";
    sql += this->user_table.username+","+this->user_table.passwd+" ";
    sql += "where "+this->user_table.username+"=\'"+user_name+"\' ";
    sql += "from "+this->user_table.table+";\n";
    // Execute
    int ret = this->ExecuteSql(sql);
    if(ret == EXE_ERROR){
        return EXE_ERROR;
    }
    
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
       int num_fields = mysql_num_fields(result);   // Get field number
       int num_rows = mysql_num_rows(result);       // Get row number
       if(num_rows == 0){
           return LOGIN_FAIL;
       }
       for(int i=0; i<num_rows; i++)
        {
            // Get next row
            this->row = mysql_fetch_row(result);
            if(!this->row) 
                break;
            if(!memcmp(this->row[1], password_md5.c_str(), LENGTH_MD5)){
                mysql_free_result(this->result);
                return LOGIN_SUCCESS;
            }
            else{
                mysql_free_result(this->result);
                return LOGIN_FAIL;
            }
        }
    }
    else  // result == NULL
    {
        mysql_free_result(this->result);
        return LOGIN_FAIL;
    }

}

/******************************************
 * Function: Check player Register
 * Parameters: 2
 * user_name: login user name
 * password: login password, md5 form
 * Return: 0->success, -1->fail
 *****************************************/
int Database::PlayerRegister(std::string user_name, std::string password_md5)
{
    // Check whether already has the user_name
    std::string sql = "select ";
    sql += this->user_table.username+" ";
    sql += "where "+this->user_table.username+"=\'"+user_name+"\' ";
    sql += "from "+this->user_table.table+";\n";
    // Execute
    int ret = this->ExecuteSql(sql);
    if(ret == EXE_ERROR){
        return EXE_ERROR;
    }
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
       int num_fields = mysql_num_fields(result);   // Get field number
       int num_rows = mysql_num_rows(result);       // Get row number
       if(num_rows > 0){
           mysql_free_result(this->result);
           return REGISTER_FAIL;
       }
    }
    else { // result == NULL
        mysql_free_result(this->result);
        return REGISTER_FAIL;
    }
    mysql_free_result(this->result);
    // Insert Users
    std::string sql = "insert into ";
    sql += this->user_table.table+" value (";
    sql += "\'"+this->GenerateNewUserId()+"\', ";
    sql += "\'"+user_name+"\', ";
    sql += "\'"+password_md5+"\');";
    // Execute
    int ret = this->ExecuteSql(sql);
    if(ret == EXE_ERROR){
        mysql_free_result(this->result);
        return EXE_ERROR;
    }
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
        mysql_free_result(this->result);
        return REGISTER_FAIL;
    }
    else  // result == NULL
    {
        if(mysql_field_count(connection) == 0)   // means: update, insert, delete
        {
            // (it was not a SELECT)
            int num_rows = mysql_affected_rows(connection);  // return update, insert, delete affect row number
            if(num_rows == 1){
                mysql_free_result(this->result);
                return REGISTER_SUCCESS;
            }
            else{
                mysql_free_result(this->result);
                return REGISTER_FAIL;
            }
        }
        else // error
        {
            std::cout << "Get result error: " << mysql_error(connection) << std::endl;
            mysql_free_result(this->result);
            return REGISTER_FAIL;
        }
    }
}

/******************************************
 * Function: Generate New UserId
 * Parameters: 2
 * user_name: user name
 * Return: user_id
 *****************************************/
std::string Database::GenerateNewUserId()
{
    // Compute user number
    int user_number;
    std::string sql = "select ";
    sql += this->user_table.username+" ";
    sql += "from "+this->user_table.table+";\n";
    // Execute
    int ret = this->ExecuteSql(sql);
    this->result = mysql_use_result(connection); // Get results
	if (this->result){ // if return something
       int user_number = mysql_num_rows(result);       // Get row number
    }
    mysql_free_result(this->result);
    // Generate user id
    std::string prefix = "";
    prefix += (PLAYER_ID_LENGTH-std::to_string(user_number).length())*'0';
    std::string new_userid = prefix + std::to_string(user_number);
    return new_userid;
}

/******************************************
 * Function: Get user_id
 * Parameters: 2
 * user_name: user name
 * Return: user_id
 *****************************************/
std::string Database::GetUserId(std::string user_name)
{
    // Check whether already has the user_name
    std::string sql = "select ";
    sql += this->user_table.userid+" ";
    sql += "where "+this->user_table.username+"=\'"+user_name+"\' ";
    sql += "from "+this->user_table.table+";\n";
    // Execute
    int ret = this->ExecuteSql(sql);
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
        int num_fields = mysql_num_fields(result);   // Get field number
        int num_rows = mysql_num_rows(result);       // Get row number
        if(num_rows > 0){
            this->row = mysql_fetch_row(result);
            mysql_free_result(this->result);
            return this->row[0];
        }
    }
    else{
        std::cout << "Get result error: " << mysql_error(connection) << std::endl;
    }
    mysql_free_result(this->result);
    return "";
}

/******************************************
 * Function: Get user_name
 * Parameters: 2
 * user_name: user_id
 * Return: user_name
 *****************************************/
std::string Database::GetUserName(std::string user_id)
{
    // Check whether already has the user_name
    std::string sql = "select ";
    sql += this->user_table.username+" ";
    sql += "where "+this->user_table.userid+"=\'"+user_id+"\' ";
    sql += "from "+this->user_table.table+";\n";
    // Execute
    int ret = this->ExecuteSql(sql);
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
        int num_fields = mysql_num_fields(result);   // Get field number
        int num_rows = mysql_num_rows(result);       // Get row number
        if(num_rows > 0){
            this->row = mysql_fetch_row(result);
            mysql_free_result(this->result);
            return this->row[0];
        }
    }
    else{
        std::cout << "Get result error: " << mysql_error(connection) << std::endl;
    }
    mysql_free_result(this->result);
    return "";
}

/******************************************
 * Function: Check username
 * Parameters: 1
 * user_name: user name
 * Return: true or false
 *****************************************/
int Database::CheckUserName(std::string user_name)
{
    // Check whether already has the user_name
    std::string sql = "select ";
    sql += this->user_table.username+" ";
    sql += "where "+this->user_table.username+"=\'"+user_name+"\' ";
    sql += "from "+this->user_table.table+";\n";
    // Execute
    int ret = this->ExecuteSql(sql);
    if(ret == EXE_ERROR){
        return EXE_ERROR;
    }
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
        int num_fields = mysql_num_fields(result);   // Get field number
        int num_rows = mysql_num_rows(result);       // Get row number
        if(num_rows > 0){
            mysql_free_result(this->result);
            return 1;
        }
        else{
            mysql_free_result(this->result);
            return 0;
        }
    }
    else { // result == NULL
        std::cout << "Get result error: " << mysql_error(connection) << std::endl;
        mysql_free_result(this->result);
        return 0;
    }
}

/******************************************
 * Function: Query friends
 * Parameters: 2
 * user_id: user id
 * Return: user_name of friends
 *****************************************/
std::vector<std::string> Database::QueryFriendName(std::string user_id)
{
    // Find all friend ids
    std::vector<std::string> friends_ids;
    std::string sql = "select ";
    sql += this->friends_table.user1_id+" ,"+this->friends_table.user2_id+" ";
    sql += "where "+this->friends_table.user1_id+"=\'"+user_id+"\' or "+this->friends_table.user2_id+"=\'"+user_id+"\' ";
    sql += "from "+this->friends_table.table+";\n";
    // Execute
    int ret = this->ExecuteSql(sql);
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
        int num_fields = mysql_num_fields(result);   // Get field number
        int num_rows = mysql_num_rows(result);       // Get row number
        if(num_rows > 0){
            for(int i=0; i<num_rows; i++){
                this->row = mysql_fetch_row(result);
                if(!strcmp(this->row[0], user_id.c_str())){
                    friends_ids.push_back(this->row[1]);
                }
                else{
                    friends_ids.push_back(this->row[0]);
                }
            }
        }
        else{
            return friends_ids;
        }
    }
    else{
        std::cout << "Get result error: " << mysql_error(connection) << std::endl;
    }
    mysql_free_result(this->result);
    // Change id to name
    std::vector<std::string> friends_names;
    for(int i=0; i<friends_names.size(); i++){
        friends_names.push_back(this->GetUserName(friends_names[i]));
    }
    return friends_names;
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
    // Insert Friends
    std::string sql = "insert into ";
    sql += this->friends_table.table+" value (";
    sql += "\'"+user1_id+"\', ";
    sql += "\'"+user2_id+"\');";
    // Execute
    int ret = this->ExecuteSql(sql);
    if(ret == EXE_ERROR){
        mysql_free_result(this->result);
        return EXE_ERROR;
    }
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
        mysql_free_result(this->result);
        return INSERT_FAIL;
    }
    else  // result == NULL
    {
        if(mysql_field_count(connection) == 0)   // means: update, insert, delete
        {
            // (it was not a SELECT)
            int num_rows = mysql_affected_rows(connection);  // return update, insert, delete affect row number
            if(num_rows == 1){
                mysql_free_result(this->result);
                return INSERT_OK;
            }
            else{
                mysql_free_result(this->result);
                return INSERT_FAIL;
            }
        }
        else // error
        {
            std::cout << "Get result error: " << mysql_error(connection) << std::endl;
            mysql_free_result(this->result);
            return INSERT_FAIL;
        }
    }
}

/******************************************
 * Function: Query friends
 * Parameters: 2
 * user_id: user id of receiver
 * Return: user_name of applier
 *****************************************/
std::vector<std::string> Database::QueryWaitFriendName(std::string user_id)
{
    // Find all friend ids
    std::vector<std::string> friends_ids;
    std::string sql = "select ";
    sql += this->wait_friends_table.user1_id+" ";
    sql += "where "+this->friends_table.user2_id+"=\'"+user_id+"\' ";
    sql += "from "+this->friends_table.table+";\n";
    // Execute
    int ret = this->ExecuteSql(sql);
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
        int num_fields = mysql_num_fields(result);   // Get field number
        int num_rows = mysql_num_rows(result);       // Get row number
        if(num_rows > 0){
            for(int i=0; i<num_rows; i++){
                this->row = mysql_fetch_row(result);
                friends_ids.push_back(this->row[0]);
            }
        }
        else{
            return friends_ids;
        }
    }
    else{
        std::cout << "Get result error: " << mysql_error(connection) << std::endl;
    }
    mysql_free_result(this->result);
    // Change id to name
    std::vector<std::string> friends_names;
    for(int i=0; i<friends_names.size(); i++){
        friends_names.push_back(this->GetUserName(friends_names[i]));
    }
    return friends_names;
    
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
    // Insert Friends
    std::string sql = "insert into ";
    sql += this->wait_friends_table.table+" value (";
    sql += "\'"+user1_id+"\', ";
    sql += "\'"+user2_id+"\');";
    // Execute
    int ret = this->ExecuteSql(sql);
    if(ret == EXE_ERROR){
        mysql_free_result(this->result);
        return EXE_ERROR;
    }
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
        mysql_free_result(this->result);
        return INSERT_FAIL;
    }
    else  // result == NULL
    {
        if(mysql_field_count(connection) == 0)   // means: update, insert, delete
        {
            // (it was not a SELECT)
            int num_rows = mysql_affected_rows(connection);  // return update, insert, delete affect row number
            if(num_rows == 1){
                mysql_free_result(this->result);
                return INSERT_OK;
            }
            else{
                mysql_free_result(this->result);
                return INSERT_FAIL;
            }
        }
        else // error
        {
            std::cout << "Get result error: " << mysql_error(connection) << std::endl;
            mysql_free_result(this->result);
            return INSERT_FAIL;
        }
    }
}

/******************************************
 * Function: Delete wait friend
 * Parameters: 2
 * user1_id: user1 id
 * user2_id: user2 id
 * Return: user_name of friends
 *****************************************/
int Database::DeleteWaitFriend(std::string user1_id, std::string user2_id)
{
    // Delete Friends
    std::string sql = "delete from ";
    sql += "where "+this->wait_friends_table.user1_id+"=\'"+user1_id+"\' and "+this->wait_friends_table.user1_id+"=\'"+user1_id+"\';";
    // Execute
    int ret = this->ExecuteSql(sql);
    if(ret == EXE_ERROR){
        mysql_free_result(this->result);
        return EXE_ERROR;
    }
    this->result = mysql_use_result(connection); // Get results
	if (this->result)  // if return something
    {
        mysql_free_result(this->result);
        return DELETE_FAIL;
    }
    else  // result == NULL
    {
        if(mysql_field_count(connection) == 0)   // means: update, insert, delete
        {
            // (it was not a SELECT)
            int num_rows = mysql_affected_rows(connection);  // return update, insert, delete affect row number
            if(num_rows == 1){
                mysql_free_result(this->result);
                return DELETE_OK;
            }
            else{
                mysql_free_result(this->result);
                return DELETE_FAIL;
            }
        }
        else // error
        {
            std::cout << "Get result error: " << mysql_error(connection) << std::endl;
            mysql_free_result(this->result);
            return DELETE_FAIL;
        }
    }
    return 0;
}

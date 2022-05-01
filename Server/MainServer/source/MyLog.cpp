#include "MyLog.h"

MyLog MyLog::instance;

/******************************************
 * Function: Initialize MyLog
 * Parameters: 0
 * Return: None
 *****************************************/
MyLog::MyLog(){

}

/******************************************
 * Function: Destructor
 * Parameters: 0
 * Return: None
 *****************************************/
MyLog::~MyLog(){
    // Initial user and password

}

/******************************************
 * Function: Get Instance
 * Parameters: 0
 * Return: The single instance of Database
 *****************************************/
MyLog& MyLog::Instance()
{
    return MyLog::instance;
}

/******************************************
 * Function: Get Time
 * Parameters: 0
 * Return: None
 *****************************************/
std::string MyLog::GetTime(){
    time_t t = time(0);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%x %X",localtime(&t));
    std::string a = tmp;
    return a;
}

/******************************************
 * Function: Write Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::Log(std::string content){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    config_file_name << content;
    config_file_name.close();
}

/******************************************
 * Function: Write Basic Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogBasic(int fd, std::string username){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content += "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Register\n";
    content += "Info: \n";
    content += "UserName: ";
    content += username;
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}


/******************************************
 * Function: Write Connect Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogConnect(int fd, struct sockaddr_in remote){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content = "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Connect\n";
    content += "Info: \n";
    content += "IP: ";
    content += inet_ntoa(remote.sin_addr);
    content += "\n";
    content += "Port: ";
    content += std::to_string(remote.sin_port);
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}

/******************************************
 * Function: Write Login Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogLogin(int fd, std::string username, int state){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content = "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Login\n";
    content += "Info: \n";
    content += "UserName: ";
    content += username;
    content += "\n";
    content += "State: ";
    content += std::to_string(state);
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}

/******************************************
 * Function: Write Register Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogRegister(int fd, std::string username, int state){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content = "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Register\n";
    content += "Info: \n";
    content += "UserName: ";
    content += username;
    content += "\n";
    content += "State: ";
    content += std::to_string(state);
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}

/******************************************
 * Function: Write HallRoom Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogHallRoom(int fd, std::string username){
    this->LogBasic(fd, username);
}

/******************************************
 * Function: Write Friend Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogFriend(int fd, std::string username){
    this->LogBasic(fd, username);
}

/******************************************
 * Function: Write EnterRoom Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogEnterRoom(int fd, std::string username, std::string room_no, int state){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content = "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Friend\n";
    content += "Info: \n";
    content += "UserName: ";
    content += username;
    content += "\n";
    content += "RoomNo: ";
    content += room_no;
    content += "\n";
    content += "SeatNo: ";
    content += std::to_string(state);
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}

/******************************************
 * Function: Write RoomInfo Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogRoomInfo(int fd, std::string username, std::string room_no){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content = "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Friend\n";
    content += "Info: \n";
    content += "UserName: ";
    content += username;
    content += "\n";
    content += "RoomNo: ";
    content += room_no;
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}

/******************************************
 * Function: Write ModifyChar Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogModifyChar(int fd, std::string username, int character){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content = "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Friend\n";
    content += "Info: \n";
    content += "UserName: ";
    content += username;
    content += "\n";
    content += "Character: ";
    content += std::to_string(character);
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}

/******************************************
 * Function: Write Ready Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogReady(int fd, std::string username){
    this->LogBasic(fd, username);
}

/******************************************
 * Function: Write CancelReady Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogCancelReady(int fd, std::string username){
    this->LogBasic(fd, username);
}

/******************************************
 * Function: Write StartGame Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogStartGame(int fd, std::string username){
    this->LogBasic(fd, username);
}

/******************************************
 * Function: Write ExitLogin Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogExitLogin(int fd, std::string username){
    this->LogBasic(fd, username);
}

/******************************************
 * Function: Write Quit Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogQuit(int fd, std::string username){
    this->LogBasic(fd, username);
}

/******************************************
 * Function: Write ExitRoom Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogExitRoom(int fd, std::string username){
    this->LogBasic(fd, username);
}

/******************************************
 * Function: Write CreateRoom Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogCreateRoom(int fd, std::string username, std::string room_name){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content = "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Friend\n";
    content += "Info: \n";
    content += "UserName: ";
    content += username;
    content += "\n";
    content += "RoomName: ";
    content += room_name;
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}

/******************************************
 * Function: Write AddFriend Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogAddFriend(int fd, std::string username, std::string friend_name){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content = "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Friend\n";
    content += "Info: \n";
    content += "UserName: ";
    content += username;
    content += "\n";
    content += "RoomName: ";
    content += friend_name;
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}

/******************************************
 * Function: Write ReplyFriend Log
 * Parameters: 0
 * Return: None
 *****************************************/
void MyLog::LogReplyFriend(int fd, std::string username, std::string friend_name){
    std::ofstream config_file_name(this->log_file_name, std::ios::out | std::ios::app);
    if(!config_file_name){
        std::cout << "Fail to Read Config File!" << std::endl;
        return;
    }
    std::string content;
    content = this->GetTime() + "\n";
    content = "Client fd: " + std::to_string(fd) + "\n";
    content += "Command: Friend\n";
    content += "Info: \n";
    content += "UserName: ";
    content += username;
    content += "\n";
    content += "RoomName: ";
    content += friend_name;
    content += "\n";
    content += "\n";
    config_file_name << content;
    config_file_name.close();
}

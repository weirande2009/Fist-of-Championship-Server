#ifndef MYLOG_H
#define MYLOG_H
#include "utilities/common.h"

class MyLog{
    // Variables
private:
    static MyLog instance;       // single instance
    std::string log_file_name = "/root/weiran/Fist-of-Championship/Server/NewMainServer/Log/mylog.txt";     // Log file


public:

// Functions
private:

public:
    MyLog();
    ~MyLog();
    std::string GetTime();
    static MyLog& Instance();  // get instance of Database
    void Log(std::string content);
    void LogBasic(int fd, std::string username, std::string cmd);
    void LogConnect(int fd, struct sockaddr_in remote);
    void LogLogin(int fd, std::string username, int state);
    void LogRegister(int fd, std::string username, int state);
    void LogHallRoom(int fd, std::string username);
    void LogFriend(int fd, std::string username);
    void LogEnterRoom(int fd, std::string username, std::string room_no, int state);
    void LogRoomInfo(int fd, std::string username, std::string room_no);
    void LogModifyChar(int fd, std::string username, int character);
    void LogReady(int fd, std::string username);
    void LogCancelReady(int fd, std::string username);
    void LogStartGame(int fd, std::string username);
    void LogExitLogin(int fd, std::string username);
    void LogQuit(int fd, std::string username);
    void LogExitRoom(int fd, std::string username);
    void LogCreateRoom(int fd, std::string username, std::string room_name);
    void LogAddFriend(int fd, std::string username, std::string friend_name);
    void LogReplyFriend(int fd, std::string username, std::string friend_name);

};


#endif
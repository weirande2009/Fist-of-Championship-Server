#ifndef SERVER_H
#define SERVER_H
#include "common.h"


class Server{
// Variables
private:
    

protected:
    int server_fd;  // Server's handle
    std::string server_ip;  // Server's IP
    int server_port;  // Server's port

public:


// Functions
private:


protected:


public:
    Server();
    ~Server();
    virtual void Start() = 0;  // Start listening and wait for connecting



};




#endif
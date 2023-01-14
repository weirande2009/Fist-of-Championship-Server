#ifndef CLIENT_H
#define CLIENT_H
#include "common.h"

class Client{
// Variables
private:

protected:
    std::string client_ip;  // Server's IP
    int client_port;  // Server's port

public:
    int client_fd;  // Server's handle

// Functions
private:


protected:


public:
    Client();
    ~Client();


};




#endif
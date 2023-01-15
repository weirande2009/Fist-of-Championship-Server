#ifndef MAINSERVERCONTROLLER_H
#define MAINSERVERCONTROLLER_H
#include "utilities/common.h"
#include "server/MainServer.h"

class MainServerController{

// Variables
private:
    std::string ip;
    int port;
    MainServer *main_server;

public:
    
// Functions
private:

public:
    MainServerController(); 
    ~MainServerController();


};




#endif

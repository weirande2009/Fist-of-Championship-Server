#ifndef MAINSERVERCONTROLLER_H
#define MAINSERVERCONTROLLER_H
#include "common.h"
#include "MainServer.h"

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

#include "server/MainServerController.h"

MainServerController::MainServerController()
{
    this->ip = "0.0.0.0";
    this->port = 12345;
    main_server = new MainServer(this->ip, this->port);
    main_server->Start();
}

MainServerController::~MainServerController()
{
    delete main_server;
}
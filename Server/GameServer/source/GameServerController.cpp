#include "GameServerController.h"

GameServerController::GameServerController()
{
    
}

GameServerController::GameServerController(int _total_player_number)
{
    this->ip = "0.0.0.0";
    this->port = 12345;
    game_server = new GameServer(this->ip, this->port, _total_player_number);
    game_server->Start();
}

GameServerController::~GameServerController()
{
    delete main_server;
}




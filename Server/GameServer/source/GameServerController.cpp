#include "GameServerController.h"

GameServerController::GameServerController()
{
    
}

GameServerController::GameServerController(int _total_player_number, int _port)
{
    this->ip = "0.0.0.0";
    this->port = _port;
    this->total_player_number  = _total_player_number;

}

GameServerController::~GameServerController()
{
    
}

void GameServerController::Start()
{
    while(1){
        game_server = new GameServer(this->ip, this->port, this->total_player_number);
        game_server->Start();   
        delete game_server;
    }
}


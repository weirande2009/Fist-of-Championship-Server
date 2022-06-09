#include "GameServerController.h"

int main(int argc, char** argv){
    int total_player_number = atoi(argv[1]);
    GameServerController game_server_controller = GameServerController(total_player_number);
    return 0;
}
#include "GameServerController.h"

int main(int argc, char** argv){
    int total_player_number = atoi(argv[1]);
    int port = atoi(argv[2]);
    std::cout << port << std::endl;
    GameServerController game_server_controller = GameServerController(total_player_number, port);
    game_server_controller.Start();
    return 0;
}

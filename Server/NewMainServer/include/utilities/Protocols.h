#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "utilities/common.h"

/* Server->Client */
// #define S_LOGIN 1               // Login state
// #define S_REGISTER 2            // Register state
// #define S_HALL_ROOM 3           // Room info in hall
// #define S_FRIEND 4              // Friends info
// #define S_FRIEND_UPDATE 5       // Update friends info
// #define S_ENTER_ROOM 6          // Enter room
// #define S_ROOM_INFO 7           // Room info
// #define S_EXIT_ROOM 8           // Exit room
// #define S_UPDATE_ROOM 9         // Others enter room, modify character, get ready
// #define S_MODIFY_CHAR 10        // Modify character
// #define S_READY 11              // Get ready for the game
// #define S_CANCEL_READY 12       // Cancel ready
// #define S_START_GAME 13         // Start game
// #define S_EXIT_LOGIN 14         // Exit login
// #define S_QUIT 15               // Quit game
// #define S_ADD_FRIEND 16         // Add friends info
// #define S_OTHER_ADD_FRIEND 17   // Others add friends
// #define S_CREATE_ROOM 18        // Create a new room

/* Client->Server */
// #define C_LOGIN 1               // Login info
// #define C_REGISTER 2            // Register info
// #define C_HALL_ROOM 3           // Get room info in hall
// #define C_FRIEND 4              // Get friends info
// #define C_ENTER_ROOM 5          // Apply to enter room
// #define C_ROOM_INFO 6           // Get room info
// #define C_MODIFY_CHAR 7         // Apply to modify character
// #define C_READY 8               // Apply to ready
// #define C_CANCEL_READY 9        // Apply to cancel ready
// #define C_START_GAME 10         // Apply to start game
// #define C_EXIT_LOGIN 11         // Apply to exit login
// #define C_QUIT 12               // Apply to quit game
// #define C_EXIT_ROOM 13          // Apply to exit room
// #define C_CREATE_ROOM 14        // Apply to create a new room
// #define C_ADD_FRIEND 15         // Apply to add friend
// #define C_REPLY_FRIEND 16       // Reply to add friend

/* Other macro definition */
/* Room */
#define MAX_ROOM_NUMBER_IN_PAGE 8   // Max room number in a page of client
#define MAX_ROOM_NAME_LENGTH  32    // Max length of room name
#define ROOM_NO_LENGTH 8            // Max length of room no
#define SEAT_NUM_IN_ROOM 8          // Seat number in a room
#define SEAT_EMPTY -1               // State of seat empty
#define SEAT_OCCUPIED 1             // State of sear occupied
#define ENTER_ROOM_OK 1             // State of enter room ok
#define NO_AVAILABLE_SEAT -1        // State of full room
#define ROOM_EMPTY -1               // State of empty room
#define EXIT_ROOM_OK 1              // State of exit room ok
#define EXIT_ROOM_FAIL 0            // State of exit room fail
#define ROOM_IN_GAME 1              // State of room in game
#define ROOM_NOT_IN_GAME 0          // State of room not in game
/* Player */
#define MAX_PLAYER_NUM_IN_ROOM 8    // Max player number in a room
#define MAX_PLAYER_NAME_LENGTH 32   // Max length of player name
#define PLAYER_ID_LENGTH 8          // Length of player id
#define PLAYER_ONLINE 1             // State of player online
#define PLAYER_OFFLINE 2            // State of player offline
#define PLAYER_READY 1              // State of player ready
#define PLAYER_NO_READY 0           // State of player no ready
#define ROOM_OWNER 1                // State of room owner
#define ROOM_NOT_OWNER -1           // State of 
/* Network */
#define LENGTH_MD5 16              // Length of md5 code
#define PACKAGE_HEAD_LENGTH 8       // Length of package head
#define S_CMD_NUM 18                // Server command number
#define C_CMD_NUM 14                // Client command number
/* Client */
#define CLIENT_NO_LOGIN 0            // Client no login
#define CLIENT_LOGIN 1               // Client login
/* Login */
#define LOGIN_SUCCESS 1             // State of login success
#define LOGIN_FAIL -1               // State of login fail
#define REGISTER_SUCCESS 1          // State of register success
#define REGISTER_FAIL -1            // State of register fail
/* Game */
#define GAME_START 1                // State of game start
#define GAME_NO_START 0             // State of game no start
#define GAME_OVER 2                 // State of game over
#define GAME_SERVER_PORT_DOWN 10000 // Down of game server port
#define GAME_SERVER_PORT_UP 12000   // Up of game server port
#define GAME_SERVER_PORT_NUM 2000   // Number of game server port
/* Database */
#define EXE_ERROR -4                // SQL Execute error
#define INSERT_OK 1                 // State of insert ok
#define INSERT_FAIL -1              // State of insert fail
#define DELETE_OK 1                 // State of insert ok
#define DELETE_FAIL -1              // State of insert fail
/* Friend */
#define SEND_FRIEND_OK 1            // State of send msg to friend ok
#define SEND_FRIEND_FAIL -1         // State of send msg to friend fail
#define REPLY_FRIEND_OK 2           // State of replying from friend ok
#define REPLY_FRIEND_FAIL -2        // State of replying from friend fail

#define GAME_SERVER_IP "108.61.142.36"

/* Structures of various functions */

struct PackageHead{
    int cmd;
    int length;
};

// /*
//  * Function: Login info from server to client
//  * Info: None
//  */
// struct S_Login{
//     int cmd = S_LOGIN;
//     int length = sizeof(struct S_Login);
//     int state;                                  // state of login
// };

// /*
//  * Function: Register info from server to client
//  * Info: None
//  */
// struct S_Register{
//     int cmd = S_REGISTER;
//     int length = sizeof(struct S_Register);
//     int state;                                  // state of login
// };

// /*
//  * Function: Room info in the hall
//  * Info: None
//  */
// struct RoomInfo{
//     int player_number;                          // player number in the room
//     char room_name[MAX_ROOM_NAME_LENGTH];       // room name
//     char room_no[ROOM_NO_LENGTH];               // room number
//     int state;                                  // room state
// };

// /*
//  * Function: Hall info
//  * Info: None
//  */
// struct S_HallRoom{
//     int cmd = S_HALL_ROOM;
//     int length = sizeof(struct S_HallRoom);
//     int total_room_num;
//     int page_room_num;
//     struct RoomInfo room_info[MAX_ROOM_NUMBER_IN_PAGE] ; // room info
// };

// /*
//  * Function: Friend info in the hall
//  * Info: None
//  */
// struct FriendInfo{
//     char player_name[MAX_PLAYER_NAME_LENGTH];   // room name
//     int state;                                  // online or offline
// };

// /*
//  * Function: Friend info from server to client
//  * Info: The real length of friend_info should be added to length
//  */
// struct S_Friend{
//     int cmd = S_FRIEND;
//     int length = sizeof(S_Friend);        
//     struct FriendInfo* friend_info;
// };

// /*
//  * Function: Friend updated info from server to client
//  * Info:
//  */
// struct S_FriendUpdate{
//     int cmd = S_FRIEND_UPDATE;
//     int length = sizeof(S_FriendUpdate);        
//     struct FriendInfo friend_info;
// };

// /*
//  * Function: Enter room state from server to client
//  * Info:
//  */
// struct S_EnterRoom{
//     int cmd = S_ENTER_ROOM;
//     int length = sizeof(S_EnterRoom);        
//     int seat_no;                                // seat no in the room
//     int state;                                  // state of entering
// };

// /*
//  * Function: Player info in a seat
//  * Info:
//  */
// struct PlayerInfo{
//     char player_name[MAX_PLAYER_NAME_LENGTH];   // player_name
//     int char_type;                              // character type
//     int ready;                                  // whether ready
//     int owner;                                  // room owner
// };

// /*
//  * Function: Seat info in a room
//  * Info:
//  */
// struct SeatInfo{
//     int empty;                              // whether the seat is empty
//     int seat_no;                            // seat no
//     struct PlayerInfo player_info;          // player info
// };

// /*
//  * Function: Room info from server to client
//  * Info:
//  */
// struct S_RoomInfo{
//     int cmd = S_ROOM_INFO;
//     int length = sizeof(S_RoomInfo);        
//     struct SeatInfo seat_info[SEAT_NUM_IN_ROOM];    // seat info in the room
// };

// /*
//  * Function: Exit room info from server to client
//  * Info:
//  */
// struct S_ExitRoom{
//     int cmd = S_EXIT_ROOM;
//     int length = sizeof(S_ExitRoom);        
//     int state;                                      // state of exiting
// };

// /*
//  * Function: Others update room info from server to client
//  * Info:
//  */
// struct S_UpdateRoom{
//     int cmd = S_UPDATE_ROOM;
//     int length = sizeof(S_UpdateRoom);
//     struct SeatInfo seat_info;                      // state of seat
// };

// /*
//  * Function: Modify character type from server to client
//  * Info:
//  */
// struct S_ModChar{
//     int cmd = S_MODIFY_CHAR;
//     int length = sizeof(S_ModChar);        
//     int state;                                  // state of modifying new character type
// };

// /*
//  * Function: Get Ready info from server to client
//  * Info:
//  */
// struct S_Ready{
//     int cmd = S_READY;
//     int length = sizeof(S_Ready);  
//     int state;                                          // state of ready      
// };

// /*
//  * Function: Cancel ready from server to client
//  * Info:
//  */
// struct S_CancelReady{
//     int cmd = S_CANCEL_READY;
//     int length = sizeof(S_CancelReady);
//     int state;                                          // state of cancel ready
// };

// /*
//  * Function: Start game info from server to client
//  * Info:
//  */
// struct S_StartGame{
//     int cmd = S_START_GAME;
//     int length = sizeof(S_StartGame);      
//     int state;                                          // state of starting game
// };

// /*
//  * Function: Exit login info from server to client
//  * Info:
//  */
// struct S_ExitLogin{
//     int cmd = S_EXIT_LOGIN;
//     int length = sizeof(S_ExitLogin);      
//     int state;                                          // state of exiting login
// };

// /*
//  * Function: Exit login info from server to client
//  * Info:
//  */
// struct S_Quit{
//     int cmd = S_QUIT;
//     int length = sizeof(S_Quit);      
//     int state;                                          // state of quiting
// };

// /*
//  * Function: Add friend info from server to client
//  * Info:
//  */
// struct S_AddFriend{
//     int cmd = S_ADD_FRIEND;
//     int length = sizeof(S_AddFriend);      
//     int state;                                          // state of adding
// };

// /*
//  * Function: Add friend info from server to client
//  * Info:
//  */
// struct AddFriendInfo{
//     char friend_name[MAX_PLAYER_NAME_LENGTH];           // friend name
// };

// /*
//  * Function: Others add friend info from server to client
//  * Info:
//  */
// struct S_OtherAddFriend{
//     int cmd = S_OTHER_ADD_FRIEND;
//     int length = sizeof(S_OtherAddFriend);      
//     struct AddFriendInfo friend_info;                      // friend info
// };

// /*
//  * Function: Create room info from server to client
//  * Info:
//  */
// struct S_CreateRoom{
//     int cmd = S_CREATE_ROOM;
//     int length = sizeof(S_CreateRoom);      
//     int state;                      // friend info
// };

// /*
//  * Function: Login info from client to server
//  * Info:
//  */
// struct C_Login{
//     int cmd = C_LOGIN;
//     int length = sizeof(C_Login);      
//     char name[MAX_PLAYER_NAME_LENGTH];          // player name
//     char password_md5[LENGTH_MD5];              // password in md5
// };

// /*
//  * Function: Register info from client to server
//  * Info:
//  */
// struct C_Register{
//     int cmd = C_REGISTER;
//     int length = sizeof(C_Register);      
//     char name[MAX_PLAYER_NAME_LENGTH];          // player name
//     char password_md5[LENGTH_MD5];              // password in md5
// };

// /*
//  * Function: Apply to get hall room info from client to server
//  * Info:
//  */
// struct C_HallRoom{
//     int cmd = C_HALL_ROOM;
//     int length = sizeof(C_HallRoom);      
//     int page;                                   // page of rooms
// };

// /*
//  * Function: Apply to get hall room info from client to server
//  * Info:
//  */
// struct C_Friend{
//     int cmd = C_FRIEND;
//     int length = sizeof(C_Friend);
// };

// /*
//  * Function: Apply to enter room info from client to server
//  * Info:
//  */
// struct C_EnterRoom{
//     int cmd = C_ENTER_ROOM;
//     int length = sizeof(C_EnterRoom);
//     char room_no[ROOM_NO_LENGTH];               // room no
// };

// /*
//  * Function: Apply to get room info from client to server
//  * Info:
//  */
// struct C_RoomInfo{
//     int cmd = C_ROOM_INFO;
//     int length = sizeof(C_RoomInfo);
//     char room_no[ROOM_NO_LENGTH];               // room no
// };

// /*
//  * Function: Apply to modify character info from client to server
//  * Info:
//  */
// struct C_ModChar{
//     int cmd = C_MODIFY_CHAR;
//     int length = sizeof(C_ModChar);
//     int char_type;                              // room no
// };

// /*
//  * Function: Apply to get ready from client to server
//  * Info:
//  */
// struct C_Ready{
//     int cmd = C_READY;
//     int length = sizeof(C_Ready);
// };

// /*
//  * Function: Apply to cancel ready from client to server
//  * Info:
//  */
// struct C_CancelReady{
//     int cmd = C_CANCEL_READY;
//     int length = sizeof(C_CancelReady);
// };

// /*
//  * Function: Apply to start game from client to server
//  * Info:
//  */
// struct C_StartGame{
//     int cmd = C_START_GAME;
//     int length = sizeof(C_StartGame);
// };

// /*
//  * Function: Apply to exit login from client to server
//  * Info:
//  */
// struct C_ExitLogin{
//     int cmd = C_EXIT_LOGIN;
//     int length = sizeof(C_ExitLogin);
// };

// /*
//  * Function: Apply to quit game from client to server
//  * Info:
//  */
// struct C_Quit{
//     int cmd = C_QUIT;
//     int length = sizeof(C_Quit);
// };

// /*
//  * Function: Apply to exit room from client to server
//  * Info:
//  */
// struct C_ExitRoom{
//     int cmd = C_EXIT_ROOM;
//     int length = sizeof(C_ExitRoom);
// };

// /*
//  * Function: Apply to create room from client to server
//  * Info:
//  */
// struct C_CreateRoom{
//     int cmd = C_CREATE_ROOM;
//     int length = sizeof(C_CreateRoom);
//     char room_name[MAX_ROOM_NAME_LENGTH];           // room name
// };

// /*
//  * Function: Apply to add friend
//  * Info:
//  */
// struct C_AddFriend{
//     int cmd = C_ADD_FRIEND;
//     int length = sizeof(C_AddFriend);
//     char friend_name[MAX_PLAYER_NAME_LENGTH];           // room name
// };

// /*
//  * Function: Reply to add friend
//  * Info:
//  */
// struct C_ReplyFriend{
//     int cmd = C_REPLY_FRIEND;
//     int length = sizeof(C_ReplyFriend);
//     char friend_name[MAX_PLAYER_NAME_LENGTH];           // room name
//     int state;                                          // state of the reply
// };



#endif




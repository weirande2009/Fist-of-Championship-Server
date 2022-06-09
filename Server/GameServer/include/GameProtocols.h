#ifndef GAMEPROTOCOL_H
#define GAMEPROTOCOL_H
#include "common.h"

/* Macro definition */
/* Server */
#define CLIENT_NUMBER 8                     // Client Number
#define PACKAGE_HEAD_LENGTH 8



/* Structures of various functions */

struct PackageHead{
    int cmd;
    int length;
};

#pragma pack(1);
struct PlayerOperation{
    unsigned char operations;               // Various click operation
    int mouse_pos_x;                        // x of mouse position
    int mouse_pos_y;                        // y of mouse position
    unsigned char armor;                    // armor no.
    unsigned char low_level_weapon;         // low level weapon no.
    unsigned char middle_level_weapon;      // middle level weapon
    unsigned char high_level_weapon;        // high level weapon
};
#pragma pack();


#endif


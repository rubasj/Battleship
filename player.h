//
// Created by janru on 10.01.2022.
//

#ifndef BATTLESHIP_PLAYER_H
#define BATTLESHIP_PLAYER_H

#include <string.h>
#include <stdlib.h>

#define DISCONNECTED -1
#define IN_QUEUE 2
#define CONNECTED 1 // hrac je pripojen
#define IN_GAME 3


#define ON_TURN 1
#define WAIT_ON_TURN 0

typedef struct _player {
    int socket;
    int player_state;
    char *nick;

    int player_id;

}player;

player *player_create(int socket, char *nick);
void player_change_state(player *player, int state);
int player_get_state(player *player);
int player_is_in_game(player *player);
void change_game_stat(player *player, int game_stat);
void free_player(player **player);
#endif //BATTLESHIP_PLAYER_H

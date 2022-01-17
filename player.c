//
// Created by janru on 10.01.2022.
//

#include "player.h"


player *player_create(int socket, char *nick) {
    player *temp;


    if(!socket)
        return NULL;
    temp = (player *)malloc(sizeof(player));
    temp->socket = socket;
    temp->nick = (char *)malloc(sizeof(char *) * strlen(nick));


    temp->game_id = -1;
    temp->player_id = -1;

    temp->player_state = CONNECTED;


    strcpy(temp->nick, nick);
    return temp;
}



void player_change_state(player *player, int state) {
    player->player_state = state;
}

int player_get_state(player *player){
    return player->player_state;
}
int player_is_in_game(player *player){
    return 1 ;
}

void change_game_stat(player *player, int game_stat){
    player->game_on_turn = game_stat;
}

void free_player(player **player){
    free(*player);
}

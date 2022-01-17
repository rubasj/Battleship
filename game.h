//
// Created by janru on 11.01.2022.
//

#ifndef BATTLESHIP_GAME_H
#define BATTLESHIP_GAME_H

#include "board.h"
#include "player.h"

#define GAME_OK 0
#define GAME_WAIT 1


typedef struct game {
    board *board1;
    board *board2;
    player *player1;
    player *player2; // TODO Check pointers for player in free game

    int game_id;
    int player_turn_socket;
    int game_state;
}game;
#endif //BATTLESHIP_GAME_H


game *init_game(board *b1, board *b2, player *p1, player *p2, int game_id);
char player_turn(game *game, int player, uint target);
int free_game(game **game);
int is_game_over(game *game);
void set_game_state(game *game, int state);
char *get_current_board_state(game *game, char *nickname);
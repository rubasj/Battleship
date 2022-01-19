//
// Created by janru on 11.01.2022.
//

#include "game.h"


game *init_game(board *b1, board *b2, player *p1, player *p2, int game_id){
    game *temp;

    if (!b1 || !b2 || !p1 || !p2) {

    }

    temp = (game *)malloc(sizeof(game));
    if(!temp){
        return NULL;
    }
    temp->board1 = b1;
    temp->board2 = b2;
    temp->player1 = p1;
    temp->player2 = p2;


    temp->game_id = game_id;
    temp->player_turn_socket = p1->player_id;
    temp->game_state = GAME_OK;
    return temp;
}



char player_turn(game *game, int player, uint target){
    char attack_result;
    /* Zjistit sock hráče */
    if(game->player_turn_socket == player){
        /* Pokud je na tahu hráč 1*/
        if(game->player1->player_id == player){
            attack_result = is_hit(game->board1, target);
            /* Switch player */
            if(attack_result != '4')
                game->player_turn_socket = game->player2->player_id;
            return attack_result;
        }
        else
        {
            /* Pokud je na tahu hráč 2 */
            attack_result = is_hit(game->board2, target);
            /* Switch player */
            if(attack_result != '4')
                game->player_turn_socket = game->player1->player_id;
            return attack_result;
        }

    }
    else
    {
        return '0';
    }
}


void game_free(game **poor) {
    board_free(&(*poor)->board1);
    board_free(&(*poor)->board2);
    free(*poor);
    *poor = NULL;

}
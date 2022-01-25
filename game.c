#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "game.h"


void create_wanna_play(wanna_play **wanna_plays) {
    (*wanna_plays) = calloc(1, sizeof(wanna_play));
    (*wanna_plays) -> size = 0;
    (*wanna_plays) -> socket_IDs = calloc(1, sizeof(int));
}


void add_wanna_play(wanna_play **wanna_plays, int socket_ID) {
    (*wanna_plays) -> size++;
    printf("Player [%d] wants to play\n", socket_ID);
    (*wanna_plays) -> socket_IDs = realloc((*wanna_plays) -> socket_IDs, (*wanna_plays) -> size * sizeof(int));
    (*wanna_plays) -> socket_IDs[((*wanna_plays) -> size) - 1] = socket_ID;
    printf("Info: %d Player(s) want to play\n", (*wanna_plays) -> size);
}


void remove_wanna_play(wanna_play **wanna_plays, int socket_ID) {
    int i;
    int socket;
    int count = (*wanna_plays) -> size;
    for(i = 0; i < count; i++) {
        socket = (*wanna_plays) -> socket_IDs[i];
        if (socket == socket_ID) {
            (*wanna_plays) -> size--;
            if (i < (count - 1)) {
                (*wanna_plays) -> socket_IDs[i] = (*wanna_plays) -> socket_IDs[((*wanna_plays) -> size)];
            }
            (*wanna_plays) -> socket_IDs = realloc((*wanna_plays) -> socket_IDs, (*wanna_plays) -> size * sizeof(wanna_play));
            printf("Player [%d] removed from queue\n", socket_ID);
            printf("Info: %d Player(s) want to play game\n\n", (*wanna_plays) -> size);
            return;
        }
    }
}


void create_games(games **all_games) {
    int max_games = MAX_GAMES;
    (*all_games) = calloc(1, sizeof(games));
    (*all_games) -> games_count = 0;
    (*all_games) -> games = calloc(1, max_games * sizeof(game));
}


game *create_game(games **all_games, char *name_1, char *name_2) {
    (*all_games) -> games_count++;
    printf("Number of games: %d\n\n", (*all_games) -> games_count);
    (*all_games) -> games = realloc((*all_games) -> games, (*all_games) -> games_count * sizeof(game));
    game *game = NULL;
    init_game(&game, name_1, name_2);
    (*all_games) -> games[((*all_games) -> games_count) - 1] = game;
    (*all_games) -> games[((*all_games) -> games_count) - 1] -> game_ID = ((*all_games) -> games_count) - 1;
    return game;
}


void init_game(game **game, char *name_1, char *name_2) {
    (*game) = calloc(1, sizeof(game));
    (*game) -> name_1 = name_1;
    (*game) -> name_2 = name_2;
    (*game) -> player_1_on_turn = 1;
    (*game) -> player_2_on_turn = 0;
    (*game) -> player_1_ships = 17;
    (*game) -> player_2_ships = 17;

    (*game) -> b1 = board_create();
    (*game) -> b2 = board_create();


    int i;


}



game *get_game_by_player_name(Players *array_players, games **all_games, Player **cl){
    int i;

    for(i = 0; i < (*all_games)->games_count; i++){
        if(strcmp((*all_games)->games[i]->name_1, (*cl)->name) == 0)
            return (*all_games)->games[i];
        if(strcmp((*all_games)->games[i]->name_2, (*cl)->name) == 0)
            return (*all_games)->games[i];
    }
    return NULL;
}




void check_game_end(Players **array_clients, game *this_game, games **all_games) {


    if (this_game->b1->ship_alive == 0 && this_game->b2->ship_alive != 0){
        //player 1 lose
        char message_1[100];
        char message_2[100];

        sprintf(message_1, "GAME_OVER|%d\n", 0);
        sprintf(message_2, "GAME_OVER|%d\n", 1);// 1 - WINNER

        send_message(get_player_by_name(*array_clients, this_game->name_1)->socket_ID, message_1);
        send_message(get_player_by_name(*array_clients, this_game->name_2)->socket_ID, message_2);

        get_player_by_name(*array_clients, this_game->name_1)->state = IN_LOBBY;
        get_player_by_name(*array_clients, this_game->name_2)->state = IN_LOBBY;

        remove_game(array_clients, all_games, this_game->game_ID);

    }
    else if(this_game->b1->ship_alive != 0 && this_game->b2->ship_alive == 0){
        //player 2 lose
        char message_1[100];
        char message_2[100];

        sprintf(message_1, "GAME_OVER|%d\n", 1); // 1 - WINNER
        sprintf(message_2, "GAME_OVER|%d\n", 0);


        send_message(get_player_by_name(*array_clients, this_game->name_1)->socket_ID, message_1);
        send_message(get_player_by_name(*array_clients, this_game->name_2)->socket_ID, message_2);

        get_player_by_name(*array_clients, this_game->name_1)->state = DISCONNECT;
        get_player_by_name(*array_clients, this_game->name_2)->state = DISCONNECT;

        remove_game(array_clients, all_games, this_game->game_ID);
    }

}



void remove_game(Players **pls, games **all_games, int game_ID) {
    int i;
    int count = (*all_games) -> games_count;
    int index;


    for (i = 0; i < count; i++) {
        if (i == game_ID) {
            if (i < (count - 1)) {
                board_free(&(*all_games) -> games[i]->b1);
                board_free(&(*all_games) -> games[i]->b2);
                free((*all_games) -> games[i]);
                (*all_games) -> games[i] = (*all_games) -> games[((*all_games) -> games_count) - 1];
            }

            (*all_games) -> games_count--;
            if (((*all_games) -> games_count) > 0) {
                (*all_games) -> games = realloc((*all_games) -> games, (*all_games) -> games_count * sizeof(game));
                index = i;
                (*all_games) -> games[i] -> game_ID = index;
            }
            break;
        }
    }
}


board *board_create() {
    board *tmp;

    tmp = (board *)malloc(sizeof(board));

    if (!tmp) {
        printf("MEMORY: Board not allocated \n");
        return NULL;
    }

    tmp->cols = BOARD_SIZE;
    tmp->rows = BOARD_SIZE;

    tmp->items = (char *) malloc(BOARD_SIZE * BOARD_SIZE * sizeof(char ));

    if (!tmp->items) {
        free(tmp);
        return NULL;
    }

    board_fill(tmp);
    board_set(tmp); /* Auto set ships in board. */

    return tmp;
}


void board_fill(board *bd) {
    size_t i;

    if (!bd || !bd->items) {
        return;
    }

    for (i = 0; i < bd->cols * bd->rows; ++i) {
        bd->items[i] = EMPTY_ITEM;
    }


}


void board_free(board **poor) {
    if (!poor || !*poor)
        return;

    (*poor)->cols = 0;
    (*poor)->rows = 0;

    free((*poor)->items);
    (*poor)->items = NULL;

    free(*poor);
    *poor = NULL;
}


void board_set(board *bd) {
    uint board_size = bd->cols * bd->rows;
    int ships_generated = 0;
    int r;

    srand(time(NULL));
    do
    {
        r = rand() % board_size;
        if(bd->items[r] != SHIP_ITEM){
            bd->items[r] = SHIP_ITEM;
            ships_generated++;
        }


    } while (SHIP_COUNT!=ships_generated);


}

char is_hit(board *board, size_t pos){
    printf("Board attack position info: %d\n", pos);
    if(board->items[pos] == SHIP_ITEM || board->items[pos] == EMPTY_ITEM){
        if(board->items[pos] == SHIP_ITEM){
            board->items[pos]= HIT_ITEM;

            board->ship_alive--;


            printf("Ship count: %d\n", board->ship_alive);
            return HIT_ITEM;
        }
        else{

            board->items[pos]=MISSED_ITEM;

            return MISSED_ITEM;
        }
    }
    else
    {
        return INVALID_HIT;
    }

}


char* get_reduced_items(board *bd) {
    char* arr;

    arr = (char *) malloc(sizeof (bd->items));
    for (int i = 0; i < bd->rows * bd->cols; ++i) {
        if (bd->items[i] == SHIP_ITEM) {
            arr[i] = EMPTY_ITEM;
        } else {
            arr[i] = bd->items[i];
        }
    }

    return arr;
}


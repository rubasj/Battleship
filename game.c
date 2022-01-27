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

    if (!all_games || !*all_games || !name_1 || !name_2) {
        printf("Create game: missing parameter.\n");
        return NULL;
    }

    (*all_games) -> games_count++;
    printf("Number of games: %d\n\n", (*all_games) -> games_count);
    (*all_games) -> games = realloc((*all_games) -> games, (*all_games) -> games_count * sizeof(game));


    game *gm = NULL;

    srand(time(0));
    Board *tmp1 = board_create();

    Board *tmp2 = board_create();

    gm = (game*)malloc(sizeof(game));
    gm->name_1 = name_1;
    gm->name_2 = name_2;
    gm->player_1_on_turn = 1;
    gm->player_2_on_turn = 0;
    gm->player_1_ships = 17;
    gm->player_2_ships = 17;

    gm->b1 = tmp1;
    gm->b2 = tmp2;

    printf("Create game: OK");

    (*all_games) -> games[((*all_games) -> games_count) - 1] = gm;
    (*all_games) -> games[((*all_games) -> games_count) - 1] -> game_ID = ((*all_games) -> games_count) - 1;
    return gm;
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


    if (this_game->b1->ship_alive == 0){
        //player 1 lose
        char message_1[100];
        char message_2[100];

        sprintf(message_1, "GAME_OVER|%d\n", 0);
        sprintf(message_2, "GAME_OVER|%d\n", 1);// 1 - WINNER

        send_message(get_player_by_name(*array_clients, this_game->name_1)->socket_ID, message_1);
        send_message(get_player_by_name(*array_clients, this_game->name_2)->socket_ID, message_2);

        get_player_by_name(*array_clients, this_game->name_1)->state = IN_LOBBY;
        get_player_by_name(*array_clients, this_game->name_2)->state = IN_LOBBY;

        remove_game(all_games, this_game->game_ID);

    }
    else if(this_game->b2->ship_alive == 0){
        //player 2 lose
        char message_1[100];
        char message_2[100];

        sprintf(message_1, "GAME_OVER|%d\n", 1); // 1 - WINNER
        sprintf(message_2, "GAME_OVER|%d\n", 0);


        send_message(get_player_by_name(*array_clients, this_game->name_1)->socket_ID, message_1);
        send_message(get_player_by_name(*array_clients, this_game->name_2)->socket_ID, message_2);

        get_player_by_name(*array_clients, this_game->name_1)->state = IN_LOBBY;
        get_player_by_name(*array_clients, this_game->name_2)->state = IN_LOBBY;

        remove_game(all_games, this_game->game_ID);
    }

}



void remove_game(games **all_games, int game_ID) {
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


Board *board_create() {
    Board *temp = NULL;
    int i;
    temp = (Board *)malloc(sizeof(Board));
    if(!temp)
        return NULL;

    printf("OK MALLOC.");
    temp->x_size = BOARD_SIZE;
    temp->y_size = BOARD_SIZE;
    temp->board_array = (char *)malloc(sizeof(int) * temp->x_size * temp->y_size);
    for(i = 0; i < (temp->x_size * temp->y_size); i++){
        temp->board_array[i] = '0';
    }
    board_fill(temp);
    board_set(temp);
    temp->ship_alive = SHIP_COUNT;
    return temp;


}

void board_print(Board *b) {
    for (int i = 0; i < (b->x_size * b->y_size); i++) {
        if (i % 10 == 0)
            printf("\n");

        printf("%c ", b->board_array[i]);

    }
}

void board_fill(Board *bd) {
    size_t i;

    if (!bd || !bd->board_array) {
        return;
    }

    for (i = 0; i < bd->x_size * bd->y_size; ++i) {
        bd->board_array[i] = EMPTY_ITEM;
    }


}


void board_free(Board **poor) {
    if (!poor || !*poor)
        return;

    (*poor)->x_size = 0;
    (*poor)->y_size = 0;

    free((*poor)->board_array);
    (*poor)->board_array = NULL;

    free(*poor);
    *poor = NULL;
}


void board_set(Board *bd) {
    uint board_size = bd->y_size * bd->x_size;
    int ships_generated = 0;
    int r;

    do
    {
        r = rand() % (board_size - 0 + 1) ;
        if(bd->board_array[r] != SHIP_ITEM){
            bd->board_array[r] = SHIP_ITEM;
            ships_generated++;
        }


    } while (SHIP_COUNT!=ships_generated);

    printf("BOARD FILLED:\n%s\n", bd->board_array);

}

char is_hit(Board *board, size_t pos){
//    printf("Board attack position info: %s\n", board->board_array);
    if(board->board_array[pos]== '1' || board->board_array[pos]== '0'){
        if(board->board_array[pos] == '1'){
            board->board_array[pos]='3';
            board->ship_alive--;
            printf("Ship count: %d\n", board->ship_alive);
            return '3';
        }
        else
        {
            board->board_array[pos]='2';
            return '2';
        }
    }
    else
    {
        return '4';
    }


}


char* get_reduced_items(Board *bd) {
    char* arr;

    arr = (char *) malloc(sizeof (bd->board_array));
    for (int i = 0; i < bd->x_size * bd->y_size; ++i) {
        if (bd->board_array[i] == SHIP_ITEM) {
            arr[i] = EMPTY_ITEM;
        } else {
            arr[i] = bd->board_array[i];
        }
    }

    return arr;
}


//
// Created by janru on 10.01.2022.
//


#include "board.h"



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





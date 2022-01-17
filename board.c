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

    return tmp;


}


void board_fill(board *bd) {
    size_t i;

    if (!bd || !bd->items) {
        return;
    }

    for (i = 0; i < bd->cols * bd->rows; ++i) {
        bd->items[i] = '0';
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
    int board_size = bd->cols * bd->rows;
    int ships_generated = 0;
    int r;
    int i = 0;
    srand(time(NULL));
    do
    {
        r = rand() % board_size;
        if(bd->items[r] != '1'){
            bd->items[r] = '1';
            ships_generated++;
        }


    } while (SHIP_COUNT!=ships_generated);


}

int is_hit(board *board, size_t pos){
    printf("Board attack position info: %d\n", pos);
    if(board->items[pos]== '1' || board->items[pos]== '0'){
        if(board->items[pos] == '1'){
            board->items[pos]= '3';

            board->ship_alive--;


            printf("Ship count: %d\n", board->ship_alive);
            return '3';
        }
        else{

            board->items[pos]='2';

            return '2';
        }
    }
    else
    {
        return '4';
    }


}
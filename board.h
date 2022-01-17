//
// Created by janru on 10.01.2022.
//


#ifndef BATTLESHIP_BOARD_H
#define BATTLESHIP_BOARD_H


#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>


#define BOARD_SIZE 10
#define SHIP_COUNT 12
typedef unsigned int uint;


typedef struct _board {
    size_t rows;
    size_t cols;
    char *items;
    uint ship_alive;

}board;

board *board_create();
void board_fill(board *bd);
void board_set(board *bd);
int is_hit(board *board, size_t pos);



#endif //BATTLESHIP_BOARD_H

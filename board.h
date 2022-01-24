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
#define SHIP_COUNT 17
typedef unsigned int uint;

#define EMPTY_ITEM '0'
#define SHIP_ITEM '1'
#define HIT_ITEM '3'
#define MISSED_ITEM '2'
#define INVALID_HIT '4'

typedef struct _board {
    size_t rows;
    size_t cols;
    char *items;
    uint ship_alive;

}board;

board *board_create();
void board_fill(board *bd);
void board_set(board *bd);
char is_hit(board *board, size_t pos);
void board_free(board **poor);

/**
 * For creating opponent array without ships.
 * @param bd
 * @return
 */
char* get_reduced_items(board *bd); // for opponent


#endif //BATTLESHIP_BOARD_H

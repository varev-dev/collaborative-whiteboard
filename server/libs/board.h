#ifndef BOARD_H
#define BOARD_H

#include <stdlib.h> // for size_t

typedef struct color Color;

typedef struct board {
    Color **data;
    size_t  rows;
    size_t  cols;
} Board;

Board* board_create(unsigned int rows, unsigned int cols);
void   board_free(Board **board);
void   board_print(const Board *board);

#endif //BOARD_H

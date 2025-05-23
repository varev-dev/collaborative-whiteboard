#include "board.h"
#include "utils.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

Board* board_create(unsigned int rows, unsigned int cols) {
    Board *board = malloc(sizeof(Board));
    if (board == NULL) {
        return NULL;
    }

    board->rows = rows;
    board->cols = cols;

    board->data = malloc(rows * sizeof(Color*));
    if (board->data == NULL) {
        free(board);
        return NULL;
    }
    memset(board->data, 0, rows * sizeof(Color*));

    for (int i = 0; i < rows; i++) {
        board->data[i] = malloc(cols * sizeof(Color));
        if (board->data[i] == NULL) {
            board_free(&board);
            return NULL;
        }

        memset(board->data[i], 0, cols * sizeof(Color));
    }

    return board;
}

void board_free(Board **board) {
    assert(board != NULL);

    if (*board == NULL) {
        return;
    }

    if ((*board)->data != NULL) {
        for (int i = 0; i < (*board)->rows; i++) {
            free((*board)->data[i]);
        }

        free((*board)->data);
    }

    free(*board);
    *board = NULL;
}

void board_print(const Board *board) {
    assert(board != NULL && board->data != NULL);

    for (int i = 0; i < board->rows; i++) {
        for (int j = 0; j < board->cols; j++) {
            printf("(%i %i %i) ", board->data[i][j].r, board->data[i][j].g, board->data[i][j].b);
        }

        printf("\n");
    }
}
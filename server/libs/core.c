#include "core.h"

static unsigned int current_id = 0;

Connection* connection_create(Board* board, int socket) {
    if (board == NULL) {
        error("Board cannot be NULL.\n");
        return NULL;
    }

    Connection *conn = malloc(sizeof(Connection));
    if (conn == NULL) {
        return NULL;
    }

    conn->id = current_id++;
    conn->board = board;
    conn->socket = socket;

    return conn;
}

void connection_close(Connection** conn) {
    if (conn == NULL || *conn == NULL) {
        return;
    }

    close((*conn)->socket);
    free(*conn);

    *conn = NULL;
}

Board* board_create(size_t rows, size_t cols) {
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
            board_destroy(&board);
            return NULL;
        }

        memset(board->data[i], 0, cols * sizeof(Color));
    }

    return board;
}

void board_destroy(Board** board) {
    if (board == NULL || *board == NULL) {
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

void board_print(Board* board) {
    if (board == NULL || board->data == NULL) {
        error("Trying to print a NULL board");
        return;
    }

    for (int i = 0; i < board->rows; i++) {
        for (int j = 0; j < board->cols; j++) {
            printf("(%i %i %i) ", board->data[i][j].r, board->data[i][j].g, board->data[i][j].b);
        }

        printf("\n");
    }
}

void error(const char* format, ...) {
    va_list args;

    fprintf(stderr, "[ERROR]: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void logg(const char* format, ...) {
    va_list args;

    fprintf(stdout, "[LOG]: ");

    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void die(const char* format, ...) {
    va_list args;

    fprintf(stderr, "[EXIT]: ");

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    exit(0);
}

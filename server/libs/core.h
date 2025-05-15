#ifndef CORE_H
#define CORE_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>

#define BUFFER_SIZE 4096

typedef struct color_t{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef struct board_t {
    Color** data;
    size_t  rows;
    size_t  cols;
} Board;

typedef struct connection_t {
    unsigned int id;
    char         readbuf[BUFFER_SIZE];
    char         writebuf[BUFFER_SIZE];
    int          socket;
    Board*       board;
} Connection;

Connection* connection_create(Board* board, int socket);
void        connection_close(Connection** conn);

Board* board_create(size_t rows, size_t cols);
void   board_destroy(Board** board);
void   board_print(Board* board);

void error(const char* format, ...);
void logg(const char* format, ...);
void die(const char* format, ...);

#endif //CORE_H

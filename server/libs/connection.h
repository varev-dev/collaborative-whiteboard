#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdbool.h>

#define BUFFER_SIZE 4096

typedef struct connection {
    unsigned int id;
    int          fd;
    char         readbuf[BUFFER_SIZE];
    char         writebuf[BUFFER_SIZE];
    bool         should_close;
} Connection;

Connection* connection_create(int socket);
void        connection_close(Connection **conn);
int         connection_cmp(void *a, void *b);

#endif //CONNECTION_H

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdbool.h>

typedef struct buffer Buffer;

typedef struct connection {
    unsigned int id;
    int          fd;
    Buffer      *readbuf;
    Buffer      *writebuf;
    bool         should_close;
} Connection;

Connection* connection_create(int socket);
void        connection_free(Connection **conn);
int         connection_cmp(void *a, void *b);

#endif //CONNECTION_H

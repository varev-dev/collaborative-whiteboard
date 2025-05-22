#include "connection.h"
#include "board.h"
#include "utils.h"
#include <unistd.h>

static unsigned int current_id = 0;

Connection* connection_create(int socket) {
    Connection *conn = malloc(sizeof(Connection));
    if (conn == NULL) {
        return NULL;
    }

    conn->id = current_id++;
    conn->fd = socket;
    conn->should_close = false;

    return conn;
}

void connection_close(Connection **conn) {
    if (conn == NULL || *conn == NULL) {
        return;
    }

    close((*conn)->fd);
    free(*conn);

    *conn = NULL;
}

int connection_cmp(void *a, void *b) {
    Connection *ca = a;
    Connection *cb = b;

    if (ca->id < cb->id) return -1;
    if (ca->id > cb->id) return 1;

    if (ca->fd < cb->fd) return -1;
    if (ca->fd > cb->fd) return 1;

    return 0;
}
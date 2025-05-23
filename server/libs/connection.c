#include "connection.h"
#include "board.h"
#include "utils.h"
#include "buffer.h"

#include <assert.h>
#include <unistd.h>

static unsigned int current_id = 0;

Connection* connection_create(int socket) {
    Connection *c = malloc(sizeof(Connection));
    if (c == NULL) {
        return NULL;
    }

    c->id = current_id++;
    c->fd = socket;
    c->should_close = false;
    c->readbuf = buffer_create();
    c->writebuf = buffer_create();

    if (c->readbuf == NULL || c->writebuf == NULL) {
        buffer_free(&c->writebuf);
        buffer_free(&c->readbuf);
        free(c);
        return NULL;
    }

    return c;
}

void connection_free(Connection **conn) {
    assert(conn != NULL);
    if (*conn == NULL) {
        return;
    }

    Connection *c = *conn;

    buffer_free(&c->writebuf);
    buffer_free(&c->readbuf);
    close(c->fd);
    free(c);

    *conn = NULL;
}

int connection_cmp(void *a, void *b) {
    assert(a != NULL && b != NULL);

    Connection *ca = a;
    Connection *cb = b;

    if (ca->id < cb->id) return -1;
    if (ca->id > cb->id) return 1;

    if (ca->fd < cb->fd) return -1;
    if (ca->fd > cb->fd) return 1;

    return 0;
}
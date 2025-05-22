#include "server.h"
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include "utils.h"
#include "dllist.h"
#include "board.h"
#include "connection.h"

void close_connection(void *conn, void *server);
int  create_server_socket(struct sockaddr_in *serv_addr);
int  set_fd_nonblock(int fd);

Server* server_create(in_addr_t addr, uint16_t port) {
    struct sockaddr_in serv_addr = {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = addr;

    Server *s = malloc(sizeof(Server));
    if (s == NULL) {
        error("Allocating Server.\n");
        return NULL;
    }

    s->fd = -1;
    s->epoll_fd = -1;
    s->events = NULL;
    s->board = NULL;
    s->connections = NULL;
    s->max_events = MAX_EVENTS;
    s->event_ptr = NULL;

    s->fd = create_server_socket(&serv_addr);
    if (s->fd == -1) {
        free(s);
        return NULL;
    }

    s->epoll_fd = epoll_create1(0);
    if (s->epoll_fd == -1) {
        error("Creating epoll fd: %s\n", ERRNO_STR);
        server_destroy(&s);
        return NULL;
    }

    s->events = malloc(s->max_events * sizeof(struct epoll_event));
    if (s->events == NULL) {
        error("Creating epoll events: %s\n", ERRNO_STR);
        server_destroy(&s);
        return NULL;
    }

    s->event_ptr = connection_create(s->fd);
    if (s->event_ptr == NULL) {
        error("Creating server's connection pointer for epoll: %s\n", ERRNO_STR);
        server_destroy(&s);
        return NULL;
    }

    struct epoll_event e;
    e.events = EPOLLIN;
    e.data.ptr = s->event_ptr;
    if (epoll_ctl(s->epoll_fd, EPOLL_CTL_ADD, s->fd, &e) == -1) {
        error("Adding listening socket to epoll: %s\n", ERRNO_STR);
        server_destroy(&s);
        return NULL;
    }

    s->connections = dllist_create();
    if (s->connections == NULL) {
        error("Creating a connections list.\n");
        server_destroy(&s);
        return NULL;
    }

    return s;
}

void server_destroy(Server **server) {
    if (server == NULL || *server == NULL) {
        return;
    }

    Server *s = *server;
    if (s->fd > -1) {
        close(s->fd);
    }

    if (s->epoll_fd > -1) {
        close(s->epoll_fd);
    }

    board_destroy(&s->board);
    dllist_clear(s->connections, close_connection, *server);
    dllist_destroy(&s->connections);
    connection_close(&s->event_ptr);

    free(s->events);
    free(s);
    *server = NULL;
}

int server_add_connection(Server *server, Connection *conn) {
    if (server == NULL || conn == NULL) {
        return -1;
    }

    if (set_fd_nonblock(conn->fd) == -1) {
        error("Setting connection fd as nonblocking: %s\n", ERRNO_STR);
        return -1;
    }

    struct epoll_event e;
    e.events = EPOLLIN | EPOLLET;
    e.data.ptr = conn;
    if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, conn->fd, &e) == -1) {
        error("Adding connection to epoll: %s\n", ERRNO_STR);
        return -1;
    }

    dllist_push(server->connections, conn);
    return 0;
}

int server_set_connection_mode(Server *server, Connection *conn, SocketMode mode) {
    if (conn == NULL || server == NULL) {
        return -1;
    }

    struct epoll_event e;
    e.events = mode | EPOLLET;
    e.data.ptr = conn;
    return epoll_ctl(server->epoll_fd, EPOLL_CTL_MOD, conn->fd, &e);
}

int create_server_socket(struct sockaddr_in *serv_addr) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        error("Create a listening socket: %s\n", ERRNO_STR);
        return -1;
    }

    if (set_fd_nonblock(fd) == -1) {
        error("Setting the listening socket as nonblocking: %s\n", ERRNO_STR);
        close(fd);
        return -1;
    }

    if (bind(fd, (const struct sockaddr*)serv_addr, sizeof(*serv_addr)) == -1) {
        error("Binding the listening socket: %s\n", ERRNO_STR);
        close(fd);
        return -1;
    }

    if (listen(fd, SOMAXCONN) == -1) {
        error("Listening on the listening socket: %s\n", ERRNO_STR);
        close(fd);
        return -1;
    }

    return fd;
}

int set_fd_nonblock(int fd) {
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

void close_connection(void *conn, void *server) {
    Server *s = server;
    Connection *c = conn;

    epoll_ctl(s->epoll_fd, EPOLL_CTL_DEL, c->fd, NULL);
    connection_close(&c);
}

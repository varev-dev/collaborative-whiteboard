#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

typedef struct board Board;
typedef struct dllist DLList;
typedef struct connection Connection;

typedef struct server {
    int                 fd;
    int                 epoll_fd;
    struct epoll_event *events;
    Board              *board;
    DLList             *connections;
    int                 max_events;
    Connection         *event_ptr; // only for epoll_event.ptr. DO NOT USE
} Server;

typedef enum socket_mode {
    READ = EPOLLIN,
    WRITE = EPOLLOUT
} SocketMode;

Server* server_create(in_addr_t addr, uint16_t port);
void    server_destroy(Server **server);
int     server_add_connection(Server *server, Connection *conn);
int     server_set_connection_mode(Server *server, Connection *conn, SocketMode mode);

#endif //SERVER_H

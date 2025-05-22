#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include "utils.h"
#include "dllist.h"
#include "board.h"
#include "connection.h"
#include "server.h"

int main() {
	// create server
	Server *s = server_create(htonl(INADDR_ANY), 5000);

	while (true) {
		// wait for events
		int count = epoll_wait(s->epoll_fd, s->events, s->max_events, -1);
		if (count == -1) {
			error("Waiting for events: %s\n", ERRNO_STR);
			break;
		}

		for (int i = 0; i < count; i++) {
			if (s->events[i].data.ptr == s->event_ptr) {
				// if accept
				int conn_fd = accept(s->fd, NULL, NULL);
				if (conn_fd == -1) {
					error("Accepting a connection: %s\n", ERRNO_STR);
					continue;
				}

				// create connection
				Connection *c = connection_create(conn_fd);
				if (c == NULL) {
					error("Creating a connection: %s\n", ERRNO_STR);
					close(conn_fd);
					continue;
				}

				// add connection to server
				if (server_add_connection(s, c) == -1) {
					connection_close(&c);
				}

				if (c != NULL) {
					logg("Connection %i accepted.\n", c->id);
				}

				continue;
			}

			if (s->events[i].events & READ) {
				// if read
				assert(s->events[i].data.ptr != NULL);

				Connection *c = s->events[i].data.ptr;

				// read from connection
				size_t size = read(c->fd, c->readbuf, BUFFER_SIZE - 1);
				if (size == -1) {
					error("Reading from socket: %s\n", ERRNO_STR);
					continue;
				}

				c->readbuf[size] = '\0';
				// decode message
				// (create session, join session, update board, get board)

				memcpy(c->writebuf, c->readbuf, size + 1);
				for (int i = 0; i < size; i++) {
					if (c->writebuf[i] >= 'a' && c->writebuf[i] <= 'z') {
						c->writebuf[i] -= 32;
					}
				}

				server_set_connection_mode(s, c, READ | WRITE);
			}

			if (s->events[i].events & WRITE) {
				// if write
				assert(s->events[i].data.ptr != NULL);

				Connection *c = s->events[i].data.ptr;

				// write to connection
				size_t size = write(c->fd, c->writebuf, strlen(c->writebuf));
				if (size == -1) {
					error("Writing to socket: %s\n", ERRNO_STR);
					continue;
				}

				server_set_connection_mode(s, c, READ);
			}
			// if close
		}
	}

	server_destroy(&s);
}

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
#include "buffer.h"
#include "connection.h"
#include "server.h"

int main() {
	// create server
	Server *s = server_create(htonl(INADDR_ANY), 5000);
	char temp_buf[TEMP_BUFFER_SIZE];

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
					connection_free(&c);
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
				size_t size = read(c->fd, temp_buf, TEMP_BUFFER_SIZE - 1);
				if (size == -1) {
					error("Reading from socket: %s\n", ERRNO_STR);
					continue;
				}

				buffer_append(c->readbuf, temp_buf, size);
				// decode message
				// (create session, join session, update board, get board)
			}

			if (s->events[i].events & WRITE) {
				// if write
				assert(s->events[i].data.ptr != NULL);

				Connection *c = s->events[i].data.ptr;

				// write to connection
				size_t size = write(c->fd, c->writebuf->data_end, strlen((const char*)c->writebuf->data_begin));
				if (size == -1) {
					error("Writing to socket: %s\n", ERRNO_STR);
					continue;
				}

				buffer_consume(c->writebuf, size);
				if (buffer_isempty(c->writebuf)) {
					server_set_connection_mode(s, c, READ);
				}
			}
			// if close
		}
	}

	server_free(&s);
}
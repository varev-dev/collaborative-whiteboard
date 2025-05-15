#include "libs/core.h"
#include "libs/dllist.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* connection_handler(void *conn);
void  close_connections(DLList **connections);
void  close_connection(void *connection);

int main(int argc, char* argv[]) {
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1) {
		die("Failed to create a listening socket.\n");
	}

	struct sockaddr_in serv_addr = {};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000); 

	if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		die("Failed to bind the listening socket.\n");
	}

	if (listen(listenfd, 10) == -1) {
		die("Failed to listen on the listening socket.\n");
	}

	Board *board = board_create(10, 10);
	if (board == NULL) {
		die("Failed to create a board.\n");
	}

	DLList *connections = dllist_create();
	if (connections == NULL) {
		die("Failed to create connections list.\n");
	}

    board_print(board);

	int connfd;
	pthread_t thread_id;

	for (;;) {
		connfd = accept(listenfd, NULL, NULL);
		if (connfd == -1) {
			continue;
		}

		Connection *connection = connection_create(board, connfd);
		if (connection == NULL) {
			error("Failed to create a connection.\n");
			continue;
		}

		dllist_push(connections, connection);

		fprintf(stdout, "Connection accepted\n");
		pthread_create(&thread_id, NULL, connection_handler, connection);
	}

    board_destroy(&board);
	dllist_clear(connections, close_connection);
	dllist_destroy(&connections);

	pthread_mutex_destroy(&lock);
}

void* connection_handler(void *conn) {
	Connection *connection = conn;
	size_t read_size;
	
	do {
		read_size = recv(connection->socket, connection->readbuf, BUFFER_SIZE - 1, 0);
		if (read_size == 0) {
			break;
		}

		connection->readbuf[read_size] = '\0';

		for (int i = 0; i < read_size; i++) {
			if (connection->readbuf[i] >= 'a' && connection->readbuf[i] <= 'z')
			connection->readbuf[i] -= 32;
		}

		// handle_request(connection->readbuf);
		write(connection->socket, connection->readbuf, read_size);

		memset(connection->readbuf, 0, BUFFER_SIZE);
	} while(read_size > 2);
	
	fprintf(stderr, "Client disconnected\n");
	
	connection_close(&connection);
	pthread_exit(NULL);
}

void close_connection(void *connection) {
	connection_close((Connection**)&connection);
}
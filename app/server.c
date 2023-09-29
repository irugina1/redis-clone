#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>


#include "parse_resp.h"
#include "resp_to_cmd.h"
#include "hashmap.h"

void *handle_client(void *args){
	int client_socket = *( (int* )args);  // cast pointer and then dereference
	// handle the client
	char buffer[1024];
	while(1){
		// get request from client
		ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
		if (bytes_received <= 0) {
			printf("client disconnected: %s\n", strerror(errno));
			break;
		}
		printf("\nreceived %zu bytes\n", bytes_received);
		buffer[bytes_received] = '\0'; // null-terminate
		const char *ptr = buffer;
		print_raw(ptr);
		printf("\n");
		// parse the request
		// TODO free memory after i'm done with obj below
		resp_object_t * obj = parse_resp(&ptr);
		cmd_object_t * cmd = resp_to_command(obj);
		if (cmd->type == CMD_PING) {
			char *reply = "+PONG\r\n";
			if (send(client_socket, reply, strlen(reply), 0) == -1){
				printf("replying to client failed: %s\n", strerror(errno));
				break;
			}
		}
		if (cmd->type == CMD_ECHO) {
			char* echo_message = cmd->args[1];
			char* reply = malloc(strlen(echo_message) + 2);
			sprintf(reply, "+%s", echo_message);
			if (send(client_socket, reply, strlen(reply), 0) == -1){
				printf("replying to client failed: %s\n", strerror(errno));
				break;
			}
		}
		else{

		}
    }
	close(client_socket);
	free(args);
	return NULL;
}


int main() {
	// Disable output buffering
	setbuf(stdout, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// Uncomment this block to pass the first stage
	int server_fd;
	socklen_t client_addr_len;
	struct sockaddr_in client_addr;
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}
	
	// Since the tester restarts your program quite often, setting REUSE_PORT
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEPORT failed: %s \n", strerror(errno));
		return 1;
	}
	
	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
									 .sin_port = htons(6379),
									 .sin_addr = { htonl(INADDR_ANY) },
									};
	
	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}
	
	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}
	
	client_addr_len = sizeof(client_addr);

    while (1){
		int *client_socket_ptr = malloc(sizeof(int));
		*client_socket_ptr = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);
		if (*client_socket_ptr == -1) {
			printf("accept failed: %s\n", strerror(errno));
			free(client_socket_ptr);
			continue;
		}

		pthread_t thread_id;
		if (pthread_create(&thread_id, NULL, handle_client, client_socket_ptr) != 0){
			printf("failed to create thread %s\n", strerror(errno));
			free(client_socket_ptr);
		}
		pthread_detach(thread_id);
	}

	close(server_fd);
	return 0;
}

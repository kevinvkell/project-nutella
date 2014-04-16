//Kevin Kell
// 3/21/13
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include "server.h"
#include "msock.h"

#define MULTICAST_ADDR "239.0.0.1"
#define MULTICAST_PORT 10000
#define PLAYER_PORT 10001

int player();
int watch_movie();
int request_accepted(char *response);

int main(int argc, char* argv[]){
	int pid;

	pid = fork();
	if(pid < 0) {
		perror("fork");
		exit(1);
	}
	if(pid > 0) {
		printf("forked process %d\n\n", pid);
		player();
		kill(pid, SIGTERM);
		wait(&pid);
		exit(0);
	}
	else {
		server();
		exit(0);
	}
}

int player() {
//	struct addrinfo *results;
//	struct addrinfo hints;
	char message[512];
	char input[512];
//	char buffer[10000];
//	int socket_descriptor;
//	int bytes;

	int sock;

	while(1){
		fgets(input, sizeof input, stdin);

		if ((sock = msockcreate(SEND, MULTICAST_ADDR, MULTICAST_PORT)) < 0) {
			perror("msockcreate");
			exit(1);
		}

		sprintf(message, "%d\t%s\t%d\t", REQUEST_KVK, input, PLAYER_PORT); 

		if(msend(sock, message, strlen(message) + 1) < 0){
			perror("msend");
			exit(1);
		}
	}
	return 0;
/*
//clear the hints struct
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

//fill in the results struct using the hints
	if(getaddrinfo(server_ip, server_port, &hints, &results) != 0){
		perror("getaddrinfo");
		exit(1);
	}

//get the socket descriptor using the information gained from getaddrinfo
	if((socket_descriptor = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) == -1){
		perror("socket");
		exit(1);
	}

//connect to the peers 
	if (connect(socket_descriptor, results->ai_addr, results->ai_addrlen) == -1) {
		close(socket_descriptor);
		perror("connect");
		exit(1);
	}

	fprintf(stderr, "connected to server\nsending cridentials...\n");

	memset(message, 0, sizeof(message));
	strcpy(message, input);

	if(send(socket_descriptor, message, (sizeof(message) + 1), 0) == -1){
		perror("send");
		exit(1);
	}

	memset(buffer, 0, sizeof(buffer));

	if((bytes = recv(socket_descriptor, buffer, sizeof(buffer), 0)) == -1){
		perror("recv");
		exit(1);
	}

	if(request_accepted(buffer)) {
		watch_movie();
		printf("movie over\n");
		return 0;
	}
	else{
		printf("movie not found\n");
		close(socket_descriptor);
		freeaddrinfo(results);
		return 0;
	}
*/
}

int request_accepted(char *response){
	return 0;
}

int watch_movie(){
	return 0;
}

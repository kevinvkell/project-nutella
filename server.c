//Kevin Kell
//3/25/13
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <time.h>
#include "msock.h"
#include "server.h"

#define MULTICAST_ADDR "239.0.0.1"
#define MULTICAST_PORT 10000
#define BUFF_SIZE 100
#define CONNECTIONS 5
#define PASSWORD "password"
#define USERNAME "kvkell"

int send_movie();
int has_movie_title(char *title);

int server(){
  
//	struct sockaddr_storage client_address;
//	struct addrinfo *results;
//	struct addrinfo hints;
	char buffer[10000];
	char input[1000];
	char *token;
//	int socket_descriptor;
//	int new_socket_descriptor;
	int bytes;
//	int pid;
//	socklen_t address_size;
//	char port[10] = "9191";

	int sock;
//	char message[BUFF_SIZE];

	if ((sock = msockcreate(RECV, MULTICAST_ADDR, MULTICAST_PORT)) < 0) {
		perror("msockcreate");
		exit(1);
	}

	while(1){
		char *name;
		int port;
		struct sockaddr_in info;
		char address[INET_ADDRSTRLEN];

		memset(input, 0, sizeof input);

		bytes = mrecv(sock, &info, buffer, sizeof buffer);

		inet_ntop(AF_INET, &info, address, INET_ADDRSTRLEN);
		printf("\nsender address: %s\n", address);


		printf("received %d bytes\n", bytes);
		strcpy(input, buffer);

		token = strtok(input, "\t");
		if(atoi(token) == REQUEST_KVK){
			printf("request:\n");

			token = strtok(NULL, "\t\n");
			printf("movie name: %s\n", token);
			name = token;

			token = strtok(NULL, "\t");
			printf("port number: %s\n", token);
			port = atoi(token);

			if(has_movie_title(name)){
				printf("I have the movie\n");
			}
			else{
				printf("I do not have the movie\n");
			}
		}
		else{
			printf("response:\n");
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
	if(getaddrinfo(NULL, port, &hints, &results) != 0){
		perror("getaddrinfo");
		exit(1);
	}

	//get the socket descriptor using the information gained from getaddrinfo
	if((socket_descriptor = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) == -1){
		perror("socket");
		exit(1);
	}

	//bind with the port we will be listening from
	if(bind(socket_descriptor, results->ai_addr, results->ai_addrlen) == -1){
		perror("bind");
		exit(1);
	}

	printf("listening...\n");

	//listen for communication
	if(listen(socket_descriptor, CONNECTIONS) == -1){
		perror("listen");
		exit(1);
	}

	while(1) {
//wait for any children
		pid = waitpid(-1, NULL, WNOHANG);
//accept any connection
		address_size = sizeof(client_address);
		if((new_socket_descriptor = accept(socket_descriptor, (struct sockaddr *)&client_address, &address_size)) == -1){
			perror("accept");
			exit(1);
		}
		printf("accepted connection\n");

		if((bytes = recv(new_socket_descriptor, buffer, sizeof(buffer), 0)) == -1){
			perror("recv");
			exit(1);
		}

		if(has_movie_title(buffer)){
			pid = fork();
			if(pid < 0) {
				perror("fork");
				exit(1);
			}
			if(pid > 0) {
				printf("forked process %d\n\n", pid);
				close(new_socket_descriptor);
			}
			else {
				if(send_movie() != 0) {
					exit(1);
				}
				else {
					exit(0);
				}
			}
		}
		else{
			close(new_socket_descriptor);
		}
	}
*/
}

int has_movie_title(char *title){
	char file[strlen("./") + strlen(title)];
	struct stat file_info;

	strcpy(file, "./");
	strcat(file, title);

	if(stat(file, &file_info) != 0){
		perror("stat");
		return 0;
	}
	else{
		return 1;
	}
}

int send_movie(){
	return 0;
}

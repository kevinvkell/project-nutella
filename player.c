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
#include <time.h>
#include "server.h"
#include "msock.h"

#define MULTICAST_ADDR "239.0.0.1"
#define MULTICAST_PORT 10006

int player(int id);
int watch_movie(char *address, char *port);
int request_accepted(char *response);

int main(int argc, char* argv[]){
	int pid;
	int id;

	srand(time(NULL));
	id = rand();

	pid = fork();
	if(pid < 0) {
		perror("fork");
		exit(1);
	}
	if(pid > 0) {
		printf("forked process %d\n\n", pid);
		player(id);
		kill(pid, SIGTERM);
		wait(&pid);
		exit(0);
	}
	else {
		server(id);
		exit(0);
	}
}

int player(int id) {
	char message[512];
	char input[512];
	int sock;
	struct sockaddr_in info;
	char address[INET_ADDRSTRLEN];
	char *token;
	char *port;
	int i;
	char *name;
	int bytes;
	int sender_id;

	while(1){
		fgets(input, sizeof input, stdin);

		if(strcmp(input, "exit\n") == 0){
			msockdestroy(sock);
			exit(0);
		}

		if ((sock = msockcreate(SEND, MULTICAST_ADDR, MULTICAST_PORT)) < 0) {
			perror("msockcreate");
			exit(1);
		}

		sprintf(message, "%d\t%s\t%d", REQUEST_KVK, input, id); 

		if(msend(sock, message, strlen(message) + 1) < 0){
			perror("msend");
			exit(1);
		}

		i = 0;
		while(1){

			printf("waiting for response from peers %d\n", i);

	                if((bytes = mrecv(sock, &info, message, sizeof message)) < 0){
				perror("mrecv");
			}

			printf("received response:\n%s\n", message);

			inet_ntop(AF_INET, &(info.sin_addr), address, INET_ADDRSTRLEN);
			printf("\nsender address: %s\n", address);

			token = strtok(message, "\t");
			if(atoi(token) == RESPONSE_KVK){
				printf("response:\n");

				token = strtok(NULL, "\t\n");
				name = token;

				token = strtok(NULL, "\t");
				port = token;

				token = strtok(NULL, "\t");
				sender_id = atoi(token);
				if(atoi(token) == id){
					break;
				}

				printf("NAME: %s\nPORT: %s\nINPUT: %s\nID: %d\n", name, port, input, sender_id);

				if(strcmp(name, strtok(input, "\n")) == 0){
					watch_movie(address, port);
					break;
				}
			}
			if(i++ > 2){
				break;
			}
			sleep(1);
		}
	}
	return 0;
}

int request_accepted(char *response){
	return 0;
}

int watch_movie(char *address, char *port){
	struct addrinfo *results;
	struct addrinfo hints;
	char message[512];
	int socket_descriptor;
	int socket_descriptor_receive;
	int bytes;
	struct sockaddr_storage other_address;
	socklen_t address_length = sizeof other_address;

	printf("watching movie:\n");

//clear the hints struct
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

//fill in the results struct using the hints
	if(getaddrinfo(address, port, &hints, &results) != 0){
		perror("getaddrinfo");
		exit(1);
	}

//get the socket descriptor using the information gained from getaddrinfo
	if((socket_descriptor = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) == -1){
		perror("socket");
		exit(1);
	}

	memset(message, 0, sizeof(message));
	strcpy(message, "lets do this");
	if(sendto(socket_descriptor, message, (strlen(message) + 1), 0, results->ai_addr, results->ai_addrlen) == -1){
		perror("sendto");
		exit(1);
	}

	close(socket_descriptor);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

//fill in the results struct using the hints
	if(getaddrinfo(NULL, port, &hints, &results) != 0){
		perror("getaddrinfo");
		exit(1);
	}

//get the socket descriptor using the information gained from getaddrinfo
	if((socket_descriptor_receive = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) == -1){
		perror("socket");
		exit(1);
	}

	const int true_value = 1;
	setsockopt(socket_descriptor_receive, SOL_SOCKET, SO_REUSEADDR, &true_value, sizeof(int));

	if(bind(socket_descriptor_receive, results->ai_addr, results->ai_addrlen) == -1){
		perror("bind");
		exit(1);
	}

	printf("bound to socket...\n");

	memset(message, 0, sizeof message);
	while(1){
		printf("looking for messages...\n");

		if((bytes = recvfrom(socket_descriptor_receive, message, sizeof message, 0, (struct sockaddr *) &other_address, &address_length)) == -1){
			perror("recvfrom");
			exit(1);
		}

		printf("received message\n%s\n", message);
		printf("\033[2J");
		printf("\033[0;0f");
		if(strcmp(message, "end of movie") == 0){
			printf("THE END\n");
			return 0;
		}
		else{
			printf("%s\n", message);
		}
	}

	return 0;
}

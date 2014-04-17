//Kevin Kell
//3/25/13
#define _XOPEN_SOURCE
#define _GNU_SOURCE

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
#define MULTICAST_PORT 10006
#define PLAYER_PORT "10007"

int send_movie(char *title, char *address);
int has_movie_title(char *title);

int server(int id){
  
	char buffer[10000];
	char input[1000];
	char *token;
	int bytes;
	int sock;
	char message[512];
	int sender_id;

	if ((sock = msockcreate(RECV, MULTICAST_ADDR, MULTICAST_PORT)) < 0) {
		perror("msockcreate");
		exit(1);
	}

	while(1){
		char *name;
		struct sockaddr_in info;
		char address[INET_ADDRSTRLEN];

		memset(input, 0, sizeof input);

		bytes = mrecv(sock, &info, buffer, sizeof buffer);

		inet_ntop(AF_INET, &(info.sin_addr), address, INET_ADDRSTRLEN);
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
			sender_id = atoi(token);
			if(id == sender_id){
				continue;
			}

			if(has_movie_title(name)){
				memset(message, 0, sizeof(message));
				sprintf(message, "%d\t%s\t%s\t%d", RESPONSE_KVK, name, PLAYER_PORT, id);
				msend(sock, message, (strlen(message) + 1));
				printf("I have the movie\n");
				send_movie(name, address);
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

int send_movie(char *title, char *address){
	struct addrinfo *results;
	struct addrinfo hints;
	char message[512];
	int socket_descriptor;
	int bytes;
	char *line = NULL;
	size_t size = 0;
	FILE *movie_file;
	char file_name[strlen("./") + strlen(title)];
	int i = 0;
        struct sockaddr_storage other_address;
        socklen_t address_length = sizeof other_address;


	printf("sending movie\n");

//clear the hints struct
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;

//fill in the results struct using the hints
        if(getaddrinfo(NULL, PLAYER_PORT, &hints, &results) != 0){
                perror("getaddrinfo");
                exit(1);
        }

//get the socket descriptor using the information gained from getaddrinfo
        if((socket_descriptor = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) == -1){
                perror("socket");
                exit(1);
        }

	if(bind(socket_descriptor, results->ai_addr, results->ai_addrlen) == -1){
		perror("bind");
		exit(1);
	}

	i = 0;
	while(1){

		printf("waiting for response %d\n", i);

		memset(message, 0, sizeof(message));
		if((bytes = recvfrom(socket_descriptor, message, sizeof message, MSG_DONTWAIT, (struct sockaddr *) &other_address, &address_length)) == -1){
			perror("recvfrom");
		}

		printf("received response:\n%s\n", message);
		if(strcmp(message, "lets do this") == 0){
			break;
		}

		if(i++ > 2){
			return 0;
		}

		sleep(1);
	}

	close(socket_descriptor);

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;

//fill in the results struct using the hints
        if(getaddrinfo(address, PLAYER_PORT, &hints, &results) != 0){
                perror("getaddrinfo");
                exit(1);
        }

//get the socket descriptor using the information gained from getaddrinfo
        if((socket_descriptor = socket(results->ai_family, results->ai_socktype, results->ai_protocol)) == -1){
                perror("socket");
                exit(1);
        }


	strcpy(file_name, "./");
	strcat(file_name, title);

	if((movie_file = fopen(file_name, "r")) == NULL){
		perror("fopen");
		exit(1);
	}

//	printf("opened file %s\n", file_name);

        memset(message, 0, sizeof(message));
	while((bytes = getline(&line, &size, movie_file)) > 0){

//		printf("read line:\n%s\n", line);

		if(strcmp(line, "end\n") == 0){

//			printf("found exit line\n");

			if(sendto(socket_descriptor, message, (strlen(message) + 1), 0, results->ai_addr, results->ai_addrlen) == -1){
				perror("sendto");
				exit(1);
			}

//			printf("sent message:\n%s\n", message);

			sleep(1);
			free(line);
			line = NULL;
			memset(message, 0, sizeof(message));
			continue;
		}
		strcat(message, line);
//		printf("new message: %s\n", message);
		free(line);
		line = NULL;
	} 

	memset(message, 0, sizeof(message));
	strcpy(message, "end of movie");
	if(sendto(socket_descriptor, message, (strlen(message) + 1), 0, results->ai_addr, results->ai_addrlen) == -1){
		perror("sendto");
		exit(1);
	}

	printf("movie sent\n");	
	fclose(movie_file);

	return 0;
}

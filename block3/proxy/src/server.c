#include "proxy.h"

#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_SERVER_QUEUE_SIZE 10

void* clientThread(void* arg) {
	int clientSocket = *(int*)arg;
	free(arg);
	
	handleRequest(clientSocket);
	close(clientSocket);
	return NULL;
}

void startServer(int port) {
	int serverSocket, clientSocket;
	struct sockaddr_in serverAddr, clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0) {
		perror("Can't create socket");
		exit(EXIT_FAILURE);
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("Can't bind sokcet");
		close(serverSocket);
		exit(EXIT_FAILURE);
	}

	if (listen(serverSocket, MAX_SERVER_QUEUE_SIZE) < 0) {
		perror("Listen error");
		close(serverSocket);
		exit(EXIT_FAILURE);
	}

	printf("[INFO] Server started and waiting for connections....\n");

	while(1) {
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
		char addrBuf[sizeof(struct in_addr)];
		inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, addrBuf, clientAddrLen);
		printf("[INFO] Server accepted the new connection: %s:%d\n", addrBuf, ntohs(clientAddr.sin_port));
		if (clientSocket < 0) {
			perror("[ERROR] Error while accepting connection");
			continue;
		}

		int* clientSocketPtr = malloc(sizeof(int));
		if (clientSocketPtr == NULL) {
			perror("[ERROR] malloc error");
			continue;
		}
		*clientSocketPtr = clientSocket;

		pthread_t clientThreadId;
		pthread_create(&clientThreadId, NULL, clientThread, clientSocketPtr);
		pthread_detach(clientThreadId);
	}
}

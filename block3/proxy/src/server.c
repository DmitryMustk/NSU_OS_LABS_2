#include "proxy.h"
#include "logger.h"

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

typedef struct {
	int clientSocket;
	Logger* logger;
} ClientThreadRoutineArgs;

void* clientThreadRoutine(void* args) {
	int clientSocket = ((ClientThreadRoutineArgs*)args)->clientSocket;
	Logger* logger   = ((ClientThreadRoutineArgs*)args)->logger;
	
	handleRequest(clientSocket, logger);
	free(args);
	close(clientSocket);
	return NULL;
}

void startServer(int port, Logger* logger) {
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

	logMessage(logger, LOG_INFO, "Server started and waiting for connections....");

	while(1) {
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
		char addrBuf[sizeof(struct in_addr)];
		inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, addrBuf, clientAddrLen);
		logMessage(logger, LOG_INFO, "Server accepted the new connection: %s:%d", addrBuf, ntohs(clientAddr.sin_port));
		if (clientSocket < 0) {
			logMessage(logger, LOG_INFO, "Error while accepting connection");
			continue;
		}

		ClientThreadRoutineArgs* args = malloc(sizeof(ClientThreadRoutineArgs));
		if (args == NULL) {
			logMessage(logger, LOG_ERROR, "ClientThreadRoutineArgs malloc error");
			continue;
		}
	
		args->clientSocket = clientSocket;
		args->logger = logger;

		pthread_t clientThreadId;
		pthread_create(&clientThreadId, NULL, clientThreadRoutine, args);
		pthread_detach(clientThreadId);
	}
}

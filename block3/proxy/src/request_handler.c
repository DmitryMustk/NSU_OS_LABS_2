#include "proxy.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define HTTP_PORT 80

static int parseURL(const char* url, char* host, char* path, int* port) {
	char* urlCopy = strdup(url);
	if (!urlCopy) {
		return -1;
	}

	*port = HTTP_PORT;
	if (sscanf(urlCopy, "http://%99[^:/]:%d/%1999[^\n]", host, port, path) == 3) {
        // URL with specified port
    } else if (sscanf(urlCopy, "http://%99[^/]/%1999[^\n]", host, path) == 2) {
        // URL without specified port
    } else if (sscanf(urlCopy, "http://%99[^:/]:%d[^\n]", host, port) == 2) {
        strcpy(path, "");
    } else if (sscanf(urlCopy, "http://%99[^\n]", host) == 1) {
        strcpy(path, "");
    } else {
        free(urlCopy);
        return -1;  // Invalid URL
    }
	host[strlen(host) - 1] = '\0';
	free(urlCopy);
	return 0;
}

static int connectToHost(const char* host, int port) {
	struct addrinfo hints, *res, *p;
	char portStr[6];
	int sockFd;

	snprintf(portStr, sizeof(portStr), "%d", port);
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(host, portStr, &hints, &res) != 0) {

		return -1;
	}

	for (p = res; p != NULL; p = p->ai_next) {
		if ((sockFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			continue;
		}
		if (connect(sockFd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockFd);
			continue;
		}
		break;
	}

	freeaddrinfo(res);
	return (p == NULL) ? -1: sockFd;
}

typedef struct {
	CacheEntry* entry;
	const char host[256];
	const char path[1024];
	int port;
} DownloadDataArgs;

void* downloadData(void* args) {
	CacheEntry* entry = ((DownloadDataArgs*)args)->entry;
	const char* host  = ((DownloadDataArgs*)args)->host;
	const char* path  = ((DownloadDataArgs*)args)->path;
	int port          = ((DownloadDataArgs*)args)->port;


	int serverSocket = connectToHost(host, port);
	printf("[INFO] Connected to: %s:%d\n", host, port);
	if (serverSocket < 0) {
		perror("[ERROR] Can't connect to server");
		cacheMarkComplete(entry);
		return NULL;
	}

	char request[BUFFER_SIZE];
	snprintf(request, sizeof(request), "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);
	send(serverSocket, request, strlen(request), 0);

	char buffer[BUFFER_SIZE];
	ssize_t bytesReceived;
	ssize_t bytesDownload = 0;
	while ((bytesReceived = recv(serverSocket, buffer, sizeof(buffer), 0)) > 0) {
		cacheInsertData(entry, buffer, bytesReceived);
		bytesDownload += bytesReceived;
	}

	printf("[INFO] Downloaded %zd bytes\n", bytesDownload);

	cacheMarkComplete(entry);
	close(serverSocket);

	return NULL;
}

void handleRequest(int clientSocket) {
	char buffer[BUFFER_SIZE];
	char method[16], url[URL_MAX_LENGHT], protocol[16];
	char host[256], path[1024];
	int port;

	if (recv(clientSocket, buffer, sizeof(buffer) - 1, 0) <= 0) {
		return;
	}
	buffer[BUFFER_SIZE - 1] = '\0';
	sscanf(buffer, "%s %s %s", method, url, protocol);
	
	if (strcmp(method, "GET") != 0) {
		const char* response = "HTTP/1.0 405 Method Not Allowed\r\n\r\n";
		send(clientSocket, response, strlen(response), 0);
		return;
	}

	if (parseURL(url, host, path, &port) < 0) {
		const char* response = "HTTP/1.0 400 Bad Request\r\n\r\n";
		send(clientSocket, response, strlen(response), 0);
		return;
	}

	//Check cache
	CacheEntry* entry = getOrCreateCacheEntry(url);
	pthread_mutex_lock(&entry->mutex);
	if (!entry->isComplete) {
		pthread_mutex_unlock(&entry->mutex);
		DownloadDataArgs* args = malloc(sizeof(DownloadDataArgs));
		if (args == NULL) {
			perror("Malloc error");
			return;
		}

		args->entry = entry;
		strncpy(args->path, path, sizeof(args->path));
		strncpy(args->host, host, sizeof(args->host));
		args->port = port;

		pthread_create(&entry->downloadThread, NULL, downloadData, args);

	} else {
		printf("[INFO] Cache hit detected!\n");
	}

	//Send to client
	size_t sentBytes = 0;
	while (sentBytes < entry->downloadedSize || !entry->isComplete) {
		while (sentBytes < entry->downloadedSize) {
			size_t chunk = send(clientSocket, entry->data + sentBytes, entry->downloadedSize - sentBytes, 0);
			if (chunk <= 0) {
				break;
			}
			sentBytes += chunk;
		}
		if (!entry->isComplete) {
			pthread_cond_wait(&entry->cond, &entry->mutex);
		}
	}

	pthread_mutex_unlock(&entry->mutex);
}

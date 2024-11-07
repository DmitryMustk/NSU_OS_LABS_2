#ifndef PROXY_H
#define PROXY_H

#include <pthread.h>

#define PORT             9008
#define CACHE_SIZE_LIMIT 1048576 //1Mb
#define BUFFER_SIZE      8192
#define URL_MAX_LENGHT   2048

typedef struct CacheEntry {
	char url[URL_MAX_LENGHT];
	char* data;
	size_t dataSize;
	size_t downloadedSize;
	int isComplete;
	time_t lastAccessTime;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	struct CacheEntry* next;
} CacheEntry;

//cache.c funcs
CacheEntry* getOrCreateCacheEntry(const char* url);
void cacheCleanup(void);
void cacheInsertData(CacheEntry* entry, const char* data, size_t length);
void cacheMarkComplete(CacheEntry* entry);

//request_handler.c funcs
void handleRequest(int clientSocket);
void downloadData(CacheEntry* entry, const char* host, const char* path, int port);

//server.c funcs
void startServer(int port);

#endif //PROXY_H

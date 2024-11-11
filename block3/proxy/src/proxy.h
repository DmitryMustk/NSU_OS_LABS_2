#ifndef PROXY_H
#define PROXY_H

#include <pthread.h>

#define PORT             9003
#define CACHE_SIZE_LIMIT 1048576 //1Mb
//Should be >= 16kB to fully fit HTTP headers in single buffer
#define BUFFER_SIZE      16384  //16kB 
#define URL_MAX_LENGHT   2048

typedef struct CacheEntry {
	char url[URL_MAX_LENGHT];
	char* data;
	size_t dataSize;
	size_t downloadedSize;
	int isComplete;
	int isOk;
	time_t lastAccessTime;
	pthread_mutex_t mutex;
	pthread_t downloadThread;
	pthread_cond_t cond;
	struct CacheEntry* next;
} CacheEntry;

//cache.c funcs
CacheEntry* getOrCreateCacheEntry(const char* url);
void deleteEntry(CacheEntry* entry);
void cacheCleanup(void);
void cacheInsertData(CacheEntry* entry, const char* data, size_t length);
void cacheMarkComplete(CacheEntry* entry);
void cacheMarkOk(CacheEntry* entry, int val);

//request_handler.c funcs
void handleRequest(int clientSocket);
void* downloadData(void* args);
//server.c funcs
void startServer(int port);

#endif //PROXY_H

#include "proxy.h"

#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

CacheEntry* cache = NULL;
size_t curCacheSize = 0;
pthread_mutex_t cacheMutex = PTHREAD_MUTEX_INITIALIZER;

CacheEntry* getOrCreateCacheEntry(const char* url) {
	pthread_mutex_lock(&cacheMutex);
	CacheEntry* entry = cache;

	while (entry) {
		if (strcmp(entry->url, url) == 0) {
			pthread_mutex_unlock(&cacheMutex);
			return entry;
		}
		entry = cache->next;
	}

	entry = malloc(sizeof(CacheEntry));
	if (!entry) {
		pthread_mutex_unlock(&cacheMutex);
		return NULL;
	}

	strncpy(entry->url, url, URL_MAX_LENGHT);
	entry->data = NULL;
	entry->dataSize = 0;
	entry->downloadedSize = 0;
	entry->isComplete = 0;
	pthread_mutex_init(&entry->mutex, NULL);
	pthread_cond_init(&entry->cond, NULL);

	entry->next = cache;
	cache = entry;

	pthread_mutex_unlock(&cacheMutex);
	return entry;
}

void cacheInsertData(CacheEntry *entry, const char *data, size_t length) {
	pthread_mutex_lock(&entry->mutex);

	entry->data = realloc(entry->data, entry->downloadedSize + length);
	memcpy(entry->data + entry->downloadedSize, data, length);
	entry->downloadedSize += length;

	pthread_cond_broadcast(&entry->cond);
	pthread_mutex_unlock(&entry->mutex);
}

void cacheMarkComplete(CacheEntry *entry) {
	pthread_mutex_lock(&entry->mutex);
	entry->isComplete = 1;
	pthread_cond_broadcast(&entry->cond);
	pthread_mutex_unlock(&entry->mutex);
}

void cacheCleanup(void) {
	pthread_mutex_lock(&cacheMutex);
	CacheEntry* cur = cache;
	CacheEntry* prev = NULL;

	while (cur) {
		if (cur->isComplete && cur->downloadedSize > CACHE_SIZE_LIMIT / 2) {
			if (prev) {
				prev->next = cur->next;
			} else {
				cache = cur->next;
			}

			free(cur->data);
			pthread_mutex_destroy(&cur->mutex);
			pthread_cond_destroy(&cur->cond);
			free(cur);
			cur = (prev) ? prev->next : cache;
		} else {
			prev = cur;
			cur = cur->next;
		}
	}

	pthread_mutex_unlock(&cacheMutex);
}

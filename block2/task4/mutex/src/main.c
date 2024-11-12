#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "./lib/mutex.h"

volatile int counter = 0;

void* t1Routine(void* args) {
	mutex_t* lock = (mutex_t*) args;
	for (int i = 0; i < 1000000; ++i) {
		mutex_lock(lock);
		counter++;
		mutex_unlock(lock);
	}

	return NULL;
}

void* t2Routine(void* args) {
	mutex_t* lock = (mutex_t*) args;
	for (int i = 0; i < 1000000; ++i) {
		mutex_lock(lock);
		counter++;
		mutex_unlock(lock);
	}

	return NULL;
}

int main(void) {
	mutex_t mutex;
	mutex_init(&mutex);

	pthread_t t1, t2;

	pthread_create(&t1, NULL, t1Routine, &mutex);
	pthread_create(&t2, NULL, t2Routine, &mutex);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("counter = %d\n", counter);
	
	return 0;
}

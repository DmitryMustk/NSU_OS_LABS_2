#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "./lib/spinlock.h"

volatile int counter = 0;

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);
	CPU_SET(n, &cpuset);

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	printf("set_cpu: set cpu %d\n", n);
}


void* t1Routine(void* args) {
	set_cpu(0);
	spinlock_t* lock = (spinlock_t*) args;
	for (int i = 0; i < 10000000; ++i) {
		spinlock_lock(lock);
		counter++;
		spinlock_unlock(lock);
	}

	return NULL;
}

void* t2Routine(void* args) {
	set_cpu(1);
	spinlock_t* lock = (spinlock_t*) args;
	for (int i = 0; i < 10000000; ++i) {
		spinlock_lock(lock);
		counter++;
		spinlock_unlock(lock);
	}

	return NULL;
}



int main(void) {
	spinlock_t lock;
	spinlock_init(&lock);

	pthread_t t1, t2;

	pthread_create(&t1, NULL, t1Routine, &lock);
	pthread_create(&t2, NULL, t2Routine, &lock);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	printf("counter = %d\n", counter);
	
	return 0;
}

#include "mythread.h"

#define _GNU_SOURCE
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

int threadFunc(void* args) {
	void* (*start_routine)(void*) = ((void**)(args))[0];
	void* routineArg = ((void**)(args))[1];

	start_routine(routineArg);

    void** castedArgs = (void**)args;
	free(castedArgs);
	return 0;
}

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg) {
	void* stack = malloc(STACK_SIZE);
	if (stack == NULL) {
		perror("malloc");
		return -1;
	}

	void** funcArgs = malloc(3 * sizeof(void*));
	if (funcArgs == NULL) {
		perror("malloc");
		free(stack);
		return -1;
	}

	funcArgs[0] = start_routine;
	funcArgs[1] = arg;
	funcArgs[2] = stack;
	int flags = CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM | CLONE_THREAD;  

	thread->tid = clone(threadFunc, (char*)stack + STACK_SIZE, flags | SIGCHLD, funcArgs); 

	if (thread->tid == -1) {
		perror("clone");
		free(stack);
		free(funcArgs);
		return -1;
	}
	return 0;
}

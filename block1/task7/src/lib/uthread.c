#include <ucontext.h>
#include <stdlib.h>
#include <stdio.h>

#include "uthread.h"

#define STACK_SIZE   1024 * 1024
#define MAX_UTHREADS 128

static uthread_t* curThread = NULL;
static ucontext_t mainContext;

int uthread_create(uthread_t *thread, void *(*start_routine)(void *), void *arg) {
	if (getcontext(&(thread->context)) == -1) {
		return -1;
	}

	thread->stack = malloc(STACK_SIZE);
	if (thread->stack == NULL) {
		return -1;
	}	

	thread->context.uc_stack.ss_sp   = thread->stack;
	thread->context.uc_stack.ss_size = STACK_SIZE;
	thread->context.uc_link          = &mainContext;

	makecontext(&(thread->context), (void (*)(void))start_routine, 1, arg); 

	thread->finished = 0;
	return 0;
}

void uthread_yield(void) {
	if (curThread != NULL && !curThread->finished) {
		swapcontext(&(curThread->context), &mainContext);
	}
}

void uthread_run(uthread_t* thread) {
	curThread = thread;
	swapcontext(&mainContext, &(thread->context));
}

void uthread_exit(uthread_t* thread) {
	free(thread->stack);
	thread->finished = 1;
}

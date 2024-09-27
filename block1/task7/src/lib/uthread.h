#ifndef UTHREAD_H
#define UTHREAD_H

#include <ucontext.h>

typedef struct uthread_t {
	ucontext_t context;
	void* stack;
	int finished;
} uthread_t;

int  uthread_create(uthread_t* thread, void* (*start_routine)(void *), void* arg);
void uthread_run(uthread_t* thread);
void uthread_yield(void);
void uthread_exit(uthread_t* thread);

#endif //UTHREAD_H

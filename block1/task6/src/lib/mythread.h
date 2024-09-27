#ifndef MYTHREAD_H
#define MYTHREAD_H

typedef struct {
	int tid;
} mythread_t;

int mythread_create(mythread_t* thread, void* (*start_routine)(void*), void* arg);

#endif //MYTHREAD_H

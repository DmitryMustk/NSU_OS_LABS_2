#ifndef TASK_2_4_MUTEX_H
#define TASK_2_4_MUTEX_H

#include <stdatomic.h>
#include <sys/types.h>

typedef struct {
	_Atomic int lock;
	_Atomic pid_t tid;
} mutex_t;

void mutex_init(mutex_t* mutex);
void mutex_lock(mutex_t* mutex);
int  mutex_unlock(mutex_t* mutex);

#endif //TASK_2_4_MUTEX_H


#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdatomic.h>

typedef struct {
	_Atomic int state;  
} spinlock_t;

void spinlock_init(spinlock_t* lock);
void spinlock_lock(spinlock_t* lock);
int  spinlock_unlock(spinlock_t* lock);

#endif // SPINLOCK_H


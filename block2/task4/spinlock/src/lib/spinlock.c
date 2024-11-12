#include <stdatomic.h>
#define _GNU_SOURCE

#include <errno.h>
#include <linux/futex.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include "spinlock.h"

#define STATUS_LOCK 0
#define STATUS_UNLOCK 1

static int futex_wake(int* futexAddr) {
	int err = syscall(SYS_futex, futexAddr, FUTEX_WAKE, 1, NULL, NULL, 0);
	if (err == -1 && errno != EAGAIN) {
		perror("futex_wake error:");
		return -1;
	}
	return 0;
}

void spinlock_init(spinlock_t* lock) {
	atomic_store(&lock->state, STATUS_UNLOCK);
}

void spinlock_lock(spinlock_t* lock) {
	while (1) {
		int expected = STATUS_UNLOCK;
		if (atomic_compare_exchange_strong(&lock->state, &expected, STATUS_LOCK)) {
			break; 
		}
	}
}

int spinlock_unlock(spinlock_t* lock) {
	int expected = STATUS_LOCK;
	if (atomic_compare_exchange_strong(&lock->state, &expected, STATUS_UNLOCK)) {
		return 0;
	}
	return -1;
}


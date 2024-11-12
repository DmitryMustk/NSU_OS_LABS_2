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
#include "mutex.h"

#define STATUS_LOCK 0
#define STATUS_UNLOCK 1
#define NO_TID -1

static int futex(int* uaddr, int futexOp, int val, const struct timespec* timeout, int* uaddr2, int val3) {
	return syscall(SYS_futex, uaddr, futexOp, val, timeout, uaddr2, val3);
}

static int futex_wait(int* futexAddr, int val) {
	int err = futex(futexAddr, FUTEX_WAIT, val, NULL, NULL, 0);
	if (err == -1 && errno != EAGAIN) {
		perror("futex_wait error: ");
		return -1;
	}
	return 0;
}

static int futex_wake(int* futexAddr) {
	int err = futex(futexAddr, FUTEX_WAKE, 1, NULL, NULL, 0);
	if (err == -1 && errno != EAGAIN) {
		perror("futex_wake error:");
		return -1;
	}
	return 0;
}

void mutex_init(mutex_t* mutex) {
	atomic_store(&mutex->lock, STATUS_UNLOCK);
	atomic_store(&mutex->tid, NO_TID);
}

void mutex_lock(mutex_t* mutex) {
	while (1) {
		int expected = STATUS_UNLOCK;
		if (atomic_compare_exchange_strong(&mutex->lock, &expected, STATUS_LOCK)) {
			atomic_store(&mutex->tid, gettid());
			break;
		}
		int* lock_ptr = (int*)&mutex->lock;
		futex_wait(lock_ptr, STATUS_LOCK);
	}
}

int mutex_unlock(mutex_t* mutex) {
	if (atomic_load(&mutex->tid) != gettid()) {
		return -1;
	}
	atomic_store(&mutex->tid, NO_TID);
	atomic_store(&mutex->lock, STATUS_UNLOCK);
	
	int* lock_ptr = (int*)&mutex->lock;
	futex_wake(lock_ptr);
	return 0;
}


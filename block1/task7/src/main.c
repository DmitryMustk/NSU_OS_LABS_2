#include <stdio.h>
#include "./lib/uthread.h"

void *thread_function(void *arg) {
    printf("Thread started with argument: %s\n", (char *)arg);
    for (int i = 0; i < 3; ++i) {
        printf("Thread %s, iteration %d\n", (char *)arg, i);
		uthread_yield();  
    }
    return NULL;
}

int main(void) {
    uthread_t thread1;

    uthread_create(&thread1, thread_function, "1");

	
	uthread_run(&thread1);
	printf("From main\n");
	uthread_run(&thread1);
	printf("From main\n");
	uthread_run(&thread1);

	uthread_exit(&thread1);
    return 0;
}


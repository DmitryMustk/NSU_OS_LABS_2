#include "./lib/mythread.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* routine(void* arg) {
	char* str = (char*)arg;
	printf("hello from mythread\n");
	printf("my arg: %s\n", str);
	return NULL;

}

int main(void) {
	mythread_t thread;

	char* str = "arg";
	if (mythread_create(&thread, routine, (void*)str) != 0) {
		printf("Can't create thread");
		return -1;
	}

	sleep(3);
	return 0;
}

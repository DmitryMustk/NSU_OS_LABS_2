#include <stdio.h>
#include <signal.h>

#include <pthread.h>
#include <time.h>

void* t1Routine(void* args) {
	(void) args;

	sigset_t sigSet;
	sigfillset(&sigSet);
	pthread_sigmask(SIG_BLOCK, &sigSet, NULL);

	printf("Thread 1: all signals are blocked\n");
	while (1) {	}

	return NULL;
}


void sigintHandler(int sig) {
	if (sig == SIGINT) {
		printf("Thread 2: catch SIGINT\n");
	}
}

void* t2Routine(void* args) {
	(void)args;

	sigset_t sigSet;
    sigemptyset(&sigSet);
    sigaddset(&sigSet, SIGINT);
    pthread_sigmask(SIG_UNBLOCK, &sigSet, NULL);


	struct sigaction sigAction;
	sigAction.sa_handler = sigintHandler;
	if (sigemptyset(&sigAction.sa_mask) != 0) {
		perror("sigemptyset error");
		return NULL;
	}
	sigAction.sa_flags = 0;
	sigaction(SIGINT, &sigAction, NULL);

	printf("Thread 2: waiting for SIGINT\n");
	while (1) {}

	return NULL;
}

void* t3Routine(void* args) {
	(void) args;

	sigset_t sigSet;
	
	if (sigemptyset(&sigSet) != 0 || sigaddset(&sigSet, SIGQUIT) != 0) {
		perror("Invalid signum");
		return NULL;
	}

	//Block SIGQUIT because wait him with sigwait
	pthread_sigmask(SIG_BLOCK, &sigSet, NULL);

	printf("Thread 3: waiting for SIGQUIT\n");

	int sig;
	sigwait(&sigSet, &sig);
	if (sig == SIGQUIT) {
		printf("Thread 3: catch SIGQUIT\n");
	}

	return NULL;
}

int main(void) {
	sigset_t sigSet;
	if (sigemptyset(&sigSet) != 0 ||
		sigaddset(&sigSet, SIGINT) != 0 ||
		sigaddset(&sigSet, SIGQUIT) != 0
	) {
		perror("Invalid signum");
		return -1;
	}

	pthread_sigmask(SIG_BLOCK, &sigSet, NULL);


	pthread_t thread1, thread2, thread3;

	pthread_create(&thread1, NULL, t1Routine, NULL);
	pthread_create(&thread2, NULL, t2Routine, NULL);
	pthread_create(&thread3, NULL, t3Routine, NULL);


	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);

	return 0;
}

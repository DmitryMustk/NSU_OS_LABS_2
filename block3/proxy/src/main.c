#include <stdio.h>

#include "proxy.h"


//Add LRU algorithm

int main(void) {
	printf("Starting proxy server on %d port....\n", PORT);
	startServer(PORT);
	
	return 0;
}

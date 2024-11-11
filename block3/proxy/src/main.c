#include <stdio.h>

#include "proxy.h"

int main(void) {
	printf("[INFO] Starting proxy server on %d port....\n", PORT);
	startServer(PORT);
	
	return 0;
}

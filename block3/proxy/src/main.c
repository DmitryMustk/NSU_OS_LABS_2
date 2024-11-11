#include <stdio.h>

#include "proxy.h"
#include "logger.h"

int main(void) {
	Logger* logger = createLogger("proxy.log", LOG_DEBUG, 1);
    
	logMessage(logger, LOG_INFO, "Starting proxy server on %d port....", PORT);
	startServer(PORT, logger);
	
	return 0;
}

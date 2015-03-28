#include <stdio.h>
#include <unistd.h>

#include "queue.h"
#include "util.h"


//Macros
#define NUM_CPU sysconf(_SC_NPROCESSORS_ONLN)	//Identifying number of CPU cores
#define MAX_INPUT_FILES 10
#define MIN_RESOLVER_THREADS 2
#define MAX_RESOLVER_THREADS NUM_CPU
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

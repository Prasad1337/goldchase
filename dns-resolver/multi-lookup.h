#include <stdio.h>
#include <unistd.h>

#include "queue.h"
#include "util.h"


//Macros
#define MIN_ARGS 3
#define USAGE "./multi-lookup <inputFilePath> <inputFilePath> ... <outputFilePath>"
#define SBUFSIZE 1025
#define INPUTFS "%1024s"

#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MIN_RESOLVER_THREADS 2
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN
#define QUEUE_SIZE 1024

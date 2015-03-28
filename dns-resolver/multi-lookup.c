//Header file includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "multi-lookup.h"


//Macros
#define numCPU sysconf(_SC_NPROCESSORS_ONLN)	//Identifying number of CPU cores
#define MAX_INPUT_FILES 10
#define MIN_RESOLVER_THREADS 2
#define MAX_RESOLVER_THREADS numCPU
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN


//Main
int main(int argc, char* argv[])
{
	const char *fileNames[10];
	const char *inFiles[9];
	const char *outFile;
	int numFiles=0;

	if(argc==1)
		printf("No arguments issued! Please try again...\n");
	else
	{
		int i;

		for(i=1;i<argc;i++,numFiles++)
		{
			fileNames[numFiles]=argv[i];
		}
		numFiles--;

		outFile=fileNames[numFiles];

		for(i=0;i<numFiles;i++)
			inFiles[i]=fileNames[i];


	}

	return EXIT_SUCCESS;
}

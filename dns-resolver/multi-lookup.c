//Header file includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "multi-lookup.h"


//Main
int main(int argc, char** argv)
{
	const char **fileNames;
	const char **inFiles;
	const char *outFile;
	int numFiles=0;
	int NUM_THREADS;
	
	if(MAX_RESOLVER_THREADS<MIN_RESOLVER_THREADS)
		NUM_THREADS=MIN_RESOLVER_THREADS;
	else
		NUM_THREADS=MAX_RESOLVER_THREADS;

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

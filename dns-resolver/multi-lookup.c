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
	const char *fileNames[10];
	const char *inFiles[9];
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

		for(i=0;i<=numFiles;i++)
		{
			if(access(fileNames[i],F_OK)==-1)
			{
			    printf("FILE: %s does not exist! Please try again...\n",fileNames[i]);
			    return EXIT_FAILURE;
			}
		}

		outFile=fileNames[numFiles];

		for(i=0;i<numFiles;i++)
			inFiles[i]=fileNames[i];


	}

	return EXIT_SUCCESS;
}

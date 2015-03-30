//
// Pradyumna Kamat : pkamat (Prasad1337)
//


//Header file includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "multi-lookup.h"


//Function prototypes
void* read_in_file(char*);
void* resolve_domain();


//Establishing mutexes
pthread_mutex_t q_mutex;
pthread_mutexattr_t q_attr;

pthread_mutex_t out_mutex;
pthread_mutexattr_t out_attr;

pthread_mutex_t in_mutex;
pthread_mutexattr_t in_attr;


//Global definitions
FILE* outF=NULL;
queue Q;
int in_files=0;


//Main
int main(int argc, char** argv)
{
    const int qSize=QUEUE_SIZE;
    int num_in_files=argc-2;
    int rc;
    long t;
    
    pthread_t file_threads[num_in_files];
    pthread_t resolver_threads[MAX_RESOLVER_THREADS];

	pthread_mutexattr_settype(&q_attr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&q_mutex,&q_attr);

	pthread_mutexattr_settype(&out_attr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&out_mutex,&out_attr);
	
	pthread_mutexattr_settype(&in_attr,PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&in_mutex,&in_attr);

	
	if(argc<MIN_ARGS)
	{
		printf("Too few arguments! Please try again...\n");
		printf("Usage:\n\t%s\n",USAGE);
		return EXIT_FAILURE;
	}
	
    if(argc>MAX_INPUT_FILES+1)
    {
		printf("Too many arguments! Please try again...\n");
		printf("Usage:\n\t%s\n",USAGE);
		return EXIT_FAILURE;
    }

    outF=fopen(argv[(argc-1)],"w");
    if(!outF)
    {
		printf("Error Opening Output File! Please try again...\n");
		return EXIT_FAILURE;
    }

    if(queue_init(&Q,qSize)==QUEUE_FAILURE)
    {
		printf("Queue initialization failed! Please try again...\n");
		return EXIT_FAILURE;
    }
    
    printf("Processing... Please wait!\n");
    for(t=0;t<num_in_files;t++)
    {
		rc=pthread_create(&file_threads[t],NULL,read_in_file,argv[t+1]);
		if(rc)
		{
			printf("Thread error incurred: %d\nPlease try again...\n",rc);
			return EXIT_FAILURE;
		}
	}
	
	for(t=0;t<MAX_RESOLVER_THREADS;t++)
	{
		rc=pthread_create(&resolver_threads[t],NULL,resolve_domain,NULL);
		if(rc)
		{
			printf("Thread error incurred: %d\nPlease try again...\n",rc);
			return EXIT_FAILURE;
		}
	}
	
    for(t=0;t<num_in_files;t++)
    {
		pthread_join(file_threads[t],NULL);
    }

    pthread_mutex_lock(&in_mutex);
	in_files=1;
	pthread_mutex_unlock(&in_mutex);
    
    for(t=0;t<MAX_RESOLVER_THREADS;t++)
    {
		pthread_join(resolver_threads[t],NULL);
    }
    
    printf("All done!\n");	//Threads completed
	fclose(outF);
	
    queue_cleanup(&Q);

	pthread_mutex_destroy(&q_mutex);
	pthread_mutex_destroy(&out_mutex);
	pthread_mutex_destroy(&in_mutex);

    return EXIT_SUCCESS;
}


void* read_in_file(char* file_name)
{
    char* hostname;
    FILE* inFILE=NULL;
	 
	inFILE=fopen(file_name,"r");
	if(!inFILE)
	{
		printf("Error Opening Input File: %s\nPlease try again...\n",file_name);
		exit(EXIT_FAILURE);
	}
	
	hostname=(char*)malloc((MAX_NAME_LENGTH+1)*sizeof(char));
	
	while(fscanf(inFILE,INPUTFS,hostname)>0)
	{
		pthread_mutex_lock(&q_mutex);
		
		if(queue_push(&Q,hostname)==QUEUE_FAILURE)
		{
			printf("Queue failure! Please try again...\n");
			exit(EXIT_FAILURE);
		}

		pthread_mutex_unlock(&q_mutex);
		
		hostname=(char*)malloc((MAX_NAME_LENGTH+1)*sizeof(char));
	 }
	 
	fclose(inFILE);
    
    free(hostname);
    hostname=NULL;
    
    pthread_exit(NULL);
    return NULL;
}


void* resolve_domain()
{	
	char *hostname;
    char ip[INET6_ADDRSTRLEN];
	int q_stat=0;
	int loopC=0;
	
	pthread_mutex_lock(&q_mutex);
	q_stat=!queue_is_empty(&Q);
	pthread_mutex_unlock(&q_mutex);
	
	pthread_mutex_lock(&in_mutex);
	loopC = (!in_files)||(q_stat);
	pthread_mutex_unlock(&in_mutex);
	
	do
	{
		pthread_mutex_lock(&q_mutex);
		if((hostname=(char*)queue_pop(&Q))==NULL)
			pthread_mutex_unlock(&q_mutex);
		
		else
		{
			pthread_mutex_unlock(&q_mutex);
			
			if(dnslookup(hostname,ip,sizeof(ip))==UTIL_FAILURE)
			{
				printf("DNS Lookup error! Please try again...\n");
				strncpy(ip,"",sizeof(ip));
			}
			
			pthread_mutex_lock(&out_mutex);
			fprintf(outF,"%s,%s\n",hostname,ip);
			pthread_mutex_unlock(&out_mutex);
			
			free(hostname);
			hostname=NULL;
		}
		
		pthread_mutex_lock(&q_mutex);
		q_stat=!queue_is_empty(&Q);
		pthread_mutex_unlock(&q_mutex);

		pthread_mutex_lock(&in_mutex);
		loopC=(!in_files)||(q_stat);
		pthread_mutex_unlock(&in_mutex);
		
	}while(loopC);
	
    pthread_exit(NULL);
    return NULL;
}

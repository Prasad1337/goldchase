// 
// 
// 
// Pradyumna Kamat : pkamat
//


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>
#include <limits.h>
#include <sys/time.h>

#include "helper-routines.h"

using namespace std;


/*Define Global Variables*/
pid_t childpid=-1;	//Child PID
int parentpid=-1;	//Parent PID
timeval t1,t2,p_start,p_end,c_start,c_end;
int numtests=10000;
double elapsedTime,pTime,cTime;
double cMax=0,cMin=0;
double pMax=0,pMin=0;
double cTotal=0,pTotal=0;
int waits=1;
int flag1=1,flag2=1;


//Function prototypes
void sigint_handler(int);
void sigusr1_handler(int);
void sigusr2_handler(int);


//
// main - The main routine 
//
int main(int argc,char **argv)
{
	//Initialize Constants here

	//variables for Pipe
	int fd1[2],fd2[2];
	//byte size messages to be passed through pipes 
	char childmsg[]="1";
	char parentmsg[]="2";
	char quitmsg[]="q";

	/*Three Signal Handlers You Might Need
	*
	*I'd recommend using one signal to signal parent from child
	*and a different SIGUSR to signal child from parent
	*/
	Signal (SIGUSR1, sigusr1_handler);	//User Defined Signal 1
	Signal (SIGUSR2, sigusr2_handler);	//User Defined Signal 2
	Signal (SIGINT, sigint_handler);
	

	//Determine the number of messages was passed in from command line arguments
	//Replace default numtests w/ the commandline argument if applicable 
	if(argc<2)
	{
		printf("Not enough arguments\n");
		exit(0);
	}

	printf ("Number of Tests %d\n", numtests);
	
	//Start timing
	gettimeofday(&t1,NULL);

	if(strcmp(argv[1],"-p")==0)
	{
		pipe(fd1);
		pipe(fd2);

		int pid=-1;	//error case
		
		pid=fork();
		int flag=0;
		int stat;

		while(numtests>0)
		{
			if(pid>0)
			{
				gettimeofday(&p_start,NULL);

				if(flag=1)
				{
					read(fd2[0],parentmsg,sizeof(parentmsg));
					gettimeofday(&p_end,NULL);
					   
					flag=0;
					

					write(fd1[1],childmsg,(strlen(childmsg)+1));

					pTime=(p_end.tv_sec-p_start.tv_sec)*1000.0;
					pTime+=(p_end.tv_usec-p_start.tv_usec)/1000.0;
					
					if(pTime<pMin||pMin==0)
						pMin=pTime;

					if (pTime>pMax||pMax==0)
						pMax=pTime;

					pTotal=pTime+pTotal;
				}
			}

			else
			{
				gettimeofday(&c_start,NULL);

				write(fd2[1],parentmsg,(strlen(parentmsg)+1));
				read(fd1[0],childmsg,sizeof(childmsg));
				
				gettimeofday(&c_end,NULL);

				flag=1;

				cTime=(c_end.tv_sec-c_start.tv_sec)*1000.0;
				cTime+=(c_end.tv_usec-c_start.tv_usec)/1000.0;
				
				if (cTime<cMin||cMin==0)
					cMin=cTime;

				if (cTime>cMax||cMax==0)
					cMax=cTime;

				cTotal=cTime+cTotal;
				        
				if(numtests==5)
				{
					cout<<"Child: "<<endl;
					cout<<"Avg: "<<cTotal/10000<<endl;
					cout<<"min: "<<cMin<<endl;
					cout<<"max: "<<cMax<<endl;
				}
			}
			numtests=numtests-1;
		}

		//Stop timing
		gettimeofday(&t2,NULL);

		int wait = 0;

		close(fd1[0]);
		close(fd1[1]);
		close(fd2[1]);
		close(fd2[1]);

		kill(pid,SIGKILL);

		waitpid(getppid(),&stat,0);
		
		cout<<"Parent: "<<endl;
		cout<<"Avg: "<<pTotal/10000<<endl;
		cout<<"min: "<<pMin<<endl;
		cout<<"max: "<<pMax<<endl;

		//Compute time
		elapsedTime=(t2.tv_sec-t1.tv_sec)*1000.0;	//s to ms
		elapsedTime+=(t2.tv_usec-t1.tv_usec)/1000.0;	//us to ms
		printf("Elapsed Time %f\n",elapsedTime);
		exit(0);
	}

	if(strcmp(argv[1],"-s")==0)
	{
		int pid;
		int sigflag=0;
		fflush(NULL);

		childpid=fork();

		if(childpid!=0)
		{
			cout<<"parent pid "<<getpid()<<endl;
			cout<<"child pid "<<childpid<<endl;
			
			while(numtests>0)
			{
				gettimeofday(&t2,NULL);
				kill(childpid,SIGUSR1);
				
				while(waits==1);	//wait
				
				waits=1;
			}

		}

		gettimeofday(&t2,NULL);

		elapsedTime=(t2.tv_sec-t1.tv_sec)*1000.0;
		elapsedTime+=(t2.tv_usec-t1.tv_usec)/1000.0;
		printf("Elapsed Time %f\n",elapsedTime);
	}
	
	return 0;
}


void sigint_handler(int sig)
{
	int pid=getpid();

	while(pid>0)
		wait(0);

	gettimeofday(&t2,NULL);

	elapsedTime=(t2.tv_sec-t1.tv_sec)*1000.0;
	elapsedTime+=(t2.tv_usec-t1.tv_usec)/1000.0;
	printf("Elapsed Time %f\n",elapsedTime);

	if(childpid==0)
	{
		printf("Child's Results for Signal IPC mechanisms\n");
		printf("Process ID is %d, group id is %d\n",getpid(),getgid());
		printf("Round trip times\n");
		printf("Average: %f\n",(pTotal/10000));
		printf("Maximum: %f\n",pMax);
		printf("Minimum: %f\n",pMin);
		kill(pid,SIGKILL);
	}


	else
	{
		printf("Parent's Results for Signal IPC mechanisms\n");
		printf("Process ID is %d, group id is %d\n",getpid(),getgid());
		printf("Round trip times\n");
		printf("Average: %f\n",(pTotal/10000));
		printf("Maximum: %f\n",pMax);
		printf("Minimum: %f\n",pMin);
	}
}

void sigusr1_handler(int)
{
	gettimeofday(&p_end,NULL);
	if(flag2==0)
	{
		pTime=(p_end.tv_sec-p_start.tv_sec)*1000.0;
		pTime+=(p_end.tv_usec-p_start.tv_usec)/1000.0;
		
		if(pTime<pMin||pMin==0)
			pMin=pTime;

		if (pTime>pMax||pMax==0)
			pMax=pTime;

		pTotal=pTime+pTotal;
	}

	gettimeofday(&p_start,NULL);
	flag2=0;
	kill(getppid(),SIGUSR2);

}

void sigusr2_handler(int sig)
{
	gettimeofday(&p_end,NULL);
	
	if(flag1==0)
	{
		pTime=(p_end.tv_sec-p_start.tv_sec)*1000.0;
		pTime +=(p_end.tv_usec-p_start.tv_usec)/1000.0;
		
		if(pTime<pMin||pMin==0)
			pMin=pTime;

		if(pTime>pMax||pMax==0)
			pMax=pTime;

		pTotal=pTime+pTotal;
	}

	numtests=numtests-1;

	if (numtests==0)
	{
	  kill(childpid,SIGINT);
	  kill(parentpid,SIGINT);
	}

	gettimeofday(&p_start,NULL);
	flag1=0;
	kill(childpid,SIGUSR1);
	waits=0;
}

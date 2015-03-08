// 
// 
// 
// 
//

using namespace std;

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
#include <iostream>

#include "helper-routines.h"

/*Define Global Variables*/
pid_t childpid;
int parentpid;
timeval t1, t2, parentstart, parentend, childstart, childend;
int numtests;
double elapsedTime, parentTime, childTime;
int iswaiting = 1;
double childmax = 0;
double childmin = 0;
double childtotal = 0;
double parentmin = 0;
double parentmax = 0;
double parenttotal = 0;
int Sflag =1;
int Sflag2 =1;


void
sigusr1_handler (int sig)
{

  gettimeofday (&parentend, NULL);
if(Sflag2 == 0){
		
 parentTime = (parentend.tv_sec - parentstart.tv_sec) * 1000.0;
  parentTime += (parentend.tv_usec - parentstart.tv_usec) / 1000.0;
  if (parentTime < parentmin || parentmin == 0)
    parentmin = parentTime;

  if (parentTime > parentmax || parentmax == 0)
    parentmax = parentTime;

  parenttotal = parentTime + parenttotal;
}

  //cout << "sigusr1 " << getppid() << endl;

  gettimeofday (&parentstart, NULL);
   Sflag2 = 0;
  kill (getppid (), SIGUSR2);







/*  childTime = (childend.tv_sec - childstart.tv_sec) * 1000.0;
  childTime += (childend.tv_usec - childstart.tv_usec) / 1000.0;
  if (childTime < childmin || childmin == 0)
    childmin = childTime;

  if (childTime > childmax || childmax == 0)
    childmax = childTime;

  childtotal = childTime + childtotal;
  //cout << numtests << endl;               
*/



  //wait(0);
  //cout << numtests << endl;



}

void
sigusr2_handler (int sig)
{

  gettimeofday (&parentend, NULL);
  if(Sflag == 0){

 parentTime = (parentend.tv_sec - parentstart.tv_sec) * 1000.0;
  parentTime += (parentend.tv_usec - parentstart.tv_usec) / 1000.0;
  if (parentTime < parentmin || parentmin == 0)
    parentmin = parentTime;

  if (parentTime > parentmax || parentmax == 0)
    parentmax = parentTime;

  parenttotal = parentTime + parenttotal;
}

  numtests = numtests - 1;
 if(numtests % 1000 == 0)
 //cout << numtests << endl;
  if (numtests == 0)
    {
      //cout << "CALLED" << endl;
      kill (childpid, SIGINT);
      kill(parentpid, SIGINT);
    }

//cout << "here " << childpid << endl;
 
gettimeofday (&parentstart, NULL);
  Sflag = 0;
  kill (childpid, SIGUSR1);
  iswaiting = 0;
  
//exit(0);


 



}

void
sigint_handler (int sig)
{

  int pid = getpid();

if(pid > 0)
	wait(0);

  gettimeofday (&t2, NULL);

  // compute and print the elapsed time in millisec
  elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;	// sec to ms
  elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;	// us to ms
  printf ("Elapsed Time %f\n", elapsedTime);

  
  
  /*cout << "mypid is: " << pid << endl;
  cout << parentpid << endl;
  cout << "Child pid " << childpid << endl;
  cout << getppid() << endl;*/
  if(childpid == 0)
{
 
  printf("Child's Results for Signal IPC mechanisms\n");
  printf("Process ID is %d, group id is %d\n", getpid(), getgid());
  printf("Round trip times\n");
  printf("Average: %f\n", (parenttotal / 10000));
  printf("Maximum: %f\n", parentmax);
  printf("Minimum: %f\n", parentmin);
  kill (pid, SIGKILL);
}


else{

  printf("Parent's Results for Signal IPC mechanisms\n");
  printf("Process ID is %d, group id is %d\n", getpid(), getgid());
  printf("Round trip times\n");
  printf("Average: %f\n", (parenttotal / 10000));
  printf("Maximum: %f\n", parentmax);
  printf("Minimum: %f\n", parentmin);



}

/*
  cout << "Child: " << endl;
  cout << "Avg: " << childtotal / 10000 << endl;
  cout << "min: " << childmin << endl;
  cout << "max: " << childmax << endl;
*/


//  exit (0);
//printf ("Elapsed Time %f\n", elapsedTime);

}


//
// main - The main routine 
//
int
main (int argc, char **argv)
{
  //Initialize Constants here

  //variables for Pipe
  int fd1[2], fd2[2], nbytes;
  //byte size messages to be passed through pipes 
  char childmsg[] = "1";
  char parentmsg[] = "2";
  char quitmsg[] = "q";

  /*Three Signal Handlers You Might Need
   *
   *I'd recommend using one signal to signal parent from child
   *and a different SIGUSR to signal child from parent
   */
  Signal (SIGUSR1, sigusr1_handler);	//User Defined Signal 1
  Signal (SIGUSR2, sigusr2_handler);	//User Defined Signal 2
  Signal (SIGINT, sigint_handler);

  numtests = 10000;

  //Determine the number of messages was passed in from command line arguments
  //Replace default numtests w/ the commandline argument if applicable 
  if (argc < 2)
    {

      printf ("Not enough arguments\n");
      exit (0);

    }

  printf ("Number of Tests %d\n", numtests);
  // start timer
  gettimeofday (&t1, NULL);

  if (strcmp (argv[1], "-p") == 0)
    {

      pipe (fd1);
      pipe (fd2);


      char readbuffer[80];
      int pid = fork ();
      int flag = 0;
      int status;


      while (numtests > 0)
	{

	  if (pid > 0)
	    {

	      gettimeofday (&parentstart, NULL);


	      if (flag = 1)
		{


		  read (fd2[0], parentmsg, sizeof (parentmsg));
		  gettimeofday (&parentend, NULL);
		  //cout << "Parent Recieved: "<< parentmsg[0] << endl;      
		  flag = 0;
		  //waitpid(getppid(),&status,0); 

		  write (fd1[1], childmsg, (strlen (childmsg) + 1));



		  parentTime =
		    (parentend.tv_sec - parentstart.tv_sec) * 1000.0;
		  parentTime +=
		    (parentend.tv_usec - parentstart.tv_usec) / 1000.0;
		  if (parentTime < parentmin || parentmin == 0)
		    parentmin = parentTime;

		  if (parentTime > parentmax || parentmax == 0)
		    parentmax = parentTime;

		  parenttotal = parentTime + parenttotal;

		}

	    }

	  else
	    {

	      gettimeofday (&childstart, NULL);


	      write (fd2[1], parentmsg, (strlen (parentmsg) + 1));

	      read (fd1[0], childmsg, sizeof (childmsg));
	      //cout << "Child Recieved: "<< childmsg[0] << endl;   
	      //waitpid(getpid(),&status,0);
	      gettimeofday (&childend, NULL);


	      flag = 1;


	      childTime = (childend.tv_sec - childstart.tv_sec) * 1000.0;
	      childTime += (childend.tv_usec - childstart.tv_usec) / 1000.0;
	      if (childTime < childmin || childmin == 0)
		childmin = childTime;

	      if (childTime > childmax || childmax == 0)
		childmax = childTime;

	      childtotal = childTime + childtotal;
	      //cout << numtests << endl;               
	      if (numtests == 2)
		{

		  cout << "Child: " << endl;
		  cout << "Avg: " << childtotal / 10000 << endl;
		  cout << "min: " << childmin << endl;
		  cout << "max: " << childmax << endl;




		}

	      // write(fd1[1], childmsg, (strlen(childmsg)+1));
	      //_exit(0);
	      //exit(getpid());
	    }
	  numtests = numtests - 1;
	}


      // stop timer
      gettimeofday (&t2, NULL);

      int wait = 0;

      close (fd1[0]);
      close (fd1[1]);
      close (fd2[1]);
      close (fd2[1]);


      kill (pid, SIGKILL);


      waitpid (getppid (), &status, 0);
      //int pid_p = getpid();             
      //cout << pid_p << endl;
      cout << "Parent: " << endl;
      cout << "Avg: " << parenttotal / 10000 << endl;
      cout << "min: " << parentmin << endl;
      cout << "max: " << parentmax << endl;
//}


      //      }

      // compute and print the elapsed time in millisec
      elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;	// sec to ms
      elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;	// us to ms
      printf ("Elapsed Time %f\n", elapsedTime);
      exit (0);
    }

  if (strcmp (argv[1], "-s") == 0)
    {
      int pid;
      int sigflag = 0;
      fflush (NULL);

      childpid = fork ();

      if (childpid != 0)
	{
	  cout << "parent pid " << getpid () << endl;
	  cout << "child pid " << childpid << endl;
	  while (numtests > 0)
	    {

	      gettimeofday (&t2, NULL);
	      kill (childpid, SIGUSR1);
	      while (iswaiting == 1);
	      iswaiting = 1;
	      // if(numtests == 0)
	      //kill(childpid, SIGINT);
	      // cout << numtests << endl;
	    }

	}

      else if (pid == 0)
	{

	  for (;;);


	}




      gettimeofday (&t2, NULL);

      // compute and print the elapsed time in millisec
      elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;	// sec to ms
      elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;	// us to ms
      printf ("Elapsed Time %f\n", elapsedTime);

    }

}

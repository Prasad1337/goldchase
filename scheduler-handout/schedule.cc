//
//
//	Pradyumna Kamat : pkamat (Prasad1337)
//
//


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "schedule.h"


/**
 * Function to initialize any global variables for the scheduler. 
 */
void init()
{
	pcount=0;	//process count
	q=1;	//quanta tracker [starts with 1]
	root=(struct node *)malloc(sizeof(struct node));
	cur=root;
}


/**
 * Function to add a process to the scheduler
 * @Param pid - the ID for the process/thread to be added to the 
 *      scheduler queue
 * @Param priority - priority of the process being added
 * @return true/false response for if the addition was successful
 */
int addProcess(int pid, int priority)
{
	if(pcount<=INT_MAX)
	{
		if(pcount>0)
		{
			cur->next=(struct node *)malloc(sizeof(struct node));
			cur=cur->next;
		}

		cur->pid=pid;
		cur->priority=priority;
		cur->next=root;
	    ++pcount;

	    return 1;
	}

	return 0;
}


/**
 * Function to remove a process from the scheduler queue
 * @Param pid - the ID for the process/thread to be removed from the
 *      scheduler queue
 * @Return true/false response for if the removal was successful
 */
int removeProcess(int pid)
{
	struct node *temp;
	struct node *temp1;

	if(root->pid==pid)
	{
		if(root->next!=root)
			temp=root->next;
		else
			temp=(struct node *)malloc(sizeof(struct node));

		free(root);
		root=temp;
		--pcount;

		return 1;
	}

	temp=root;

	while(temp->next!=root)
	{
		if(temp->next->pid==pid)
		{
			temp1=temp->next;
			temp->next=temp1->next;
			free(temp1);
			--pcount;

			return 1;
		}

		temp=temp->next;
	}

	return 0;
}


/**
 * Function to get the next process from the scheduler
 * @Param time - pass by reference variable to store the quanta of time
 * 		the scheduled process should run for
 * @Return returns the thread id of the next process that should be 
 *      executed, returns -1 if there are no processes
 */
int nextProcess(int &time)
{
	/*if(pcount==0)
		return -1;

	else if(pcount==1)
	{
		time=root->priority;
		return -1;
	}

	else
	{*/
		if(cur->next==root)
			cur=root;

		else
			cur=cur->next;
		
		time=cur->priority;
		return cur->pid;
	//}
}


/**
 * Function that returns a boolean 1 True/0 False based on if there are any 
 * processes still scheduled
 * @Return 1 if there are processes still scheduled 0 if there are no more 
 *		scheduled processes
 */
int hasProcess()
{
	if(pcount>0)
		return 1;

	return 0;
}

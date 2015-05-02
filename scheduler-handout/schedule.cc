//
//
//	Pradyumna Kamat : pkamat (Prasad1337)
//
//


//Header file includes
#include <stdlib.h>
#include <stdio.h>

#include "schedule.h"


//Function prototypes
int isEmpty(Queue *);
int removeItem(int,Queue *);
Node* findItem(int,Queue *);


/**
 * Function to initialize any global variables for the scheduler. 
 */
void init()
{
	cQ=0;

	for(int i=0;i<4;i++)
		initQ(Q+i);
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
	addQ(pid,(Q+(priority-1)));
}


/**
 * Function to remove a process from the scheduler queue
 * @Param pid - the ID for the process/thread to be removed from the
 *      scheduler queue
 * @Return true/false response for if the removal was successful
 */
int removeProcess(int pid)
{
	for(int i=0;i<4;i++)
	{
		if(removeItem(pid,(Q+i)))
			return 1;
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
	int val;

	   if(!isEmpty(Q+cQ))
		{
			val=popQ(Q+cQ);
			time=4-cQ;
			addQ(val,Q+cQ);
			cQ=(cQ+1)%4;
			
			return val;
		}

		cQ=(cQ+1)%4;

		if(!isEmpty(Q+cQ))
		{
			val=popQ(Q+cQ);
			time=4-cQ;
			addQ(val,Q+cQ);
			cQ=(cQ+1)%4;
			return val;
			
		}

		cQ=(cQ+1)%4;

		if(!isEmpty(Q+cQ))
		{
			val=popQ(Q+cQ);
			time=4-cQ;
			addQ(val,Q+cQ);
			cQ=(cQ+1)%4;
			return val;
			
		}

		cQ=(cQ+1)%4;

		if(!isEmpty(Q+cQ))
		{
			val=popQ(Q+cQ);
			time=4-cQ;
			addQ(val,Q+cQ);
			cQ=(cQ+1)%4;
			return val;
			
		}
}

/**
 * Function that returns a boolean 1 True/0 False based on if there are any 
 * processes still scheduled
 * @Return 1 if there are processes still scheduled 0 if there are no more 
 *		scheduled processes
 */
int hasProcess()
{
	for(int i=0;i<4;i++)
	{
		if(!isEmpty(Q+i))
			return 1;
	}

	return 0;
}



void initQ(Queue *q)
{
	q->beg=NULL;
	q->end=NULL;
}


void addQ(int pid,Queue *q)
{
	Node *temp=(Node *)malloc(sizeof(Node));
	temp->val=pid;
	temp->prev=NULL;
	temp->next=NULL;
	
	if(q->beg==NULL && q->end==NULL)
	{
		q->beg=temp;
		q->end=temp;
	}

	else if(q->beg==q->end)
	{
		temp->prev=q->beg;
		q->end=temp;
		q->beg->next=q->end;
	}

	else
	{
		temp->prev=q->end;
		q->end->next=temp;
		q->end=temp;
	}
}


int popQ(Queue *q)
{	
	Node *temp;
	int val;
	
	if(q->beg==NULL && q->end==NULL)
		return -1;

	else if(q->beg==q->end)
	{
		val=q->beg->val;
		free(q->beg);
		q->beg=NULL;
		q->end=NULL;
			
		return val;
	}

	else 
	{
		val=q->beg->val;
		q->beg=q->beg->next;
		free(q->beg->prev);
		q->beg->prev=NULL;

		return val;		
	}
	
}



int removeItem(int pid,Queue *q)
{
	Node *temp=findItem(pid,q);	
	
	if(temp !=NULL)
	{
		if(temp->val==pid)
		{
			if(temp==q->beg)
				q->beg=q->beg->next;

			if(temp==q->end)
				q->end=q->end->prev;

			if(temp->prev!=NULL)
				temp->prev->next=temp->next;

			if(temp->next!=NULL)
				temp->next->prev=temp->prev;
		}

		return 1;
	}

	else
		return 0;
}


Node* findItem(int pid,Queue *q)
{
	Node *temp;
	for(temp=q->beg;temp!=NULL;temp=temp->next)
	{
		if(temp->val==pid)
			return temp;
	}

	return NULL;
}


int isEmpty(Queue *q)
{
	if(q->beg==NULL && q->end==NULL)
		return 1;	
	else
		return 0;
}

#include "schedule.h"
#include <stdlib.h>
#include <stdio.h>



Queue processQue[4];
int currentQue;
/**
 * Function to initialize any global variables for the scheduler. 
 */
 
void initQueue(Queue *q)
{
	q->first=NULL;
	q->last=NULL;	
}

void addQueue(Queue *q,int value)
{
	Node *temp = (Node *)malloc(sizeof(Node));
	temp->data = value;
	temp->prev=NULL;
	temp->next=NULL;
	
	//if the queue is empty
	if(q->first == NULL && q->last==NULL)
	{
		q->first = temp;
		q->last = temp;
		
	}// if it has 1 node 
	else if( q-> first == q->last)
	{
		temp->prev=q->first;		
		q->last=temp;
		q->first->next=q->last;
	}//more than 2
	else 
	{
		temp->prev=q->last;
		q->last->next=temp;
		q->last=temp;	
		
	}
	
}

int popQueue(Queue *q)
{	
	Node *temp ;
	int data;
	//if the queue is empty
	if(q->first == NULL && q->last==NULL)
	{
		//printf("Popping from Queue of size 0..\n");
		
		return -1;	
	}// if it has 1 node 
	else if( q-> first == q->last)
	{
		//printf("Popping from Queue of size 1...\n");
		data=q->first->data;
		free(q->first);
		q->first=NULL;
		q->last=NULL;
			
		return data;	
	}//more than 2
	else 
	{
		//printf("Popping from Queue of size 2 or greater\n");
		data=q->first->data;
		q->first=q->first->next;
		free(q->first->prev);
		q->first->prev=NULL;
		return data;		
	}
	
}
int isEmpty(Queue *q)
{
	if(q->first == NULL && q->last==NULL)
		return 1;	
	else
		return 0;
}
void printQ(Queue *q)
{
	Node *temp;
	
	for(temp = q->first;temp != NULL; temp=temp->next)
	{
		printf("data : %d \n",temp->data);
	}
}

Node* findElement(Queue *q,int data)
{
	Node *temp;
	for(temp=q->first;temp!=NULL;temp=temp->next)
	{
		if(temp->data == data)
		{
			return temp; 
				
		}
	}
	return NULL;
}
int removeElement(Queue *q,int data)
{
	Node *temp = findElement(q,data);	
	
	if(temp !=NULL)
	{
		if(temp->data == data)
		{
			if(temp==q->first)
			{
				q->first=q->first->next;
			}
			if(temp==q->last)
			{
				q->last=q->last->prev;
			}
			if(temp->prev !=NULL )
			{
				temp->prev->next=temp->next;
				
			}
			if(temp->next !=NULL )
			{
				temp->next->prev=temp->prev;
			}
				
		}
		return 1;
	}
	else
	{
		//printf("no such process...in remove processes function \n ");
		return 0;
	}
	
	
}

void init(){
	
	int i;
	
	currentQue =0;
	for(i=0;i<4;i++)
		initQueue(processQue+i);
	
	
}

/**
 * Function to add a process to the scheduler
 * @Param pid - the ID for the process/thread to be added to the 
 *      scheduler queue
 * @Param priority - priority of the process being added
 * @return true/false response for if the addition was successful
 */
int addProcess(int pid, int priority){
	
	addQueue((processQue+(priority-1)),pid);
	
}

/**
 * Function to remove a process from the scheduler queue
 * @Param pid - the ID for the process/thread to be removed from the
 *      scheduler queue
 * @Return true/false response for if the removal was successful
 */
int removeProcess(int pid){
	
	int i,j;
	
	for(i=0;i<4;i++)
	{
			
			 if((removeElement((processQue+i),pid)))
			 {
				 return 1;
			}
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
int nextProcess(int &time){
	int data,i;
	
	
	
		
	   if(!isEmpty(processQue+currentQue))
		{
			data = popQueue(processQue+currentQue);
			time = 4 - currentQue;
			addQueue(processQue+currentQue,data);
			currentQue = (currentQue+1)%4;
			return data;
			
		}
		currentQue = (currentQue+1)%4;
		if(!isEmpty(processQue+currentQue))
		{
			data = popQueue(processQue+currentQue);
			time = 4 - currentQue;
			addQueue(processQue+currentQue,data);
			currentQue = (currentQue+1)%4;
			return data;
			
		}
		currentQue = (currentQue+1)%4;
		if(!isEmpty(processQue+currentQue))
		{
			data = popQueue(processQue+currentQue);
			time = 4 - currentQue;
			addQueue(processQue+currentQue,data);
			currentQue = (currentQue+1)%4;
			return data;
			
		}
		currentQue = (currentQue+1)%4;
		if(!isEmpty(processQue+currentQue))
		{
			data = popQueue(processQue+currentQue);
			time = 4 - currentQue;
			addQueue(processQue+currentQue,data);
			currentQue = (currentQue+1)%4;
			return data;
			
		}
}

/**
 * Function that returns a boolean 1 True/0 False based on if there are any 
 * processes still scheduled
 * @Return 1 if there are processes still scheduled 0 if there are no more 
 *		scheduled processes
 */
int hasProcess(){
	
	int i;
	for(i=0;i<4;i++)
	{
			if(!isEmpty(processQue+i))
			{
				//printf("Queue not empty \n");
				return 1;
			}
	}
	/*for(i=0;i<4;i++)
	{
			printQ(processQue+i);
	}*/
	return 0;
	
}

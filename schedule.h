#ifndef _schedule_h_
#define _schedule_h_


typedef struct Node
{
	struct Node *prev;
	struct Node *next;
	int data;
		
	
}Node;


typedef struct Queue
{
	Node *first;
	Node *last;
	
}Queue;

extern int currentQue;

extern Queue processQue[4];

void init();
int addProcess(int pid, int priority);
int removeProcess(int pid);
int nextProcess(int &time);
int hasProcess();


/////////
void addQueue(Queue *q,int value);
void initQueue(Queue *q);
int popQueue(Queue *q);
void printQ(Queue *q);
//////
#endif

#ifndef _schedule_h_
#define _schedule_h_


typedef struct node
{
	struct node *prev;
	struct node *next;
	int data;
}Node;


typedef struct queue
{
	node *first;
	node *last;
}Queue;


static int currentQue;
static Queue processQue[4];


void addQueue(Queue *q,int value);
void initQueue(Queue *q);
int popQueue(Queue *q);

void init();
int addProcess(int pid, int priority);
int removeProcess(int pid);
int nextProcess(int &time);
int hasProcess();

#endif

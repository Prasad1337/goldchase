#ifndef _schedule_h_
#define _schedule_h_


typedef struct node
{
	struct node *next;
	struct node *prev;
	int val;
}Node;


typedef struct queue
{
	node *beg;
	node *end;
}Queue;


static int cQ;
static Queue Q[4];


void init();
int addProcess(int pid, int priority);
int removeProcess(int pid);
int nextProcess(int &time);
int hasProcess();

void addQ(int,Queue *);
void initQ(Queue *);
int popQ(Queue *);

#endif

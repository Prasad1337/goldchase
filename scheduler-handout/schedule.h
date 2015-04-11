#ifndef _schedule_h_
#define _schedule_h_

#include <unistd.h>
#include <sys/types.h>

void init();
int addProcess(int pid, int priority);
int removeProcess(int pid);
int nextProcess(int &time);
int hasProcess();

struct node {
    pid_t pid;
    int priority;
    struct node *next;
};

struct node *root;
struct node *cur;

int pcount;

#endif

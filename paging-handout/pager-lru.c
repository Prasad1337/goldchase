#include <errno.h>
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "simulator.h"


static int tick=0;    //artificial time
static int timestamps[MAXPROCESSES][MAXPROCPAGES];


static int pages_alloc(Pentry q[MAXPROCESSES],int proc)
{
    int pg,c=0;

    for(pg=0;pg<MAXPROCPAGES;pg++)
    {
        if(q[proc].pages[pg])
            c++;
    }

    return c;
}


static void lru_page(Pentry q[MAXPROCESSES],int proc,int tick,int *evictee)
{
    int pg,t;

    *evictee=-1;
    t=tick+1;  

    for(pg=0;pg<MAXPROCPAGES;pg++)
    {
        if(!q[proc].pages[pg])
            continue;

        if(timestamps[proc][pg]<t)
        {
            t=timestamps[proc][pg];
            *evictee=pg;

            if(t<=1)
                break;
        }
    }

    if(*evictee<0)
    {
        printf("page for process %d w/ %u active pages not found with age < %u\n",proc,(unsigned int)pages_alloc(q,proc),tick);
        fflush(stdout);
    }
}
    
static void lru_pageit(Pentry q[MAXPROCESSES],int tick)
{
    int proc,pg,evicted;
    
    for(proc=0;proc<MAXPROCESSES;proc++)
    {
        pg=q[proc].pc/PAGESIZE;
        timestamps[proc][pg]=tick; 
        
        if(pagein(proc,pg))
            continue;

        if(pages_alloc(q,proc)<1) 
            continue;

        lru_page(q,proc,tick,&evicted);
        pageout(proc,evicted);
    }
}

void term()
{
    printf("final tick value was %d\n",tick);  
}

void pageit(Pentry q[MAXPROCESSES])
{
    if(tick<1)
    {
        int proc,pg;

        for(proc=0;proc<MAXPROCESSES;proc++)
        {
            for(pg=0;pg<MAXPROCPAGES;pg++)
                timestamps[proc][pg]=0; 
        }

        tick=1;
        atexit(term);
    }
    
    lru_pageit(q,tick);
    tick++;
}

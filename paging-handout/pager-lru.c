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
    int pg;
    int t;

    *evictee = -1;
    t = tick+1;  

    for(pg = 0; pg < MAXPROCPAGES; pg++) {
        if(!q[proc].pages[pg]) /* cant evict this */
            continue;

        if(timestamps[proc][pg] < t) {
            t = timestamps[proc][pg];
            *evictee = pg;

            if(t <= 1) /* cant do any better than that! */
                break;
        }
    }           

    if(*evictee < 0) {
        printf("page for process %d w/ %u active pages not found with age < %u\n", proc, (unsigned int) pages_alloc(q, proc), tick);
        fflush(stdout);
        //endit();
    }
}
    
static void lru_pageit(Pentry q[MAXPROCESSES], int tick)
{
    int proc, pg, evicted;
    
    for(proc = 0; proc < MAXPROCESSES; proc++) {
        if(!q[proc].active) /* done if its not active */
            continue;

        pg = q[proc].pc/PAGESIZE;
        /* note this time for future eviction decisions */
        timestamps[proc][pg] = tick; 
        
        /* done if the page is already in memory */
        if(q[proc].pages[pg]) 
            continue;

        /* the page is not in memory.
         * if pagein give 1 the page is either 
         * on its way already or we just got it
         * started, so we are done with this process
         */
        if(pagein(proc, pg) )
            continue;

        /* there are no free physical pages */
        if(pages_alloc(q, proc) < 1) 
            continue; /* must have at least one page to evict */

        lru_page(q, proc, tick, &evicted);
        pageout(proc, evicted);
    }
}

void exit_fn()
{
    printf("final tick value was %d\n", tick);  
}

void pageit(Pentry q[MAXPROCESSES])
{
    if(tick < 1)
    {
        int proc,pg;

        for(proc=0;proc<MAXPROCESSES;proc++)
        {
            for(pg=0;pg<MAXPROCPAGES;pg++)
                timestamps[proc][pg]=0; 
        }

        tick = 1;

        if(atexit(exit_fn) != 0)
        {
            perror(NULL);
            exit(1);
        }
    }   
    
    lru_pageit(q,tick);
    tick++;
} 


/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 *  This file contains an lru pageit
 *      implmentation.
 */
#include <errno.h>
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "simulator.h"

enum swapin_state {
    SWAPOUT = 1,
    SWAPIN,
    SWAPFAIL
};

static size_t pages_alloc(Pentry q[MAXPROCESSES], int proc) {
    int page;
    size_t amt = 0;

    for(page = 0; page < MAXPROCPAGES; page++) {
        if(q[proc].pages[page])
            amt++;
    }

    return amt;
}

static uint32_t timestamps[MAXPROCESSES][MAXPROCPAGES];

static void timestamps_init() {
    int proc, page;

    for(proc = 0; proc < MAXPROCESSES; proc++)
        for(page = 0; page < MAXPROCPAGES; page++)
            timestamps[proc][page] = 0; 

}

static void lru_page(Pentry q[MAXPROCESSES], int proc, uint32_t tick, int *evictee) {
    int page;
    uint32_t t;

    *evictee = -1;
    /* want to do better than or equal to
     * a page that was referenced just 
     * now. adding 1 to tick should
     * ensure this function always
     * returns a page
     */
    t = tick+1;  

    for(page = 0; page < MAXPROCPAGES; page++) {
        if(!q[proc].pages[page]) /* cant evict this */
            continue;

        if(timestamps[proc][page] < t) {
            t = timestamps[proc][page];
            *evictee = page;

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


static uint32_t tick = 0; // artificial time
    
static void lru_pageit(Pentry q[MAXPROCESSES], uint32_t tick) {
    int proc, page, evicted;
    
    for(proc = 0; proc < MAXPROCESSES; proc++) {
        if(!q[proc].active) /* done if its not active */
            continue;

        page = q[proc].pc/PAGESIZE;
        /* note this time for future eviction decisions */
        timestamps[proc][page] = tick; 
        
        /* done if the page is already in memory */
        if(q[proc].pages[page]) 
            continue;

        /* the page is not in memory.
         * if pagein give 1 the page is either 
         * on its way already or we just got it
         * started, so we are done with this process
         */
        if(pagein(proc, page) )
            continue;

        /* there are no free physical pages */
        if(pages_alloc(q, proc) < 1) 
            continue; /* must have at least one page to evict */

        lru_page(q, proc, tick, &evicted);

        if(!pageout(proc, evicted) ) {
            //endit();
        }
    }
}

void exit_fn() {
    printf("final tick value was %d\n", tick);  
}

void pageit(Pentry q[MAXPROCESSES]) { 
    /* tick starts at 1, so 0 means this is the first run
     * or an overflow. either way, reset timestamps.
     */ 
    if(tick < 1) {
        timestamps_init();      
        tick = 1;
        if(atexit(exit_fn) != 0) {
            perror(NULL);
            exit(1);
        }
    }   
    
    lru_pageit(q, tick);
    
    /* advance time for next pageit iteration */
    tick++;
} 


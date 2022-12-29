#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS 2
#define THREAD_1 1
#define THREAD_2 2

typedef struct
{
    int threadIdx;
} threadParams_t;


pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];
struct sched_param nrt_param;

// On the Raspberry Pi, the MUTEX semaphores must be 
// statically initialized
//
// This works on all Linux platforms, but dynamic 
// initialization does not work
// on the R-Pi in particular as of June 2020.
//
pthread_mutex_t rsrcA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rsrcB = PTHREAD_MUTEX_INITIALIZER;

volatile int rsrcACnt = 0;
volatile int rsrcBCnt = 0;
volatile int noWait = 0;


void* grabRsrcs(void* threadp)
{
    threadParams_t* threadParams = (threadParams_t *)threadp;
    int threadIdx = threadParams->threadIdx;

    if(threadIdx == THREAD_1)
    {
        fputs("THREAD 1 grabbing resources\n", stdout);
        pthread_mutex_lock(&rsrcA);
        pthread_mutex_lock(&rsrcB);
        ++rsrcACnt;
        if(!noWait) 
		    sleep(1);
        fputs("THREAD 1 got A, trying for B\n", stdout);
        ++rsrcBCnt;
        fputs("THREAD 1 got A and B\n", stdout);
        pthread_mutex_unlock(&rsrcB);
        pthread_mutex_unlock(&rsrcA);
        fputs("THREAD 1 done\n", stdout);
    }
    else
    {
        fputs("THREAD 2 grabbing resources\n", stdout);
        pthread_mutex_lock(&rsrcB);
        pthread_mutex_lock(&rsrcA);
        ++rsrcBCnt;
        if(!noWait)
			sleep(1);
        fputs("THREAD 2 got B, trying for A\n", stdout);
        ++rsrcACnt;
        fputs("THREAD 2 got B and A\n", stdout);
        pthread_mutex_unlock(&rsrcA);
        pthread_mutex_unlock(&rsrcB);
        fputs("THREAD 2 done\n", stdout);
    }
    pthread_exit(NULL);
}


int main (int argc, char *argv[])
{
    int rc = 0;
	int safe = 0;
    int rsrcACnt = 0;
	int rsrcBCnt = 0;
	int noWait = 0;
	if(argc < 2)
		fputs("Will set up unsafe deadlock scenario\n", stdout);
	else if(argc == 2)	
		if(strncmp("safe", argv[1], 4) == 0)
			safe=1;
		else if(strncmp("race", argv[1], 4) == 0)
			noWait=1;
		else
			fputs("Will set up unsafe deadlock scenario\n", stdout);
	else
		fputs("Usage: deadlock [safe|race|unsafe]\n");

    printf("Creating thread %d\n", THREAD_1);
    threadParams[THREAD_1].threadIdx = THREAD_1;
    rc = pthread_create(&threads[0], NULL, grabRsrcs, (void *)&threadParams[THREAD_1]);
    if (rc)
    {
	    printf("ERROR; pthread_create() rc is %d\n", rc); 
		perror(NULL); 
		exit(-1);
	}
    fputs("Thread 1 spawned\n", stdout);

    if(safe) // Make sure Thread 1 finishes with both resources first
        if(pthread_join(threads[0], NULL) == 0)
            printf("Thread 1: %x done\n", (unsigned int)threads[0]);
        else
            perror("Thread 1");

   printf("Creating thread %d\n", THREAD_2);
   threadParams[THREAD_2].threadIdx=THREAD_2;
   rc = pthread_create(&threads[1], NULL, grabRsrcs, (void *)&threadParams[THREAD_2]);
   if (rc) {printf("ERROR; pthread_create() rc is %d\n", rc); perror(NULL); exit(-1);}
   printf("Thread 2 spawned\n");

   printf("rsrcACnt=%d, rsrcBCnt=%d\n", rsrcACnt, rsrcBCnt);
   printf("will try to join CS threads unless they deadlock\n");

   if(!safe)
   {
     if(pthread_join(threads[0], NULL) == 0)
       printf("Thread 1: %x done\n", (unsigned int)threads[0]);
     else
       perror("Thread 1");
   }

   if(pthread_join(threads[1], NULL) == 0)
     printf("Thread 2: %x done\n", (unsigned int)threads[1]);
   else
     perror("Thread 2");

   if(pthread_mutex_destroy(&rsrcA) != 0)
     perror("mutex A destroy");

   if(pthread_mutex_destroy(&rsrcB) != 0)
     perror("mutex B destroy");

   printf("All done\n");

   exit(0);
}

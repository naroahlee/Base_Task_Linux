#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sched.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#include "calibration.h"
#include "tracing.h"



/*
 * Global data structure to record the data
 * The output is per-task.
 * To minimize interference, you should dump it to /dev/shm (shared memory)
 */


/*
 * Task related parameters, time unit is milli second
 */
int wcet = 0;                   /* worest case execution time */
int period = 0;                 /* period (we assume deadline equals period) */
long long duration = 0;         /* task execution duration */
int priority = 0;               /* task priority */
int count = 0;                  /* number of jobs to execute (duration / period) */
int gs32looptime = 0;
volatile int32_t gs32JobID = 0;  /* job index */
int32_t gs32PID;
uint16_t gu16RefID;
char gacFileName[100];

int mapwcet2looptime(int s32wcet)
{
	int s32looptime;
	if(s32wcet <= 0)
	{
		s32looptime = 0;
	}
	else if(s32wcet <= 52)
	{
		s32looptime = (int)(s32wcet / 2.6) + 1;
	}
	else if(s32wcet <= 10000)
	{
		s32looptime = (int)((9280.0 / 898.0 * s32wcet) - 516.38);
	}
	else
	{
		s32looptime = s32wcet / 10 + 1;
	}

	return s32looptime;
}
/*
 * 1ms workload on a specific machine
 * You need to tune this value or change workload
 */
static inline void workload_for_100us(void) {
    volatile double d64temp = 0.0;
    int64_t s64i;

   	for (s64i = 0; s64i < WORKLOAD_QUANTUM; s64i++ )
	{
       	d64temp = sqrt((double)s64i * s64i);
		d64temp = d64temp;
	}
	return;
}

/*
 * each job's work
 * record start time, finish time
 */
static void work(int sig, siginfo_t *extra, void *cruft)
{
	int32_t s32i;

    /* We have reached the count. Print res and quit */
	if (count != 0 && gs32JobID >= count) {
		dump_tracing_to_file(gacFileName);
        /* sleep(2); */             /* sleep for 10 sec, wait for other task to finish */
        exit(1);
    }

	/* TBD Record JobID & StartTime */
	/* Warning: when we run this line of code   */
	/*  That essentially means we are scheduled */
	/*  So, it is not the Release Time          */
	/*  We had to computer the Release Time     */
    /*  After we get all these Information      */

	add_record_task(TASK_EVENT_START_JOB,  gu16RefID, gs32PID, gs32JobID);
    for (s32i = 0; s32i < gs32looptime; s32i++)
	{
        workload_for_100us();
    }
	/* TBD Record JobID & FinishTime */
	add_record_task(TASK_EVENT_FINISH_JOB, gu16RefID, gs32PID, gs32JobID);
	
	gs32JobID++;

	return;
}


static void exit_dump(int sig, siginfo_t *extra, void *cruft)
{
	printf("SIGINT Rcved! Dump File\n");
	dump_tracing_to_file(gacFileName);
	/* sleep(1); */
	exit(1);
	return;
}

/*
 * Set affinity of the task, alwasy pin it to core 0
 */
#if 0
static void set_cpuaffinity(void)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) < 0)
	{
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
	return;
}
#endif

static void set_sched(void)
{

    struct sched_param sched;
    sched.sched_priority = priority;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &sched) < 0)
	{
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }
	return;
}

int main(int argc, char *argv[])
{
    struct sigaction sa;
    sigset_t allsigs;
    struct itimerspec timerspec;
    struct timespec now;
    timer_t timer;
    struct sigevent timer_event;

	wcet =		atoi(argv[1]);        
	period =	atoi(argv[2]);
	count =		atoi(argv[3]);
	priority =  atoi(argv[4]);
	gu16RefID = atoi(argv[5]);

	gs32looptime = mapwcet2looptime(wcet);

	if(argc != 6)
	{
		fprintf(stderr, "Argu ERROR!\n");
		exit(EXIT_FAILURE);
	}

    printf("Task[%d] wcet[%d] period[%d] count[%d] priority[%d]\n",
			gu16RefID, wcet, period, count, priority);
 
	sprintf(gacFileName, "Task%04d.trace", gu16RefID);
	/* Set SCHED_FIFO */
	set_sched();

	/* INIT Tracing */
	init_tracing();
	gs32PID = getpid();

	/* Bind a sighandler, using SIGRTMIN */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = work;

    if (sigaction(SIGRTMIN, &sa, NULL) < 0) {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }

	/* Bind another sighander, using SIGINT */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = exit_dump;

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
	

	raise(SIGSTOP);

    /* the timer */
    timerspec.it_interval.tv_sec = period / 1000;
	/* Correct the Timer Interval Error Regardless of rdtsc*/
    timerspec.it_interval.tv_nsec = (period % 1000) * 1000000;

    /* the start time */
    /* if(clock_gettime(CLOCK_MONOTONIC, &now) < 0) { */
    if(clock_gettime(CLOCK_REALTIME, &now) < 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    // Start one second from now.
	add_record_task(TASK_EVENT_SET_TIMER, gu16RefID, gs32PID, 0);
    timerspec.it_value.tv_sec = now.tv_sec + 1;
	/* Correct the Timer Start Error Regardless of rdtsc*/
    timerspec.it_value.tv_nsec = now.tv_nsec;

    timer_event.sigev_notify = SIGEV_SIGNAL;
    timer_event.sigev_signo = SIGRTMIN;
    timer_event.sigev_value.sival_ptr = (void *)&timer;

    if (timer_create(CLOCK_REALTIME, &timer_event, &timer) < 0) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    if (timer_settime(timer, TIMER_ABSTIME, &timerspec, NULL) < 0) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&allsigs);



    while(1) {
        sigsuspend(&allsigs);
    }
    
    exit(EXIT_SUCCESS);
}


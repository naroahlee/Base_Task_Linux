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

#ifdef __x86_64__
static __inline__ uint64_t rdtscp(void)
{
	uint32_t hi, lo;
	__asm__ __volatile__ ("rdtscp" : "=a"(lo), "=d"(hi));
	return (
			 ((uint64_t)lo) | 
			(((uint64_t)hi) << 32)
		   );
}
#endif



/*
 * 1ms workload on a specific machine
 * You need to tune this value or change workload
 */
static inline void workload_quantum(void) {
    volatile double d64temp = 0.0;
    int64_t s64i;
   	for (s64i = 0; s64i < WORKLOAD_QUANTUM; s64i++)
	{
       	d64temp = sqrt((double)s64i * s64i);
		d64temp = d64temp;
	}
	return;
}

uint64_t do_Exp_once(int32_t s32Qcnt)
{
	uint64_t u64start;
	uint64_t u64end;
	int s32i;
	u64start = rdtscp();
	for(s32i = 0; s32i < s32Qcnt; s32i++)
	{
		workload_quantum();
	}
	u64end = rdtscp();
	return (u64end - u64start);
}

/*
 * each job's work
 * record start time, finish time
 */



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

static void set_sched(int32_t s32prio)
{

    struct sched_param sched;
    sched.sched_priority = s32prio;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &sched) < 0)
	{
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }
	return;
}

int main(int argc, char *argv[])
{
	FILE* fout;
	int32_t s32cnt;			/* Sample cnt */
	int32_t s32Qcnt;		/* Quantum Pieces cnt */
	int32_t s32prio;
	int		s32i;
	uint64_t u64Execute;
	char acOutput[60] = "";
	uint64_t* pu64rec;
	record_header strRec;

	if(argc != 5)
	{
		fprintf(stderr, "Useage: ./calibration Samples Qcnt Prio OutputFile!\n");
		exit(EXIT_FAILURE);
	}

	s32cnt  = atoi(argv[1]);
	s32Qcnt = atoi(argv[2]);
	s32prio = atoi(argv[3]);
	strcpy(acOutput, argv[4]);

	fout = fopen(acOutput, "wb");
	if(NULL == fout)
	{
		fprintf(stderr, "Open %s Error!\n", acOutput);
		exit(EXIT_FAILURE);
	}

    printf("Qcnt[%d] Prio[%d] Sample[%d]\n",
			s32Qcnt, s32prio, s32cnt);

	strRec.s32Qcnt = s32Qcnt;
	strRec.s32cnt  = s32cnt;
	

	pu64rec = (uint64_t*)malloc(sizeof(uint64_t) * s32cnt);
	if(NULL == pu64rec)
	{
		fprintf(stderr, "malloc Error!\n");
		exit(EXIT_FAILURE);
	}
	

	fwrite(&strRec, sizeof(record_header), 1, fout);

	/* Set SCHED_FIFO */
	set_sched(s32prio);

	for(s32i = 0; s32i < s32cnt; s32i++)
	{
		u64Execute = do_Exp_once(s32Qcnt);
		pu64rec[s32i] = u64Execute;
	}

	fwrite(pu64rec, sizeof(uint64_t), s32cnt, fout);
    
	free(pu64rec);
	fclose(fout);
    exit(EXIT_SUCCESS);
}


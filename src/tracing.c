/* tracing.h */
/* enable tracing feature */
#include <stdio.h>
#include <stdlib.h>
#include "tracing.h"

/* Event Record Buf List */
static event_record gstrRecList[EVENT_BUF_MAX];
/* only available in this file */
static event_record* gpstrcuritem = &gstrRecList[0];
const static event_record* gpstrLast = &gstrRecList[EVENT_BUF_MAX - 1];


static event_identify gstrEventList[] = 
{
	{"HOST_EVENT_RCV_CHGREQ"    , HOST_EVENT_RCV_CHGREQ},
	{"HOST_EVENT_ATT_CHGEND"    , HOST_EVENT_ATT_CHGEND},
	{"HOST_EVENT_SND_UPSYNACK"  , HOST_EVENT_SND_UPSYNACK},
	{"HOST_EVENT_SND_UPASYACK"  , HOST_EVENT_SND_UPASYACK},
	{"HOST_EVENT_RCV_CHGSYNACK" , HOST_EVENT_RCV_CHGSYNACK},
	{"HOST_EVENT_RCV_CHGASYACK" , HOST_EVENT_RCV_CHGASYACK},
	{"HOST_EVENT_ATT_FINDSEQ"   , HOST_EVENT_ATT_FINDSEQ},
	{"HOST_EVENT_ATT_XLREQ"     , HOST_EVENT_ATT_XLREQ},
	{"HOST_EVENT_ATT_XLRET"     , HOST_EVENT_ATT_XLRET},
	{"HOST_EVENT_ATT_EXIT"      , HOST_EVENT_ATT_EXIT},
	{"HOST_EVENT_SND_SYSSTART"  , HOST_EVENT_SND_SYSSTART},
	{"HOST_EVENT_ATT_REGOVER"   , HOST_EVENT_ATT_REGOVER},
    
	{"GUEST_EVENT_RCV_UPSYNACK" , GUEST_EVENT_RCV_UPSYNACK},
	{"GUEST_EVENT_RCV_UPASYACK" , GUEST_EVENT_RCV_UPASYACK},
	{"GUEST_EVENT_SND_CHGSYNACK", GUEST_EVENT_SND_CHGSYNACK},
	{"GUEST_EVENT_SND_CHGASYACK", GUEST_EVENT_SND_CHGASYACK},
	{"GUEST_EVENT_RCV_SYSSTART" , GUEST_EVENT_RCV_SYSSTART},

	{"TASK_EVENT_RELEASE_JOB"   , TASK_EVENT_RELEASE_JOB},
	{"TASK_EVENT_START_JOB"     , TASK_EVENT_START_JOB},
	{"TASK_EVENT_FINISH_JOB"    , TASK_EVENT_FINISH_JOB},
	{"TASK_EVENT_CATCH_DEADLINE", TASK_EVENT_CATCH_DEADLINE},
	{"TASK_EVENT_MISS_DEADLINE" , TASK_EVENT_MISS_DEADLINE},
	{"TASK_EVENT_SET_TIMER"     , TASK_EVENT_SET_TIMER},
};

static int32_t getDefEventNum(void)
{
	return sizeof(gstrEventList) / sizeof(event_identify);
}

char* getEventNamefromID(EVENT_ID s32ID)
{
	int s32i = 0;
	for(s32i = 0; s32i < getDefEventNum(); s32i++)
	{
		if(s32ID == gstrEventList[s32i].s32id)
		{
			return gstrEventList[s32i].acEvent;
		}
	}
	return NULL;
}

/* Read the rdtsc value              */
/* I don't allow external invocation */
#if defined(__i386__)
static __inline__ uint64_t rdtsc(void)
{
	unsigned long long int x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	return x;
}
#elif defined(__x86_64__)
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

void init_tracing(void)
{
	/* rewind the pointer */
	gpstrcuritem = &gstrRecList[0];
	return;
}

void add_record(event_record *pstrEvt)
{
	memcpy((void *)gpstrcuritem, (void *)pstrEvt, sizeof(event_record));
	/* Boundary Protection: Yeah, we will definately lost some data */
	gpstrcuritem = (gpstrcuritem != gpstrLast) ? gpstrcuritem + 1
											   : &gstrRecList[0];
	return;
}

__inline__ void add_record_host(EVENT_ID s32event, int32_t s32rsv)
{
	uint64_t u64tmp;
	u64tmp = rdtscp();

	/* Write a Record */
	gpstrcuritem->u16type  = HOST_EVENT;
	gpstrcuritem->s32id    = s32event;
	gpstrcuritem->s32srcID = 0;
	gpstrcuritem->s32rsv   = s32rsv;
	gpstrcuritem->u64tsc   = u64tmp;

	/* Boundary Protection: Yeah, we will definately lost some data */
	gpstrcuritem = (gpstrcuritem != gpstrLast) ? gpstrcuritem + 1
											   : &gstrRecList[0];
	
	return;
}


__inline__ void add_record_guest(EVENT_ID s32event, int32_t s32RegID, int32_t s32rsv)
{
	uint64_t u64tmp;
	u64tmp = rdtscp();

	/* Write a Record */
	gpstrcuritem->u16type  = GUEST_EVENT;
	gpstrcuritem->s32id    = s32event;
	gpstrcuritem->s32srcID = s32RegID;
	gpstrcuritem->s32rsv   = s32rsv;
	gpstrcuritem->u64tsc   = u64tmp;

	/* Boundary Protection: Yeah, we will definately lost some data */
	gpstrcuritem = (gpstrcuritem != gpstrLast) ? gpstrcuritem + 1
											   : &gstrRecList[0];
	
	return;
}

__inline__ void add_record_task(EVENT_ID s32event, uint16_t u16RefID, int32_t s32PID, volatile int32_t s32JID)
{
	uint64_t u64tmp;
	u64tmp = rdtscp();

	/* Write a Record */
	gpstrcuritem->u16type  = TASK_EVENT;
	gpstrcuritem->u16refID = u16RefID; 
	gpstrcuritem->s32id    = s32event;
	gpstrcuritem->s32srcID = s32PID;
	gpstrcuritem->s32rsv   = s32JID;
	gpstrcuritem->u64tsc   = u64tmp;

	/* Boundary Protection: Yeah, we will definately lost some data */
	gpstrcuritem = (gpstrcuritem != gpstrLast) ? gpstrcuritem + 1
											   : &gstrRecList[0];
	
	return;
}

__inline__ void add_record_task_job0_release(uint16_t u16RefID, int32_t s32PID, int32_t s32delayms)
{
    uint64_t u64tmp;
    u64tmp = rdtscp();

    /* Write a Record */
    gpstrcuritem->u16type  = TASK_EVENT;
    gpstrcuritem->u16refID = u16RefID;
    gpstrcuritem->s32id    = TASK_EVENT_RELEASE_JOB;
    gpstrcuritem->s32srcID = s32PID;
    gpstrcuritem->s32rsv   = 0;
    gpstrcuritem->u64tsc   = u64tmp + s32delayms * KHZ;

    /* Boundary Protection: Yeah, we will definately lost some data */
    gpstrcuritem = (gpstrcuritem != gpstrLast) ? gpstrcuritem + 1
                                               : &gstrRecList[0];
	return;
}

/* a special fxn, we donot record tsc directly, we just record them */
void add_record_task_other(EVENT_ID s32evtID, event_record* pstrEvent, uint64_t u64tsc)
{
	gpstrcuritem->u16type  = TASK_EVENT;
	gpstrcuritem->s32id    = s32evtID;
	gpstrcuritem->u16refID = pstrEvent->u16refID;
	gpstrcuritem->s32srcID = pstrEvent->s32srcID;
	gpstrcuritem->s32rsv   = pstrEvent->s32rsv;
	gpstrcuritem->u64tsc   = u64tsc;

	/* Boundary Protection: Yeah, we will definately lost some data */
	gpstrcuritem = (gpstrcuritem != gpstrLast) ? gpstrcuritem + 1
											   : &gstrRecList[0];
}

void add_record_task_sup(EVENT_ID s32evtID, int32_t s32jobID, event_record* pstrEvent, uint64_t u64tsc)
{
	gpstrcuritem->u16type  = TASK_EVENT;
	gpstrcuritem->s32id    = s32evtID;
	gpstrcuritem->u16refID = pstrEvent->u16refID;
	gpstrcuritem->s32srcID = pstrEvent->s32srcID;
	gpstrcuritem->s32rsv   = s32jobID;
	gpstrcuritem->u64tsc   = u64tsc;

	/* Boundary Protection: Yeah, we will definately lost some data */
	gpstrcuritem = (gpstrcuritem != gpstrLast) ? gpstrcuritem + 1
											   : &gstrRecList[0];
}

void dump_tracing_to_file(char* pcName)
{
	FILE* fp;
	event_record* pstrtemp;
	fp = fopen(pcName, "wb");
	if(NULL == fp)
	{
		fprintf(stderr, "[TRC] ERR: Dump File Error!\n");
		exit(EXIT_FAILURE);
	}

	pstrtemp = &gstrRecList[0];
	while(pstrtemp != gpstrcuritem)
	{
		fwrite((void *)pstrtemp, sizeof(event_record), 1, fp);
		pstrtemp++;
	}

	fclose(fp);
	return;
}

void dump_tracing_to_screen(void)
{
	event_record* pstrtemp;

	pstrtemp = &gstrRecList[0];
	while(pstrtemp != gpstrcuritem)
	{
		printf("TYPE[%d] SRC[%d] EVENT[%d] TSC[%20lu]\n", 
				pstrtemp->u16type,
				pstrtemp->s32srcID,
				pstrtemp->s32id,
				pstrtemp->u64tsc);
		pstrtemp++;
	}

	return;
}

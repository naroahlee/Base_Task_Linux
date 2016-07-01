/* tracing.h */
#ifndef __TRACING__
#define __TRACING__

#include <stdint.h>
#include <string.h>
#define EVENT_BUF_MAX (40960)
#define EVENT_NAME_LEN (40)

#define HOST_EVENT    (1)
#define GUEST_EVENT   (2)
#define TASK_EVENT    (3)

/* ============== HOST EVENT =============== */
#define HOST_EVENT_RCV_CHGREQ	  (101)
#define HOST_EVENT_ATT_CHGEND     (102)
#define HOST_EVENT_SND_UPSYNACK   (103)
#define HOST_EVENT_SND_UPASYACK   (104)
#define HOST_EVENT_RCV_CHGSYNACK  (105)
#define HOST_EVENT_RCV_CHGASYACK  (106)
#define HOST_EVENT_ATT_FINDSEQ    (107)
#define HOST_EVENT_ATT_XLREQ      (108)
#define HOST_EVENT_ATT_XLRET      (109)
#define HOST_EVENT_ATT_REGOVER    (110)
#define HOST_EVENT_SND_SYSSTART   (198)
#define HOST_EVENT_ATT_EXIT       (199)

#define GUEST_EVENT_RCV_UPSYNACK  (201)
#define GUEST_EVENT_RCV_UPASYACK  (202)
#define GUEST_EVENT_SND_CHGSYNACK (203)
#define GUEST_EVENT_SND_CHGASYACK (204)
#define GUEST_EVENT_RCV_SYSSTART  (205)

#define TASK_EVENT_RELEASE_JOB    (301)
#define TASK_EVENT_START_JOB      (302)
#define TASK_EVENT_FINISH_JOB	  (303)
#define TASK_EVENT_CATCH_DEADLINE (304)
#define TASK_EVENT_MISS_DEADLINE  (305)
#define TASK_EVENT_SET_TIMER      (306)

#define HZ                 (3299040000)
#define KHZ                (3299040)
#define MHZ                (3299)

typedef int32_t EVENT_ID;
typedef uint16_t EVENT_TYPE;

typedef struct event_record_t
{
	EVENT_TYPE u16type;
	uint16_t   u16refID;
	EVENT_ID s32id;
	int32_t  s32srcID;
	int32_t  s32rsv;
	uint64_t u64tsc;
}event_record;

typedef struct event_identify_t
{
	char acEvent[EVENT_NAME_LEN];
	EVENT_ID s32id;
} event_identify;

void init_tracing(void);

void add_record(event_record *pstrEvt);
__inline__ void add_record_host(EVENT_ID s32event, int32_t s32rsv);
__inline__ void add_record_guest(EVENT_ID s32event, int32_t s32RegID, int32_t s32rsv);
__inline__ void add_record_task(EVENT_ID s32event, uint16_t u16RefID, int32_t s32PID, volatile int32_t s32JID);
void add_record_task_other(EVENT_ID s32evtID, event_record* pstrEvent, uint64_t u64tsc);
void add_record_task_sup(EVENT_ID s32evtID, int32_t s32jobID, event_record* pstrEvent, uint64_t u64tsc);
void dump_tracing_to_file(char* pcName);
void dump_tracing_to_screen(void);
char* getEventNamefromID(EVENT_ID s32ID);

#endif

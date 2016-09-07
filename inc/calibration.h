/* calibration.h */
#ifndef __CALIBRATION_H__ 
#define __CALIBRATION_H__

/* For 100us */
#define WORKLOAD_QUANTUM (5650)

typedef struct record_header_t
{
	int32_t s32Qcnt;
	int32_t s32cnt;
} record_header;

#endif


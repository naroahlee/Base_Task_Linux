#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"

#define MAX_ELEMENT_NUM (1000)
#define MAX_TESTLEN (255)

#if defined(__i386__)
static __inline__ unsigned long long rdtsc(void)
{
	unsigned long long int x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
	return x;
}
#elif defined(__x86_64__)
static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}
#endif

int main(int argc, char* argv[])
{
	int s32i;
	int s32Temp;
	ElementType strTemp;
	ElementType * pstrTemp;
	Priority_Queue pstrPQ;
	pstrPQ = PQ_Init(MAX_ELEMENT_NUM);
	(void)PQ_Print(pstrPQ);

	for(s32i = 0; s32i < MAX_TESTLEN; s32i++)
	{
		srand((unsigned int) rdtsc());
		strTemp = (rand() % 1000);
		printf("Key = %d\n", strTemp);
		PQ_Insert(pstrPQ, strTemp);
	}
	(void)PQ_Print(pstrPQ);

	for(s32i = 0; s32i < MAX_TESTLEN; s32i++)
	{
		pstrTemp = PQ_FindMax(pstrPQ);
		printf("%4d", *pstrTemp);
		PQ_DeleteMax(pstrPQ);
		//(void)PQ_Print(pstrPQ);

	}
	printf("\n");

	(void)PQ_Destroy(pstrPQ);
	return 0;
}

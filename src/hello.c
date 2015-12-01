#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"

#define MAX_ELEMENT_NUM (1000)

int main(int argc, char* argv[])
{
	Priority_Queue pstrPQ;
	pstrPQ = PQ_Init(MAX_ELEMENT_NUM);
	(void)PQ_Destroy(pstrPQ);
	return 0;
}

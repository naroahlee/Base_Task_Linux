#include <stdio.h>
#include <stdlib.h>
#include "priority_queue.h"


Priority_Queue PQ_Init(int s32MaxSize)
{
	Priority_Queue pstrRet;
	if(NULL == (pstrRet = malloc(sizeof(HeapStruct))))
	{
		printf("PQ_Init() Malloc Error!\n");
		exit(-1);
	}
	
	if(NULL == (pstrRet->pstrEleList = malloc((s32MaxSize + 1) * sizeof(ElementType))))
	{
		printf("PQ_Init() Elelist alloc Error!\n");
		exit(-1);
	}

	pstrRet->s32Capacity = s32MaxSize;
	pstrRet->s32Size = 0;
	pstrRet->pstrEleList[0].s32Key = MAX_DATA;
	return pstrRet;
}

int PQ_Destroy(Priority_Queue pstrPQ)
{
	free(pstrPQ->pstrEleList);
	free(pstrPQ);
	return 0;
}

int PQ_IsEmpty(Priority_Queue pstrPQ)
{
	if(0 == pstrPQ->s32Size)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int PQ_IsFull(Priority_Queue pstrPQ)
{
	if(pstrPQ->s32Size < pstrPQ->s32Capacity)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int PQ_Insert(Priority_Queue pstrPQ, ElementType strX)
{
	int s32i;
	if(1 == PQ_IsFull(pstrPQ))
	{
		printf("ProrityQueue Full! Error!\n");
		return -1;
	}

	for(s32i = pstrPQ->s32Size; pstrPQ->pstrEleList[s32i / 2].s32Key < strX.s32Key; s32i /= 2)
	{
		pstrPQ->pstrEleList[s32i] = pstrPQ->pstrEleList[s32i / 2];
	}
	pstrPQ->pstrEleList[s32i] = strX;
	pstrPQ->s32Size++;

	return 0;
}

int PQ_DeleteMax(Priority_Queue pstrPQ)
{
	int s32i, s32Child;
	ElementType strLastElement;
	if(1 == PQ_IsEmpty(pstrPQ))
	{
		printf("Priority Queue is Empty!\n");
		return -1;
	}

	strLastElement = pstrPQ->pstrEleList[pstrPQ->s32Size - 1];

	for(s32i = 1; s32i * 2 <= pstrPQ->s32Size; s32i = s32Child)
	{
		s32Child = s32i * 2;
		if( s32Child != pstrPQ->s32Size
		&& (pstrPQ->pstrEleList[s32Child + 1].s32Key > pstrPQ->pstrEleList[s32Child].s32Key)
		  )
		{
			s32Child++;
		}

		/* Percolate one level */
		if(strLastElement.s32Key < pstrPQ->pstrEleList[s32Child].s32Key)
		{
			pstrPQ->pstrEleList[s32i] = pstrPQ->pstrEleList[s32Child];
		}
		else
		{
			break;
		}
	}

	pstrPQ->pstrEleList[s32i] = strLastElement;
	pstrPQ->s32Size--;
	return 0;
}

ElementType* PQ_FindMax(Priority_Queue pstrPQ)
{
	if(1 == PQ_IsEmpty(pstrPQ))
	{
		printf("Priority Queue is empty!\n");
		return NULL;
	}
	return &(pstrPQ->pstrEleList[1]);
}

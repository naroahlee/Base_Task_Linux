#ifndef __PRIORITY_QUEUE__
#define __PRIORITY_QUEUE__

#define MAX_DATA (999999)

typedef struct CritElement_t
{
	int s32Key;
} CritElement;

typedef CritElement ElementType;

typedef struct HeapStruct_t
{
	int s32Capacity;
	int s32Size;
	ElementType *pstrEleList;
} HeapStruct;

typedef HeapStruct *Priority_Queue;

extern Priority_Queue PQ_Init(int s32MaxSize);
extern int PQ_Destroy(Priority_Queue pstrPQ);
extern int PQ_IsEmpty(Priority_Queue pstrPQ);
extern int PQ_IsFull(Priority_Queue pstrPQ);
extern int PQ_Insert(Priority_Queue pstrPQ, ElementType strX);
extern int PQ_DeleteMax(Priority_Queue pstrPQ);
extern ElementType* PQ_FindMax(Priority_Queue pstrPQ);


#endif

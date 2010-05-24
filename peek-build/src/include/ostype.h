




#ifndef __ostype_h__

#define __ostype_h__

#include "fsmdefs.h"

/*======================== MEMORY SERVICE =======================*/

uint32		FsmMemInit(void * start, uint32 size);
uint32		FsmMemExit(void);
void *		FsmMalloc(uint32 size);
void *		FsmTryMalloc(uint32 size);
uint32		FsmFree(void *mem_ptr);
uint32		FsmMemAvailable(uint32 * available);

void		FsmMemoryMove(uint8 * pDst, const uint8 * pSrc, uint32 num_bytes);
void		FsmMemorySet(uint8 * buf, uint8 byte_value, uint32 bytes);


/*======================== TIME SERVICE =========================*/

uint32		FsmGetCurrentTime(void);
uint32		FsmGetCurrentDate(void);

/*====================== SEMAPHORE SERVICE ======================*/

#if (OS_TYPE == OS_NUCLEUS)

typedef NU_SEMAPHORE	BinSemT;

typedef NU_SEMAPHORE *	HBSEM;

typedef struct
{
	NU_TASK *  	hOwnerTask;
	BinSemT		BinSem;
	uint16		OwnCount;
} MtxSemT;

typedef MtxSemT * HMSEM;

typedef struct 
{
	NU_TASK		TaskCb;
} FsmTaskCbT;

typedef FsmTaskCbT *		HFSMTASK;

typedef	void (*TASK_ENTRY)(UNSIGNED, void *);

#else

typedef HANDLE			HBSEM;	
typedef HANDLE			HMSEM;
typedef HANDLE			HFSMTASK;

#endif


extern HBSEM    FsmCreateBinSem(uint8 state);
extern uint32	FsmDeleteBinSem(HBSEM hBinSem);
extern uint32	FsmReleaseBinSem(HBSEM hBinSem);
extern uint32	FsmGetBinSemTry(HBSEM hBinSem);
extern uint32	FsmGetBinSem(HBSEM hBinSem);
extern uint32	FsmGetBinSemTimeout(HBSEM hBinSem, uint32 time_out);


HMSEM		FsmCreateMtxSem(uint8 state);
uint32		FsmDeleteMtxSem(HMSEM hMtxSem);
uint32		FsmReleaseMtxSem(HMSEM  hMtxSem);
uint32		FsmGetMtxSemTry(HMSEM hMtxSem);
uint32		FsmGetMtxSem(HMSEM hMtxSem);


#if (OS_TYPE == OS_NUCLEUS)


#define TASK_SLEEP(a)				NU_Sleep(a)

#define GET_CURRENT_TASK()			NU_Current_Task_Pointer()

#define DISABLE_INTERRUPTS()		SysIntDisable(SYS_ALL_INT);

#define ENABLE_INTERRUPTS() 		SysIntEnable(SYS_ALL_INT);


#else

#define TASK_SLEEP(a)					Sleep((a) * 10)


#endif


HFSMTASK	FsmCreateTask(uint32 taskid, 
						  char * Name, 
						  uint32 Priority, 
						  void * pStack,
						  uint32 stack_size, 
						  uint32 task_entry);

uint32		FsmDeleteTask(HFSMTASK hTask);



#endif /*__ostype_h__*/







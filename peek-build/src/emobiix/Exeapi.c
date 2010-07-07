#include "nucleus.h"
#include "exeapi.h"
#include "monapi.h"
#include "bal_os.h"

extern int TCD_Interrupt_Level;

//NU_MEMORY_POOL *ExeSystemMemory;
//uint32		*ExeIntStackP; // this is some structure we need to work out
//		ExeTaskCb // same

unsigned long get_NU_Task_HISR_Pointer()
{
        if(!TCC_Current_Task_Pointer())
                return (uint32)TCC_Current_HISR_Pointer();
        return 0;
}

void BOSFree(void* Ptr)
{
	free(Ptr);
}

void* BOSMalloc( unsigned long size )
{
	return (void *)malloc(size);
}

BOSEventWaitT BOSEventWait(BOSTaskIdT TaskId, bool Signal, 
                                  BOSMessageT Message, uint32 Timeout)
{
	return ExeEventWait(TaskId, Signal, Message, Timeout);
}

void BOSMsgBufferFree(void *MsgBufferP)
{
	ExeMsgBufferFree(MsgBufferP);
}

void * BOSMsgBufferGet(uint32 MsgBufferSize) 
{
	return (void *) ExeMsgBufferGet(MsgBufferSize);

}

int BOSMsgSend(BOSTaskIdT TaskId, BOSMailboxIdT MailboxId, uint32 MsgId, 
                        void *MsgBufferP, uint32 MsgSize)
{
	return ExeMsgSend(TaskId, MailboxId, MsgId, MsgBufferP, MsgSize);

}

bool BOSMsgRead(BOSTaskIdT TaskId, BOSMailboxIdT MailboxId, uint32 *MsgIdP, 
                        void **MsgBufferP, uint32 *MsgSizeP)
{
	return ExeMsgRead(TaskId, MailboxId, MsgIdP, MsgBufferP, MsgSizeP);

}

int BOSMsgSendToFront(BOSTaskIdT TaskId, BOSMailboxIdT MailboxId, uint32 MsgId, 
                        void *MsgBufferP, uint32 MsgSize)
{
	return ExeMsgSendToFront(TaskId, MailboxId, MsgId, MsgBufferP, MsgSize);
}
bool ExeMsgRead(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, uint32 *MsgIdP, 
                       void **MsgBufferP, uint32 *MsgSizeP) 
{
	
}

uint32 ExeMsgCheck(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId)
{

}

void MonFault(MonFaultUnitT UnitNum, uint32 FaultCode1, uint32 FaultCode2, MonFaultTypeT FaultType)
{
	emo_printf("MonFault [ UnitNum:%d, FaultCode1:%d, FaultCode2:%d, FaultType:%d]\n", UnitNum, FaultCode1, FaultCode2, FaultType);
	while(1) {
		TCCE_Task_Sleep(10000);
	}
}

void CallExeFault(void) {
	MonFault(MON_EXE_FAULT_UNIT, 3, 0, MON_HALT);
}

ExeEventWaitT ExeEventRetireve( ExeTaskIdT TaskId, uint32 RequestEvent, uint32 Timeout )
{
	uint32 *ret_flags = 0;
	// Need to get group_ptr from ExeTaskCb
	//EVCE_Retrieve_Events(,,1/* operation */, RequestEvent);

}
	
void ExeTaskWait(uint32 Ticks) {
	TCCE_Task_Sleep(Ticks);
}

void ExeHisrActivate(ExeHisrT *HisrCbP)
{
	if(TCCE_Activate_HISR((NU_HISR *)HisrCbP)) {
		CallExeFault();
	}
	return;
}

void ExeSignalSet(ExeTaskIdT TaskId, ExeSignalT SignalFlg)
{
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	//if(EVCE_Set_Events(XXX, SignalFlg, 0)) { // Needs to lookup group_ptr in ExeTaskCb
	//	CallExeFault();
	//}

	return;
}

void ExeTimerCreate(ExeTimerT *TimerCbP, void (*Routine)(uint32), uint32 TimerId,
					 uint32 InitialTime, uint32 RescheduledTime)
{
	NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
	NU_TASK  *cTCTP;
	char cTimer[] = "Timer";

 	if(!cHISR) {	
		if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
		}
	}
	if(!InitialTime)
		InitialTime = 0x16;

	TimerCbP->tm_initial_time = InitialTime;
	

	TMSE_Create_Timer((NU_TIMER *) TimerCbP, cTimer, Routine,
			 TimerId, InitialTime, RescheduledTime, NU_DISABLE_TIMER);

	return;
}

void ExeTimerStop(ExeTimerT *TimerCbP) {
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	TMSE_Control_Timer((NU_TIMER *)TimerCbP, NU_DISABLE_TIMER);
}

void ExeTimerStart(ExeTimerT *TimerCbP)
{
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	TMSE_Control_Timer((NU_TIMER *)TimerCbP, NU_ENABLE_TIMER);
}

void ExeTimerReset(ExeTimerT *TimerCbP, void (*Routine)(uint32),
                           uint32 InitialTime, uint32 RescheduledTime)
{
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	TMSE_Reset_Timer((NU_TIMER *)TimerCbP, Routine, InitialTime, RescheduledTime, NU_DISABLE_TIMER);
}

void ExeTimerDelete(ExeTimerT *TimerCbP)
{
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	ExeInterruptDisable((SysIntT)0x80);
	TMSE_Delete_Timer((NU_TIMER *)TimerCbP);
	ExeInterruptEnable();
}

void ExeTimerGetRemainTime(ExeTimerT *TimerCbP, uint32 *RemainTime)
{
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	TMF_Get_Remaining_Time((NU_TIMER *)TimerCbP, RemainTime);

}

void ExeInterruptDisable(SysIntT IntMask)
{
	TCD_Interrupt_Level = TCT_Control_Interrupts(TCD_Interrupt_Level | IntMask);

}
void ExeInterruptEnable(void)
{
	// Needs work
	//TCT_Control_Interrupts();
}

void ExePreemptionChange(ExePreemptionT Preemption)
{
	TCSE_Change_Preemption(((Preemption << 24) >> 24));
}

// Needs work
void ExeBufferCreate(ExeBufferT *BufferCbP, uint32 NumRec, uint32 RecSize)
{
	void **rPtr = NULL;
	char bMem[] = "FixMem";

	//if(DMCE_Allocate_Memory((NU_MEMORY_POOL *)ExeSystemMemory, rPtr, (NumRec * RecSize)+0xA, 0))
		return;
	//if(!PMCE_Create_Partition_Pool((NU_MEMORY_POOL *)BufferCbP, bMem, *rPtr, (NumRec * RecSize)+0xA))
	//	return;
	//DMCE_Deallocate_Memory(*rPtr);
}

void * ExeBufferGet(ExeBufferT *BufferCbP)
{
	void **rPtr = NULL;
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	if(!PMCE_Allocate_Partition((NU_PARTITION_POOL *)BufferCbP, rPtr, 0)) {
		MonFault(MON_EXE_FAULT_UNIT, 5, 0, MON_HALT);
	}

	return rPtr;

}

void ExeBufferFree(void *BufferP) {
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	PMCE_Deallocate_Partition(BufferP);
}

void ExeSemaphoreCreate(ExeSemaphoreT *SemaphoreCbP, uint32 InitialCount) {
	char cSema[] = "Semaphor";

	SMCE_Create_Semaphore((NU_SEMAPHORE *)SemaphoreCbP, cSema, InitialCount, 0x6);
}

void ExeSemaphoreDelete(ExeSemaphoreT *SemaphoreCbP) {
	SMCE_Delete_Semaphore((NU_SEMAPHORE *)SemaphoreCbP);
}

int32 ExeSemaphoreGet(ExeSemaphoreT *SemaphoreCbP, uint32 Timeout) {
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	return SMCE_Obtain_Semaphore((NU_SEMAPHORE *)SemaphoreCbP, Timeout);
}

void ExeSemaphoreRelease(ExeSemaphoreT *SemaphoreCbP) {
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }
	SMCE_Release_Semaphore((NU_SEMAPHORE *)SemaphoreCbP);
}

uint32 ExeThreadIDGet(void) {
	return get_NU_Task_HISR_Pointer();
}

void *ExeMalloc(NU_MEMORY_POOL *pool, uint32 size, uint32 suspend) {
	void **rPtr = NULL;

	DMCE_Allocate_Memory(pool,rPtr,size,suspend);
	return rPtr;
}

void ExeFree(void * ptr) {
	DMCE_Deallocate_Memory(ptr);
}

void ExeMemoryPoolCreate(NU_MEMORY_POOL *pool, char *name, 
                       void *start_address, uint32 pool_size,
                        uint32 min_allocation) {

        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	DMCE_Create_Memory_Pool(pool, name, start_address,
		 		pool_size, min_allocation, 0x06);
}


void ExeMemoryPoolDelete(NU_MEMORY_POOL *pool) {

        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	DMCE_Delete_Memory_Pool(pool);
}

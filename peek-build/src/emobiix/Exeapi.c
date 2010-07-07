#include "nucleus.h"
#include "exeapi.h"
#include "exedefs.h"
#include "bal_def.h"
#include "monapi.h"
#include "bal_os.h"

extern int TCD_Interrupt_Level;
ExeTaskCbT *ExeTaskCb[EXE_NUM_TASKS];

//NU_MEMORY_POOL *ExeSystemMemory;
//uint32		*ExeIntStackP; // this is some structure we need to work out

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
void ExeDecMsgBuffStats(void * MsgBuffPtr)
{


}
void ExeMsgBufferFree(void *MsgBufferP) 
{
	NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
	NU_TASK  *cTCTP;

	if(!cHISR) {
		if(!(cTCTP = TCC_Current_Task_Pointer())) {
			MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
		}
	}

	if(PMCE_Deallocate_Partition(MsgBufferP))
		MonFault(MON_EXE_FAULT_UNIT, 3, 0, MON_HALT);

	ExeDecMsgBuffStats(MsgBufferP);
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

typedef struct {
	uint32 masks[EXE_NUM_MAILBOX];
}sMailQueueSig;


ExeEventWaitT ExeEventWait(ExeTaskIdT TaskId, bool Signal, ExeMessageT Message, uint32 Timeout)
{
uint32 suspend;
uint32 signal_mask;
uint32 timeout;
int ret;
ExeTaskCbT *task;
int i;
sMailQueueSig MailQueueSig = { EXE_MAILBOX_1, EXE_MAILBOX_2, EXE_MAILBOX_3, EXE_MAILBOX_4, EXE_MAILBOX_5 };

timeout = Timeout;
//                PUSH    {R0-R7,LR}
//                STR     R3, [SP,#0x24+timeout]

signal_mask = 0;
//                MOVS    R7, #0
//                STR     R7, [SP,#0x24+signal_mask]

if (Signal == EXE_SIGNAL_TRUE)
	signal_mask = ~0xFD; // EXE_SIGNAL_1 ... EXE_SIGNAL_23 | EXE_SIGNAL_TYPE

//                CMP     R1, #1
//                BNE     L1
//                MOVS    R1, #0xFD ; '²'
//                MVNS    R1, R1
//                STR     R1, [SP,#0x24+signal_mask]

//L1:                                      ; CODE XREF: $ExeEventWait+Aj
if (Message == EXE_MESSAGE_TRUE)
{
	signal_mask |= (EXE_MESSAGE_TYPE | EXE_MAILBOX_1 | EXE_MAILBOX_2 | EXE_MAILBOX_3 | EXE_MAILBOX_4 | EXE_MAILBOX_5);
}
//                CMP     R2, #1
//                BEQ     L2
//                LDR     R1, [SP,#0x24+signal_mask]
//                ORRS    R1, R2
//                STR     R1, [SP,#0x24+signal_mask]
//               B       L3
//; ---------------------------------------------------------------------------
else 
{
	signal_mask |= Message;
}
//L2:                                      ; CODE XREF: $ExeEventWait+14j
//                MOV     R1, SP
//                LDRB    R3, [R1,#0x24+signal_mask]
//                MOVS    R2, #0xF9 ; '·'
//                ORRS    R2, R3
//                STRB    R2, [R1,#0x24+signal_mask]

//L3:                                      ; CODE XREF: $ExeEventWait+1Cj
task = ExeTaskCb[TaskId];  // extern /*const*/ ExeTaskCbT     *ExeTaskCb[];
//                LSLS    R0, R0, #2
//                LDR     R1, =_ExeTaskCb
//                ADDS    R6, R1, R0
//                LDR     R1, [R6]
//                LDR     R0, [R1]
if (task->NumMsgs)
{
//                CMP     R0, #0
//                BLE     L6
//                MOVS    R5, #5 
//                ADDS    R4, R7, #0 
	for (i = 0; i < EXE_NUM_MAILBOX; i++) 
	{ 
//L4:                                      ; CODE XREF: $ExeEventWait+64j
//                ADDS    R0, R4, R1
//                LDR     R0, [R0,#4]
		if (!task->NumMsgsInQueue[i])
			continue;
//                CMP     R0, #0
//                BLE     L5
//                MOVS    R0, 0x228  
//                ADDS    R0, R0, R1
//                LDR     R1, =_MailQueueSig 
//                LDR     R1, [R1,R4]
//                MOVS    R2, #1 
//                ORRS    R1, R2  
//                MOVS    R2, R7

//STATUS          EVCE_Set_Events(NU_EVENT_GROUP *group_ptr, UNSIGNED events, OPTION operation); 
		if (EVCE_Set_Events(&task->EventGroupCb, MailQueueSig.masks[i] | EXE_MESSAGE_TYPE, 0))
			CallExeFault();

//                BL      $EVCE_Set_Events
//                CMP     R0, #0
//                BEQ     L5
//                BL      $CallExeFault

//L5:                                      ; CODE XREF: $ExeEventWait+40j
//                                        ; $ExeEventWait+58j
//                ADDS    R4, #4 
//                LDR     R1, [R6]
//                SUBS    R5, #1
//                BNE     L4
	}
}

//L6:                                      ; CODE XREF: $ExeEventWait+34j
suspend = timeout;
//                LDR     R0, [SP,#0x24+timeout]
//                STR     R0, [SP,#0x24+suspend]
//                MOVS    R0, 0x228
//               ADDS    R0, R0, R1
//               LDR     R1, [SP,#0x24+signal_mask]
//                MOVS    R2, #1
//                ADD     R3, SP, #0x24+signal_mask

// im not seeing it...
//STATUS EVCE_Retrieve_Events(NU_EVENT_GROUP *group_ptr, UNSIGNED requested_flags, OPTION operation, UNSIGNED *retrieved_flags, UNSIGNED suspend);
ret = EVCE_Retrieve_Events(&task->EventGroupCb, signal_mask, NU_OR_CONSUME, &signal_mask, suspend);

switch (ret)
{
	case NU_TIMEOUT:
		return EXE_TIMEOUT_TYPE;

	case NU_SUCCESS:
		return signal_mask;

	default:
		CallExeFault();
}

return suspend;
//                BL      $EVCE_Retrieve_Events
//                MOVS    R4, R0 // ret == 0
//                BEQ     L8   // success
//                MOVS    R1, R0  // ret != 0
//                ADDS    R1, #0x32 ; '2'  // ret + 32 == 0? 
//                BEQ     L7  // timeout
//                BL      $CallExeFault

//L7:                                      ; CODE XREF: $ExeEventWait+82j
//                ADDS    R4, #0x32 ; '2' 
//                BNE     L8              
//                MOVS    R0, #4
//                STR     R0, [SP,#0x24+signal_mask]

//L8:                                      ; CODE XREF: $ExeEventWait+7Cj
//                                        ; $ExeEventWait+8Aj
//                LDR     R0, [SP,#0x24+signal_mask]
//                STR     R0, [SP,#0x24+suspend]
//                POP     {R0-R7,PC}
}

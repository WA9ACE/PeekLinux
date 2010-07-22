#include "nucleus.h"
#include "exeapi.h"
#include "exedefs.h"
#include "monapi.h"
#include "monids.h"
#include "bal_os.h"

typedef struct 
{
	uint32	msgId;
	uint32	msgSize;
	void	*msgBuf;
} sExeMsg;

extern int TCD_Interrupt_Level;
ExeTaskCbT *ExeTaskCb[EXE_NUM_TASKS];

extern uint32 *ExeIntStackP;

extern NU_MEMORY_POOL  ExeSystemMemory;
#define NU_Thread_Id get_NU_Task_HISR_Pointer()

typedef struct {
        uint32 masks[EXE_NUM_MAILBOX];
}sMailQueueSig;

ExeMsgBuffInfoT ExeMsgBuffInfo[EXE_NUM_DIFF_MSG_BUFFS];

sMailQueueSig MailQueueSig = { EXE_MAILBOX_1, EXE_MAILBOX_2, EXE_MAILBOX_3, EXE_MAILBOX_4, EXE_MAILBOX_5 };

unsigned long get_NU_Task_HISR_Pointer()
{
        if(!TCC_Current_Task_Pointer())
                return (uint32)TCC_Current_HISR_Pointer();
        return 0;
}

void BOSFree(void* Ptr)
{
	p_free(Ptr);
}

void* BOSMalloc( unsigned long size )
{
	return (void *)p_malloc(size);
}

BOSEventWaitT BOSEventWait(BOSTaskIdT TaskId, bool Signal, 
                                  BOSMessageT Message, uint32 Timeout)
{
	return (BOSEventWaitT)ExeEventWait((ExeTaskIdT) TaskId, Signal, (ExeMessageT)Message, Timeout);

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
	return ExeMsgSend((ExeTaskIdT)TaskId, (ExeMailboxIdT)MailboxId, MsgId, MsgBufferP, MsgSize);

}

bool BOSMsgRead(BOSTaskIdT TaskId, BOSMailboxIdT MailboxId, uint32 *MsgIdP, 
                        void **MsgBufferP, uint32 *MsgSizeP)
{
	return ExeMsgRead((ExeTaskIdT) TaskId, (ExeMailboxIdT) MailboxId, MsgIdP, MsgBufferP, MsgSizeP);

}

int BOSMsgSendToFront(BOSTaskIdT TaskId, BOSMailboxIdT MailboxId, uint32 MsgId, 
                        void *MsgBufferP, uint32 MsgSize)
{
	return ExeMsgSendToFront((ExeTaskIdT) TaskId, (ExeMailboxIdT) MailboxId, MsgId, MsgBufferP, MsgSize);
}

uint32 ExeMsgCheck(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId)
{
	ExeTaskCbT *task;

	task = ExeTaskCb[TaskId];
	if(task->NumMsgsInQueue[MailboxId] < 0) 
		return 0;
	return task->NumMsgsInQueue[MailboxId];
}

void MonFault(MonFaultUnitT UnitNum, uint32 FaultCode1, uint32 FaultCode2, MonFaultTypeT FaultType)
{
	emo_printf("MonFault [ UnitNum:%d, FaultCode1:%d, FaultCode2:%d, FaultType:%d]\n", UnitNum, FaultCode1, FaultCode2, FaultType);
	while(1) {
		TCCE_Task_Sleep(10000);
	}
}

extern char BalMemoryPoolBuffer[];

void CallExeFault(void) {
	bpoold(&BalMemoryPoolBuffer, 1, 1);
	MonFault(MON_EXE_FAULT_UNIT, 3, 0, MON_HALT);
}

ExeEventWaitT ExeEventRetireve( ExeTaskIdT TaskId, uint32 RequestEvent, uint32 Timeout )
{
	ExeTaskCbT *task;
	ExeEventWaitT retFlags = (ExeEventWaitT)RequestEvent;

        task = ExeTaskCb[TaskId];

	EVCE_Retrieve_Events(&task->EventGroupCb, retFlags, NU_OR_CONSUME, (UNSIGNED *)&retFlags, Timeout);

	return retFlags;
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
	ExeTaskCbT *task;


        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	task = ExeTaskCb[TaskId]; 
	
        if (EVCE_Set_Events(&task->EventGroupCb, EXE_SIGNAL_TYPE | SignalFlg, 0)) 
		MonFault(MON_EXE_FAULT_UNIT, 3, 0, MON_HALT);

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

	ExeInterruptDisable(SYS_IRQ_INT);
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
	IntMask = (SysIntT)TCT_Control_Interrupts(TCD_Interrupt_Level | IntMask);
	*ExeIntStackP = IntMask;
	ExeIntStackP++;

}

void ExeInterruptEnable(void)
{
	ExeIntStackP--;
	TCT_Control_Interrupts(*ExeIntStackP);
}

void ExePreemptionChange(ExePreemptionT Preemption)
{
	TCSE_Change_Preemption(((Preemption << 24) >> 24));
}

void ExeBufferCreate(ExeBufferT *BufferCbP, uint32 NumRec, uint32 RecSize)
{
	void *rPtr = NULL;
	uint32 size = NumRec * (RecSize + 0xA);

	if(DMCE_Allocate_Memory(&ExeSystemMemory, &rPtr, size, 0))
		return;
	if(!PMCE_Create_Partition_Pool((NU_PARTITION_POOL *)BufferCbP, "FixMem", rPtr, size, RecSize, NU_SEMAPHORE_SUSPEND))
		return;
	DMCE_Deallocate_Memory(rPtr);
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

	if(PMCE_Deallocate_Partition(BufferP) != 0)
		MonFault(MON_EXE_FAULT_UNIT, 4, 2, MON_HALT);
}

typedef struct bufMsgs { 
	void *message;
	uint32 *messageId;
	uint32 actual_size;

}bufMsg_s;

bool ExeMsgRead(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, uint32 *MsgIdP, void **MsgBufferP, uint32 *MsgSizeP)
{
	register int errCode;
	register ExeTaskCbT *task = ExeTaskCb[TaskId];

	uint32 suspend = 0;
	uint32 actual_size = 0;
	sExeMsg msg;

	errCode = QUCE_Receive_From_Queue(&task->MailQueueCb[MailboxId], &msg, 3, &actual_size, suspend);
	if (errCode != 0)
	{
		if (errCode == NU_QUEUE_EMPTY)
			return 0;

		CallExeFault();
	}

	ExeInterruptDisable(SYS_IRQ_INT);

	task->NumMsgs--;
	task->NumMsgsInQueue[MailboxId]--;

	ExeInterruptEnable();

	*MsgIdP = msg.msgId;
	*MsgBufferP = msg.msgBuf;
	*MsgSizeP = msg.msgSize;

	return 1;
}

static ExeFaultType2T ExeFaultType2;

static ExeFaultType3T ExeFaultType3;

void ExeFault(ExeFaultTypeT ExeFaultType, ExeErrsT ExeError,
		void *ExeFaultData, uint16 FaultSize)
{

	MonFault(MON_EXE_FAULT_UNIT, ExeFaultType, ExeError, MON_HALT);
	// They don't do anything here
}

void MonTrace(uint16 TraceId, uint32 NumArgs, ...) {

	// They don't do anything here
}

int ExeMsgSend(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, uint32 MsgId, void *MsgBufferP, uint32 MsgSize)
{
	register int errCode = 0;
	register ExeTaskCbT *task = ExeTaskCb[TaskId];

	sExeMsg msg;
	int retVal = 0;

	msg.msgId = MsgId;
	msg.msgBuf = MsgBufferP;
	msg.msgSize = MsgSize;

	if (!TCC_Current_HISR_Pointer())
	{
		if (!TCC_Current_Task_Pointer())
		{
			MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
		}
	}

	if (MsgBufferP)
	{
		if (((char *)MsgBufferP)[MsgSize] != 0xED)
		{
			ExeFaultType3.MsgId = MsgId;
			ExeFaultType3.MsgSize = MsgSize;
			ExeFaultType3.DestTaskId = TaskId;
			ExeFaultType3.MboxId = MailboxId;

			ExeFault(EXE_FAULT_TYPE_3, EXE_MSG_BUFF_OVERWRITE_ERR, &ExeFaultType3, sizeof(ExeFaultType3T));
		}
	}

	//ExeIncMsgBuffSendStats(MsgBufferP, MsgId, TaskId);

	errCode = QUCE_Send_To_Queue(&task->MailQueueCb[MailboxId], &msg, 3, 0);
	if (!errCode)
	{
		ExeInterruptDisable(SYS_IRQ_INT);

		task->NumMsgs++;
		task->NumMsgsInQueue[MailboxId]++;

		TCD_Interrupt_Level = ((TCD_Interrupt_Level << 0x19) >> 0x19);

		if (EVCE_Set_Events(&task->EventGroupCb, MailQueueSig.masks[MailboxId] | EXE_MESSAGE_TYPE, 0))
			CallExeFault();

		ExeInterruptEnable();

		retVal = 0;
	}
	else // if (!= 0)
	{
		if (TaskId != EXE_UI_ID || MailboxId != EXE_MAILBOX_4_ID)
			emo_printf("INFO:ExeMsgSend faild, TaskID:%d, Status:%d\n", TaskId, errCode);

		if (TaskId == EXE_IOP_ID && MailboxId == EXE_MAILBOX_1_ID)
		{
			if (MsgBufferP)
			{
				if (PMCE_Deallocate_Partition(MsgBufferP) != 0)
					MonFault(MON_EXE_FAULT_UNIT, 4, 3, MON_HALT);

				//ExeDecMsgBuffStats(MsgBufferP);
			}
			else
			{
				if (EVCE_Set_Events(&task->EventGroupCb, EXE_SIGNAL_12 | EXE_SIGNAL_TYPE, 0))
					CallExeFault();
			}
		}
		else
		{
			ExeFaultType3.DestTaskId = TaskId;
			ExeFaultType3.MboxId = MailboxId;
			ExeFaultType3.MsgId = MsgId;
			ExeFaultType3.MsgSize = MsgSize;

			ExeFault(EXE_FAULT_TYPE_3, EXE_MAIL_QUEUE_FULL_ERR, &ExeFaultType3, sizeof(ExeFaultType3T));
		}

		retVal = -1;
	}

	if (TaskId != EXE_IOP_ID && TaskId != EXE_HWD_ID)
		MonTrace(MON_CP_MSG_BUFF_STATS_SPY_ID, 3, msg.msgId, get_NU_Task_HISR_Pointer(), TaskId);

	return retVal;
}

int ExeMsgSendToFront(ExeTaskIdT TaskId, ExeMailboxIdT MailboxId, 
			uint32 MsgId, void *MsgBufferP, uint32 MsgSize)
{
	uint32 msgId = MsgId;
	void *msgBufferP = MsgBufferP;
	uint32 msgSize = MsgSize;
	ExeTaskCbT *task = ExeTaskCb[TaskId];
	int errCode = 0;
	int retVal = 0;

	if (!TCC_Current_HISR_Pointer())
	{
		if (!TCC_Current_Task_Pointer())
		{
			MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
		}
	}

	if (MsgBufferP)
	{
		if (((char *)MsgBufferP)[MsgSize] != 0xED)
		{
			ExeFaultType3.MsgId = MsgId;
			ExeFaultType3.MsgSize = MsgSize;
			ExeFaultType3.DestTaskId = TaskId;
			ExeFaultType3.MboxId = MailboxId;

			ExeFault(EXE_FAULT_TYPE_3, EXE_MSG_BUFF_OVERWRITE_ERR, &ExeFaultType3, sizeof(ExeFaultType3T));
		}
	}

	//ExeIncMsgBuffSendStats(MsgBufferP, MsgId, TaskId);

	errCode = QUSE_Send_To_Front_Of_Queue(&task->MailQueueCb[MailboxId], &msgId, 3, 0);
	if (!errCode)
	{
		ExeInterruptDisable(SYS_IRQ_INT);

		task->NumMsgs++;
		task->NumMsgsInQueue[MailboxId]++;

		TCD_Interrupt_Level = ((TCD_Interrupt_Level << 0x19) >> 0x19);

		if (EVCE_Set_Events(&task->EventGroupCb, MailQueueSig.masks[MailboxId] | EXE_MESSAGE_TYPE, 0))
			CallExeFault();

		ExeInterruptEnable();

		retVal = 0;
	}
	else // if (!= 0)
	{
		if (TaskId == EXE_IOP_ID && MailboxId == EXE_MAILBOX_1_ID)
		{
			if (MsgBufferP)
			{
				if (PMCE_Deallocate_Partition(MsgBufferP) != 0)
					MonFault(MON_EXE_FAULT_UNIT, 4, 4, MON_HALT);

				//ExeDecMsgBuffStats(MsgBufferP);
			}
			else
			{
				if (EVCE_Set_Events(&task->EventGroupCb, EXE_SIGNAL_12 | EXE_SIGNAL_TYPE, 0))
					CallExeFault();
			}
		}
		else
		{
			ExeFaultType3.DestTaskId = TaskId;
			ExeFaultType3.MboxId = MailboxId;
			ExeFaultType3.MsgId = MsgId;
			ExeFaultType3.MsgSize = MsgSize;

			ExeFault(EXE_FAULT_TYPE_3, EXE_MAIL_QUEUE_FULL_ERR, &ExeFaultType3, sizeof(ExeFaultType3T));
		}

		retVal = -1;
	}

	if (TaskId != EXE_IOP_ID && TaskId != EXE_HWD_ID)
		MonTrace(MON_CP_MSG_BUFF_STATS_SPY_ID, 3, MsgId, get_NU_Task_HISR_Pointer(), TaskId);

	return retVal;
}

void * ExeMsgBufferGet(uint32 MsgBufferSize)
{
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;
	void *retPtr;	
	int allocRet=-1;
	int i;

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }

	emo_printf("ExeMsgBufferGet - 0x%08x - MaxBuffersize 0x%08x\n", MsgBufferSize, ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_4].BuffSize);	
	if(MsgBufferSize <= ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_4].BuffSize) {
		for(i=0;i < EXE_NUM_DIFF_MSG_BUFFS;i++) {
			if(MsgBufferSize > ExeMsgBuffInfo[i].BuffSize) 
				continue;
			allocRet = PMCE_Allocate_Partition(ExeMsgBuffInfo[i].BuffCbP, &retPtr, 0);
			if(!allocRet)
				break;
		}

		if(!allocRet) {
			((char *)retPtr)[MsgBufferSize] = 0xED;
			return retPtr;
		} else {
			ExeFault(EXE_FAULT_TYPE_2, EXE_UNHANDLED_INT_ERR, &ExeFaultType2, sizeof(ExeFaultType2T));
			return (void *)0;
		}
		
	} 
	
	ExeFault(EXE_FAULT_TYPE_2, EXE_MSG_BUFF_MEM_SIZE_ERR, &ExeFaultType2, sizeof(ExeFaultType2T));
	
	return (void *)0;	

}
void ExeMsgBufferFree(void *MsgBufferP) 
{
	NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
	NU_TASK  *cTCTP;
	int ret = 0;

	if(!cHISR) {
		if(!(cTCTP = TCC_Current_Task_Pointer())) {
			MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
		}
	}
	ret = PMCE_Deallocate_Partition(MsgBufferP);
	if(ret != 0)
		MonFault(MON_EXE_FAULT_UNIT, 4, ret, MON_HALT);

	//ExeDecMsgBuffStats(MsgBufferP);
}

uint32 ExeGetSemaphoreCount(ExeSemaphoreT *SemaphoreCbP) 
{
        NU_HISR  *cHISR = TCC_Current_HISR_Pointer();
        NU_TASK  *cTCTP;
	uint32   current_count = 0;
	uint32   tasks_waiting = 0;
	NU_TASK *first_task;
	OPTION   suspend_type = 0;
	char name[100];

        if(!cHISR) {
                if(!(cTCTP = TCC_Current_Task_Pointer())) {
                        MonFault(MON_EXE_FAULT_UNIT, 3, get_NU_Task_HISR_Pointer(), MON_HALT);
                }
        }
	SMF_Semaphore_Information((NU_SEMAPHORE *)SemaphoreCbP, (char *)&name, &current_count, &suspend_type, &tasks_waiting, &first_task);
	return current_count;
}

void ExeSemaphoreCreate(ExeSemaphoreT *SemaphoreCbP, uint32 InitialCount) {
	SMCE_Create_Semaphore((NU_SEMAPHORE *)SemaphoreCbP, "Semaphor", InitialCount, 0x6);
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
	
        emo_printf("ExeSemaphoreGet with timeout %d\n", Timeout);
        TCCE_Task_Sleep(10);

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
	void *rPtr = NULL;

	DMCE_Allocate_Memory(pool, &rPtr, size, suspend);
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

ExeEventWaitT ExeEventWait(ExeTaskIdT TaskId, bool Signal, ExeMessageT Message, uint32 Timeout)
{
	uint32 suspend;
	uint32 signal_mask;
	uint32 timeout;
	int ret;
	ExeTaskCbT *task;
	int i;
	timeout = Timeout;
	signal_mask = 0;

	if (Signal == EXE_SIGNAL_TRUE)
		signal_mask = ~0xFD; // EXE_SIGNAL_1 ... EXE_SIGNAL_23 | EXE_SIGNAL_TYPE

	if (Message == EXE_MESSAGE_TRUE)
		signal_mask |= (EXE_MESSAGE_TYPE | EXE_MAILBOX_1 | EXE_MAILBOX_2 | EXE_MAILBOX_3 | EXE_MAILBOX_4 | EXE_MAILBOX_5);
	else
		signal_mask |= Message;

	task = ExeTaskCb[TaskId];
	if (task->NumMsgs)
	{
		for (i = 0; i < EXE_NUM_MAILBOX; i++) 
		{ 
			if (!task->NumMsgsInQueue[i])
				continue;

			if (EVCE_Set_Events(&task->EventGroupCb, MailQueueSig.masks[i] | EXE_MESSAGE_TYPE, 0))
				CallExeFault();
		}
	}

	suspend = timeout;
	ret = EVCE_Retrieve_Events(&task->EventGroupCb, signal_mask, NU_OR_CONSUME, &signal_mask, suspend);

	switch (ret)
	{
		case NU_TIMEOUT:
			return EXE_TIMEOUT_TYPE;

		case NU_SUCCESS:
			return (ExeEventWaitT)signal_mask;

	    default:
		CallExeFault();
	}

	return (ExeEventWaitT)suspend;
}

void ExeIncMsgBuffStats(void * MsgBuffPtr, uint32 MsgBuffType, uint32 MsgBuffSize, uint32 TaskId) { }
void ExeIncMsgBuffSendStats(void * MsgBuffPtr, uint32 MsgId, uint32 TaskId) { }
void ExeDecMsgBuffStats(void * MsgBuffPtr) { }


static ExeBufferT BufferCb1;
static ExeBufferT BufferCb2;
static ExeBufferT BufferCb3;
static ExeBufferT BufferCb4;

void ExeInit(void) {
	ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_1].BuffSize = EXE_SIZE_MSG_BUFF_1;
	ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_1].BuffCbP  = &BufferCb1;
	ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_2].BuffSize = EXE_SIZE_MSG_BUFF_2;
	ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_2].BuffCbP  = &BufferCb2;
	ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_3].BuffSize = EXE_SIZE_MSG_BUFF_3;
	ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_3].BuffCbP  = &BufferCb3;
	ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_4].BuffSize = EXE_SIZE_MSG_BUFF_4;
	ExeMsgBuffInfo[EXE_MSG_BUFF_TYPE_4].BuffCbP  = &BufferCb4;

	ExeBufferCreate(&BufferCb1, EXE_NUM_MSG_BUFF_1, 0x24);
	ExeBufferCreate(&BufferCb2, EXE_NUM_MSG_BUFF_2, 0xc4);
	ExeBufferCreate(&BufferCb3, EXE_NUM_MSG_BUFF_3, 0x1c4);
	ExeBufferCreate(&BufferCb4, EXE_NUM_MSG_BUFF_4, 0x234);
}

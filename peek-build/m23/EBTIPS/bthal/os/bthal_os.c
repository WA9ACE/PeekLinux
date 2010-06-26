/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      bthal_os.c
*
*   DESCRIPTION:    This file contain implementation os related issues for Locosto
*                   1) Event handling.
*                   2) Semaphores management
*                   3) Timers management
*
*   AUTHOR:         Yuval Hevrony
*
\*******************************************************************************/

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#define BTHAL_OS_PRAGMAS
#include "bthal_config.h"

#include "typedefs.h"
#include "vsi.h"
#include "bts.h"
#include "osapi.h"
#include "bthal_os.h"
#include "bthal_utils.h"
#include "bthal_log_modules.h"

#define BTHAL_OS_LOG_ERROR(msg)		    BTHAL_LOG_ERROR(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_OS, msg)	
#define BTHAL_OS_LOG_INFO(msg)			BTHAL_LOG_INFO(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_OS, msg)
#define BTHAL_OS_LOG_FUNCTION(msg)		BTHAL_LOG_FUNCTION(__FILE__, __LINE__, BTHAL_LOG_MODULE_TYPE_OS, msg)


/****************************************************************************
 *
 * Defines
 *
 ***************************************************************************/
#if XA_DEBUG == XA_ENABLED
#define BTHAL_Assert(exp)  (((exp) != 0) ? (void)0: BTHAL_UTILS_Assert(#exp,__FILE__,(U16)__LINE__))
#else
#define BTHAL_Assert(exp) (void)0
#endif

#define BTHAL_OS_NOT_FOUND              (0xff)


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/* typedef for semaphore structure */
typedef struct 
{
	int semHandle;					/* Handle of semaphore */
	int taskHandle;				/* Handle of task which took the semaphore */
	BTHAL_U16 lockCount;					/* Number of locks - unlocks */
} Semaphore;

typedef struct T_BTHAL_OS_TASK_PARAMS
{
	/* GPF task handle */
	BTHAL_INT gpfTaskHandle;

	/* Callbacks provided by the task */
	EventCallback taskCallback;				

	/* Number of created timers */
	BTHAL_U8 numOfCreatedTimers;
	
} BTHAL_OS_TASK_PARAMS;


typedef struct T_BTHAL_OS_TIMER_PARAMS
{
	/* Identifier for the timer events */ 
	BTHAL_BOOL timerUsed;

	/* associate timerHandle to taskHandle (used in TimerFired()) */
	BthalOsTaskHandle taskHandle;

	/* Pointer to timer's name string in order to prevent timer's creation twice */
	char *timerName;

	/* Index of timer used in GPF for every task */
	BTHAL_U8 timerIndexInTask;

	/* timer event */
	BTHAL_U32 timerEvent;			 

} BTHAL_OS_TIMER_PARAMS;

typedef struct T_BTHAL_OS_PARAMS
{
	/* Tasks parameters */
	BTHAL_OS_TASK_PARAMS taskParams[BTHAL_OS_MAX_NUM_OF_TASKS];

	/* timer parameters */
	BTHAL_OS_TIMER_PARAMS timerParams[BTHAL_OS_MAX_NUM_OF_TIMERS];

	/* Handles to the semaphores */
	Semaphore semaphores[BTHAL_OS_MAX_NUM_OF_SEMAPHORES];

	/* Mutex for OS_StopHardware() & OS_ResumeHardware() */
	Semaphore hwMutex;		

} BTHAL_OS_PARAMS;   

/********************************************************************************
 *
 * Globals
 *
 *******************************************************************************/

/* A bit mask representing all events that can be sent to a task. Each cell in the
 * array refers to a certain task: 0 - Stack task
 *						           1 - Transport task
 *						           2 - A2DP task
 *						           3 - FM task.
 * When the bit corresponding to an event is set, there's no need to send another 
 * message to the stack task. 
 */
static BTHAL_U32 taskEventsFlag[BTHAL_OS_MAX_NUM_OF_TASKS]; 


/***************************** BTHAL_OS_PARAMS ***************************************/

/* BTHAL control block */
BTHAL_OS_PARAMS BthalOsData;				 

/* btt, bts, btu handles */
extern int btt_handle;
extern int bts_handle;
extern int btu_handle;
extern int btav_handle;
#if FM_STACK == XA_ENABLED
extern int fms_handle;
#endif


/********************************************************************************
 *
 * Internal function prototypes
 *
 *******************************************************************************/

/* Semaphore handling functions */
static BTHAL_BOOL OsLockSemaphore(Semaphore *sem, BthalOsTime timeout);
static BTHAL_BOOL OsUnlockSemaphore(Semaphore *sem);
static BTHAL_BOOL OsCreateSemaphore(const char *semaphoreName, Semaphore *sem);
static BTHAL_BOOL OsDestroySemaphore(Semaphore *sem);

/* timer functions */
static BTHAL_BOOL OsCancelTimer(BthalOsTimerHandle timerHandle);

/* Converting BTHAL OS task handle to GPF task handle */
static int  OsTaskHandleToGpfHandle(BthalOsTaskHandle taskHandle);

/* Converting GPF task handle to BTHAL OS task handle */
static BthalOsTaskHandle OsGpfTaskHandleToOsHandle(BTHAL_U8 gpfTaskHandle);

/* Converting GPF timer index to BTHAL OS timer handle */
BthalOsTimerHandle OsGpfTimerIndexToOsHandle(int gpfTaskHandle,
                                                                                    BTHAL_U8 gpfTimerIndex);

#ifdef MEMORY_SUPERVISION
	#define FILE_LINE_TYPE      ,const char *file, int line
	#define FILE_LINE_MACRO		,__FILE__,__LINE__
#else
	#define FILE_LINE_TYPE
	#define FILE_LINE_MACRO
#endif

#define BTHAL_OS_NO_HANDLE (-1)
#define BTHAL_OS_SEM_WAIT_INTERVAL 100


/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTHAL_OS_Init()
 *
 */
BthalStatus BTHAL_OS_Init(BthalCallBack	callback)
{
	BTHAL_U32 idx;

	BTHAL_OS_LOG_FUNCTION(("BTHAL_OS: Init"));
	
	/* Init tasks parameters */
	for (idx=0; idx < BTHAL_OS_MAX_NUM_OF_TASKS; idx++)
	{
		BthalOsData.taskParams[idx].taskCallback = 0;
		BthalOsData.taskParams[idx].numOfCreatedTimers = 0;
		BthalOsData.taskParams[idx].gpfTaskHandle = 0;
	}

	/* init timer handles array */
	for (idx=0; idx < BTHAL_OS_MAX_NUM_OF_TIMERS; idx++)
	{
		BthalOsData.timerParams[idx].timerUsed = BTHAL_FALSE;
		BthalOsData.timerParams[idx].taskHandle = 0;
		BthalOsData.timerParams[idx].timerEvent = 0;
		BthalOsData.timerParams[idx].timerIndexInTask = 0;
		BthalOsData.timerParams[idx].timerName = 0;
	}

	/* init semaphore handles array */
	for (idx=0; idx < BTHAL_OS_MAX_NUM_OF_SEMAPHORES; idx++)
	{
		BthalOsData.semaphores[idx].semHandle = 0;
	}
	
	/* Create the hardware semaphore */
	if (BTHAL_FALSE == OsCreateSemaphore("sm_bt_hw", &(BthalOsData.hwMutex)))
	{
		BTHAL_OS_LOG_ERROR(("BTHAL_OS: Init failed in creating hardware mutex"));
		return BTHAL_STATUS_FAILED;
	}
	
	/* Initilization of the events flag */
	for (idx=0; idx < BTHAL_OS_MAX_NUM_OF_TASKS ; idx++)
	{
		taskEventsFlag[idx] = 0;
	}
	
	BTHAL_OS_LOG_INFO(("BTHAL_OS: Created Hardware Semaphore"));

	return BTHAL_STATUS_SUCCESS;    
}


/*-------------------------------------------------------------------------------
 * BTHAL_OS_Deinit()
 *
 */
BthalStatus BTHAL_OS_Deinit(void)
{
	if (BTHAL_TRUE == OsDestroySemaphore(&(BthalOsData.hwMutex))) 
	{
		return BTHAL_STATUS_SUCCESS;
	}

	return BTHAL_STATUS_FAILED;
}

/*-------------------------------------------------------------------------------
 * BTHAL_OS_CreateTask()
 *
 */
BthalStatus BTHAL_OS_CreateTask(BthalOsTaskHandle taskHandle, 
								EventCallback callback, 
								const char *taskName)
{
	BTHAL_OS_TASK_PARAMS *taskPar;
    
	if (taskHandle > BTHAL_OS_MAX_NUM_OF_TASKS-1)
	{
		BTHAL_Assert(0);
		return BTHAL_STATUS_FAILED;
	}
    
	taskPar = &BthalOsData.taskParams[taskHandle];
    
	taskPar->taskCallback = callback;
	taskPar->numOfCreatedTimers = 0;
	taskPar->gpfTaskHandle = OsTaskHandleToGpfHandle(taskHandle);

	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_OS_DestroyTask()
 *
 */
BthalStatus BTHAL_OS_DestroyTask(BthalOsTaskHandle taskHandle)
{
	if (taskHandle > BTHAL_OS_MAX_NUM_OF_TASKS-1)
	{
		BTHAL_Assert(0);
		return BTHAL_STATUS_FAILED;
	}
    
	BthalOsData.taskParams[taskHandle].taskCallback = 0;
    
	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_OS_SendEvent()
 *
 */
BthalStatus BTHAL_OS_SendEvent(BthalOsTaskHandle taskHandle, BthalOsEvent evt)
{
	int gpfTaskHandle = BthalOsData.taskParams[taskHandle].gpfTaskHandle;
    
	if (gpfTaskHandle == -1)
	{
		BTHAL_Assert(0);
		return BTHAL_STATUS_FAILED;
	}
	/* If a message with a similar event was already sent to the stack/a2dp task and wasn't handled yet, don't 
	 * sent another useless one.
	 */
	if (!(evt & taskEventsFlag[taskHandle])) 
	{
		taskEventsFlag[taskHandle] |= evt;
	BTHAL_Assert(VSI_OK == vsi_c_ssend(gpfTaskHandle, evt, 0, 0 FILE_LINE_MACRO));
	
	}
	
    	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_OS_CreateSemaphore()
 *
 */
BthalStatus BTHAL_OS_CreateSemaphore(const char *semaphoreName, 
									 BthalOsSemaphoreHandle *semaphoreHandle)
{
	BTHAL_U32 idx;
	Semaphore * sem;

	BTHAL_OS_LOG_FUNCTION(("BTHAL_OS: Create Semaphore"));

	/* search for free entry */
	for (idx=0; idx < BTHAL_OS_MAX_NUM_OF_SEMAPHORES; idx++)
	{
		if (BthalOsData.semaphores[idx].semHandle == 0)
		{
			break;
		}
	}
	
    	if (idx >= BTHAL_OS_MAX_NUM_OF_SEMAPHORES)
	{
		/* exceeds maximum of available handles */
		BTHAL_OS_LOG_ERROR(("BTHAL_OS: Failed creating Semaphore %d. More then max %d", 
						  idx+1,
						  BTHAL_OS_MAX_NUM_OF_SEMAPHORES));
		return BTHAL_STATUS_NO_RESOURCES;
	}

	/* create the semaphore */
	sem = &(BthalOsData.semaphores[idx]);
	if (BTHAL_FALSE == OsCreateSemaphore(semaphoreName, sem))
	{
		BTHAL_OS_LOG_ERROR(("BTHAL_OS: Failed creating Semaphore"));
		return BTHAL_STATUS_FAILED;
	}
	
	*semaphoreHandle = (BthalOsSemaphoreHandle)idx;

	BTHAL_OS_LOG_INFO(("BTHAL_OS: Created Semaphore id %d", idx));

	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_OS_DestroySemaphore()
 *
 */
BthalStatus BTHAL_OS_DestroySemaphore(BthalOsSemaphoreHandle semaphoreHandle)
{
	Semaphore * sem;
	
	if (semaphoreHandle >= BTHAL_OS_MAX_NUM_OF_SEMAPHORES)
	{
		BTHAL_Assert(0);
		return BTHAL_STATUS_FAILED;
	}

	if (0 == BthalOsData.semaphores[semaphoreHandle].semHandle)
	{
		/* semaphore does not exists */
		return BTHAL_STATUS_FAILED;
	}

	sem = &(BthalOsData.semaphores[semaphoreHandle]);

	if (BTHAL_TRUE == OsDestroySemaphore(sem))
	{
		return BTHAL_STATUS_SUCCESS;
	}

	return BTHAL_STATUS_FAILED;	
}

/*-------------------------------------------------------------------------------
 * BTHAL_OS_LockSemaphore()
 *
 */
BthalStatus BTHAL_OS_LockSemaphore(BthalOsSemaphoreHandle semaphoreHandle,
									BthalOsTime timeout)
{
	Semaphore * sem;
	
	if (semaphoreHandle >= BTHAL_OS_MAX_NUM_OF_SEMAPHORES)
	{
		BTHAL_Assert(0);
		return BTHAL_STATUS_FAILED;
	}

	if (BthalOsData.semaphores[semaphoreHandle].semHandle == 0)
	{
		/* semaphore does not exists */
		return BTHAL_STATUS_FAILED;
	}

	sem = &(BthalOsData.semaphores[semaphoreHandle]);

	/* lock the semaphore */	
	if (BTHAL_TRUE  == OsLockSemaphore(sem, timeout))
	{
		return BTHAL_STATUS_SUCCESS;
	}

	return BTHAL_STATUS_FAILED;
}
	

/*-------------------------------------------------------------------------------
 * BTHAL_OS_UnlockSemaphore()
 *
 */
BthalStatus BTHAL_OS_UnlockSemaphore(BthalOsSemaphoreHandle semaphoreHandle)
{
	Semaphore * sem;
	
	if (semaphoreHandle >= BTHAL_OS_MAX_NUM_OF_SEMAPHORES)
	{
		BTHAL_Assert(0);
		return BTHAL_STATUS_FAILED;
	}

	if (0 == BthalOsData.semaphores[semaphoreHandle].semHandle)
	{
		/* semaphore does not exists */
		return BTHAL_STATUS_FAILED;
	}

	sem = &(BthalOsData.semaphores[semaphoreHandle]);

	if (BTHAL_TRUE  == OsUnlockSemaphore(sem))
	{
		return BTHAL_STATUS_SUCCESS;
	}

	return BTHAL_STATUS_FAILED;
}

/*-------------------------------------------------------------------------------
 * BTHAL_OS_StopHardware()
 *
 */
BthalStatus BTHAL_OS_StopHardware(void)
{
	if (BTHAL_TRUE == OsLockSemaphore(&(BthalOsData.hwMutex), 0))
	{
		return BTHAL_STATUS_SUCCESS;
	}

	return BTHAL_STATUS_FAILED;
}

/*-------------------------------------------------------------------------------
 * BTHAL_OS_ResumeHardware()
 *
 */
BthalStatus BTHAL_OS_ResumeHardware(void)
{
	if (BTHAL_TRUE == OsUnlockSemaphore(&(BthalOsData.hwMutex)))
	{
		return BTHAL_STATUS_SUCCESS;
	}

	return BTHAL_STATUS_FAILED;	
}


/*-------------------------------------------------------------------------------
 * BTHAL_OS_CreateTimer()
 * note: timer event is created in bthal_os_init(). 
 */
BthalStatus BTHAL_OS_CreateTimer(BthalOsTaskHandle taskHandle, 
								const char *timerName, 
								BthalOsTimerHandle *timerHandle)
{
	BTHAL_U8 idx;

	BTHAL_OS_LOG_FUNCTION(("BTHAL_OS: Create Timer"));
    
	/* Currently GPF API does not allow destroying a timer correctly. Hence, we
         * we prevent creation the same timer twice */
	for (idx=0; idx < BTHAL_OS_MAX_NUM_OF_TIMERS; idx++)
	{
		/* Check whether a timer with the same name was already created */
		if (BthalOsData.timerParams[idx].timerName == (char *)timerName)
		{
        		BTHAL_OS_LOG_ERROR(("BTHAL_OS: Timer with the same name was already created %s",
        						           timerName));
			return BTHAL_STATUS_FAILED;
		}
	}
	
	/* look for unused timer */
	for (idx=0; idx < BTHAL_OS_MAX_NUM_OF_TIMERS; idx++)
	{
		/* found unused timer */
		if (BthalOsData.timerParams[idx].timerUsed == BTHAL_FALSE)
		{
			break;
		}
	}
	
	if (idx >= BTHAL_OS_MAX_NUM_OF_TIMERS)
	{
		/* exceeds maximum of available timers */
		BTHAL_OS_LOG_ERROR(("BTHAL_OS: Failed created timer %d. More then max %d",
							idx+1,
							BTHAL_OS_MAX_NUM_OF_TIMERS));
		return BTHAL_STATUS_NO_RESOURCES;
	}
	
	/* mark as used */
	BthalOsData.timerParams[idx].timerUsed = BTHAL_TRUE;  
	
	/* associate timerHandle to taskHandle */
	BthalOsData.timerParams[idx].taskHandle = taskHandle;

	/* Store timer index which will be used in GPF API */
	BthalOsData.timerParams[idx].timerIndexInTask =
	                                BthalOsData.taskParams[taskHandle].numOfCreatedTimers++;

       /* Store pointer to timer's name in order to prevent creation of the same timer twice */
       BthalOsData.timerParams[idx].timerName = (char *)timerName;

	*timerHandle = (BthalOsTimerHandle)idx;

	BTHAL_OS_LOG_INFO(("BTHAL_OS: Created Timer id %d", idx));

	return BTHAL_STATUS_SUCCESS;
}


/*-------------------------------------------------------------------------------
 * BTHAL_OS_DestroyTimer()
 *
 */
BthalStatus BTHAL_OS_DestroyTimer(BthalOsTimerHandle timerHandle)
{	
	if (timerHandle >= BTHAL_OS_MAX_NUM_OF_TIMERS)
	{
		BTHAL_Assert(0);
		return BTHAL_STATUS_FAILED;
	}
			
	if (BthalOsData.timerParams[timerHandle].timerUsed == BTHAL_TRUE) 
	{
		BthalOsData.timerParams[timerHandle].timerUsed = BTHAL_FALSE;
		return BTHAL_STATUS_SUCCESS;
	}

	BTHAL_OS_LOG_ERROR(("BTHAL_OS: Can't destroy unused timer"));
	
	return BTHAL_STATUS_FAILED;
}


/*-------------------------------------------------------------------------------
 * BTHAL_OS_ResetTimer()
 *
 */
BthalStatus BTHAL_OS_ResetTimer(BthalOsTimerHandle timerHandle, 
								BthalOsTime time, 
								BthalOsEvent evt)
{
	int gpfTaskHandle;
	BthalOsTaskHandle taskHandle;
	BTHAL_U8 gpfTimerIndex;
	
	/* verify :
	 * 1- valid timer handle
	 * 2- the timer was created 
	 */
	BTHAL_Assert(timerHandle < BTHAL_OS_MAX_NUM_OF_TIMERS);
	BTHAL_Assert(BthalOsData.timerParams[timerHandle].timerUsed == BTHAL_TRUE);

	/* Cancel the timer */
	if (BTHAL_FALSE == OsCancelTimer(timerHandle))
	{
		return BTHAL_STATUS_FAILED;
	}

	BthalOsData.timerParams[timerHandle].timerEvent = evt;

	taskHandle = BthalOsData.timerParams[timerHandle].taskHandle;
	gpfTaskHandle = BthalOsData.taskParams[taskHandle].gpfTaskHandle;
    
	if (gpfTaskHandle == -1)
	{
		return BTHAL_STATUS_FAILED;
	}
	else
	{
              gpfTimerIndex = BthalOsData.timerParams[timerHandle].timerIndexInTask;
        
              if(VSI_OK != vsi_t_start(gpfTaskHandle, gpfTimerIndex, (unsigned long)time))
		{
			BTHAL_Assert(0);
			return BTHAL_STATUS_FAILED;
		}
	}
			
	return BTHAL_STATUS_SUCCESS;
}


/*-------------------------------------------------------------------------------
 * BTHAL_OS_CancelTimer()
 *
 */
BthalStatus BTHAL_OS_CancelTimer(BthalOsTimerHandle timerHandle)
{
	BTHAL_Assert(timerHandle < BTHAL_OS_MAX_NUM_OF_TIMERS);
	BTHAL_Assert(BthalOsData.timerParams[timerHandle].timerUsed == BTHAL_TRUE);
	
	if (BTHAL_TRUE == OsCancelTimer(timerHandle))
	{
		return BTHAL_STATUS_SUCCESS;
	}

	return BTHAL_STATUS_FAILED;
}


/*-------------------------------------------------------------------------------
 * BTHAL_OS_Sleep()
 *
 */
BthalStatus BTHAL_OS_Sleep(BthalOsTime time)
{
	vsi_t_sleep(btt_handle, time);

	return BTHAL_STATUS_SUCCESS;
}

/*-------------------------------------------------------------------------------
 * BTHAL_OS_GetSystemTime()
 *
 */
BthalStatus BTHAL_OS_GetSystemTime(BthalOsTime *time)
{
	if (VSI_OK != vsi_t_time(bts_handle, time))
	{
		BTHAL_Assert(0);
		return BTHAL_STATUS_FAILED;
	}
    
	return BTHAL_STATUS_SUCCESS;
}


/*---------------------------------------------------------------------------
 * Cancel a timer
 */
static BTHAL_BOOL OsCancelTimer(BthalOsTimerHandle timerHandle)
{
	unsigned long time;
	BTHAL_OS_TIMER_PARAMS *timer = &BthalOsData.timerParams[timerHandle];
       BTHAL_U8 gpfTimerIndex = timer->timerIndexInTask;
       int gpfTaskHandle = BthalOsData.taskParams[timer->taskHandle].gpfTaskHandle; 

	if (gpfTaskHandle == -1)
	{
		BTHAL_Assert(0);
		return BTHAL_STATUS_FAILED;
	}
    
       BTHAL_Assert(VSI_OK == vsi_t_status (gpfTaskHandle, gpfTimerIndex, &time));
       
       if (time)
       {
		/* Stop timer */
		BTHAL_Assert(VSI_OK == vsi_t_stop(gpfTaskHandle, gpfTimerIndex));
       }

	return BTHAL_TRUE;
}

/*---------------------------------------------------------------------------
 * Callback function for events. 
 */
void OsTaskEventFunc(BTHAL_INT gpfTaskHandle, BTHAL_U32 evt)
{
	BthalOsTaskHandle taskHandle = OsGpfTaskHandleToOsHandle(gpfTaskHandle);
    
	if ( BTHAL_OS_NOT_FOUND == taskHandle)
	{
		BTHAL_Assert(0);
	}
	else
	{
		taskEventsFlag[taskHandle] &= ~evt;
		BthalOsData.taskParams[taskHandle].taskCallback(evt);	
	}
}

/*---------------------------------------------------------------------------
 * Callback function for  timeout events. 
 */
void OsTaskTimerFunc(BTHAL_INT gpfTaskHandle, BTHAL_U16 gpfTimerIndex)
{
    BthalOsTaskHandle taskHandle = OsGpfTaskHandleToOsHandle(gpfTaskHandle);
    BthalOsTimerHandle timerHandle = OsGpfTimerIndexToOsHandle(gpfTaskHandle,
                                                                                                           gpfTimerIndex);
    
	/* Make sure the timer_index is OK */
	if ( (BTHAL_OS_NOT_FOUND == taskHandle) ||
            (BTHAL_OS_NOT_FOUND == timerHandle) ||
            (taskHandle != BthalOsData.timerParams[timerHandle].taskHandle))
	{
		BTHAL_OS_LOG_ERROR(("OsTaskTimerFunc(): taskHandle %d, timerHandle %d",
                                                    taskHandle,
                                                    timerHandle));
	}
	else
	{
		BthalOsData.taskParams[taskHandle].taskCallback(BthalOsData.timerParams[timerHandle].timerEvent);	
	}
}

/*---------------------------------------------------------------------------
 * Convert the task handle to task ID. 
 */
static int OsTaskHandleToGpfHandle(BthalOsTaskHandle taskHandle)
{
	switch(taskHandle)
	{
		case BTHAL_OS_TASK_HANDLE_STACK:
			return bts_handle;

		case BTHAL_OS_TASK_HANDLE_TRANSPORT:
			return btu_handle;
			
		case BTHAL_OS_TASK_HANDLE_A2DP:
			return btav_handle;
#if FM_STACK == XA_ENABLED
		case BTHAL_OS_TASK_HANDLE_FM:
			return fms_handle;
#endif /*FM_STACK == XA_ENABLED */
            
		default:
			BTHAL_Assert(0);
	}

	return (-1);
}

/*---------------------------------------------------------------------------
 * Convert task handle used in GPF to BTHAL OS task handle. 
 */
static BthalOsTaskHandle OsGpfTaskHandleToOsHandle(BTHAL_U8 gpfTaskHandle)
{
	BTHAL_U32 idx;
	BTHAL_OS_TASK_PARAMS *taskPar = &BthalOsData.taskParams[0];

	/* Find timer's handle in timers array */
	for (idx=0; idx < BTHAL_OS_MAX_NUM_OF_TASKS; idx++, taskPar++)
	{
		if ((0 != taskPar->taskCallback) && (taskPar->gpfTaskHandle == gpfTaskHandle))
		{
/*                    BTHAL_LOG_Event(("BTHAL_OS: OsGpfTaskHandleToOsHandle() - gpfTaskHandle %d, BthalOsTaskHandle %d",
                                                    gpfTaskHandle,
                                                    idx)); */
                    return (idx);
		}
	}

	return (BTHAL_OS_NOT_FOUND);
}

/*---------------------------------------------------------------------------
 * Convert timer index used in GPF to BTHAL OS timer handle. 
 */
BthalOsTimerHandle OsGpfTimerIndexToOsHandle(int gpfTaskHandle,
                                                                                    BTHAL_U8 gpfTimerIndex)
{
	BTHAL_U32 idx;
	BTHAL_OS_TIMER_PARAMS *timerParams = &BthalOsData.timerParams[0];
       BthalOsTaskHandle taskHandle = OsGpfTaskHandleToOsHandle(gpfTaskHandle);


	/* Find timer's handle in timers array */
	for (idx=0; idx < BTHAL_OS_MAX_NUM_OF_TIMERS; idx++, timerParams++)
	{
		if ((BTHAL_TRUE == timerParams->timerUsed) &&
                  (timerParams->taskHandle == taskHandle) &&
		    (timerParams->timerIndexInTask == gpfTimerIndex))
		{
		    return (idx);
		}
	}

	return (BTHAL_OS_NOT_FOUND);
}


/*-------------------------------------------------------------------------------
 * OsLockSemaphore()
 *
 * Thefunction tries to obtain a semaphore
 *
 * If timeout > 0, quit trying after timeout, else it will wait til semaphore is free. 
 *
 * If the current task already has the semaphore or it successfully obtains the semaphore, 
 * increase the count. Otherwise, the calling task will be blocked.
 *
 * Returns: BTHAL_TRUE / BTHAL_FALSE
 */
BTHAL_BOOL OsLockSemaphore(Semaphore *sem, BthalOsTime timeout)
{
	int status = 0;
	BthalOsTime timeLeft = timeout;
	int task = vsi_p_handle(btt_handle, 0);

	/* another task wants the semaphore */
    if (sem->taskHandle != task)
	{
		while (timeLeft > 0)
		{
			/* semaphore is released */
			if (sem->lockCount == 0)
			{
				timeLeft = 0;
			}
			else
			{

				vsi_t_sleep(btt_handle, BTHAL_OS_SEM_WAIT_INTERVAL);
				if (timeLeft > BTHAL_OS_SEM_WAIT_INTERVAL)
				{
					timeLeft -= BTHAL_OS_SEM_WAIT_INTERVAL;
				}
				else /* timeout and semaphore is still locked */
				{
					return BTHAL_FALSE;
				}
			}
		}
	
		status = vsi_s_get(btt_handle, sem->semHandle);
		if (status != 0)
		{
			BTHAL_Assert(0);
			return BTHAL_FALSE;
		}
	}

    if (status == 0)
   	{
        sem->taskHandle = task;
        sem->lockCount++;
    }
	
	return BTHAL_TRUE;
}


/*-------------------------------------------------------------------------------
 * OsUnlockSemaphore()
 */
BTHAL_BOOL OsUnlockSemaphore(Semaphore *sem)
{
	if (sem->lockCount <= 0)
	{
		BTHAL_Assert(0);
		return BTHAL_FALSE;
	}

	sem->lockCount--;
    if (sem->lockCount == 0)
	{
		if (vsi_s_release(btt_handle, sem->semHandle) != 0)
		{
			BTHAL_Assert(0);
			return BTHAL_FALSE;
		}
		sem->taskHandle = BTHAL_OS_NO_HANDLE;
    }

    return BTHAL_TRUE;
}

/*-------------------------------------------------------------------------------
 * OsCreateSemaphore()
 */
static BTHAL_BOOL OsCreateSemaphore(const char *semaphoreName, Semaphore *sem)
{
	sem->semHandle = vsi_s_open(btt_handle, (char *)semaphoreName, 1);
	if (sem->semHandle < 0)
	{
		BTHAL_Assert(0);
		return BTHAL_FALSE;
	}
	sem->taskHandle = BTHAL_OS_NO_HANDLE;
	sem->lockCount = 0;
	
	return BTHAL_TRUE;
}

/*-------------------------------------------------------------------------------
 * OsDestroySemaphore()
 */
static BTHAL_BOOL OsDestroySemaphore(Semaphore *sem)
{
	if (VSI_OK != vsi_s_close(btt_handle, sem->semHandle))
	{
		BTHAL_Assert(0);
		return BTHAL_FALSE;
	}

	sem->semHandle = 0;

	return BTHAL_TRUE;
}



#include "fms.h"
#include "fm_os.h"
#include "bthal_log.h"
#include "btl_config.h"

#if 0


/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/

static TIFM_BOOL    fmInit = TIFM_FALSE;  /* Is the stack initialized? */
BthalOsSemaphoreHandle  fmStackMutexHandle;	/* Mutex for locking stack */ 

/****************************************************************************
 *
 * Prototypes
 *
 ****************************************************************************/
static void Fm_Stack_EventCallback(BthalOsEvent evtMask);

/****************************************************************************
 *
 * Functions
 *
 ****************************************************************************/

TIFM_BOOL FM_OsInit(void)
{
	BthalStatus bthalStatus;

    /* Initialize only once */
    if (fmInit == TIFM_TRUE) 
	{
		return TIFM_TRUE;
	}

	bthalStatus = BTHAL_OS_CreateSemaphore("FMS_SEM", &fmStackMutexHandle);
	if (bthalStatus == BTHAL_STATUS_FAILED)
	{
		FM_OsDeinit();
        return TIFM_FALSE;
	}

	FMS_Init();

  	bthalStatus = BTHAL_OS_CreateTask(BTHAL_OS_TASK_HANDLE_FM, 
									  Fm_Stack_EventCallback, 
									  "FMS_TASK");

	if (bthalStatus == BTHAL_STATUS_FAILED) {
		TIFM_Report(("Failed to Create FM Task\n"));
        FMS_Deinit();
        return TIFM_FALSE;
	}		
	/* Create FM timer */
	bthalStatus = BTHAL_OS_CreateTimer(BTHAL_OS_TASK_HANDLE_FM,
										"FM_TIMER",
										&FMC(fmTimerHandle));
    
	if (bthalStatus == BTHAL_STATUS_FAILED) {
		
		TIFM_Report(("Failed to Create FM Timer\n"));
		FMS_Deinit();
        return TIFM_FALSE;
    }

	fmInit = TIFM_TRUE;
	return TIFM_TRUE;
}

TIFM_BOOL FM_OsDeinit(void)
{
	BthalStatus bthalStatus;
	TIFM_BOOL status = TIFM_TRUE;

	if(fmInit)
	{
		fmInit = TIFM_FALSE;

		
		bthalStatus = BTHAL_OS_DestroyTimer(FMC(fmTimerHandle));
		if (bthalStatus == BTHAL_STATUS_FAILED)
		{
			TIFM_Report(("Failed to destroy FM Timer\n"));
			status = TIFM_FALSE;
		}

		bthalStatus = BTHAL_OS_DestroyTask(BTHAL_OS_TASK_HANDLE_FM);
		if (bthalStatus == BTHAL_STATUS_FAILED)
		{
			TIFM_Report(("Failed to destroy FM task\n"));
			status = TIFM_FALSE;
		}

		 /* Get rid of resources allocated during init */
		bthalStatus = BTHAL_OS_DestroySemaphore(fmStackMutexHandle);
		if (bthalStatus == BTHAL_STATUS_FAILED)
		{
			TIFM_Report(("Failed to destroy FM semaphore\n"));
			status = TIFM_FALSE;
		}

		/* Deinitialize the FMS */
		FMS_Deinit();

	}
	return status;
}

void NotifyFms(void)
{
    BthalStatus bthalStatus;

	/* Send a event to the stack task */
	bthalStatus = BTHAL_OS_SendEvent(BTHAL_OS_TASK_HANDLE_FM, OS_EVENT_FM_STACK_TASK_PROCESS);
	
	TIFM_Assert(bthalStatus == BTHAL_STATUS_SUCCESS);
}


static void Fm_Stack_EventCallback(BthalOsEvent evtMask)
{
	if (evtMask & OS_EVENT_FM_STACK_TASK_PROCESS)
	{
		Fm_Process();
	}
	if (evtMask & OS_EVENT_FM_TIMER_EXPIRED)
	{
		Fm_Timer_Process();
	}
}



#endif /*BTL_CONFIG_FM_STACK == BTL_CONFIG_ENABLED*/



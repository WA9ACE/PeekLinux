#include "nucleus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"

#include "pei.h"
#include "tok.h"
#include "gsm.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "exedefs.h"
#include "exepowr.h"
#include "emopei.h"

/*=========================== MACROS =========================================*/

#define VSI_CALLER ems_handle, 
#define pei_create ems_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*===========================Global Variables==================================*/

T_HANDLE ems_handle;
T_HANDLE EmshComm = VSI_ERROR;

static BOOL first_access = TRUE;
static BOOL exit_flag = FALSE;

extern ExeTaskCbT     *ExeTaskCb[];
extern NU_MEMORY_POOL  ExeSystemMemory;

extern MailQueueT EMSMailQueueTable[1];

/*===========================Function Definition================================*/
/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_monitor
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame in case sudden entity
	 |               specific data is requested (e.g. entity Version).
	 |
	 | Parameters  : out_monitor       - return the address of the data to be
	 |                                   monitoredCommunication handle
	 |
	 | Return      : PEI_OK            - successful (address in out_monitor is valid)
	 |               PEI_ERROR         - not successful
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_monitor (void ** out_monitor)
{
	RVM_TRACE_DEBUG_HIGH ("ems_pei_monitor");

	return PEI_OK;
} /* End pei_monitor(..) */


/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_config
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame when a primitive is
	 |               received indicating dynamic configuration.
	 |
	 |               This function is not used in this entity.
	 |
	 |Parameters   :  in_string   - configuration string
	 |
	 | Return      : PEI_OK            - sucessful
	 |               PEI_ERROR         - not successful
	 |
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_config (char *inString)
{
	RVM_TRACE_DEBUG_HIGH ("ems_pei_config");

	return PEI_OK;

}/* End pei_config(..) */


/*
	 +------------------------------------------------------------------------------
	 |  Function     :  pei_timeout
	 +------------------------------------------------------------------------------
	 |  Description  :  Process timeout.
	 |
	 |  Parameters   :  index     - timer index
	 |
	 |  Return       :  PEI_OK    - timeout processed
	 |                  PEI_ERROR - timeout not processed
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_timeout (unsigned short index)
{
	RVM_TRACE_DEBUG_HIGH ("ems_pei_timeout");

	return PEI_OK;
}/* End pei_timeout(..) */


/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_signal
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame when a signal has been
	 |               received.
	 |
	 | Parameters  : opc               - signal operation code
	 |               *data             - pointer to primitive
	 |
	 | Return      : PEI_OK            - signal processed
	 |               PEI_ERROR         - signal not processed
	 |
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
	RVM_TRACE_DEBUG_HIGH ("ems_pei_signal");

	return(PEI_OK);
}/* End pei_signal(..) */


/*
	 +------------------------------------------------------------------------------
	 |  Function     :  pei_primitive
	 +------------------------------------------------------------------------------
	 |  Description  :  Process protocol specific primitive.
	 |
	 |  Parameters   :  prim      - pointer to the received primitive
	 |
	 |  Return       :  PEI_OK    - function succeeded
	 |                  PEI_ERROR - function failed
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_primitive (void * primptr)
{
	T_PRIM * prim = primptr;

	RVM_TRACE_DEBUG_HIGH("trans_pei_primitive");

	return PEI_OK;
}/* End pei_primitive(..) */


/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_run
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame when entering the main
	 |               loop. This function is only required in the active variant.
	 |
	 |               This function is not used.
	 |
	 |  Parameters   :  taskhandle  - handle of current process
	 |                  comhandle   - queue handle of current process
	 |
	 | Return      : PEI_OK            - sucessful
	 |               PEI_ERROR         - not successful
	 |
	 +------------------------------------------------------------------------------
 */

LOCAL SHORT pei_run(T_HANDLE task_handle, T_HANDLE com_handle)
{  
  RVM_TRACE_DEBUG_HIGH("ems_pei_run");

	while(!uiStatusGet())
		TCCE_Task_Sleep(100);

 RVM_TRACE_DEBUG_HIGH("ems_pei_run starting task");

	EMSTask(1, 0);

	return RV_OK;  
}/* End pei_run(..) */


/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_exit
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame when the entity is
	 |               terminated. All open resources are freed.
	 |
	 | Parameters  : -
	 |
	 | Return      : PEI_OK            - exit successful
	 |               PEI_ERROR         - exit not successful
	 |
	 +------------------------------------------------------------------------------
 */
LOCAL SHORT pei_exit (void)
{
	RVM_TRACE_DEBUG_HIGH ("ems_pei_exit");

	exit_flag = 1;

	return PEI_OK;
}/* End pei_exit(..) */


/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_init
	 +------------------------------------------------------------------------------
	 | Description : This function is called by the frame. It is used to initialise
	 |               the entitiy.
	 |
	 | Parameters  : handle            - task handle
	 |
	 | Return      : PEI_OK            - entity initialised
	 |               PEI_ERROR         - entity not (yet) initialised
	 |
	 +------------------------------------------------------------------------------
 */

ExeTaskCbT EMSExeTaskCb;

LOCAL SHORT pei_init (T_HANDLE handle)
{
	int i;
	void *rPtr;
	ExeTaskCbT *task;

	RVM_TRACE_DEBUG_HIGH("ems_pei_init");

	ExeTaskCb[EXE_EM_S_ID] = &EMSExeTaskCb;
  task = ExeTaskCb[EXE_EM_S_ID];

	ems_handle = handle;

	if (!EVCE_Create_Event_Group(&task->EventGroupCb, "EMSEvGrp"))
	{
		for (i = 0; i < 1; i++) 
		{
			if(!DMCE_Allocate_Memory(&ExeSystemMemory, &rPtr, EMSMailQueueTable[i].Size << 2, NU_READY)) 
			{
				QUCE_Create_Queue(&task->MailQueueCb[i], rPtr, "EMSQue", EMSMailQueueTable[i].Size, NU_FIXED_SIZE, 3, NU_SEMAPHORE_SUSPEND);
			}
		}

		task->NumMsgs = 0;
		for (i = 0; i < EXE_NUM_MAILBOX; i++)
		{
			task->NumMsgsInQueue[i] = 0;
		}
	}

 RVM_TRACE_DEBUG_HIGH("ems_pei_init done");
	return PEI_OK;
} /* End pei_init(..) */

/*
	 +------------------------------------------------------------------------------
	 | Function    : pei_create
	 +------------------------------------------------------------------------------
	 | Description :  This function is called by the frame when the process is
	 |                created.
	 |
	 | Parameters  : out_name          - Pointer to the buffer in which to locate
	 |                                   the name of this entity
	 |
	 | Return      : PEI_OK            - entity created successfuly
	 |               PEI_ERROR         - entity could not be created
	 |
	 +------------------------------------------------------------------------------
 */
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{

	static const T_PEI_INFO pei_info =
	{
		"BALUIS",         /* name */
		{              /* pei-table */
			pei_init,
			pei_exit,
			NULL,/*pei_primitive,*/           /* NO pei_primitive */
			NULL,/*pei_timeout,*/           /* NO pei_timeout */
			NULL,/*pei_signal,*/           /* NO pei_signal */   
			pei_run,        /*-- ACTIVE Entity--*/
			NULL,/*pei_config,*/           /* NO pei_config */
			NULL/*pei_monitor*/            /* NO pei_monitor */
		},
		0x1000,            /* stack size */
		1,                        /* queue entries */
		BAL_UIS_PRIORITY,     /* priority (1->low, 255->high) */
		1,                         /* number of timers */
		COPY_BY_REF	/* Flags Settings */
	};

	RVM_TRACE_DEBUG_HIGH("ems_pei_create");

	/*
	 * Close Resources if open
	 */
	if (first_access)
		first_access = FALSE;
	else
		pei_exit();

	/*
	 * Export startup configuration data
	 */
	*info = (T_PEI_INFO *)&pei_info;

	RVM_TRACE_DEBUG_HIGH("ems_pei_create done");

	return PEI_OK;
}/* End pei_create(..) */

/*======================== END OF FILE =========================================*/

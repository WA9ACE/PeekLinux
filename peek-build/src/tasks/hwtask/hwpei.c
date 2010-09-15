#include "nucleus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"
#include "gsm.h"
#include "pei.h"
#include "tok.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "emopei.h"
#include "exeapi.h"
#include "ui_pei.h"

#define         NU_VARIABLE_SIZE                13
#define         NU_QUEUE_SIZE                   18

extern int kbdEmobiixCB(T_EMOBIIX_KBD_EVENT *event);

/*=========================== MACROS =========================================*/

#define VSI_CALLER HW_handle, 
#define pei_create hw_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*===========================Global Variables==================================*/

T_HANDLE HW_handle;
T_HANDLE hCommUI = VSI_ERROR;

LOCAL BOOL first_access = TRUE;

static UINT32 HwIsReady = 0;

UINT32 HwStatusGet(void)
{
        return HwIsReady;
}

void HwStatusSet(void) {
        HwIsReady=1;
}

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
  RVM_TRACE_DEBUG_HIGH ("HW: pei_monitor");

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
  RVM_TRACE_DEBUG_HIGH ("HW: pei_config");

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
  RVM_TRACE_DEBUG_HIGH ("HW: pei_timeout");

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
  RVM_TRACE_DEBUG_HIGH ("HW: pei_signal");
  
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
	T_PRIM *prim = (T_PRIM *) primptr;
	unsigned long opc = prim->custom.opc;


	if (!prim)
		return PEI_OK;

	RVM_TRACE_DEBUG_HIGH("HW: pei_primitive");
	switch (opc)
	{
		case EMOBIIX_KBD_EVENT:
		{
			kbdEmobiixCB((T_EMOBIIX_KBD_EVENT *)(&prim->data));
			PFREE(prim);
			return PEI_OK;
		}

		default:
		{
			if (opc & SYS_MASK)
				vsi_c_primitive (VSI_CALLER prim);
		}
		break;
	}

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

LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{  
  RVM_TRACE_DEBUG_HIGH("HW: pei_run");

  hwStart();

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
   RVM_TRACE_DEBUG_HIGH ("HW: pei_exit");
 
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

NU_MEMORY_POOL ExeSystemMemory;

UINT32 ExeIntStack[20];
UINT32 *ExeIntStackP;

extern ExeTaskCbT *ExeTaskCb[];

static char SystemMemory[0xABB4];

const MailQueueT EMOMailQueueTable[] = {{BAL_TASK_MAIL_QUEUE_1, BOS_MAILBOX_1_ID},
                        {BAL_TASK_MAIL_QUEUE_2, BOS_MAILBOX_2_ID}};
ExeTaskCbT EMOExeTaskCb;

#define EXE_BAL_ID 12

LOCAL SHORT pei_init (T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;
    int i;
    void *rPtr;
    int qCstatus;
    ExeTaskCbT *task;

    RVM_TRACE_DEBUG_HIGH("HW: pei_init");
    HW_handle = handle;

	if(hCommUI < VSI_OK)
		if ((hCommUI = vsi_c_open (VSI_CALLER "UI")) < VSI_OK)
			return PEI_ERROR;

    ExeIntStackP = &ExeIntStack[0];

    DMCE_Create_Memory_Pool(&ExeSystemMemory, "SysMem", SystemMemory, 0xABB4, 0x32, NU_SEMAPHORE_SUSPEND);

    ExeTaskCb[EXE_BAL_ID] = &EMOExeTaskCb;
    task = ExeTaskCb[EXE_BAL_ID];

    if(!EVCE_Create_Event_Group(&task->EventGroupCb, "BalEvGrp")) {
    for(i=0;i < 2;i++) {
        if(!DMCE_Allocate_Memory(&ExeSystemMemory, &rPtr, (EMOMailQueueTable[i].Size << 2), NU_READY)) {
            qCstatus = QUCE_Create_Queue(&task->MailQueueCb[i], "BalQue", rPtr, EMOMailQueueTable[i].Size, NU_FIXED_SIZE, 3, NU_SEMAPHORE_SUSPEND);
        }
    }
    if(!qCstatus) {
        task->NumMsgs = 0;
        for (i = 0; i < EXE_NUM_MAILBOX; i++)
        {
        	task->NumMsgsInQueue[i] = 0;
        }
    }
	}

    ExeInit();

	return (PEI_OK);
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
               "HW",         /* name */
               {              /* pei-table */
                  pei_init,
                  pei_exit,
                  pei_primitive, 
                  NULL,           /* NO pei_timeout */
                  NULL,           /* NO pei_signal */   
                  pei_run,      
                  NULL,           /* NO pei_config */
                  NULL            /* NO pei_monitor */
               },
               0xc00,            /* stack size */
               1,                        /* queue entries */
	       	   HW_PRIORITY,     /* priority (1->low, 255->high) */
               0,                         /* number of timers */
               COPY_BY_REF	/* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("HW: pei_create");

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
  
  return PEI_OK;
}/* End pei_create(..) */

/*======================== END OF FILE =========================================*/

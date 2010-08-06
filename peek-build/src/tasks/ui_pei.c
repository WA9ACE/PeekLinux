#include "nucleus.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "ccdapi.h"

#include "vsi.h"        /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */
#include "typedefs.h"   /* to get Condat data types */

#include "pei.h"
#include "tok.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "emopei.h"

#include "p_sim.h"
#include "gprs.h"

#define         NU_VARIABLE_SIZE                13
#define         NU_QUEUE_SIZE                   18

#define ENTITY_UI

/*=========================== MACROS =========================================*/

#define VSI_CALLER UI_handle, 
#define pei_create ui_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*===========================Global Variables==================================*/
T_HANDLE UI_handle;

#define _ENTITY_PREFIXED(N) ui_##N

#define hCommAPP        _ENTITY_PREFIXED(hCommAPP)

LOCAL BOOL first_access = TRUE;
static UINT32 uiIsReady = 0;

T_HANDLE hCommAPP = VSI_ERROR;

//extern NU_MEMORY_POOL  ExeSystemMemory;

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
  RVM_TRACE_DEBUG_HIGH ("UI: pei_monitor");

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
  RVM_TRACE_DEBUG_HIGH ("UI: pei_config");

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
  RVM_TRACE_DEBUG_HIGH ("UI: pei_timeout");

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
  RVM_TRACE_DEBUG_HIGH ("UI: pei_signal");
  
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
  T_RV_HDR* msg_p;

   RVM_TRACE_DEBUG_HIGH("UI: pei_primitive");

   return PEI_OK;
}/* End pei_primitive(..) */

UINT32 uiStatusGet(void) 
{
	return uiIsReady;
}

void uiStatusSet(void) {
	uiIsReady=1;
}

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

extern void mmiInit( void);
extern void UiTask(void);

LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{  

  RVM_TRACE_DEBUG_HIGH("UI: pei_run");

  /* Wait for Main EMO task to start */
  while(!EmoStatusGet())
 	TCCE_Task_Sleep(100);

  /* Start UI Task */
  mmiInit();
  //UITask();
  
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
   RVM_TRACE_DEBUG_HIGH ("UI: pei_exit");
 
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
/*
extern ExeTaskCbT     *ExeTaskCb[];

const MailQueueT UiMailQueueTable[] = {{UI_TASK_MAIL_QUEUE_1, BOS_MAILBOX_1_ID},
				      {UI_TASK_MAIL_QUEUE_2, BOS_MAILBOX_2_ID},
				      {UI_TASK_MAIL_QUEUE_3, BOS_MAILBOX_3_ID},
				      {UI_TASK_MAIL_QUEUE_4, BOS_MAILBOX_4_ID}};
ExeTaskCbT UIExeTaskCb;
*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;
/*
    int i;
    void *rPtr;
    int qCstatus;
    ExeTaskCbT *task;
*/
    RVM_TRACE_DEBUG_HIGH("UI: pei_init");
    UI_handle = handle;

    /*
    ExeTaskCb[EXE_UI_ID] = &UIExeTaskCb;
    task = ExeTaskCb[EXE_UI_ID];

    if(!EVCE_Create_Event_Group(&task->EventGroupCb, "UiEvGrp")) {
	for(i=0;i < 4;i++) {
		if(!DMCE_Allocate_Memory(&ExeSystemMemory, &rPtr, UiMailQueueTable[i].Size, NU_READY)) {
			qCstatus = QUCE_Create_Queue(&task->MailQueueCb[i], "UiQue", rPtr, UiMailQueueTable[i].Size, NU_FIXED_SIZE, 3, NU_SEMAPHORE_SUSPEND);
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
    */
  if(hCommAPP < VSI_OK)
        if ((hCommAPP = vsi_c_open (VSI_CALLER APP_NAME)) < VSI_OK)
                return PEI_ERROR;

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
               "UI",         /* name */
               {              /* pei-table */
                  pei_init,
                  pei_exit,
                  NULL,           /* NO pei_primitive */
                  NULL,           /* NO pei_timeout */
                  NULL,           /* NO pei_signal */   
                  pei_run,        /*-- ACTIVE Entity--*/
                  NULL,           /* NO pei_config */
                  NULL            /* NO pei_monitor */
               },
               0x2000,            /* stack size */
               1,                        /* queue entries */
		BAL_UI_PRIORITY,     /* priority (1->low, 255->high) */
               0,                         /* number of timers */
               COPY_BY_REF	/* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("UI: pei_create");

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

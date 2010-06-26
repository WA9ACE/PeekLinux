/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Modul   :  RTC
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
|                 All rights reserved. 
| 
|                 This file is confidential and a trade secret of Texas 
|                 Instruments Berlin, AG 
|                 The receipt of or possession of this file does not convey 
|                 any rights to reproduce or disclose its contents or to 
|                 manufacture, use, or sell anything it may describe, in 
|                 whole, or in part, without the specific written consent of 
|                 Texas Instruments Berlin, AG. 
+----------------------------------------------------------------------------- 
|  Purpose :  This module implements the process body interface
|             for the entity RTC
|
|             Exported functions:
|
|          pei_create    - Create the Protocol Stack Entity
|
|          pei_init      - Initialize Protocol Stack Entity
|          pei_exit      - Close resources and terminate
|          pei_primitive - Process Primitive
|          pei_timeout   - Process Timeout
|          pei_signal    - Process Signals             
|          pei_run       - Process Messages
|          pei_config    - Dynamic Configuration
|          pei_monitor   - Monitoring of physical Parameters
|+----------------------------------------------------------------------------- 
*/ 

/*=========================== INCLUDES ===========================*/
#ifndef _WINDOWS
#include "nucleus.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "ccdapi.h"

#include "pei.h"
#include "tok.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "rtc/rtc_env.h"
#include "rtc/rtc_i.h"
#include "rtc/rtc_api.h"

/*=========================== MACROS =========================================*/

#define VSI_CALLER RTC_handle, 
#define pei_create rtc_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*===========================Global Variables==================================*/

T_RTC_ENV_CTRL_BLK *rtc_env_ctrl_blk = NULL;

T_HANDLE RTC_handle;
T_HANDLE rtc_hCommRTC = -1;

LOCAL BOOL first_access = TRUE;

static T_RVM_RETURN (*rtc_error_ft) (T_RVM_NAME       swe_name,
                                     T_RVM_RETURN     error_cause,
                                     T_RVM_ERROR_TYPE error_type,
                                     T_RVM_STRING     error_msg);

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
  RVM_TRACE_DEBUG_HIGH ("RTC: pei_monitor");

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
  RVM_TRACE_DEBUG_HIGH ("RTC: pei_config");

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
  RVM_TRACE_DEBUG_HIGH ("RTC: pei_timeout");

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
  RVM_TRACE_DEBUG_HIGH ("RTC: pei_signal");
  
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

   RVM_TRACE_DEBUG_HIGH("RTC: pei_primitive");

   msg_p = (T_RV_HDR*) P2D(primptr);
   rtc_handle_msg(msg_p);
   
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
  RVM_TRACE_DEBUG_HIGH("RTC pei_run");
  
  rtc_core();

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
   RVM_TRACE_DEBUG_HIGH ("RTC: pei_exit");
 
   /*
    * Close communication channels
    */
 
   vsi_c_close (VSI_CALLER rtc_hCommRTC);
   rtc_hCommRTC = VSI_ERROR;

   rtc_stop();

   rtc_kill();  
   
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
LOCAL SHORT pei_init (T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;
    
    RVM_TRACE_DEBUG_HIGH ("RTC: pei_init");

    /*
     * Initialize task handle
     */
    RTC_handle = handle;
    rtc_hCommRTC = -1;

    if(RTC_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("RTC_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( RTC_handle, "RTC" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (rtc_hCommRTC < VSI_OK)
    {
      if ((rtc_hCommRTC = vsi_c_open (VSI_CALLER "RTC" )) < VSI_OK)
        return PEI_ERROR;
    }
   
    RVM_TRACE_DEBUG_HIGH("RTC Open communication channels done"); 
  
     /* Create instance gathering all the variable used by RTC instance */
     
	if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_RTC_ENV_CTRL_BLK), (T_RVF_BUFFER**)&rtc_env_ctrl_blk) != RVF_GREEN)
	{
            /* The environemnt will cancel the EXPL instance creation. */
            RVM_TRACE_DEBUG_HIGH ("RTC: Error to get memory ");

            return RVM_MEMORY_ERR;	
	}

	/* Store the address ID. */
	rtc_env_ctrl_blk->addr_id = RTC_handle;

	/* Store the pointer to the error function. */
        rtc_error_ft = rvm_error;

        rtc_env_ctrl_blk->prim_id = EXT_MEM_POOL;
    
    /* Initialise rtc driver */
    ret = rtc_init();
    
    if (ret == RVF_OK) 
    {
       RVM_TRACE_DEBUG_HIGH("RTC_Initialize() == RV_OK");
    }
    else
    {
       RVM_TRACE_DEBUG_HIGH("RTC_Initialize() != RV_OK");
       return PEI_ERROR;
    }

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
               "RTC",         /* name */
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
               RTC_STACK_SIZE,            /* stack size */
#if (LOCOSTO_LITE==1)
               2,                        /* queue entries */
#else
		10,
#endif
		(255-RVM_RTC_TASK_PRIORITY),     /* priority (1->low, 255->high) */
               0,                         /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND /* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("RTC: pei_create");

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


/*
+------------------------------------------------------------------------------
| Function    : rtc_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

T_RVM_RETURN rtc_init(void)
{
// CQ 20001 avoid RTC wakeup interrupts every minute in NO GSM mode on OMAP730
#if  (!(defined _WINDOWS) && (BOARD == 35))
	return RVF_OK;
#else
	return RTC_Initialize();
#endif
} /* End rtc_init() */


/*
+------------------------------------------------------------------------------
| Function    : rtc_stop
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rtc_stop(void)
{
   /* other SWEs have not been killed yet, rtc can send messages to other SWEs */

   return RV_OK;
}/* End rtc_stop() */


/*
+------------------------------------------------------------------------------
| Function    : rtc_kill
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rtc_kill (void)
{
   /* free all memory buffer previously allocated */
   rvf_free_buf ((void *) rtc_env_ctrl_blk->msg_alarm_event);
   rvf_free_buf ((void *) rtc_env_ctrl_blk);

   return RV_OK;
}/* End rtc_kill() */


/*======================== END OF FILE =========================================*/

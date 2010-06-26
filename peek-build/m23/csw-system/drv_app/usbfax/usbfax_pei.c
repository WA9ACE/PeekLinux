/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Modul   :  USBFAX
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
|             for the entity USBFAX
|
|             Exported functions:
|
|             pei_create    - Create the Protocol Stack Entity
|             pei_init      - Initialize Protocol Stack Entity
|             pei_primitive - Process Primitive
|             pei_timeout   - Process Timeout
|             pei_exit      - Close resources and terminate
|             pei_run       - Process Primitive
|             pei_config    - Dynamic Configuration
|             pei_monitor   - Monitoring of physical Parameters
|+----------------------------------------------------------------------------- 
*/

/*============================ Includes ======================================*/

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

#include "rvm/rvm_ext_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "usbfax/usbfax_i.h"
#include "usbfax/usbfax_env.h"
#include "usbfax/usbfax_cfg.h"
#include "usbfax/usbfax_task_i.h"
#include "usbfax/usbfax_env.h"

/*============================ Macros ========================================*/
#define VSI_CALLER USBFAX_handle, 
#define pei_create usbfax_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_HANDLE USBFAX_handle;
T_HANDLE usbfax_hCommUSBFAX = -1;
static BOOL first_access  = TRUE;

T_USBFAX_ENV_CTRL_BLK *usbfax_env_ctrl_blk_p = NULL;

/*============================ Function Definition============================*/
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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  RVM_TRACE_DEBUG_HIGH ("USBFAX: pei_monitor");

  return PEI_OK;

} // End pei_monitor(..) 

*/


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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("USBFAX: pei_config");

  return PEI_OK;

}// End of pei_config(..) 

*/


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
SHORT usbfax_pei_timeout (int h1,int h2,USHORT tnum)
{
  rvf_send_event(usbfax_env_ctrl_blk_p->addr_id,(UINT16)(EVENT_MASK(((tnum % RVF_NUM_TASK_TIMERS)+4)) ) );

  return 0;

}/* End of pei_timeout(..) */


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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  RVM_TRACE_DEBUG_HIGH ("USBFAX: pei_signal");
  
  return PEI_OK;

}// End of pei_signal(..) 

*/


/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
   RVM_TRACE_DEBUG_HIGH ("USBFAX: pei_exit");
  
   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER usbfax_hCommUSBFAX);
   usbfax_hCommUSBFAX = VSI_ERROR;

   usbfax_stop(NULL);
   usbfax_kill();
   return PEI_OK;
}/* End of pei_exit(..) */

/*
+------------------------------------------------------------------------------
| Function    : pei_primitive
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when a primitive is
|                received and needs to be processed.
|
| Parameters  : prim      - Pointer to the received primitive
|
| Return      : PEI_OK    - function succeeded
|               PEI_ERROR - function failed
|
+------------------------------------------------------------------------------
*/

/*
LOCAL SHORT pei_primitive (void * primptr)
{
   T_RV_HDR* msg_p ; 
 
   RVM_TRACE_DEBUG_HIGH("USBFAX: pei_primitive");
   
   msg_p = (T_RV_HDR*) P2D(primptr);
   usbfax_handle_message(msg_p);
   
   return PEI_OK;
}// End of pei_primitive(..) 

*/

/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
| Parameters  : handle            - Communication handle
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{
   BOOLEAN error_occured = FALSE;
   T_RV_HDR* msg_p = NULL;
   UINT16 received_event = 0;
   char buf[64] = "";

   RVM_TRACE_DEBUG_HIGH("USBFAX: pei_run");

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
	 usbfax_handle_message(msg_p);
      }

      if (received_event & RVF_TIMER_0_EVT_MASK)
      {
	 usbfax_handle_timer(NULL);
      }
 	
   }
   return PEI_OK;
  
}/* End of pei_run(..) */

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
    
    RVM_TRACE_DEBUG_HIGH("USBFAX: Initialization");
    /*
     * Initialize task handle
     */
    USBFAX_handle = handle;
    usbfax_hCommUSBFAX = -1;

     if(USBFAX_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("USBFAX_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( USBFAX_handle, "USBFAX" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }
    /*
     * Open communication channels
     */
    if (usbfax_hCommUSBFAX < VSI_OK)
    {
      if ((usbfax_hCommUSBFAX = vsi_c_open (VSI_CALLER "USBFAX" )) < VSI_OK)
        return PEI_ERROR;
    }
 
    /* Create instance gathering all the variable used by EXPL instance */

    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_USBFAX_ENV_CTRL_BLK), (T_RVF_BUFFER**)&usbfax_env_ctrl_blk_p) == RVF_RED)
    {
       /* The environment will cancel the EXPL instance creation. */
       RVM_TRACE_DEBUG_HIGH("USB: Error to get memory ");
	    
       return RVM_MEMORY_ERR;	
    }

    /* Store the address ID. */
    usbfax_env_ctrl_blk_p->addr_id = handle;

    /* Store the pointer to the error function. */
    usbfax_env_ctrl_blk_p->error_ft = rvm_error;

    usbfax_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;

    usbfax_init();
    usbfax_start();
    
    RVM_TRACE_DEBUG_HIGH("USBFAX: pei_init complete");
    
    return RVM_OK;
}/* End pei_init() */

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
               "USBFAX",         
               {              
                 pei_init,      
                 pei_exit,      
                 NULL,//pei_primitive, 
                 NULL,           /* NO pei_timeout */
                 NULL,           /* NO pei_signal */
                 pei_run,           /* NO pei_run */
                 NULL,     /* NO pei_config */
                 NULL            /* NO pei_monitor */
               },
               USBFAX_STACK_SIZE,     /* stack size */
               10,        /* queue entries */
               (255 - USBFAX_TASK_PRIORITY),      /* priority (1->low, 255->high) */
               1,        /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND
              };

        RVM_TRACE_DEBUG_HIGH("USBFAX: pei_create");
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
     *info = (T_PEI_INFO *) &pei_info;
  
      return PEI_OK;
}/* End pei_create(..) */

/*
+------------------------------------------------------------------------------
| Function    : usbfax_change_internal_state
+------------------------------------------------------------------------------
| Description : This function is called to change the USBFAX driver state
|
| Parameters  : T_USBFAX_INTERNAL_STATE : new state
|
| Return      : PEI_OK            - entity created successfuly
|               PEI_ERROR         - entity could not be created
+------------------------------------------------------------------------------
*/
void usbfax_change_internal_state(T_USBFAX_INTERNAL_STATE state)
{
	usbfax_env_ctrl_blk_p->state = state;
}

/*
+------------------------------------------------------------------------------
| Function    : usbfax_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbfax_init (void)
{
	T_RVM_RETURN ret = RVM_OK;

	USBFAX_SEND_TRACE("USBFAX: usbfax_init called by RVM", RV_TRACE_LEVEL_DEBUG_LOW);
	/*
	 * Here the instance (usbfax_env_ctrl_blk_p) could be initialised.
	 */
	usbfax_change_internal_state(USBFAX_INITIALIZED);
	usbfax_env_ctrl_blk_p->usbfax_return_path.addr_id = usbfax_env_ctrl_blk_p->addr_id;
	usbfax_env_ctrl_blk_p->usbfax_return_path.callback_func = NULL;


    //OMAPS72906 change to static mutex
	rvf_initialize_static_mutex(&usbfax_env_ctrl_blk_p->mutex);

	return ret;
}

/*
+------------------------------------------------------------------------------
| Function    : usbfax_start
+------------------------------------------------------------------------------
| Description : Called to start the USBFAX SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbfax_start(void)
{
	T_RVM_RETURN ret = RVM_OK;

	USBFAX_SEND_TRACE("USBFAX: start called", RV_TRACE_LEVEL_DEBUG_LOW);
	usbfax_change_internal_state(USBFAX_IDLE); //swe has started
	USBFAX_SEND_TRACE("USBFAX: state changed to USBFAX_IDLE", RV_TRACE_LEVEL_DEBUG_LOW);

	usbfax_env_ctrl_blk_p->host_serial_driver_connected = FALSE;

	usbfax_cdc_devices_initialize();

	return ret;
}

/*
+------------------------------------------------------------------------------
| Function    : usbfax_stop
+------------------------------------------------------------------------------
| Description : Called to stop the USBFAX SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbfax_stop (T_RV_HDR * msg)
{
	/*
	 * Here we should stop the activities of the SWE
	 * It is still possible to send messages to other SWE, to unregister for example.
	 */
	USBFAX_SEND_TRACE("USBFAX: stop called", RV_TRACE_LEVEL_DEBUG_LOW);

	usbfax_change_internal_state(USBFAX_STOPPED);
	USBFAX_SEND_TRACE("USBFAX: state changed to USBFAX_STOPPED", RV_TRACE_LEVEL_DEBUG_LOW);

	return RVM_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : usbfax_kill
+------------------------------------------------------------------------------
| Description : Called to stop the USBFAX SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbfax_kill (void)
{
	/*
	 * Here we cannot send messages anymore. We only free the last
	 * used resources, like the control block buffer.
	 */
	USBFAX_SEND_TRACE("USBFAX: kill called", RV_TRACE_LEVEL_DEBUG_LOW);

	usbfax_change_internal_state(USBFAX_KILLED);

	rvf_delete_mutex(&usbfax_env_ctrl_blk_p->mutex);

	rvf_free_buf(usbfax_env_ctrl_blk_p);

	return RVM_OK;
}



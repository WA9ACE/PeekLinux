/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Module  :  USBTRC
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
|             for the entity USBTRC
|
|             Exported functions:
|
|          pei_create    - Create the Protocol Stack Entity
|          pei_init      - Initialize Protocol Stack Entity
|          pei_exit      - Close resources and terminate
|          pei_run       - Process Messages
|          pei_primitive - Process Primitive
|          pei_signal    - Process Signals             
|          pei_timeout   - Process Timeout
|          pei_config    - Dynamic Configuration
|          pei_monitor   - Monitoring of physical Parameters
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

#include "usbtrc/usbtrc_i.h"
#include "usbtrc/usbtrc_env.h"
#include "usbtrc/usbtrc_pool_size.h"

/*============================ Macros ========================================*/
#define VSI_CALLER USBTRC_handle, 
#define pei_create usbtrc_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_HANDLE USBTRC_handle;
T_HANDLE usbtrc_hCommUSBTRC = -1;
static BOOL first_access  = TRUE;

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
  RVM_TRACE_DEBUG_HIGH ("USBTRC: pei_monitor");

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
  RVM_TRACE_DEBUG_HIGH ("USBTRC: pei_config");

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

/*
LOCAL SHORT pei_timeout (unsigned short index)
{
  RVM_TRACE_DEBUG_HIGH ("USBTRC: pei_timeout");

  return PEI_OK;

}// End of pei_timeout(..) 

*/


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
  RVM_TRACE_DEBUG_HIGH ("USBTRC: pei_signal");
  
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
   RVM_TRACE_DEBUG_HIGH ("USBTRC: pei_exit");

   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER usbtrc_hCommUSBTRC);
   usbtrc_hCommUSBTRC = VSI_ERROR;

   usbtrc_stop (NULL);
   usbtrc_kill ();
   
   return RVM_OK;
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
   T_RV_HDR *msg_p;
   T_RVM_RETURN ret = RVM_OK;
   
   RVM_TRACE_DEBUG_HIGH("USBTRC: pei_primitive");

   msg_p = (T_RV_HDR*) P2D(primptr);
   ret = usbtrc_handle_message(msg_p);
   
   return ret;
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
   BOOLEAN error_occured = FALSE;
   T_RV_HDR* msg_p = NULL;
   UINT16 received_event = 0;
   char buf[64] = "";

   usbtrc_start();

   RVM_TRACE_DEBUG_HIGH("USBTRC: pei_run");

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */

      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         usbtrc_handle_message(msg_p);         
      }
  	
   }
   return PEI_OK;
  
}/* End pei_run(..) */


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
    /*T_HANDLE usb_hCommUSBTRC;*/
    
    T_RV_RET ret = RV_OK;
    
    RVM_TRACE_DEBUG_HIGH("USBTRC: Initialization");
    
    /*
     * Initialize task handle
     */

    USBTRC_handle = handle;
    usbtrc_hCommUSBTRC = -1;

    if(USBTRC_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("USBTRC_handle NE gsp_get_taskid");
    }

    /* First make sure USB communication Channel is open 
    if (usb_hCommUSBTRC < VSI_OK)
    {
      if ((usb_hCommUSBTRC = vsi_c_open (VSI_CALLER "USB" )) < VSI_OK)
        return PEI_ERROR;
    }
    */
    /*
     * Open communication channels
     */
    if (usbtrc_hCommUSBTRC < VSI_OK)
    {
      if ((usbtrc_hCommUSBTRC = vsi_c_open (VSI_CALLER "USBTRC" )) < VSI_OK)
        return PEI_ERROR;
    }
   
    if( gsp_init_remu_task( USBTRC_handle, "USBTRC" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }
 
    /* Create instance gathering all the variable used by EXPL instance */
    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_USBTRC_ENV_CTRL_BLK),
                   (T_RVF_BUFFER**)&usbtrc_env_ctrl_blk_p) != RVF_GREEN)
    {
       /* The environemnt will cancel the EXPL instance creation. */
       RVM_TRACE_DEBUG_HIGH ("USBTRC: Error to get memory ");
       return RVM_MEMORY_ERR;	
    }

    /* Store the address id */
    usbtrc_env_ctrl_blk_p->addr_id = handle;

    /* Store the pointer to the error function */
    usbtrc_env_ctrl_blk_p->error_ft = rvm_error; //call_back_error_ft;
    
    /* Store the mem bank id. */
    usbtrc_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;


  
    usbtrc_init();
//    usbtrc_start();

    return (PEI_OK);
}


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
               "USBTRC",      /* ENTITY Name */
               {              /* pei Table */
                 pei_init,    /* pei_init */
                 pei_exit,    /* pei_exit */
                 NULL,        /* pei_primitive */
                 NULL,        /* pei_timeout */
                 NULL,        /* pei_signal */
                 pei_run,     /*--ACTIVE ENTITY--*/  
                 NULL,        /* pei_config */
                 NULL         /* pei_monitor */
               },
               (USBTRC_STACK_SIZE),     /* stack size */
               10,                      /* queue entries */
               (255 - USBTRC_TASK_PRIORITY),    /* priority (1->low, 255->high) */
               0,                               /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND//| PASSIVE_BODY /* FLAGS */
              };

  RVM_TRACE_DEBUG_HIGH("USBTRC: pei_create");
  
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
| Function    : usbtrc_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbtrc_init(void)
{
	USBTRC_SEND_TRACE("USBTRC: usbtrc_init called by RVM", RV_TRACE_LEVEL_DEBUG_LOW);

	/*
	 * Here the instance (usbtrc_env_ctrl_blk_p) could be initialised.
	 */
	usbtrc_env_ctrl_blk_p->usbtrc_return_path.addr_id = usbtrc_env_ctrl_blk_p->addr_id;
	usbtrc_env_ctrl_blk_p->usbtrc_return_path.callback_func = NULL;
	usbtrc_env_ctrl_blk_p->callback_function = USBT_callback_function;

	return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : usbtrc_start
+------------------------------------------------------------------------------
| Description : Called to start the USBTRC SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbtrc_start(void)
{
	T_RVM_RETURN	suscribe_result = RVM_OK;
	
	USBTRC_SEND_TRACE("USBTRC: usbtrc_start called by RVM", RV_TRACE_LEVEL_DEBUG_LOW);

	/* init USB config */
	suscribe_result = usbtrc_create_usb_device();

	if(suscribe_result != RVM_OK)
	{
		USBTRC_SEND_TRACE("USBTRC: usbtrc_start cannot create usb context", RV_TRACE_LEVEL_DEBUG_LOW);
		return  RVM_INTERNAL_ERR;
	}

	/* send suscribe request to USB LLD */
	/* usbtrc_suscribe_usb(); */
	suscribe_result = usbtrc_suscribe_to_cdc_interfaces();

	if(suscribe_result != RVM_OK)
	{
		USBTRC_SEND_TRACE("USBTRC: usbtrc_start error in subscribe", RV_TRACE_LEVEL_DEBUG_LOW);
		return  RVM_INTERNAL_ERR;
	}

	
	/* then enter INIT state to trap the suscribe response */
	/*usbtrc_change_internal_state(USBTRC_INIT_STATE);*/
	usbtrc_change_internal_state(USBTRC_DISCONNECTED_STATE);

	return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : usbtrc_stop
+------------------------------------------------------------------------------
| Description : Called to stop the USBTRC SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbtrc_stop (T_RV_HDR * msg)
{
	/*
	 * Here we should stop the activities of the SWE
	 * It is still possible to send messages to other SWE, to unregister for example.
	 */
	USBTRC_SEND_TRACE("USBTRC: usbtrc_stop called by RVM", RV_TRACE_LEVEL_DEBUG_LOW);

	/*
	 * After everything has been stopped, we inform RVM that the SWE is ready to be
	 * killed. Next step is RVM calling usbtrc_kill().
	 */
//	rvm_swe_stopped(msg);

	/* release usb ressources */
	usbtrc_destroy_usb_device();

	return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : usbtrc_kill
+------------------------------------------------------------------------------
| Description : Called to stop the USBTRC SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN usbtrc_kill (void)
{
	/*
	 * Here we cannot send messages anymore. We only free the last
	 * used resources, like the control block buffer.
	 */
	USBTRC_SEND_TRACE("USBTRC: usbtrc_kill called by RVM", RV_TRACE_LEVEL_DEBUG_LOW);

	rvf_free_buf(usbtrc_env_ctrl_blk_p);

	return RVM_OK;
}

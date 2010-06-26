/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Module  :  TTY
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
|             for the entity TTY
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

#include "tty/tty_i.h"
#include "tty/tty_env.h"

/*============================ Macros ========================================*/
#define VSI_CALLER TTY_handle,
#define pei_create tty_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

#define TTY_VERSION	 1 

/*============================ Global Variables ==============================*/
T_HANDLE TTY_handle;
T_HANDLE tty_hCommTTY = -1;

extern T_RV_RET tty_core(void);
T_TTY_ENV_CTRL_BLK *tty_env_ctrl_blk_p = NULL;


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
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  RVM_TRACE_DEBUG_HIGH ("TTY: pei_monitor");

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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("TTY: pei_config");

  return PEI_OK;

}/* End of pei_config(..) */


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
  RVM_TRACE_DEBUG_HIGH ("TTY: pei_timeout");

  return PEI_OK;

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
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  RVM_TRACE_DEBUG_HIGH ("TTY: pei_signal");
  
  return PEI_OK;

}/* End of pei_signal(..) */


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
   RVM_TRACE_DEBUG_HIGH ("TTY: pei_exit");

   /*
    * Close communication channels
    */
	
   vsi_c_close (VSI_CALLER tty_hCommTTY);
   tty_hCommTTY = VSI_ERROR;

   tty_stop();
   tty_kill();

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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_primitive (void * primptr)
{
   T_RV_HDR* prim_p;
    
   RVM_TRACE_DEBUG_HIGH("TTY: pei_primitive");

   prim_p = (T_RV_HDR*) P2D(primptr);

   // tty_handle_message(prim_p);
   
   return PEI_OK;
   
}/* End of pei_primitive(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
| Parameters  :  taskhandle  - handle of current process
|                comhandle   - queue handle of current process
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle )
{
   RVM_TRACE_DEBUG_HIGH("TTY: pei_run");
   tty_core();
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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/	
LOCAL SHORT pei_init(T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;

    RVM_TRACE_DEBUG_HIGH("TTY: Initialization");
    
    /*
     * Initialize task handle
     */
    TTY_handle = handle;
    tty_hCommTTY = -1;

     if(TTY_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("TTY_handle NE gsp_get_taskid");
    }
     
    if( gsp_init_remu_task( TTY_handle, "TTY" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }     

     /*
     * Open communication channels
     */
    if (tty_hCommTTY < VSI_OK)
    {
      if ((tty_hCommTTY = vsi_c_open (VSI_CALLER "TTY" )) < VSI_OK)
        return PEI_ERROR;
    } 

    RVM_TRACE_DEBUG_HIGH("TTY Open communication channels done"); 

    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_TTY_ENV_CTRL_BLK), (T_RVF_BUFFER**)&tty_env_ctrl_blk_p) != RVF_OK)
    {
        /* The environemnt will cancel the EXPL instance creation. */
        RVM_TRACE_DEBUG_HIGH ("TTY: Error to get memory ");
        return RVM_MEMORY_ERR;	
    }

    if(tty_env_ctrl_blk_p == NULL) 
    {
      return PEI_ERROR;
    }

    /* Store the initial State; default is IDLE. */
    tty_env_ctrl_blk_p->state = TTY_IDLE;

    /* Store the address ID. */
    tty_env_ctrl_blk_p->addr_id = handle;

	/* Store the pointer to the error function. */
	tty_env_ctrl_blk_p->error_ft = rvm_error;

	tty_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;

    tty_init();

   return PEI_OK;
} /* END pei_init */


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
GLOBAL SHORT tty_pei_create (T_PEI_INFO const **info)
{

  static const T_PEI_INFO pei_info = 
                {
    		"TTY",/* mks task name*/
    		{
      		pei_init,
      		pei_exit,
      		NULL,  /* pei_primitive */
      		NULL,  /* pei_timeout */
      		NULL,  /* pei_signal */            
      		pei_run,             
      		NULL,  /* pei_config */
      		NULL,  /* pei_monitor */
    		},
    		TTY_STACK_SIZE, 
    		10,		                /* Queue entries  */
		(255-RVM_TTY_TASK_PRIORITY),	/* Priority - 1->low, 255-> high. */
		0,		                /* Number of timers*/
    		COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND // PASSIVE_BODY  /* Flags */
  	};

   RVM_TRACE_DEBUG_HIGH("TTY: pei_create");

  /*
   *  Export startup configuration data
   */

  *info = (T_PEI_INFO *)&pei_info;

  return PEI_OK;
} /* END pei_create */


/*
+------------------------------------------------------------------------------
| Function    : tty_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN tty_init (void)
{
   /*
    * Here the instance (tty_env_ctrl_blk_p) could be initialised.
    */
    TTY_SEND_TRACE("TTY: init", RV_TRACE_LEVEL_DEBUG_LOW);
    return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : tty_stop
+------------------------------------------------------------------------------
| Description : Called to stop the TTY SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN tty_stop (void)
{
	/*
	 * Here we should stop the activities of the SWE
	 * It is still possible to send messages to other SWE, to unregister for example.
	 */
	TTY_SEND_TRACE("TTY: stop called", RV_TRACE_LEVEL_DEBUG_LOW);

	return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : tty_kill
+------------------------------------------------------------------------------
| Description : Called to stop the TTY SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN tty_kill (void)
{
	/*
	 * Here we cannot send messages anymore. We only free the last
	 * used resources, like the control block buffer.
	 */
	TTY_SEND_TRACE("TTY: kill called", RV_TRACE_LEVEL_DEBUG_LOW);

	rvf_free_buf(tty_env_ctrl_blk_p);

	return RVM_OK;
}

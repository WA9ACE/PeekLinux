/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Module  :  ETM
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
|             for the entity ETM
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

#include "etm/etm_env.h"

#include "../TEMPLATE/PEI/mon_xxx.h"
#include "../sm_remu/inc/remu_task.h"

/*============================ Macros ========================================*/
#define VSI_CALLER ETM_handle, 
#define pei_create etm_pei_create

#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )


/*============================ Global Variables ==============================*/
static T_MONITOR  etm_monitor;
static BOOL first_access = TRUE;

char ETM_VERSION[]={"1"};

T_HANDLE ETM_handle;
T_HANDLE etm_hCommETM = -1;

/*
 * Pointer on the structure gathering all the global variables
 * used by ETM instance.
 */
T_ETM_ENV_CTRL_BLK *etm_env_ctrl_blk = NULL;

/*============================ Functions =====================================*/
extern T_RV_RET etm_task(void);


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
  RVM_TRACE_DEBUG_HIGH ("ETM: pei_monitor");

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
| Parameters   :  in_string   - configuration string
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("ETM: pei_config");

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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_timeout (unsigned short index)
{
  RVM_TRACE_DEBUG_HIGH ("ETM: pei_timeout");

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
  RVM_TRACE_DEBUG_HIGH ("ETM: pei_signal");
  
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
LOCAL SHORT pei_exit(void)
{
    RVM_TRACE_DEBUG_HIGH("ETM: pei_exit");	

    etm_stop();
    
    etm_kill();
    
    return PEI_OK;
}/* End of pei_exit(..) */

/*
+------------------------------------------------------------------------------
|  Function     :  pei_primitive
+------------------------------------------------------------------------------
|  Description  :  Process protocol specific primitive.
|
|  Parameters   :  prim      - pointer to the received primitive
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_primitive (void * primptr)
{
    RVM_TRACE_DEBUG_HIGH ("ETM: pei_primitive");

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
|  Parameters   :  taskhandle  - handle of current process
|                  comhandle   - queue handle of current process
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{
    RVM_TRACE_DEBUG_HIGH ("ETM: pei_signal");

    etm_task();

    return PEI_OK;
}/* End of pei_run(..) */

/*
+------------------------------------------------------------------------------
| Function    : pei_init
+------------------------------------------------------------------------------
| Description : This function is the first one to be called soon after the 
|               calling of pei_create.
|
|  Parameters   :  taskhandle  - handle of current process
|                  comhandle   - queue handle of current process
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/	
LOCAL SHORT pei_init(T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;
    
    RVM_TRACE_DEBUG_HIGH("ETM: pei_init");
    /*
     * Initialize task handle
     */
    ETM_handle = handle;
    etm_hCommETM = -1;

    if(ETM_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("ETM_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( ETM_handle, "ETM" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }
    /*
     * Open communication channels
     */
    if (etm_hCommETM < VSI_OK)
    {
      if ((etm_hCommETM = vsi_c_open (VSI_CALLER "ETM" )) < VSI_OK)
        return PEI_ERROR;
    }	

    /* Create instance gathering all the variable used by EXPL instance */
    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_ETM_ENV_CTRL_BLK), (T_RVF_BUFFER**)&etm_env_ctrl_blk) != RVF_GREEN)
    {
	/* The environemnt will cancel the ETM instance creation. */
	return RVM_MEMORY_ERR;
    }

    /* Store the addr id */
    etm_env_ctrl_blk->addr_id = handle;
    
    /* Store the pointer to the error function */
     etm_env_ctrl_blk->error_ft =rvm_error;

     /* Store the mem bank id */
     etm_env_ctrl_blk->prim_id = EXT_MEM_POOL;

     etm_init();

     return PEI_OK;
}/* End of pei_run(..) */

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
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO const **info)
{

	static const T_PEI_INFO pei_info = 
	{
    		"ETM",/* Audio task name*/
    		{
      		pei_init,
		pei_exit,
		NULL,		/* pei_primitive */
                NULL,	        /* pei_timeout */
		NULL, 		/* pei_signal */            
                pei_run,        /*-- Active Entity--*/
      		NULL,           /* pei_config */
      		NULL,           /* pei_monitor */
    		},
    		ETM_STACK_SIZE, /* stack size */
    		10,		/* queue entries */
		(255-RVM_ETM_TASK_PRIORITY),	/* Priority: 1=>low, 255=>high */
		0,		/* Number of timers */
    		COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND  /* Flags Settings */
  	};

  	RVM_TRACE_DEBUG_HIGH ("ETM  pei_create()");

  /*
   * Close Resources if open
   */
  if (first_access)
    first_access = FALSE;
  else
    pei_exit();

  /*
   *  Export startup configuration data
   */

  *info = (T_PEI_INFO *)&pei_info;

  return PEI_OK;  
}/* End of pei_create(..) */

/*
+------------------------------------------------------------------------------
|  Function     :  etm_init
+------------------------------------------------------------------------------
|  Description  :  Called initialize the etm SWE before creating the task and 
|                  calling etm_start. 
|
|  Parameters   :  None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN etm_init(void)
{
    RVM_TRACE_DEBUG_HIGH ("ETM: etm_init");
    return RVM_OK;
}/* End of etm_init(..) */

/*
+------------------------------------------------------------------------------
|  Function     :  etm_start
+------------------------------------------------------------------------------
|  Description  :  Called to start the etm SWE, it is the body of the task.
|
|  Parameters   :  None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN etm_start(void)
{
    RVM_TRACE_DEBUG_HIGH ("ETM: etm_start");
    
    etm_task();
    
    return RV_OK;
}/* End of etm_start(..) */

/*
+------------------------------------------------------------------------------
|  Function     :  etm_stop
+------------------------------------------------------------------------------
|  Description  :  Called to stop the etm SWE.
|
|  Parameters   :  None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN etm_stop(void)
{
    RVM_TRACE_DEBUG_HIGH ("ETM: etm_stop");
    
    return RVM_OK;
}/* End of etm_stop(..) */

/*
+------------------------------------------------------------------------------
|  Function     :  etm_kill
+------------------------------------------------------------------------------
|  Description  :  Called kill the etm SWE, after the etm_stop function has been called.
|
|  Parameters   :  None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN etm_kill (void)
{
    RVM_TRACE_DEBUG_HIGH ("ETM: etm_kill");
    
    rvf_free_buf(etm_env_ctrl_blk);
    
    return RVM_OK;
}/* End of etm_kill(..) */



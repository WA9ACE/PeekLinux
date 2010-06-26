/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Module  :  MKS
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
|             for the entity MKS
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

#include "mks/mks_env.h"
#include "mks/mks_i.h"

#include "rvm/rvm_ext_priorities.h"
#include "rvm/rvm_use_id_list.h"

/*============================ Macros ========================================*/
#define VSI_CALLER MKS_handle, 
#define pei_create mks_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
char MKS_VERSION[]={"1"};

T_HANDLE MKS_handle;
T_HANDLE mks_hComm = -1;

T_MKS_ENV_CTRL_BLK* mks_env_ctrl_blk = NULL;

static BOOL first_access = TRUE;
/*============================ Extern Functions ==============================*/
extern T_RV_RET mks_core(void);


/*============================ Functions =====================================*/
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
  RVM_TRACE_DEBUG_HIGH ("MKS: pei_monitor");

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
  RVM_TRACE_DEBUG_HIGH ("MKS: pei_config");

  return PEI_OK;

} /* End of pei_config(..) */


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
LOCAL SHORT pei_timeout (int h1,int h2,USHORT tnum)
{
  TRACE_FUNCTION ("pei_timeout");
  rvf_send_event(mks_env_ctrl_blk->addr_id ,(UINT16)(EVENT_MASK(((tnum % RVF_NUM_TASK_TIMERS)+4)) ) );
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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  RVM_TRACE_DEBUG_HIGH ("MKS: pei_signal");
  
  return PEI_OK;

} /* End of pei_signal(..) */

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
    RVM_TRACE_DEBUG_HIGH ("MKS: pei_exit");

//    vsi_c_close (VSI_CALLER mks_hCommMKS);
//    mks_hCommMKS = VSI_ERROR;
 
    mks_stop();
    mks_kill();
 
    return PEI_OK;
}
   
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
   T_RV_HDR* msg_p;

   RVM_TRACE_DEBUG_HIGH("MKS: pei_primitive");

   msg_p = (T_RV_HDR*) P2D(primptr);
   
   mks_handle_msg(msg_p);

   return PEI_OK;
   
} /* End of pei_primitive(..) */

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
   RVM_TRACE_DEBUG_HIGH("MKS: pei_run");
   rvf_create_timer(mks_env_ctrl_blk->addr_id,0,TRUE, pei_timeout );
   mks_core();

   return PEI_OK;
} /* End of pei_run(..) */


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
LOCAL SHORT pei_init (T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;
    
    RVM_TRACE_DEBUG_HIGH("MKS: Initialization");
    /*
     * Initialize task handle
     */
    MKS_handle = handle;
    mks_hComm = -1;

    if(MKS_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("MKS_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( MKS_handle, "MKS" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }
    /*
     * Open communication channels
     */
    if (mks_hComm < VSI_OK)
    {
      if ((mks_hComm = vsi_c_open (VSI_CALLER "MKS" )) < VSI_OK)
        return PEI_ERROR;
    } 

    /* Create instance gathering all the variable used by EXPL instance */
    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_MKS_ENV_CTRL_BLK),
                   (T_RVF_BUFFER**)&mks_env_ctrl_blk) != RVF_GREEN)
    {
        /* The environemnt will cancel the EXPL instance creation. */
        RVM_TRACE_DEBUG_HIGH ("MKS: Error to get memory ");
	
        return RVM_MEMORY_ERR;	
    }
    
    /* Store the address id */
    mks_env_ctrl_blk->addr_id = MKS_handle;
    
    /* Store the pointer to the error function */
    mks_env_ctrl_blk->error_ft = rvm_error;
    
    /* Store the mem bank id. */
    mks_env_ctrl_blk->prim_id = EXT_MEM_POOL;
    
    mks_init();
    
    RVM_TRACE_DEBUG_HIGH("MKS: pei_init Complete");
	
    return PEI_OK;
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
	"MKS",/* mks task name*/
	{
	   pei_init,
           pei_exit,
           NULL,     /* pei_primitive */
           NULL,     /* pei_timeout */
	   NULL,     /* pei_signal */
           pei_run,  /*-- Active Entity--*/
       	   NULL,     /* pei_config */
      	   NULL,     /* pei_monitor */
    	},
    	MKS_STACK_SIZE,   //TBD: We need to check the size and correct it. This is an Initial Value
    	10,		/*Queue entries  */
	(255-255),	/*Priority - 1->low, 255-> high. */
	1,		/*Number of timers*/
        COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND /* Flags Settings */
     };

  RVM_TRACE_DEBUG_HIGH("MKS: pei_create");

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
} /* End of pei_create(..) */

/*
+------------------------------------------------------------------------------
| Function    : mks_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN mks_init(void)
{
   RVM_TRACE_DEBUG_HIGH("MKS: mks_init");
   
   return RV_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : mks_stop
+------------------------------------------------------------------------------
| Description : Called to stop the MKS SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN mks_stop(void)
{
   RVM_TRACE_DEBUG_HIGH("MKS: mks_stop");
   
   mks_stop_swe();

   return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : mks_kill
+------------------------------------------------------------------------------
| Description : Called to stop the MKS SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN mks_kill (void)
{
   RVM_TRACE_DEBUG_HIGH("MKS: mks_kill");
   
   rvf_free_buf(mks_env_ctrl_blk);

   return RVM_OK;
}

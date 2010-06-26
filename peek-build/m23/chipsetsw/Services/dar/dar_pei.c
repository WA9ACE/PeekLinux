/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Module  :  DAR
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
|             for the entity DAR
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

#include "rv/rv_defined_swe.h"

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

#include "rvm/rvm_gen.h"
#include "rv/rv_general.h"
#include "rvm/rvm_priorities.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"

#include "dar/dar_api.h"
#include "dar/dar_env.h"
#include "dar/dar_macro_i.h"

#include "../TEMPLATE/PEI/mon_xxx.h"
#include "../sm_remu/inc/remu_task.h"

/*============================ Macros ========================================*/
#define VSI_CALLER DAR_handle, 
#define pei_create dar_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
char DAR_VERSION[]={"1"};

T_HANDLE DAR_handle;
T_HANDLE dar_hCommDAR = -1;

static BOOL first_access = TRUE;

T_DAR_ENV_CTRL_BLK *dar_gbl_var_p = NULL;

/*============================ Extern Variables ==============================*/
extern UINT8 dar_exception_status;
extern UINT8 dar_previous_exception;

extern T_DAR_RECOVERY_STATUS dar_current_status;
extern T_DAR_RECOVERY_STATUS dar_previous_status;

/*============================ Extern Functions ==============================*/
extern T_RV_RET dar_core(void);

/*============================ Function Definition============================*/
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
   RVM_TRACE_DEBUG_HIGH ("DAR: pei_exit");
 
   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER dar_hCommDAR);
   dar_hCommDAR = VSI_ERROR;
 
   dar_stop();

   dar_kill();
 
   return PEI_OK;
}/* End of pei_exit(..) */

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
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle )
{
    RVM_TRACE_DEBUG_HIGH("DAR: pei_run");
    
    dar_core();
    
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
LOCAL SHORT pei_init (T_HANDLE handle)
{
    RVM_TRACE_DEBUG_HIGH("DAR: pei_init");
    /*
     * Initialize task handle
     */
    DAR_handle = handle;
    dar_hCommDAR = -1;

    if(DAR_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("DAR_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( DAR_handle, "DAR" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (dar_hCommDAR < VSI_OK)
    {
      if ((dar_hCommDAR = vsi_c_open (VSI_CALLER "DAR" )) < VSI_OK)
        return PEI_ERROR;
    }
   
    RVM_TRACE_DEBUG_HIGH("DAR Open communication channels done"); 
       
    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_DAR_ENV_CTRL_BLK), 
			    (T_RVF_BUFFER**)&dar_gbl_var_p) != RVF_GREEN)
	{
            /* The environemnt will cancel the EXPL instance creation. */
            RVM_TRACE_DEBUG_HIGH ("DAR: Error to get memory ");
            return RVM_MEMORY_ERR;	
	}

    memset((UINT8 *) dar_gbl_var_p, 0x00, sizeof (T_DAR_ENV_CTRL_BLK));  

    /* Store the memory bank IDs assigned to the DAR */
    dar_gbl_var_p->mb_dar = EXT_MEM_POOL;

    dar_gbl_var_p->addrId = DAR_handle;

    dar_gbl_var_p->callBackFctError = rvm_error; 
    
    DAR_SEND_TRACE("DAR's (Diagnose And Recovery) information set ",
                           RV_TRACE_LEVEL_DEBUG_LOW);
    
    dar_init(); 

    return PEI_OK;
    
}/* End of pei_init(..) */


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
    	    "DAR",/* Audio task name*/
    	    {
      	      pei_init,
      	      pei_exit,
      	      NULL,	   /* pei_primitive */
      	      NULL,        /* pei_timeout */
	      NULL, 	   /* pei_signal */
      	      pei_run,     /*-- Active Entity--*/        
              NULL,        /* pei_config */
              NULL         /* pei_monitor */
	    },
    	    DAR_STACK_SIZE,   /* stack size */   
	    10,		   /* Queue entries */
	    (255-RVM_DAR_TASK_PRIORITY), /* Priority - 1->low, 255-> high */
	    0,		  /*Number of timers*/
            COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND  /* Flags Settings */
  	};

  	TRACE_FUNCTION ("DAR  pei_create()");

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
|  Function     :  dar_init
+------------------------------------------------------------------------------
|  Description  :  Called initialize the dar SWE before creating the task and 
|                  calling dar_start. 
|
|  Parameters   :  None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN dar_init(void)
{
   /* Declare local variables */
   UINT16 i = 0;    

   RVM_TRACE_DEBUG_HIGH ("DAR: dar_start");   
   /* Initialization of the DAR global structure */

   /* Initialization of the dar_filter_array */ 
   for (i=0;i<DAR_MAX_GROUP_NB;i++)
   {
      dar_gbl_var_p ->dar_filter_array[i].group_nb     = DAR_INITIALIZATION_VALUE;
      dar_gbl_var_p ->dar_filter_array[i].mask_warning = DAR_INITIALIZATION_VALUE;
      dar_gbl_var_p ->dar_filter_array[i].mask_debug   = DAR_INITIALIZATION_VALUE;
   }

   /*Initialization of the index and the free index */
   dar_gbl_var_p ->index       = DAR_INVALID_VALUE;
   dar_gbl_var_p ->free_index  = DAR_INVALID_VALUE;

   /* Value of the DAR recovery status */
   dar_previous_status = dar_current_status;

   /* erase the dar_current_status value */
   dar_current_status = DAR_POWER_ON_OFF;          

   /* Value of the DAR exception status */
   dar_previous_exception = dar_exception_status;           

   /* erase the dar_exception_status value */
   dar_exception_status = DAR_NO_ABORT_EXCEPTION;          

   return RV_OK;
}/* End of dar_init(..) */


/*
+------------------------------------------------------------------------------
|  Function     :  dar_stop
+------------------------------------------------------------------------------
|  Description  :  Called to stop the dar SWE.
|
|  Parameters   :  None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN dar_stop(void)
{
   RVM_TRACE_DEBUG_HIGH ("DAR: dar_stop");

   return RV_OK;
}/* End of dar_stop(..) */


/*
+------------------------------------------------------------------------------
|  Function     :  dar_kill
+------------------------------------------------------------------------------
|  Description  :  Called kill the dar SWE, after the dar_stop function has been called.
|
|  Parameters   :  None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN dar_kill (void)
{
   RVM_TRACE_DEBUG_HIGH ("DAR: dar_kill");
   
   rvf_free_buf ((T_RVF_BUFFER *) dar_gbl_var_p);

   return RV_OK;
}/* End of dar_kill(..) */

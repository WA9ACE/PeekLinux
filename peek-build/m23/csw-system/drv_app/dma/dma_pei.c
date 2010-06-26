/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Module  :  DMA
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
|             for the entity DMA
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

#include "inth/sys_inth.h"
#include "dma/dma_cfg.h"
#include "dma/dma_i.h"
#include "dma/dma_env.h"
#include "dma/sys_dma.h"

/*============================ Macros ========================================*/
#define VSI_CALLER DMA_handle, 
#define pei_create dma_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_HANDLE DMA_handle;
T_HANDLE dma_hCommDMA = -1;
static BOOL first_access  = TRUE;

T_DMA_ENV_CTRL_BLK* dma_env_ctrl_blk_p = NULL;

/*============================ Extern Variables ==============================*/
extern T_DMA_CHANNEL_ARRAY dma_channel_array []; 
extern T_DMA_QUEUE_ARRAY   dma_queue_array   [];
extern UINT8 dma_function_status [];

/*============================ Extern Functions ==============================*/
#ifndef HISR_STACK_SHARING
extern char dma_hisr_stack[DMA_HISR_STACK_SIZE];
extern char dma_secure_hisr_stack [DMA_SECURE_HISR_STACK_SIZE];
#endif

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
  RVM_TRACE_DEBUG_HIGH ("DMA: pei_monitor");

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
  RVM_TRACE_DEBUG_HIGH ("DMA: pei_config");

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
  RVM_TRACE_DEBUG_HIGH ("DMA: pei_timeout");

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
  RVM_TRACE_DEBUG_HIGH ("DMA: pei_signal");
  
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
   RVM_TRACE_DEBUG_HIGH ("DMA: pei_exit");

   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER dma_hCommDMA);
   dma_hCommDMA = VSI_ERROR;

   dma_stop(NULL);
   dma_kill(); 
   
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
   T_RV_HDR* msg_p ; 

   RVM_TRACE_DEBUG_HIGH("DMA: pei_primitive");

   msg_p = (T_RV_HDR*) P2D(primptr);
   dma_handle_message(msg_p);

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

   RVM_TRACE_DEBUG_HIGH("DMA: pei_run");

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */	   
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         dma_handle_message(msg_p);
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
#ifndef _WINDOWS  
  T_DMA_TYPE_GLOBAL_PARAMETER  dma_type_global_parameter;
#endif //_WINDOWS

    T_RV_RET ret = RV_OK;
       
    RVM_TRACE_DEBUG_HIGH("DMA: pei_init");
    /*
     * Initialize task handle
     */
    DMA_handle = handle;
    dma_hCommDMA = -1;

    if(DMA_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("DMA_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( DMA_handle, "DMA" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }
    /*
     * Open communication channels
     */
    if (dma_hCommDMA < VSI_OK)
    {
      if ((dma_hCommDMA = vsi_c_open (VSI_CALLER "DMA" )) < VSI_OK)
        return PEI_ERROR;
    }

    RVM_TRACE_DEBUG_HIGH("DMA Open communication channels done"); 
    
    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_DMA_ENV_CTRL_BLK), (T_RVF_BUFFER**)&dma_env_ctrl_blk_p) != RVF_GREEN)
    {
            /* The environemnt will cancel the EXPL instance creation. */
        RVM_TRACE_DEBUG_HIGH ("DMA: Error to get memory ");
	
        return RVM_MEMORY_ERR;
	
    }

    /* Store the address id */
    dma_env_ctrl_blk_p->addr_id = handle;

    /* Store the pointer to the error function */
    dma_env_ctrl_blk_p->error_ft = rvm_error;
    
    /* Store the mem bank id. */
    dma_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;    
 
    dma_init ();
    dma_start();
    
    RVM_TRACE_DEBUG_HIGH("DMA: pei_init Complete"); 
    
    return (PEI_OK);
   
} /* End pei_init */


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
               "DMA",         
               {              
                 pei_init,      
                 pei_exit,      
                 NULL,         /* pei_primitive */ 
                 NULL,         /* pei_timeout */
                 NULL,         /* pei_signal */
                 pei_run,
                 NULL,         /* pei_config */
                 NULL          /* pei_monitor */
               },
               DMA_STACK_SIZE, /* stack size */
               10,             /* queue entries */
               (255 - DMA_TASK_PRIORITY),  /* priority (1->low, 255->high) */
               0,        /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND // || PASSIVE_BODY /* Flags Settings */
              };

        RVM_TRACE_DEBUG_HIGH("DMA: pei_create");
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
}

/*
+------------------------------------------------------------------------------
| Function    : dma_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN dma_init (void)
{
#ifndef _WINDOWS  
  T_DMA_TYPE_GLOBAL_PARAMETER  dma_type_global_parameter;
#endif //_WINDOWS

  UINT8 n;

	/*
	 * Here the instance (dma_env_ctrl_blk_p) could be initialised.
	 */

  DMA_SEND_TRACE ("DMA init ENTER", DMA_TRACE_LEVEL);

  if (dma_env_ctrl_blk_p == NULL)
  {
    DMA_SEND_TRACE("DMA Initialization is not yet done or failed", DMA_TRACE_LEVEL);
  }

#ifndef _WINDOWS
  /* Fill the entire stack with the pattern 0xFE */
#ifndef HISR_STACK_SHARING
  memset (dma_env_ctrl_blk_p->dma_hisr_stack, 0xFE, DMA_HISR_STACK_SIZE);
  memset (dma_env_ctrl_blk_p->dma_secure_hisr_stack, 0xFE, 
          DMA_SECURE_HISR_STACK_SIZE);
  #else
  	memset (HISR_STACK_PRIO2, 0xFE, HISR_STACK_PRIO2_SIZE);
  #endif
  
  if (NU_SUCCESS !=
      NU_Create_HISR (&dma_env_ctrl_blk_p->dma_hisr, "DMA_HISR", dma_hisr, 2,
      	#ifndef HISR_STACK_SHARING
                  dma_env_ctrl_blk_p->dma_hisr_stack, DMA_HISR_STACK_SIZE))
       #else
		  HISR_STACK_PRIO2,
	 	  HISR_STACK_PRIO2_SIZE))
    	#endif
  {
    DMA_SEND_TRACE ("DMA init HISR creation FAILED", DMA_TRACE_LEVEL);
  }

  if (NU_SUCCESS !=
      NU_Create_HISR (&dma_env_ctrl_blk_p->dma_secure_hisr, "DMA_SECURE_HISR", 
                      dma_secure_hisr, 2, 
	#ifndef HISR_STACK_SHARING
  			dma_env_ctrl_blk_p->dma_secure_hisr_stack, 
                      DMA_SECURE_HISR_STACK_SIZE))
       #else
		      HISR_STACK_PRIO2,
	 	      HISR_STACK_PRIO2_SIZE))
    	#endif
  {
    DMA_SEND_TRACE ("DMA init secure HISR creation FAILED", 
                     DMA_TRACE_LEVEL);
  }

#endif

  /* At start up all the channels are free */
  for (n=0; n<DMA_MAX_QUEUED_REQUESTS; n++)
  {
    dma_queue_array[n].specific    = DMA_QUEUE_POS_FREE;
    dma_queue_array[n].queue_index = DMA_QUEUE_POS_FREE;
    dma_queue_array[n].sw_priority = DMA_QUEUE_POS_FREE;
    dma_function_status[n]         = DMA_FUNCTION_NONE;
  }

  for (n=0; n<DMA_MAX_NUMBER_OF_CHANNEL; n++)
  {
    dma_channel_array[n].specific = DMA_QUEUE_POS_FREE;
    dma_channel_array[n].double_buf_mode = DMA_NO_DOUBLE_BUF;
  }

  dma_env_ctrl_blk_p->dma_queue_index = DMA_QUEUE_INDEX_MIN;

#ifndef _WINDOWS

  /* Configuration items */

#ifdef DMA_EMULATOR_SUSPEND_ENABLE 
    /* enable free mode of dma */
    F_DMA_SUSPEND_MODE_EMULATION_FREE_ENABLE;    
#endif
#ifdef DMA_EMULATOR_SUSPEND_DISABLE
    /*disable free mode of the dma */
    F_DMA_SUSPEND_MODE_EMULATION_FREE_DISABLE;
#endif


#ifdef DMA_POWERSAVE_ENABLE
  dma_type_global_parameter.d_dma_global_auto_gate = C_DMA_AUTO_GATE_ON;
#endif
#ifdef DMA_POWERSAVE_DISABLE
  dma_type_global_parameter.d_dma_global_auto_gate = C_DMA_AUTO_GATE_OFF;
#endif


#ifdef DMA_DSP_PRIO_DMA
dma_type_global_parameter.d_dma_global_api_prio    = C_DMA_API_PRIO_DMA;
#endif
#ifdef DMA_DSP_PRIO_ARM
  dma_type_global_parameter.d_dma_global_api_prio  = C_DMA_API_PRIO_ARM;
#endif


#ifdef DMA_RHEA_PRIO_DMA
  dma_type_global_parameter.d_dma_global_rhea_prio = C_DMA_RHEA_PRIO_DMA;
#endif
#ifdef DMA_RHEA_PRIO_ARM
  dma_type_global_parameter.d_dma_global_rhea_prio = C_DMA_RHEA_PRIO_ARM;
#endif


#ifdef DMA_IMIF_PRIO_CPU_0
  dma_type_global_parameter.d_dma_global_imif_prio = C_DMA_IMIF_PRIO_CPU_0;
#endif
#ifdef DMA_IMIF_PRIO_CPU_1
  dma_type_global_parameter.d_dma_global_imif_prio = C_DMA_IMIF_PRIO_CPU_1;
#endif
#ifdef DMA_IMIF_PRIO_CPU_2
  dma_type_global_parameter.d_dma_global_imif_prio = C_DMA_IMIF_PRIO_CPU_2;
#endif
#ifdef DMA_IMIF_PRIO_CPU_3
  dma_type_global_parameter.d_dma_global_imif_prio = C_DMA_IMIF_PRIO_CPU_3;
#endif
#ifdef DMA_IMIF_PRIO_CPU_4
  dma_type_global_parameter.d_dma_global_imif_prio = C_DMA_IMIF_PRIO_CPU_4;
#endif
#ifdef DMA_IMIF_PRIO_CPU_5
  dma_type_global_parameter.d_dma_global_imif_prio = C_DMA_IMIF_PRIO_CPU_5;
#endif
#ifdef DMA_IMIF_PRIO_CPU_6
  dma_type_global_parameter.d_dma_global_imif_prio = C_DMA_IMIF_PRIO_CPU_6;
#endif
#ifdef DMA_IMIF_PRIO_CPU_7
  dma_type_global_parameter.d_dma_global_imif_prio = C_DMA_IMIF_PRIO_CPU_7;
#endif

#ifdef DMA_AUTOGATING_ON 
  dma_type_global_parameter.d_dma_global_auto_gate = C_DMA_AUTO_GATE_ON;
#endif

#ifdef DMA_AUTOGATING_OFF 
  dma_type_global_parameter.d_dma_global_auto_gate = C_DMA_AUTO_GATE_OFF;
#endif

  /* Make the settings actual */
/*  f_dma_global_parameter_set (&dma_type_global_parameter);

  for (n=DMA_ZERO; n<DMA_MAX_NUMBER_OF_CHANNEL; n++)
  {
    f_dma_channel_allocation_set (n,C_DMA_CHANNEL_ARM);
  } */ // LSC test due to L1 crash

#endif //_WINDOWS

  DMA_SEND_TRACE ("DMA init LEAVE", DMA_TRACE_LEVEL);
  return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : dma_start
+------------------------------------------------------------------------------
| Description : Called to start the DMA SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN dma_start(void)
{
DMA_SEND_TRACE ("DMA_start ENTER", DMA_TRACE_LEVEL);

#ifndef _WINDOWS
  F_INTH_ENABLE_ONE_IT(C_INTH_DMA_IT)
  F_INTH_ENABLE_ONE_IT(C_INTH_2ND_INTH_IT)
  F_INTH_ENABLE_ONE_IT(C_INTH_SEC_DMA_IT)
#endif

DMA_SEND_TRACE ("DMA_start EXIT", DMA_TRACE_LEVEL);
  return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : dma_stop
+------------------------------------------------------------------------------
| Description : Called to stop the DMA SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN dma_stop (T_RV_HDR* hdr)
{
	/*
	 * Here we should stop the activities of the SWE
	 * It is still possible to send messages to other SWE, to unregister for example.
	 */
	DMA_SEND_TRACE("DMA stop called", DMA_TRACE_LEVEL);
#ifndef _WINDOWS
  if (NU_SUCCESS != NU_Delete_HISR (&dma_env_ctrl_blk_p->dma_hisr))
    {
      DMA_SEND_TRACE ("DMA stop HISR deletion FAILED", DMA_TRACE_LEVEL);
    }
  else
    {
      DMA_SEND_TRACE ("DMA stop HISR deleted", DMA_TRACE_LEVEL);
    }

  if (NU_SUCCESS != NU_Delete_HISR (&dma_env_ctrl_blk_p->dma_secure_hisr))
    {
      DMA_SEND_TRACE ("DMA stop secure HISR deletion FAILED", DMA_TRACE_LEVEL);
    }
  else
    {
      DMA_SEND_TRACE ("DMA stop secure HISR deleted", DMA_TRACE_LEVEL);
    }

#endif
  DMA_SEND_TRACE ("DMA stop LEAVE", DMA_TRACE_LEVEL);
 
	return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : dma_kill
+------------------------------------------------------------------------------
| Description : Called to stop the DMA SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN dma_kill (void)
{
	/*
	 * Here we cannot send messages anymore. We only free the last
	 * used resources, like the control block buffer.
	 */
	DMA_SEND_TRACE("DMA kill called", DMA_TRACE_LEVEL);

	rvf_free_buf(dma_env_ctrl_blk_p);

	return RVM_OK;
}



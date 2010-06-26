/*
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
|             for the entity DL
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
|+--------------------------------------------------------------------------- */
#define GBI_DL_PEI_C

#define ENTITY_GBI_DL

/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "nucleus.h"
#include "datalight_pei.h"
#include "ccdapi.h"
#include "tok.h"
#include "rvm/rvm_gen.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"
#include "datalight/datalight.h"
#include "datalight/datalight_api.h"
#include "datalight/datalight_pool_size.h"
#include "datalight/datalight_interface.h"
//#include "nucleus.h"
#include "gbi/gbi_pi_datalight_i.h"
#include "swconfig.cfg"

#if ((DATALIGHT_NAND == 1) || (DATALIGHT_NAND == 2))
#include "nan/board/nan_hw_functions.h"
#endif


#define pei_create datalight_pei_create

/*==== GLOBAL VARS ==========================================================*/

//extern T_GBI_RETURN dl_handle_msg(T_RV_HDR*  msg_p);

T_HANDLE                 dl_comm_handle = VSI_ERROR;
T_HANDLE                 dl_task_handle = VSI_ERROR;

/**
 * Pointer on the structure gathering all the global variables
 * used by DL instance.
 */
 T_DL_ENV_CTRL_BLK *dl_env_ctrl_blk_p = NULL;


T_RVF_MUTEX datalight_gbi_sync_mutex;

/*==== LOCAL VARS ===========================================================*/

//LOCAL SHORT pei_signal (ULONG opc, void *data);

static  BOOL          first_access  = TRUE;

/*==== PUBLIC FUNCTIONS =====================================================*/


/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - exit sucessful
|               PEI_ERROR         - exit not sueccessful
|
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_exit (void)
{
   
   DATALIGHT_TRACE_DEBUG_HIGH ("FFX: pei_exit");
   /*
    * Close communication channels
    */
   vsi_c_close (dl_task_handle,dl_comm_handle);
   dl_comm_handle = VSI_ERROR;
   rvf_free_buf(dl_env_ctrl_blk_p);
   return PEI_OK;
}


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

   DATALIGHT_TRACE_DEBUG_HIGH("FFX: pei_run");

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */	   
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         dl_handle_message(msg_p);
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
    
  	UINT32 mem_size;
  	unsigned long  uStatus; 

    DATALIGHT_TRACE_DEBUG_HIGH("Datalight: pei_init");
	
    /* Initialize task handle */
    dl_task_handle = handle;
	
	if(dl_task_handle != gsp_get_taskid())
	{
			DATALIGHT_TRACE_DEBUG_ERROR("dl_task_handle NE gsp_get_taskid");
	}
	if( gsp_init_remu_task( dl_task_handle, "DATALIGHT" ) != RVF_OK)
    {
         DATALIGHT_TRACE_DEBUG_ERROR("gsp_init_remu_task Not returning RVF_OK");
    }
     
    /* Open communication channels */
   if (dl_comm_handle < VSI_OK)
   {
      	if ((dl_comm_handle = vsi_c_open (handle, "DATALIGHT" )) < VSI_OK)
      	{
      		return PEI_ERROR;
      	}
   }
	DATALIGHT_TRACE_DEBUG_HIGH("Datalight Open communication channels done");
 

	if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_DL_ENV_CTRL_BLK), (T_RVF_BUFFER**)&dl_env_ctrl_blk_p) != RVF_OK)
	{
			DATALIGHT_TRACE_DEBUG_ERROR ("Datalight TASK: Error to get memory ");
			return PEI_ERROR;
	}
	

	/* Store the address ID. */
	dl_env_ctrl_blk_p->addr_id = dl_comm_handle;

	/* Store the Task handle. */
	dl_env_ctrl_blk_p->task_id = dl_task_handle;

	 /* Store the mem bank id.*/
	dl_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;
	 

	#if 0
     /* TODO , Disk Initializations */
     FlashFx_Disks_Init();
    #endif

	 dl_env_ctrl_blk_p->initialised = TRUE;

	 rvf_unlock_mutex(&datalight_gbi_sync_mutex);	 

	 return PEI_OK;
  
 }

/*
+------------------------------------------------------------------------------
| Function    : pei_create
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when the process is
|                created.
|
| Parameters  : out_name         - Pointer to the buffer in which to locate
|                                   the name of this entity
|
| Return      : PEI_OK            - entity created successfuly
|               PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{

	static T_PEI_INFO pei_info =
	{
    	"DATALIGHT",         /* name */
        {              /* pei-table */
            pei_init,
	    pei_exit, 
            NULL,  
            NULL,
	    NULL,
            pei_run,              /*pei_run*/
            NULL,     
	    NULL
	},
        DL_STACK_SIZE,            /* stack size */
        20,        		  /* queue entries */
        (255-DL_TASK_PRIORITY),   /* priority (1->low, 255->high) */
        0,        		  /* number of timers */
        COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND/* | PASSIVE_BODY */
    };
  
   
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

#if ((DATALIGHT_NAND == 1) || (DATALIGHT_NAND == 2))
  #if (CHIPSET == 15)
  	initialize_nand_mutex();
  #endif
#endif  


 rvf_initialize_static_mutex(&datalight_gbi_sync_mutex);
 rvf_lock_mutex(&datalight_gbi_sync_mutex);
  
  return PEI_OK;
}

/*==== END OF FILE ==========================================================*/



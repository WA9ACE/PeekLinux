/*
+----------------------------------------------------------------------------
|  Project : Reliance Datalight
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
|             for the entity reliance
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
#define REL_PEI_C

#define ENTITY_RIL


/*==== INCLUDES =============================================================*/
#include "nucleus.h"
#include "relfs_env.h"
//#include "typedefs.h"   /* to get standard types */
#include "vsi.h"        /* to get a lot of macros */
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */
#include "relfs_pei.h"    /* to get the global entity definitions */
#include <stdio.h>
#include <string.h>
#include "relfs.h"
//#include "relfs_env.h"
//#include "nucleus.h"
#include "rvf/rvf_api.h"  /* to use mutex functions*/


#define RELFS_TASK_PRIORITY  247


#if DL_GC_TIMER

#define DL_GC_TIMER_VAL     	(5000)     /* Seconds*/
#define DL_GC_INIT_TIMER_VAL 	(1000*60*2)  /* 2 minutes */
extern void FfxIdleCompactHook(void);

static T_TIME  gc_remain_time;


#endif

/* to synchronize relfs and rfs tasks */

T_RVF_MUTEX relfs_rfs_sync_mutex;

#define TRUE 1
#define FALSE 0
/*==== LOCAL VARS ===========================================================*/

static  BOOL          first_access  = TRUE;
static T_RIL_ENV_CTRL_BLK ril_env_ctrl_blk;

/*==== GLOBAL VARS ==========================================================*/

extern short ril_handle_msg(T_PRIM_HEADER*  msg_p);
T_HANDLE                 ril_comm_handle = VSI_ERROR;
T_HANDLE                 ril_task_handle = VSI_ERROR;

/**
 * Pointer on the structure gathering all the global variables
 * used by DL instance.
 */
 T_RIL_ENV_CTRL_BLK *ril_env_ctrl_blk_p = &ril_env_ctrl_blk;

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : pei_config
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a primitive is
|               received indicating dynamic configuration.
|
|
| Parameters  : handle            - Communication handle
|
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_config (char *in_string)
{
  TRACE_FUNCTION ("pei_config");
  TRACE_FUNCTION (in_string);

  return PEI_OK;
}

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

   TRACE_FUNCTION ("pei_exit");
   /*
    * Close communication channels
    */
   vsi_c_close (ril_task_handle,ril_comm_handle);
   ril_comm_handle = VSI_ERROR;

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
| Return      : PEI_OK    - function succeeded
|               PEI_ERROR - function failed
|
+------------------------------------------------------------------------------
*/


LOCAL SHORT pei_primitive (void * primptr)
{
   T_PRIM_HEADER* prim = (T_PRIM_HEADER*)primptr;
   S32 error;
   unsigned long opc = prim->opc;

   TRACE_FUNCTION ("pei_primitive");

   if (opc == Rel_req) {
			error = ril_handle_message((T_Rel_req*) P2D(primptr));
   }
   else if (opc & SYS_MASK)
    {
		/* forward this primitives to the environment */
		vsi_c_primitive(VSI_CALLER, prim);
		error = PEI_OK;
	}

	return (short) error;
}


/*
+-----------------------------------------------------------------------------
| Function    : pei_timeout
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a timer has expired.
|
| Parameters  : index             - timer index
|
| Return      : PEI_OK            - timeout processed
|               PEI_ERROR         - timeout not processed
|
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_timeout (USHORT index)
{
   TRACE_FUNCTION ("pei_timeout");

#if DL_GC_TIMER

   /* call the garbage collector */
   FfxIdleCompactHook();


   /* start the timer again */
   vsi_t_start(ril_task_handle, 0, DL_GC_TIMER_VAL);

 #endif

   return PEI_OK;

}

#ifndef HISR_STACK_SHARING
LOCAL UINT8		     REL_Hisr_Stack[256*3];
#endif


NU_HISR REL_HISR;

UINT32 GC_TICK_COUNTER=DL_GC_TIMER_VAL;
UINT8 GC_Timer_Enabled=TRUE;

static void REL_Activate_Hisr(void)
{

 NU_Activate_HISR(&REL_HISR);




}

extern void ffs_Garbage_Collection(void);
void REL_GC_timeout_hisr(void)
{

/*Post a Message to FFS
 * */
 
 ffs_Garbage_Collection();
//OMAPS00178992 RFS read/write performance improvement, 
/* Use reduced interval for 2nd invocation onwards, first period is larger to skip boot-up */
 GC_TICK_COUNTER=DL_GC_TIMER_VAL / 10;
 GC_Timer_Enabled=TRUE;
}
void GC_Tick(void)
{


  if((--GC_TICK_COUNTER)==0&& (GC_Timer_Enabled==TRUE))
  {
  
    REL_Activate_Hisr(); 
    GC_Timer_Enabled=FALSE; 
  }
}



REL_Create_Hisr()   
{

  NU_Create_HISR(&REL_HISR,"REL",
               REL_GC_timeout_hisr,2/*Priority*/,
#ifndef HISR_STACK_SHARING			
               REL_Hisr_Stack,sizeof(REL_Hisr_Stack));
#else
       HISR_STACK_PRIO2,HISR_STACK_PRIO2_SIZE);
#endif
}

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
#include "drv.h"
LOCAL SHORT pei_init (T_HANDLE handle)
{

  UINT32 mem_size;
  int32	iStatus;


	ril_task_handle = handle;
    TRACE_FUNCTION ("pei_init");
    /* Initialize task handle */

	if(ril_task_handle!= gsp_get_taskid())
    {
         RELFS_TRACE_DEBUG_HIGH("FFS_handle != gsp_get_taskid");
    }

    if( gsp_init_remu_task( ril_task_handle, "RIL_FFS" ) != RVF_OK)
    {
         RELFS_TRACE_DEBUG_HIGH("gsp_init_remu_task != returning RVF_OK");
    }
    /* Open communication channels */
   if (ril_comm_handle < VSI_OK)
   {
      	if ((ril_comm_handle = vsi_c_open (handle, RIL_NAME )) < VSI_OK)
      	{
      		return PEI_ERROR;
      	}
   }

   //init low level erase/read/write APi's
   ffsdrv.init();

   iStatus = etm_ffs_init();
   // We can only mkdir("pcm") *after* our mailbox has been allocated
   // otherwise mkdir("pcm") will fail.
  iStatus = ffs_mkdir("/pcm");

   /* Store the address ID. */
   ril_env_ctrl_blk_p->addr_id = ril_comm_handle;
   ril_env_ctrl_blk_p->mbid= EXT_MEM_POOL;
   /* Store the Task handle. */
   ril_env_ctrl_blk_p->task_id = ril_task_handle;
   ril_env_ctrl_blk_p->rfs_stream_return_path.addr_id = 0;
   ril_env_ctrl_blk_p->rfs_stream_return_path.callback_func = relfs_rfs_stream_clbk_func;
   ril_env_ctrl_blk_p->rfs_file_return_path.addr_id = 0;
   ril_env_ctrl_blk_p->rfs_file_return_path.callback_func = relfs_rfs_file_clbk_func;
   ril_env_ctrl_blk_p->Garbage_Collection_Process = FALSE;
REL_Create_Hisr();
	/*------------relfs task is up : release relfs_rfs_sync_mutex---------------*/
            rvf_unlock_mutex(&relfs_rfs_sync_mutex);

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
    	RIL_NAME,         /* name */
        {              /* pei-table */
            pei_init,
	    	pei_exit,
            pei_primitive,
            pei_timeout,
	    NULL,
            NULL,     /*pei_run*/
            pei_config,
	    NULL
	},
        (8072),     /* stack size, including garbage collector execution */
        30,        		            /* queue entries */
        (255 - RELFS_TASK_PRIORITY),      			        /* priority (1->low, 255->high) */
        1,        			        /* number of timers */
         COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND   /* flags: bit 0   active(0) body/passive(1) */
    };               /*        bit 1   com by copy(0)/reference(1) */



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
  *info = &pei_info;

  /*---------- To synchronize relfs and rfs tasks ------------*/
  rvf_initialize_static_mutex(&relfs_rfs_sync_mutex);

  /*------------------ To synchronize relfs and rfs tasks ---------------*/
 	rvf_lock_mutex(&relfs_rfs_sync_mutex);


  return PEI_OK;
}

/*==== END OF FILE ==========================================================*/


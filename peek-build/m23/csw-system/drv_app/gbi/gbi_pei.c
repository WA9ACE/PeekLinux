/*
+-----------------------------------------------------------------------------
|  Project :  Riv2Gpf
|  Module  :  GBI
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
|             for the entity GBI
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

#include "gbi/gbi_i.h"
#include "gbi/gbi_cfg.h"
#include "gbi/gbi_env.h"
#include "gbi/gbi_mmc.h"
#include "mc/mc_i.h"
#include "nan/nan_i.h"
#include "../../gpf/sm_remu/inc/remu_task.h"
#include "rv_swe.h"


/*============================ Macros ========================================*/
#define VSI_CALLER GBI_handle,
#define pei_create gbi_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_HANDLE GBI_handle;
T_HANDLE gbi_hCommGBI = -1;

LOCAL BOOL first_access = TRUE;

T_GBI_ENV_CTRL_BLK *gbi_env_ctrl_blk_p = NULL;

unsigned long gbi_start_time;
#ifdef RVM_NAN_SWE
extern  T_RVF_MUTEX nan_gbi_sync_mutex;
#endif
#ifdef RVM_NOR_BM_SWE
extern  T_RVF_MUTEX nor_bm_gbi_sync_mutex;
#endif

#ifdef RVM_DATALIGHT_SWE
extern  T_RVF_MUTEX datalight_gbi_sync_mutex;
#endif



/*============================ Generic Functions =============================*/
T_RVM_RETURN gbi_core (void);
extern T_RVM_RETURN gbi_handle_timer (T_RV_HDR * msg_p);
extern T_RVM_RETURN gbi_handle_message (T_RV_HDR *msg_p);



/*============================ Function Definition============================*/

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
  rvf_send_event(gbi_env_ctrl_blk_p->addr_id,(UINT16)(EVENT_MASK(((tnum % RVF_NUM_TASK_TIMERS)+4)) ) );
  return PEI_OK;

}/* End pei_timeout(..) */


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
   RVM_TRACE_DEBUG_HIGH ("GBI: pei_exit");

   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER gbi_hCommGBI);
   gbi_hCommGBI = VSI_ERROR;

   gbi_stop(0);

   gbi_kill();

   return PEI_OK;
}/* End pei_exit(..) */


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
   RVM_TRACE_DEBUG_HIGH("GBI: pei_run");
   rvf_create_timer(gbi_env_ctrl_blk_p->addr_id,0,TRUE, pei_timeout );
   gbi_core();
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

	#ifdef RVM_DATALIGHT_SWE
	T_HANDLE datalight_hCommGBI = -1;	
	#endif

	#ifdef RVM_MC_SWE
		T_HANDLE mc_plugin_hCommMC = -1;	
	#endif
	
	
     RVM_TRACE_DEBUG_HIGH("GBI: pei_init");

    /*
     * Initialize task handle
     */
    GBI_handle = handle;	
    gbi_hCommGBI = -1;
	

#ifdef RVM_MC_SWE
  if (mc_plugin_hCommMC < VSI_OK)
    {
      if ((mc_plugin_hCommMC = vsi_c_open (VSI_CALLER "MC" )) < VSI_OK)
        return PEI_ERROR;
    }	
#endif  


#ifdef RVM_NAN_SWE
	/*Waiting for NAN_BM initialisation*/
    rvf_lock_mutex(&nan_gbi_sync_mutex);
#endif

#ifdef RVM_NOR_BM_SWE

	/*Waiting for NOR_BM initialisation*/
    rvf_lock_mutex(&nor_bm_gbi_sync_mutex);
 #endif


 #ifdef RVM_DATALIGHT_SWE

 	if (datalight_hCommGBI < VSI_OK)
    {
      if ((datalight_hCommGBI = vsi_c_open (VSI_CALLER "DATALIGHT" )) < VSI_OK)
        return PEI_ERROR;

    }
	/*Waiting for Datalight initialisation*/
    rvf_lock_mutex(&datalight_gbi_sync_mutex);

 #endif

    if (gbi_hCommGBI < VSI_OK)
    {
      if ((gbi_hCommGBI = vsi_c_open (VSI_CALLER "GBI" )) < VSI_OK)
        return PEI_ERROR;

    }

    if(GBI_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("GBI_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( GBI_handle, "GBI" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    RVM_TRACE_DEBUG_HIGH("GBI: Open communication channels done");

     /* Create instance gathering all the variable used by GBI instance */

    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_GBI_ENV_CTRL_BLK), (T_RVF_BUFFER**)&gbi_env_ctrl_blk_p) != RVF_GREEN )
    {
       /* The environemnt will cancel the EXPL instance creation. */
       GBI_SEND_TRACE ("GBI: Error to get memory ", RV_TRACE_LEVEL_ERROR);
       return RVM_MEMORY_ERR;

    }

    /* Store the address ID. */
    gbi_env_ctrl_blk_p->addr_id = handle ;

    /* Store the pointer to the error function. */
    gbi_env_ctrl_blk_p->error_ft = rvm_error;

    gbi_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;

    gbi_env_ctrl_blk_p->state = GBI_UNINITIALISED;

    gbi_init ();

    gbi_start();

    RVM_TRACE_DEBUG_HIGH("GBI: pei_init Complete");

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
               "GBI",         /* name */
               {              /* pei-table */
                  pei_init,
	          pei_exit,
                  NULL,         /* pei_primitive */
                  NULL,         /* pei_timeout */
                  NULL,         /* pei_signal */
                  pei_run,      /*-- ACTIVE Entity--*/
                  NULL,         /* pei_config */
                  NULL          /* pei_monitor */
	       },
               GBI_STACK_SIZE,            /* stack size */
               10,                        /* queue entries */
               (255-GBI_TASK_PRIORITY),     /* priority (1->low, 255->high) */
               1,                         /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND /* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("GBI: pei_create");

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
| Function    : gbi_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN gbi_init (void)
{
  UINT8 i, j;

  /*
   * Here the instance (gbi_env_ctrl_blk_p) could be initialised.
   */

  GBI_SEND_TRACE("GBI init: initialising gbi_env_ctrl_blk_p", RV_TRACE_LEVEL_DEBUG_LOW);

  /* Initialise subscribe events array */
  for (i = 0; i < GBI_MAX_NR_OF_EVENTS; i++)
  {
    for (j = 0; j < GBI_MAX_EVENT_SUBSCRIBERS; j++)
    {
      gbi_env_ctrl_blk_p->sub_events[i][j].ret_path.callback_func = NULL;
	    gbi_env_ctrl_blk_p->sub_events[i][j].ret_path.addr_id       = 0;
    }
  }

	/* Initialise plugin table */
  for (i = 0; i < GBI_NMB_PLUGINS; i++)
  {
    gbi_env_ctrl_blk_p->plugin_table[i].msg_id_in_progress = GBI_NO_MESSAGE_IN_PROGRESS;
    gbi_env_ctrl_blk_p->plugin_table[i].plugin_info_sync   = FALSE;
    gbi_env_ctrl_blk_p->plugin_table[i].plugin_state	   = GBI_STATE_READY;

    for (j = 0; j < GBI_MAX_NR_OF_MEDIA; j++)
    {
	gbi_env_ctrl_blk_p->plugin_table[i].media_data_p[j] = NULL;
    }

    for (j = 0; j < GBI_MAX_NR_OF_PARTITIONS; j++)
    {
      gbi_env_ctrl_blk_p->plugin_table[i].partition_data_p[j] = NULL;
    }

    gbi_env_ctrl_blk_p->plugin_table[i].nmb_media_p = NULL;
    gbi_env_ctrl_blk_p->plugin_table[i].nmb_partition_p	= NULL;
  }


  gbi_env_ctrl_blk_p->plugin_nmb_in_progress = GBI_NO_PLUGIN_IN_PROGRESS;
  gbi_env_ctrl_blk_p->multiple_msg_in_progress = GBI_NO_MESSAGE_IN_PROGRESS;
  gbi_env_ctrl_blk_p->multiple_msg_ret_path.addr_id = 0;
  gbi_env_ctrl_blk_p->multiple_msg_ret_path.callback_func = NULL;
  gbi_env_ctrl_blk_p->multiple_msg_mb_id = GBI_NO_MB_ID;

  /** initialised! */
  gbi_env_ctrl_blk_p->state = GBI_INITIALISED;

  return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : gbi_start
+------------------------------------------------------------------------------
| Description : Called to start the GBI SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN gbi_start(void)
{
  T_RVM_RETURN retval = RVM_OK;
  T_GBI_I_STARTUP_EVENT_REQ_MSG	*msg_p;
  T_GBI_PLUGIN_FNCTBL const *func_ptr;
  T_GBI_RESULT result = GBI_OK;
  UINT8 plugin_idx;

  GBI_SEND_TRACE("GBI start: initialising host controller", RV_TRACE_LEVEL_DEBUG_LOW);

  gbi_env_ctrl_blk_p->state = GBI_IDLE;
  gbi_start_time = rvf_get_tick_count ();

  /* For all plugins, perform start function for pre-initialisation */
  for (plugin_idx = 0; plugin_idx < GBI_NMB_PLUGINS; plugin_idx++)
  {
    func_ptr = gbi_plugin_fnctbl[plugin_idx];
    result   = func_ptr->gbi_plugin_start();
    if (result != GBI_OK)
    {
      return RVM_INTERNAL_ERR;
    }
  }

  /* Send message to its own mailbox, to perform startup operation */
  /* reserve message buffer */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_I_STARTUP_EVENT_REQ_MSG),
                       GBI_I_STARTUP_EVENT_REQ_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /** Insufficient RAM resources to process the request */
    GBI_SEND_TRACE ("GBI API: out of memory", RV_TRACE_LEVEL_WARNING);

    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id = GBI_I_STARTUP_EVENT_REQ_MSG;

  /* Send message mailbox */
  retval = rvf_send_msg (gbi_env_ctrl_blk_p->addr_id, msg_p);

  return retval;
}

/*
+------------------------------------------------------------------------------
| Function    : gbi_stop
+------------------------------------------------------------------------------
| Description : Called to stop the GBI SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN gbi_stop (T_RV_HDR *msg)
{
  T_RVM_RETURN retval = RVM_OK;
  T_GBI_PLUGIN_FNCTBL const *func_ptr;
  T_GBI_RESULT result = GBI_OK;
  UINT8 plugin_idx;

  /* For all plugins, perform start function for pre-initialisation */
  for (plugin_idx = 0; plugin_idx < GBI_NMB_PLUGINS; plugin_idx++)
  {
    func_ptr = gbi_plugin_fnctbl[plugin_idx];
    result   = func_ptr->gbi_plugin_stop();

    if (result != GBI_OK)
    {
      return RVM_INTERNAL_ERR;
    }
  }

  /*
   * Here we should stop the activities of the SWE
   * It is still possible to send messages to other SWE, to unregister for example.
   */

  GBI_SEND_TRACE("GBI: stop called", RV_TRACE_LEVEL_DEBUG_LOW);

  gbi_env_ctrl_blk_p->state = GBI_STOPPED;

  return retval;
}

/*
+------------------------------------------------------------------------------
| Function    : gbi_kill
+------------------------------------------------------------------------------
| Description : Called to stop the GBI SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN gbi_kill (void)
{
  T_RVM_RETURN retval;

  /*
   * Here we cannot send messages anymore. We only free the last
   * used resources, like the control block buffer.
   */

  GBI_SEND_TRACE("GBI: kill called", RV_TRACE_LEVEL_DEBUG_LOW);

  retval = (T_RVM_RETURN) rvf_free_buf(gbi_env_ctrl_blk_p);
  if (retval != RV_OK)
  {
    return retval;
  }

  gbi_env_ctrl_blk_p->state = GBI_KILLED;

  return retval;
}

/*
+------------------------------------------------------------------------------
| Function    : gbi_core
+------------------------------------------------------------------------------
| Description : Called to stop the NAN SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN gbi_core (void)
{
	T_RVM_RETURN error_status;
	T_RV_HDR	*msg_p ;
	UINT16		rec_event;

	GBI_SEND_TRACE("GBI_CORE TACHE DE DEV", RV_TRACE_LEVEL_DEBUG_HIGH);

	error_status = GBI_OK;
	while (error_status == GBI_OK)
	{
		rec_event = rvf_wait(0xffff, 0); /* Wait (infinite) for all events. */
		if (rec_event & RVF_TASK_MBOX_0_EVT_MASK)
		{
			msg_p = (T_RV_HDR *)rvf_read_mbox(0);
			error_status = gbi_handle_message(msg_p);
		}
		if  ((rec_event & RVF_TIMER_0_EVT_MASK) || (rec_event & RVF_TIMER_1_EVT_MASK))
//		if  ((rec_event & RVF_TIMER_1_EVT_MASK))
		{
			error_status = gbi_handle_timer(NULL);
		}
	}
	if (error_status == GBI_MEMORY_ERR) /* If a memory error happened .. */
	{
		error_status = gbi_env_ctrl_blk_p->error_ft("GBI", RVM_MEMORY_ERR, 0, "Memory Error : the GBI primitive memory bank is RED ");
	}

	return RVM_OK;
}


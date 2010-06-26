/*
+-----------------------------------------------------------------------------
|  Project :  Riv2Gpf
|  Module  :  RFS
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
|             for the entity RFS
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

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "rfs/rfs_i.h"
#include "rfs/rfs_env.h"
#include "rfs/rfs_fm.h"

/*============================ Macros ========================================*/
#define VSI_CALLER RFS_handle,
#define pei_create rfs_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
T_RFS_ENV_CTRL_BLK *rfs_env_ctrl_blk_p = NULL;

T_HANDLE RFS_handle;
T_HANDLE rfs_hCommRFS = -1;

LOCAL BOOL first_access = TRUE;

/*~~~~~~~~~~~~~~~~~  For tasks synchronisation - mutex definitions  ~~~~~~~~~~~~~~~~~~~~~~~~*/

extern T_RVF_MUTEX relfs_rfs_sync_mutex;

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
   RVM_TRACE_DEBUG_HIGH ("RFS: pei_exit");

   /*
    * Close communication channels
    */

   rfs_stop(0);

   rfs_kill();

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
| Parameters  :  taskhandle  - handle of current process
|                comhandle   - queue handle of current process
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{
//	T_RVM_RETURN status;
   RVM_TRACE_DEBUG_HIGH("RFS: pei_run");

        //  status=rfs_core();
        rfs_core();

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
    RVM_TRACE_DEBUG_HIGH("RFS: Initialization");

    /*
     * Initialize task handle
     */
    RFS_handle = handle;
    rfs_hCommRFS = -1;

    if(RFS_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("RFS_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( RFS_handle, "RFS" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (rfs_hCommRFS < VSI_OK)
    {
      if ((rfs_hCommRFS = vsi_c_open (VSI_CALLER "RFS" )) < VSI_OK)
        return PEI_ERROR;
    }

    RVM_TRACE_DEBUG_HIGH("RFS Open communication channels done");

    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_RFS_ENV_CTRL_BLK), (T_RVF_BUFFER**)&rfs_env_ctrl_blk_p) != RVF_GREEN)
    {
       /* The environemnt will cancel the EXPL instance creation. */
       RFS_SEND_TRACE ("RFS: Error to get memory ", RV_TRACE_LEVEL_ERROR);
       return RVM_MEMORY_ERR;
    }

    memset(rfs_env_ctrl_blk_p,0,sizeof(T_RFS_ENV_CTRL_BLK));
    /* Store the address ID. */
    rfs_env_ctrl_blk_p->addr_id = RFS_handle;

    /* Store the pointer to the error function. */
    rfs_env_ctrl_blk_p->error_ft = rvm_error;

    rfs_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;

    rfs_env_ctrl_blk_p->state = RFS_UNINITIALISED;

    if(rfs_env_ctrl_blk_p == NULL)
    {
       RVM_TRACE_DEBUG_HIGH("rfs_env_ctrl_blk_p == NULL");

       return PEI_ERROR;
    }

#ifdef RVM_MC_SWE
	rvf_delay(RVF_SECS_TO_TICKS(1));
#endif

/*---------- To synchronize relfs and rfs tasks ------------*/  
		rvf_lock_mutex(&relfs_rfs_sync_mutex);

    rfs_init ();

    rfs_start();


/*------------release relfs_rfs_sync_mutex---------------*/	
    rvf_unlock_mutex(&relfs_rfs_sync_mutex);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~ delete mutex ~~~~~~~~~~~~~~~~~~~~~~~~~*/
       rvf_delete_mutex(&relfs_rfs_sync_mutex);


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
               "RFS",         /* name */
               {              /* pei-table */
                  pei_init,
	          pei_exit,
                  NULL,       /* pei_primitive */
                  NULL,       /* pei_timeout */
                  NULL,       /* pei_signal */
                  pei_run,
		  NULL,       /* pei_config */
                  NULL        /* pei_monitor */
	       },
               (RFS_STACK_SIZE ),            /* stack size */
               10,                        /* queue entries */
               (255-RFS_TASK_PRIORITY),     /* priority (1->low, 255->high) */
               0,                         /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND // | PASSIVE_BODY
	       		       /* Flags Settings */
              };

  RVM_TRACE_DEBUG_HIGH("RFS: pei_create");

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
| Function    : rfs_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rfs_init (void)
{
  UINT8   i;

  RFS_SEND_TRACE("RFS: rfs_init",RV_TRACE_LEVEL_DEBUG_MEDIUM);

  /* Initialize media and partition information */
  rfs_env_ctrl_blk_p->media_info_p        = NULL;
  rfs_env_ctrl_blk_p->nmb_of_media        = 0;
  rfs_env_ctrl_blk_p->nmb_of_fs = RFS_NMB_FSCORE;

  for (i = 0; i < RFS_NMB_FSCORE; i++)
  {
   rfs_env_ctrl_blk_p->fscore_part_table[i].partition_info_table= NULL;
  }

  rfs_env_ctrl_blk_p->open_file_cnt       = 0;

  rfs_env_ctrl_blk_p->open_dir_cnt        = 0;
  rfs_env_ctrl_blk_p->reserved_file_cnt   = 0;

  rfs_env_ctrl_blk_p->state = RFS_INITIALISED;

  /* Initialize mutex data for blocking and secure data handling */
  //OMAPS72906 change to static mutex
  rvf_initialize_static_mutex (&rfs_env_ctrl_blk_p->mutex_pair_id);
  /* Start value of mesage pair id. */
  rfs_env_ctrl_blk_p->pair_id = 0x22;
  rfs_map_pair_value_init();
  rfs_fd_list_init();

  return RVM_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : rfs_start
+------------------------------------------------------------------------------
| Description : Called to start the RFS SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rfs_start(void)
{
  T_RVM_RETURN retval = RVM_OK;
  T_RFS_I_STARTUP_MSG *msg_p;

  RFS_SEND_TRACE("RFS: rfs_start",RV_TRACE_LEVEL_DEBUG_MEDIUM);

  /* The Riviera manager shall call the rfs_start() after which the entity
   * enters the "idle" state. The function rfs_start() shall also send a
   * 'startup' message to its own mailbox. This message initiates the
   * entity to do a more complex start-up operations such as subscription
   * to GBI media change events.
   */

  rfs_env_ctrl_blk_p->state = RFS_IDLE;

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_RFS_I_STARTUP_MSG),
                       RFS_I_STARTUP_MSG,
                       (T_RV_HDR **) &msg_p) == RVF_RED)
  {
    /** Insufficient RAM resources to process the request */
    RFS_SEND_TRACE ("RFS API: out of memory", RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  msg_p->hdr.msg_id = RFS_I_STARTUP_MSG;

  /* Send message mailbox */
  retval = (T_RVM_RETURN) rvf_send_msg (rfs_env_ctrl_blk_p->addr_id, msg_p);

  return retval;

}

/*
+------------------------------------------------------------------------------
| Function    : rfs_stop
+------------------------------------------------------------------------------
| Description : Called to stop the RFS SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rfs_stop (T_RV_HDR *msg)
{
  /*
   * Here we should stop the activities of the SWE
   * It is still possible to send messages to other SWE, to unregister for example.
   */

  RFS_SEND_TRACE("RFS: stop called", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  rfs_env_ctrl_blk_p->state = RFS_STOPPED;

  return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : rfs_kill
+------------------------------------------------------------------------------
| Description : Called to stop the RFS SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rfs_kill (void)
{
  UINT8 i;

  /*
   * Here we cannot send messages anymore. We only free the last
   * used resources, like the control block buffer.
   */

  RFS_SEND_TRACE("RFS: kill called", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  rvf_delete_mutex (&rfs_env_ctrl_blk_p->mutex_pair_id);

  /* Clear used buffers and pointers */
  if (rfs_env_ctrl_blk_p->media_info_p != NULL)
  {
    rvf_free_buf(rfs_env_ctrl_blk_p->media_info_p);
    rfs_env_ctrl_blk_p->media_info_p = NULL;
    rfs_env_ctrl_blk_p->nmb_of_media = 0;
  }


  for (i = 0; i < RFS_NMB_FSCORE; i++)
  {
 
      /* Clear, perviously allocated partition info memory on memory bank */
      if (rfs_env_ctrl_blk_p->fscore_part_table[i].partition_info_table != NULL)
      {
        rvf_free_buf(rfs_env_ctrl_blk_p->fscore_part_table[i].partition_info_table);
        rfs_env_ctrl_blk_p->fscore_part_table[i].partition_info_table = NULL;
        rfs_env_ctrl_blk_p->fscore_part_table[i].nbr_partitions = 0;
      }
  }

  rfs_env_ctrl_blk_p->state = RFS_KILLED;

  rvf_free_buf(rfs_env_ctrl_blk_p);

  return RVM_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : rfs_core
+------------------------------------------------------------------------------
| Description : Core of the SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN rfs_core (void)
{
  T_RVM_RETURN error_status;
  T_RV_HDR *msg_p ;
  UINT16 rec_event;

  RFS_SEND_TRACE("RFS_CORE TACHE DE DEV", RV_TRACE_LEVEL_DEBUG_HIGH);

  error_status = RFS_OK;
  while (error_status == RFS_OK)
  {
    rec_event = rvf_wait(0xffff, 0); /* Wait (infinite) for all events. */
    if (rec_event & RVF_TASK_MBOX_0_EVT_MASK)
    {
      msg_p = rvf_read_mbox(0);
      error_status = rfs_handle_message(msg_p);
    }

    if (rec_event & RVF_TIMER_1_EVT_MASK)
    {
      error_status = rfs_handle_timer(NULL);
    }

  }

  if (error_status == RFS_MEMORY_ERR) /* If a memory error happened .. */
  {
    rfs_env_ctrl_blk_p->error_ft("RFS", RVM_MEMORY_ERR, 0,
						  " Memory Error : the RFS primitive memory bank is RED ");
  }

  return RVM_OK;
}

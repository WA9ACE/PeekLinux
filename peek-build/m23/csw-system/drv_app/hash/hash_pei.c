/*
+-----------------------------------------------------------------------------
|  Project :  Riv2Gpf
|  Modul   :  HASH
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
|             for the entity HASH
|
|             Exported functions:
|
|          pei_create    - Create the Protocol Stack Entity
|
|          pei_init      - Initialize Protocol Stack Entity
|          pei_exit      - Close resources and terminate
|          pei_primitive - Process Primitive
|          pei_timeout   - Process Timeout
|          pei_signal    - Process Signals
|          pei_run       - Process Messages
|          pei_config    - Dynamic Configuration
|          pei_monitor   - Monitoring of physical Parameters
|+-----------------------------------------------------------------------------
*/

/*==== INCLUDES =============================================================*/

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

#include "hash/hash_i.h"
#include "hash/hash_env.h"
#include "hash/hash_hw_functions.h"

/*==== DEFINITIONS ==========================================================*/

#define VSI_CALLER HASH_handle,
#define pei_create hash_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*==== VARS ==========================================================*/
/* global addr id */
T_HASH_ENV_CTRL_BLK *hash_env_ctrl_blk_p = NULL;

T_HANDLE HASH_handle;
T_HANDLE hash_hCommHASH = -1;

LOCAL BOOL first_access = TRUE;

/*===========================================================================*/

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
| Return      : PEI_OK            - successful (address in out_monitor is valid)
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  TRACE_FUNCTION ("pei_monitor");

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
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_config (char *inString)
{
  TRACE_FUNCTION ("pei_config");

  return PEI_OK;

}/* End pei_config(..) */


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
  T_RV_HDR* msg_p;
  TRACE_FUNCTION ("pei_timeout");

  if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_RV_HDR), (T_RVF_BUFFER**)&msg_p) != RVF_OK)
  {
        RVM_TRACE_DEBUG_HIGH ("HASH: Error to get memory ");
	 return RVM_MEMORY_ERR;
  }

  RVM_TRACE_DEBUG_HIGH("MC: pei_timeout");

  //g_p = (T_RV_HDR*) P2D(primptr);
  hash_handle_timer(msg_p);

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
| Return      : PEI_OK            - signal processed
|               PEI_ERROR         - signal not processed
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  TRACE_FUNCTION ("pei_signal");
  /*
   * Process signal
   */

  return(PEI_OK);
}/* End pei_signal(..) */


/*
+------------------------------------------------------------------------------
|  Function     :  pei_primitive
+------------------------------------------------------------------------------
|  Description  :  Process protocol specific primitive.
|
|  Parameters   :  prim      - pointer to the received primitive
|
|  Return       :  PEI_OK    - function succeeded
|                  PEI_ERROR - function failed
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_primitive (void * primptr)
{
//  T_RVM_RETURN error_status;
//  T_RV_HDR* msg_p;
  RVM_TRACE_DEBUG_HIGH("HASH: pei_primitive");

//   msg_p = (T_RV_HDR*) P2D(primptr);
//   error_status = hash_handle_message (msg_p);
//  if (error_status == RVM_MEMORY_ERR)
//  {
//     RVM_TRACE_DEBUG_HIGH("HASH: Error in handle_message");
//     return PEI_ERROR;
//  }

  return PEI_OK;

}/* End pei_primitive(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - exit successful
|               PEI_ERROR         - exit not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
   TRACE_FUNCTION ("pei_exit");

   /*
    * Close communication channels
    */

   vsi_c_close (VSI_CALLER hash_hCommHASH);
   hash_hCommHASH = VSI_ERROR;

   hash_stop(NULL);

   hash_kill();

   return PEI_OK;
}/* End pei_exit(..) */


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
    TRACE_FUNCTION ("pei_init");

    /*
     * Initialize task handle
     */
    HASH_handle = handle;
    hash_hCommHASH = -1;

    if(HASH_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("HASH_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( HASH_handle, "HASH" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (hash_hCommHASH < VSI_OK)
    {
      if ((hash_hCommHASH = vsi_c_open (VSI_CALLER "HASH" )) < VSI_OK)
        return PEI_ERROR;
    }

    RVM_TRACE_DEBUG_HIGH("HASH Open communication channels done");

    //--------------------------------------------------------------------------------
     /* Create instance gathering all the variable used by HASH instance */

	if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_HASH_ENV_CTRL_BLK), (T_RVF_BUFFER**)&hash_env_ctrl_blk_p) != RVF_OK)
	{
            /* The environemnt will cancel the EXPL instance creation. */
            RVM_TRACE_DEBUG_HIGH ("HASH: Error to get memory ");
            return RVM_MEMORY_ERR;
	}

        hash_env_ctrl_blk_p->initialised = FALSE;

	/* Store the address ID. */
	hash_env_ctrl_blk_p->addr_id = HASH_handle;

	/* Store the pointer to the error function. */
	hash_env_ctrl_blk_p->error_ft = rvm_error;

	hash_env_ctrl_blk_p->prim_mb_id = EXT_MEM_POOL;

    //--------------------------------------------------------------------------------

    if(hash_env_ctrl_blk_p->addr_id != NULL)
    {
      hash_init ();

      RVM_TRACE_DEBUG_HIGH("hash_Init() Done");

//     hash_start();

      RVM_TRACE_DEBUG_HIGH("hash_start() Done");
    }
    else
    {
       RVM_TRACE_DEBUG_HIGH("hash_env_ctrl_blk_p->addr_id == NULL");

       return PEI_ERROR;
    }

    return (PEI_OK);

} /* End pei_init(..) */


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

  hash_start();

   RVM_TRACE_DEBUG_HIGH("1. HASH pei_run");

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */

      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         hash_handle_message(msg_p);
      }
   }

  return PEI_OK;

}/* End pei_run(..) */


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
               "HASH",         /* name */
               {              /* pei-table */
                  pei_init,
	          pei_exit,
                  NULL, //pei_primitive,
                  pei_timeout,
                  NULL, //pei_signal,
                  pei_run,
                  pei_config,
                  NULL                  /* NO pei_monitor */
	       },
               HASH_STACK_SIZE,            /* stack size */
               10,                        /* queue entries */
               255-HASH_TASK_PRIORITY,     /* priority (1->low, 255->high) */
               0,                         /* number of timers */
               COPY_BY_REF|PRIM_NO_SUSPEND | TRC_NO_SUSPEND// |PASSIVE_BODY /* flags */
              };

  TRACE_FUNCTION ("pei_create");

  RVM_TRACE_DEBUG_HIGH("HASH: pei_create");

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

/*==== END OF FILE ==========================================================*/
T_RVM_RETURN hash_init (void)
{
  /*
   * Here the instance (hash_env_ctrl_blk_p) could be initialised.
   */

  if (HASH_CONTEXT_SIZE < sizeof (T_HASH_CLIENT_STATE))
  {
    HASH_SEND_TRACE ("SHA INIT HASH_CONTEXT_SIZE too small",
                      RV_TRACE_LEVEL_ERROR);
    return RVM_INTERNAL_ERR;
  }
  //OMAPS72906 change to static mutex
  if (rvf_initialize_static_mutex (&hash_env_ctrl_blk_p->mutex) != RVF_OK)
  {
    HASH_SEND_TRACE ("SHA INIT could not init mutex", RV_TRACE_LEVEL_ERROR);
    return RVM_INTERNAL_ERR;
  }
  hash_hw_init();
  hash_set_dma (HASH_FORCE_CPU); /* default CPU */
  hash_return_queue_init (hash_env_ctrl_blk_p->addr_id,
						 HASH_RETURN_QUEUE_EVENT,
						 &hash_env_ctrl_blk_p->path_to_return_queue);

  hash_env_ctrl_blk_p->initialised = TRUE;

  return RVM_OK;
}


T_RVM_RETURN hash_start (void)
{
  return RVM_OK;
}


T_RVM_RETURN hash_stop (T_RV_HDR * unused)
{
  /*
   * Here we should stop the activities of the SWE
   * It is still possible to send messages to other SWE, to unregister for example.
   */
  HASH_SEND_TRACE ("SHA: stop called", RV_TRACE_LEVEL_ERROR);


  return RVM_OK;
}

T_RVM_RETURN hash_kill (void)
{
  /*
   * Here we cannot send messages anymore. We only free the last
   * used resources, like the control block buffer.
   */
  HASH_SEND_TRACE ("HASH: kill called", RV_TRACE_LEVEL_DEBUG_LOW);

  if (rvf_delete_mutex (&hash_env_ctrl_blk_p->mutex) != RVF_OK)
    {
      HASH_SEND_TRACE ("HA INIT could not delete mutex",
                      RV_TRACE_LEVEL_DEBUG_LOW);
      return RVM_INTERNAL_ERR;
    }
  rvf_free_buf (hash_env_ctrl_blk_p);
  return RVM_OK;
}


/**
 * Core of the SWE.
 */
T_RVM_RETURN hash_core (void)
{
  T_RVM_RETURN error_status;
  T_RV_HDR *msg_p;
  UINT16 rec_event;

  HASH_SEND_TRACE ("HASH_CORE TACHE DE DEV", RV_TRACE_LEVEL_DEBUG_HIGH);

  error_status = RVM_OK;
  while (error_status == RVM_OK)
    {
      rec_event = rvf_wait (0xffff, 0); /* Wait (infinite) for all events. */
      if (rec_event & RVF_TASK_MBOX_0_EVT_MASK)
        {
          msg_p = rvf_read_mbox (0);
          error_status = hash_handle_message (msg_p);
        }
      if (rec_event & RVF_TIMER_1_EVT_MASK)
        {
          error_status = hash_handle_timer (NULL);
        }
    }
  if (error_status == RVM_MEMORY_ERR)   /* If a memory error happened .. */
    {
      hash_env_ctrl_blk_p->error_ft ("SHA", RVM_MEMORY_ERR, 0,
                                    " Memory Error : the SHA primitive memory bank is RED ");
    }
  return RVM_OK;
}

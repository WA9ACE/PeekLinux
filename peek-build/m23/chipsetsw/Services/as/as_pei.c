/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Module  :  AS
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
|             for the entity AS
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

#include "as/as_env.h"
#include "as/as_utils.h"
#include "as/as_i.h"
#include "as/as_pool_size.h"
#include "as/as_api.h"

#include "../TEMPLATE/PEI/mon_xxx.h"
#include "../sm_remu/inc/remu_task.h"

/*============================ Macros ========================================*/
#define VSI_CALLER AS_handle, 
#define pei_create as_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
//static T_MONITOR  as_monitor;//OMAPS00090550
char AS_VERSION[]={"1"};
T_HANDLE AS_handle;
T_HANDLE as_hCommAS = -1;
static BOOL first_access = TRUE;

T_AS_CTRL_BLK* as_ctrl_blk = NULL;

#if(AS_RFS_API == 1)
		T_WCHAR mp_uc1[40];
		T_WCHAR mp_uc2[40];
		T_WCHAR mp_uc3[40];
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
#if 0 // For Lint warnings removal
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  RVM_TRACE_DEBUG_HIGH ("AS: pei_monitor");

  return PEI_OK;

} /* End pei_monitor(..) */
#endif

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
#if 0 // For Lint warnings removal
LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("AS: pei_config");

  return PEI_OK;

}/* End of pei_config(..) */
#endif


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
#if 0 // For Lint warnings removal
LOCAL SHORT pei_timeout (unsigned short index)
{
  RVM_TRACE_DEBUG_HIGH ("AS: pei_timeout");

  return PEI_OK;

}/* End of pei_timeout(..) */
#endif

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
#if 0 // For Lint warnings removal
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  RVM_TRACE_DEBUG_HIGH ("AS: pei_signal");
  
  return PEI_OK;

}/* End of pei_signal(..) */
#endif


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
   RVM_TRACE_DEBUG_HIGH ("AS: pei_exit");

   /*
    * Close communication channels
    */
   vsi_c_close (VSI_CALLER as_hCommAS);
   as_hCommAS = VSI_ERROR;

   as_env_stop(NULL);
   
   as_env_kill();
  
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
#if 0 // For Lint warnings removal
LOCAL SHORT pei_primitive (void * primptr)
{
   T_RV_HDR* msg_p ; 

   RVM_TRACE_DEBUG_HIGH("AS: pei_primitive");

   msg_p = (T_RV_HDR*) P2D(primptr);
   as_env_handle_message(msg_p);

   return PEI_OK;
   
}/* End of pei_primitive(..) */
#endif
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

   RVM_TRACE_DEBUG_HIGH("AS: pei_run");

   /* loop to process messages */
   while (error_occured == FALSE)
   {
      /* Wait for the necessary events. */	   
      received_event = rvf_wait ( 0xffff,0);

      if (received_event & RVF_TASK_MBOX_0_EVT_MASK)
      {
         msg_p = (T_RV_HDR*) rvf_read_mbox(0);
         as_env_handle_message(msg_p);

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
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
    T_RV_RET ret = RV_OK;
    
    
    RVM_TRACE_DEBUG_HIGH("AS: Initialization");

    /*
     * Initialize task handle
     */
    AS_handle = handle;
    as_hCommAS = -1;

    if(AS_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("AS handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( AS_handle, "AS" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (as_hCommAS < VSI_OK)
    {
      if ((as_hCommAS = vsi_c_open (VSI_CALLER "AS" )) < VSI_OK)
        return PEI_ERROR;
    }
   
    RVM_TRACE_DEBUG_HIGH("AS Open communication channels done"); 
    
    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_AS_CTRL_BLK), (T_RVF_BUFFER**)&as_ctrl_blk) == RVF_RED)
	{
            /* The environemnt will cancel the EXPL instance creation. */
            RVM_TRACE_DEBUG_HIGH ("AS: Error to get memory ");
            return RVM_MEMORY_ERR;	
	}

     // Initial state.
     as_ctrl_blk->state = AS_STATE_INIT;

     // Store the address ID.
     as_ctrl_blk->addr_id          = AS_handle;
     as_ctrl_blk->rp.addr_id       = AS_handle;
     as_ctrl_blk->rp.callback_func = NULL;
     as_ctrl_blk->mb_external = EXT_MEM_POOL;
     
     if(as_ctrl_blk == NULL)
     {
        RVM_TRACE_DEBUG_HIGH("as_ctrl_blk == NULL");

        return PEI_ERROR;
     }

      as_env_init ();

      as_env_start();

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
               "AS",         /* name */
               {              /* pei-table */
                  pei_init,
	          pei_exit,
                  NULL, /* pei_primitive */
                  NULL, /* pei_timeout */
                  NULL, /*  pei_signal */
                  pei_run,
                  NULL, /* pei_config */
                  NULL, /* pei_monitor */
	       },
               AS_STACK_SIZE,            /* stack size */
               10,                        /* queue entries */
               (255-AS_TASK_PRIORITY),     /* priority (1->low, 255->high) */
               0,                         /* number of timers */
               COPY_BY_REF|PRIM_NO_SUSPEND | TRC_NO_SUSPEND // PASSIVE_BODY /* flags */
              };

  RVM_TRACE_DEBUG_HIGH("AS: pei_create");

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
| Function    : as_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN as_env_init(void)
{
#if(AS_RFS_API == 1)
		char *str1 = "/FFS/aud/ringer/ic.cfg";
		char *str2 = "/FFS/aud/ringer/sms.cfg";
		char *str3 = "/FFS/aud/ringer/alarm.cfg";
		
		convert_u8_to_unicode(str1, mp_uc1);
		convert_u8_to_unicode(str2, mp_uc2);
		convert_u8_to_unicode(str3, mp_uc3);
#endif

    AS_TRACE_0(DEBUG_LOW, "env_init() called.");

    // Global constants: RINGER part:
    // Table of filenames.
#if(AS_RFS_API == 1)
	as_ctrl_blk->filename[AS_RINGER_MODE_IC]    = mp_uc1;
    as_ctrl_blk->filename[AS_RINGER_MODE_SMS]   = mp_uc2;
    as_ctrl_blk->filename[AS_RINGER_MODE_ALARM] = mp_uc3;
#else
	as_ctrl_blk->filename[AS_RINGER_MODE_IC]    = "/aud/ringer/ic.cfg";
    as_ctrl_blk->filename[AS_RINGER_MODE_SMS]   = "/aud/ringer/sms.cfg";
    as_ctrl_blk->filename[AS_RINGER_MODE_ALARM] = "/aud/ringer/alarm.cfg";
#endif

    // MANAGEMENT part: default context values.
    as_ctrl_blk->ringer_mode = AS_RINGER_MODE_NONE;
    as_ctrl_blk->player_type = AS_PLAYER_TYPE_NONE;
    as_ctrl_blk->volume      = AS_VOLUME_SILENT;
    as_ctrl_blk->client_rp.addr_id       = RVF_INVALID_ADDR_ID;
    as_ctrl_blk->client_rp.callback_func = NULL;
    as_ctrl_blk->gprs_mode   = FALSE;
    as_ctrl_blk->interrupted = FALSE;
    as_ctrl_blk->free_msg    = FALSE;
    as_ctrl_blk->saved       = NULL;
    as_ctrl_blk->recover     = 0;
    as_ctrl_blk->play_bar_on = PLAYBAR_NOT_SUPPORTED;

   #if (AS_OPTION_CHANGE_PATH == 1)
    // PLAYER part: initialize default values for parameters.
    // Common parameters.
    as_ctrl_blk->params.output_device = AS_DEVICE_HEADSET;
  #endif

#if (AS_OPTION_BUZZER == 1)
    // BUZZER specific data and parameters.
    as_ctrl_blk->buzzer.tm_id   = 0;
    as_ctrl_blk->buzzer.counter = 0;
    as_ctrl_blk->buzzer.toggle  = FALSE;
#endif // AS_OPTION_BUZZER


#if( AS_OPTION_MP3 == 1)
    // MP3 related data initialization
    as_ctrl_blk->mp3.params.mono_stereo = AS_MP3_MONO;
    as_ctrl_blk->mp3.params.size_file_start = 0;
  //  as_ctrl_blk->mp3.params.play_bar_on = PLAYBAR_NOT_SUPPORTED;
#endif

#if( AS_OPTION_AAC == 1)
    // AAC related data initialization
    as_ctrl_blk->aac.params.mono_stereo = AS_AAC_MONO;
    as_ctrl_blk->aac.params.size_file_start = 0;
   //as_ctrl_blk->aac.params.play_bar_on = PLAYBAR_NOT_SUPPORTED;
#endif

#if(AS_OPTION_E1_MELODY == 1)
   // E1-Melody data initialization
   as_ctrl_blk->e1_melody.params.loopback = AS_E1_MELODY_LOOPBACK;
   as_ctrl_blk->e1_melody.params.melody_mode = AS_E1_MELODY_NORMAL_MODE;
#endif

#if(AS_OPTION_VM_AMR == 1)
  // VM AMR data initialization
  as_ctrl_blk->vm_amr.params.file_size_start = 0;
  as_ctrl_blk->vm_amr.params.play_mode = AUDIO_MODE_FILE;
  as_ctrl_blk->vm_amr.params.play_location = AS_AMR_PLAY_FROM_FFS;
#endif
#if( AS_OPTION_MP3 == 1 || AS_OPTION_AAC == 1)    
    as_ctrl_blk->player_data.filename[0] = 0;
    as_ctrl_blk->loop = FALSE;
    as_ctrl_blk->first_play = TRUE;	
#endif

#if( AS_OPTION_LINEAR_FS == 1)
   // Linear FS related data initialization
   as_ctrl_blk->linear_file_descriptor = NULL;
#endif
#ifdef AS_OPTION_MIDI

    // MIDI specific data and parameters.
    as_ctrl_blk->midi.player        = 0;
    as_ctrl_blk->midi.params.voice_limit     = 32;
    as_ctrl_blk->midi.params.output_channels = 2;
#endif

    return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : as_start
+------------------------------------------------------------------------------
| Description : Called to start the AS SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN as_env_start(void)
{
    mbInt16  audio_limit;

    AS_TRACE_0(DEBUG_LOW, "env_start() called.");

    // Create the configuration directory.
    (void) as_ringer_create_dir();

    // Read the configuration files for each supported ringer mode.
    // Do not take error into account. The as_ringer_read_file()
    // function sets default values if file is not available.
    (void) as_ringer_read_file(as_ctrl_blk->filename[AS_RINGER_MODE_IC],
                               &as_ctrl_blk->ringer[AS_RINGER_MODE_IC]);
    (void) as_ringer_read_file(as_ctrl_blk->filename[AS_RINGER_MODE_SMS],
                               &as_ctrl_blk->ringer[AS_RINGER_MODE_SMS]);
    (void) as_ringer_read_file(as_ctrl_blk->filename[AS_RINGER_MODE_ALARM],
                               &as_ctrl_blk->ringer[AS_RINGER_MODE_ALARM]);

#if(AS_RFS_API == 1)
    as_ctrl_blk->ringer_file_created = FALSE;
#endif
    // @todo add other inits.

//#ifdef AS_OPTION_MIDI

    as_ctrl_blk->midi.params.output_channels = 2;

//#endif

    // The AS is ready to accept messages.
    AS_CHANGE_STATE(AS_STATE_IDLE);

    return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : as_stop
+------------------------------------------------------------------------------
| Description : Called to stop the AS SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN as_env_stop(T_RV_HDR* hdr)
{
    AS_TRACE_0(DEBUG_LOW, "env_stop() called.");

    //@todo Implement graceful stop.

    // Tell RVM that the SWE is ready to be killed.
//    rvm_swe_stopped(hdr);

    return RVM_OK;
}


/*
+------------------------------------------------------------------------------
| Function    : as_kill
+------------------------------------------------------------------------------
| Description : Called to stop the AS SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN as_env_kill(void)
{
    AS_TRACE_0(DEBUG_LOW, "kill() called.");

    // Reclaim allocated resources.
    if (rvf_free_buf(as_ctrl_blk) != RVF_OK)
    {
        AS_TRACE_0(WARNING, "kill() unable to free control block.");
    }

    return RVM_OK;
}

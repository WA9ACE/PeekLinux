/*
+-----------------------------------------------------------------------------
|  Project :  Riv2Gpf
|  Module  :  AUDIO
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
|             for the entity AUDIO
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
   	#include "l1sw.cfg"
	#include "nucleus.h"
	#include "swconfig.cfg"
        #include "sys.cfg"
        #include "chipset.cfg"
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

  #include "rvf/rvf_api.h"
  #include "rv/rv_defined_swe.h"

  #include "l1_confg.h"
  #include "rvm/rvm_use_id_list.h"
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "rvm/rvm_priorities.h"

  #include "Audio/audio_ffs_i.h"
  #include "Audio/audio_api.h"
  #include "Audio/audio_structs_i.h"
  #include "Audio/audio_env_i.h"
  #include "rvf/rvf_target.h"
  #include "Audio/audio_const_i.h"
  #include "Audio/audio_macro_i.h"
  #include "Audio/audio_var_i.h"
  #include <string.h>

  #include "../TEMPLATE/PEI/mon_xxx.h"
  #include "../sm_remu/inc/remu_task.h"

/*============================ Macros ========================================*/
#define VSI_CALLER AUDIO_handle,
#define pei_create audio_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

/*============================ Global Variables ==============================*/
//Removed in order to avoid lint-errors/warning
//static T_MONITOR audio_monitor;
T_HANDLE AUDIO_handle;
T_HANDLE audio_hCommAUDIO = -1;
char AUDIO_VERSION[]={"1"};
T_RVF_ADDR_ID  audio_addr_id      = 0;

T_AUDIO_ENV_CTRL_BLK *p_audio_gbl_var = NULL;

#if (L1_EXT_AUDIO_MGT == 1)
    NU_HISR  EXT_MIDI_hisr;
#ifndef HISR_STACK_SHARING
    #if (LOCOSTO_LITE)
    	char ext_midi_hisr_stack[1476]={0xFE};
    #else
    	char ext_midi_hisr_stack[2500];
    #endif
#endif
    extern void Cust_ext_audio_mgt_hisr(void);
#endif

/*============================ Extern Functions ==============================*/
extern T_RV_RET audio_core(void);

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
#if 0 
// Removed in order avoid lint errors
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  RVM_TRACE_DEBUG_HIGH ("AUDIO: pei_monitor");

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
#if 0 
// Removed in order avoid lint errors
LOCAL SHORT pei_config (char *inString)
{
  RVM_TRACE_DEBUG_HIGH ("AUDIO: pei_config");

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
LOCAL SHORT pei_timeout (int h1,int h2,USHORT tnum)
{
  RVM_TRACE_DEBUG_HIGH ("AUDIO: pei_timeout");

  rvf_send_event(audio_addr_id,(UINT16)(EVENT_MASK(((tnum % RVF_NUM_TASK_TIMERS)+4)) ) );

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
#if 0 
// Removed in order avoid lint errors
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  RVM_TRACE_DEBUG_HIGH ("AUDIO: pei_signal");

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
   RVM_TRACE_DEBUG_HIGH ("AUDIO: pei_exit");

   /*
    * Close communication channels
    */
    /* Should we*/
    #if (L1_EXT_AUDIO_MGT)
    //    NU_Delete_HISR(EXT_MIDI_hisr);
    #endif

   vsi_c_close (VSI_CALLER audio_hCommAUDIO);
   audio_hCommAUDIO = VSI_ERROR;

   audio_stop ();
   audio_kill();

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
#if 0 
// Removed in order avoid lint errors
LOCAL SHORT pei_primitive (void * primptr)
{
   T_RV_HDR* msg_p ;

   RVM_TRACE_DEBUG_HIGH("AUDIO: pei_primitive");

   msg_p = (T_RV_HDR*) P2D(primptr);
//   audio_handle_message(msg_p);

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
|Parameters   :  taskhandle  - handle of current process
|                comhandle   - queue handle of current process
|
| Return      : PEI_OK            - successful 
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{
   RVM_TRACE_DEBUG_HIGH("AUDIO: pei_run");
   rvf_create_timer(audio_addr_id,0,TRUE, pei_timeout );
   audio_core();
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

#if (VOICE_MEMO) || (MELODY_E1)
      UINT8   i=0;
#endif

    T_RV_RET ret = RV_OK;

    RVM_TRACE_DEBUG_HIGH("AUDIO: Initialization");

    /*
     * Initialize task handle
     */
    AUDIO_handle = handle;
    audio_hCommAUDIO = -1;

    if(AUDIO_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("AUDIO_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( AUDIO_handle, "AUDIO" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }
    /*
     * Open communication channels
     */
    if (audio_hCommAUDIO < VSI_OK)
    {
      if ((audio_hCommAUDIO = vsi_c_open (VSI_CALLER "AUDIO" )) < VSI_OK)
        return PEI_ERROR;
    }

    audio_addr_id = audio_hCommAUDIO; /* Entity Communication Handle Initialization */
                              /* Entity's MailBox Handle Initialization */

    if (rvf_get_buf(EXT_MEM_POOL, sizeof(T_AUDIO_ENV_CTRL_BLK),
                   (T_RVF_BUFFER**)&p_audio_gbl_var) != RVF_GREEN)
    {
        /* The environemnt will cancel the EXPL instance creation. */
        RVM_TRACE_DEBUG_HIGH ("AUDIO: Error to get memory ");

        return RVM_MEMORY_ERR;
    }

    /* Initialize the Global Environment Control Block */
    memset((UINT8 *) p_audio_gbl_var, 0x00, sizeof (T_AUDIO_ENV_CTRL_BLK));

    /* Store the memory bank IDs assigned to the AUDIO */
    p_audio_gbl_var->mb_external = EXT_MEM_POOL;
    p_audio_gbl_var->mb_internal = INT_MEM_POOL;
    p_audio_gbl_var->mb_audio_ffs = EXT_MEM_POOL;

    /* Store the address ID assigned to the AUDIO */
    p_audio_gbl_var->addrId = handle;

    /* Store the address ID of the audio FFS task */
    p_audio_gbl_var->audio_ffs_addrId = handle;

    /* Store the funct to be called whenever any unrecoverable error occurs.*/
     p_audio_gbl_var->callBackFct = rvm_error;
     TRACE_FUNCTION(" AUDIO (env). AUDIO's information set ");

	/* Initialization of the audio keybeep state */
#if (KEYBEEP)
      p_audio_gbl_var->keybeep.state = AUDIO_KEYBEEP_IDLE;
#endif
    /* Initialization of the audio tones state */
#if (TONE)
      p_audio_gbl_var->tones.state = AUDIO_TONES_IDLE;
#endif
    /* Initialization of the audio melody E1 state */
#if (MELODY_E1)
      p_audio_gbl_var->melody_E1_0.state = AUDIO_MELODY_E1_IDLE;
      p_audio_gbl_var->melody_E1_1.state = AUDIO_MELODY_E1_IDLE;
#endif
    /* Initialization of the audio melody E2 state */
#if (MELODY_E2)
      p_audio_gbl_var->melody_E2_0.state = AUDIO_MELODY_E2_IDLE;
      p_audio_gbl_var->melody_E2_1.state = AUDIO_MELODY_E2_IDLE;

      /* Initalize the flag to valie the .lsi file */
      p_audio_gbl_var->melody_E2_load_file_instruments.file_downloaded = FALSE;
#endif
    /* Initialization of the audio voice memorization state */
#if (VOICE_MEMO)
      p_audio_gbl_var->vm_play.state    = AUDIO_VM_PLAY_IDLE;
      p_audio_gbl_var->vm_record.state  = AUDIO_VM_RECORD_IDLE;
#endif
#if (L1_EXT_AUDIO_MGT==1)
      p_audio_gbl_var->midi.state = AUDIO_IDLE;
#endif
#if (L1_MP3==1)
      p_audio_gbl_var->audio_mp3.state = AUDIO_IDLE;
      p_audio_gbl_var->audio_mp3.mp3_mono_stereo_mmi_command = 1; /* initialize to 1, 0-mono, 1-stereo */
#endif
#if (L1_AAC==1)
      p_audio_gbl_var->audio_aac.state = AUDIO_IDLE;
      p_audio_gbl_var->audio_aac.aac_mono_stereo_mmi_command = 1; /* initialize to 1, 0-mono, 1-stereo */
#endif
#if (L1_AUDIO_DRIVER)
      for (i = 0; i < AUDIO_DRIVER_MAX_CHANNEL; i++)
      {
        p_audio_gbl_var->audio_driver_session[i].session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;
        p_audio_gbl_var->audio_driver_session[i].session_req.nb_buffer = 0;
      }
#endif
#if (VOICE_MEMO) || (MELODY_E1) || (MELODY_E2)
      for (i=0; i< AUDIO_FFS_MAX_CHANNEL; i++)
      {
        p_audio_gbl_var->audio_ffs_session[i].session_req.valid_channel = FALSE;
      }
#endif
    /* Initialization of the audio speech reco state machine */
#if (SPEECH_RECO)
      p_audio_gbl_var->speech_reco.sr_enroll.state = AUDIO_SR_ENROLL_IDLE;
      p_audio_gbl_var->speech_reco.sr_update.state = AUDIO_SR_UPDATE_IDLE;
      p_audio_gbl_var->speech_reco.sr_reco.state   = AUDIO_SR_RECO_IDLE;
#endif

    /* initialize the state machine of audio mode managers */
    p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_IDLE;
    p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state = AUDIO_MODE_LOAD_IDLE;
    /* initialize the name ofthe current volume file */
    p_audio_gbl_var->audio_mode_var.audio_mode_path_name[0] = 0;

    #if (L1_PCM_EXTRACTION) //#if (VOICE_MEMO_PCM) //VENKAT
      p_audio_gbl_var->vm_pcm_play.state    = AUDIO_VM_PCM_PLAY_IDLE;
      p_audio_gbl_var->vm_pcm_record.state  = AUDIO_VM_PCM_RECORD_IDLE;
      p_audio_gbl_var->vbuf_pcm_play.state    = AUDIO_VBUF_PCM_PLAY_IDLE;
      p_audio_gbl_var->vbuf_pcm_record.state  = AUDIO_VBUF_PCM_RECORD_IDLE;
    #endif

#if (PSP_STANDALONE != 1)
#if (L1_EXT_AUDIO_MGT)
      // Create HISR for Ext MIDI activity
      //==================================
      NU_Create_HISR( &EXT_MIDI_hisr,
                      "H_EXT_MIDI",
                      Cust_ext_audio_mgt_hisr,
                      2,
		#ifndef HISR_STACK_SHARING
                      ext_midi_hisr_stack,
                      sizeof(ext_midi_hisr_stack));
      	#else
                      HISR_STACK_PRIO2,
                      HISR_STACK_PRIO2_SIZE);
      	#endif
#endif
#endif
	audio_init();
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
    		"AUDIO",/* Audio task name*/
    		{
      		pei_init,
      		pei_exit,
      		NULL,     /* pei_primitive */
      		NULL,     /* pei_timeout */
      		NULL,     /* pei_signal */
      		pei_run,
      		NULL,     /* pei_config */
      		NULL,     /* pei_monitor */
    		},
    		AUDIO_STACK_SIZE,
    		10,		               /* Queue entries */
	        (255-RVM_AUDIO_TASK_PRIORITY), /* Priority - 1->low, 255-> high. */
                1,                             /* Number of timers */
#if (LOCOSTO_LITE==1)
                COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND // no need since anyway its from internal mempory
#else
                COPY_BY_REF | PRIM_NO_SUSPEND | TRC_NO_SUSPEND |INT_DATA_TASK  // allocate stack from internla memory
#endif
		       /* Flags Settings */
  	  };

   RVM_TRACE_DEBUG_HIGH("AUDIO: pei_create");

  /*
   *  Export startup configuration data
   */

  *info = (T_PEI_INFO *)&pei_info;

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : audio_init
+------------------------------------------------------------------------------
| Description : This function is called to initialise.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN audio_init(void)
{
  /* Put global audio initialization */
	audio_accessory_int_init();

  return RV_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : audio_stop
+------------------------------------------------------------------------------
| Description : Called to stop the AUDIO SWE.
|
| Parameters  : Message
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN audio_stop(void)
{
  /* other SWEs have not been killed yet, audio can send messages to other SWEs      */

  return RV_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : audio_kill
+------------------------------------------------------------------------------
| Description : Called to stop the AUDIO SWE.
|
| Parameters  : None
|
| Return      : PEI_OK            - successful
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
T_RVM_RETURN audio_kill (void)
{
  /* free all memory buffer previously allocated */
  rvf_free_buf ((T_RVF_BUFFER *) p_audio_gbl_var);
  return RV_OK;
}

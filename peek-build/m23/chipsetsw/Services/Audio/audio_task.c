/****************************************************************************/
/*                                                                          */
/*  Name        audio_task.c                                                */
/*                                                                          */
/*  Function    this file contains the main AUDIO function: audio_task      */
/*              It contains the body of the AUDIO task.                     */
/*              It will initialize the AUDIO and then wait for messages     */
/*              or functions calls.                                         */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  18 May 2001  Create                                                     */
/*                                                                          */
/*  Author   Francois Mazard - Stephanie Gerthoux                           */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"

#ifdef RVM_AUDIO_MAIN_SWE
  #ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

  #include "l1_types.h"
  #include "l1_confg.h"
  #include "rvf/rvf_api.h"
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_features_i.h"
  #include "audio/audio_api.h"
  #include "audio/audio_env_i.h"
  #include "audio/audio_ffs_i.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_macro_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_messages_i.h"
  #if (L1_GTT == 1)
    #include "tty/tty_i.h"
    #include "l1gtt_signa.h"
  #endif

  #ifndef _WINDOWS
    /* include the usefull L1 header */
    #define BOOL_FLAG
    #define CHAR_FLAG
    #include "cust_os.h"
    #include "l1audio_cust.h"
    #include "l1audio_msgty.h"
    #include "l1audio_signa.h"
    #include "l1_signa.h"
  #else
    /* include the usefull L1 header */
    #define BOOL_FLAG
    //#define CHAR_FLAG
    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_defty.h"
    #include "l1audio_msgty.h"
    #include "l1audio_signa.h"
    #include "l1_const.h"

  #if (L1_DYN_DSP_DWNLD == 1)
    #include "l1_dyn_dwl_const.h"
    #include "l1_dyn_dwl_defty.h"
  #endif

    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
      #include "l1mp3_signa.h"
    #endif
    #if (L1_AAC == 1)
      #include "l1aac_defty.h"
    #endif
    #include "l1_defty.h"
    #include "l1_msgty.h"
    #include "l1_signa.h"
    #include "l1_varex.h"
    #include "audio/tests/audio_test.h"
  #endif

  #ifndef _WINDOWS
    #if (TRACE_TYPE==4)
      extern void l1_trace_message(xSignalHeaderRec *msg);
    #endif
  #endif
  #if (L1_GTT == 1)
    extern void tty_manager (T_RV_HDR *p_message);
    extern T_TTY_ENV_CTRL_BLK *tty_env_ctrl_blk_p;
  #endif
  #if (L1_EXT_AUDIO_MGT==1)
    extern UINT8 audio_midi_message_switch(T_RV_HDR *p_message);
  #endif
  #if (L1_MP3==1)
    extern UINT8 audio_mp3_message_switch(T_RV_HDR *p_message);
  #endif
  #if (L1_AAC==1)
    extern UINT8 audio_aac_message_switch(T_RV_HDR *p_message);
  #endif
  extern   BOOLEAN audio_compatibilities_manager (T_RV_HDR *p_message);

  #ifdef _WINDOWS
    void audio_vm_amr_play_l1_simulator(UINT16 event, T_RV_HDR *p_message);
    void audio_vm_amr_record_l1_simulator(UINT16 event, T_RV_HDR *p_message);
    void audio_midi_l1_simulator(UINT16 event, T_RV_HDR *p_message);
    void audio_mp3_l1_simulator(UINT16 event, T_RV_HDR *p_message);
    void audio_aac_l1_simulator(UINT16 event, T_RV_HDR *p_message);
  #endif


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_allocate_l1_message                                */
  /*                                                                              */
  /*    Purpose:  This function is called to allocate a buffer for the message    */
  /*              to the l1                                                       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Size                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  DummyStruct *audio_allocate_l1_message(UINT16 size)
  {
    #ifdef _WINDOWS
      DummyStruct *buffer = NULL;
      T_RVF_MB_STATUS mb_status;
      /* allocate a buffer in the riviera environement */
      if (size == 0)
      {
        /* allocate the memory for a message without parameter */
        mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                                 sizeof(T_RV_HDR),
                                 (T_RVF_BUFFER **) (&buffer));
      }
      else
      {
        /* allocate the memory for a message with some parameters */
        mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                                 size,
                                 (T_RVF_BUFFER **) (&buffer));
      }

      /* If insufficient resources, then report a memory error and abort. */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)buffer);
        audio_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (NULL);
      }
      else
      if (mb_status == RVF_RED)
      {
          audio_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (NULL);
      }
      return buffer;

    #else
      /* allocate a buffer in the L1 environement */
      xSignalHeaderRec* p_msg;

#if (PSP_STANDALONE != 1)
      p_msg = os_alloc_sig(size);
#endif

      return (p_msg->SigP);
    #endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_deallocate_l1_message                              */
  /*                                                                              */
  /*    Purpose:  This function is called to deallocate a buffer for the message  */
  /*              to the l1.                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Key Beep Parameters,                                            */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_deallocate_l1_message(DummyStruct *message)
  {
    #ifdef _WINDOWS
      T_RV_RET  status;

      /* deallocate the memory */
      status = rvf_free_buf((T_RVF_BUFFER *)message);
      if (status != RVF_GREEN)
      {
        AUDIO_SEND_TRACE(" AUDIO ERROR (env). A wrong message is deallocated ",
                       RV_TRACE_LEVEL_ERROR);
      }

    #else
      /* deallocate a buffer in the L1 environement */
      xSignalHeaderRec *p_signal;

      p_signal = (xSignalHeaderRec*)message;
      p_signal--;

#if (PSP_STANDALONE != 1)
      os_free_sig(p_signal);
#endif

    #endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_send_l1_message                                    */
  /*                                                                              */
  /*    Purpose:  This function is called to send the message to the L1           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        the message id and the message,                                       */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_RV_RET audio_send_l1_message(INT16 message_id, DummyStruct *message)
  {
    #ifdef _WINDOWS
      #if ((AUDIO_REGR == SW_COMPILED) || (AUDIO_MISC == SW_COMPILED))
      ((T_RV_HDR *)message)->msg_id = message_id;

      switch (message_id)
      {
        /* send the messsage to the audio entity */
        #if (KEYBEEP)
          case MMI_KEYBEEP_START_REQ:
          case MMI_KEYBEEP_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->keybeep.return_path.addr_id,
                          message);
            break;
          }
        #endif
        #if (TONE)
          case MMI_TONE_START_REQ:
          case MMI_TONE_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->tones.return_path.addr_id,
                          message);
            break;
          }
        #endif
        #if (MELODY_E1)
          case MMI_MELODY0_START_REQ:
          case MMI_MELODY0_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->melody_E1_0.return_path.addr_id,
                          message);
            break;
          }
          case MMI_MELODY1_START_REQ:
          case MMI_MELODY1_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->melody_E1_1.return_path.addr_id,
                          message);
            break;
          }
        #endif
        #if (MELODY_E2)
          case MMI_MELODY0_E2_START_REQ:
          case MMI_MELODY0_E2_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->melody_E2_0.return_path.addr_id,
                          message);
            break;
          }
          case MMI_MELODY1_E2_START_REQ:
          case MMI_MELODY1_E2_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->melody_E2_1.return_path.addr_id,
                          message);
            break;
          }
       #endif
        #if (VOICE_MEMO)
          case MMI_VM_PLAY_START_REQ:
          case MMI_VM_PLAY_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->vm_play.return_path.addr_id,
                          message);
            break;
          }
          case MMI_VM_RECORD_START_REQ:
          case MMI_VM_RECORD_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->vm_record.return_path.addr_id,
                          message);
            break;
          }
        #endif
/*VENKAT*/
#if(L1_PCM_EXTRACTION)				
          case MMI_PCM_DOWNLOAD_START_REQ:
          case MMI_PCM_DOWNLOAD_STOP_REQ:
          {
		  	AUDIO_SEND_TRACE_PARAM("audio_send_l1_message() L1 msg id: ", message_id,RV_TRACE_LEVEL_DEBUG_LOW);
		  	if(p_audio_gbl_var->vmemo_vbuf_session == AUDIO_VMEMO_PCM_SESSION)
		  	{
		            rvf_send_msg (p_audio_gbl_var->vm_pcm_play.return_path.addr_id,
                          message);
			}
			else if(p_audio_gbl_var->vmemo_vbuf_session == AUDIO_VBUF_PCM_SESSION)
			{
			     rvf_send_msg (p_audio_gbl_var->vbuf_pcm_play.return_path.addr_id,
                          message);
			}
            break;
          }
          case MMI_PCM_UPLOAD_START_REQ:
          case MMI_PCM_UPLOAD_STOP_REQ:
          {
		  	AUDIO_SEND_TRACE_PARAM("audio_send_l1_message() L1 msg id: ", message_id,RV_TRACE_LEVEL_DEBUG_LOW);
		  	if(p_audio_gbl_var->vmemo_vbuf_session == AUDIO_VMEMO_PCM_SESSION)
		  	{
		            rvf_send_msg (p_audio_gbl_var->vm_pcm_record.return_path.addr_id,
                          message);
			}
			else if(p_audio_gbl_var->vmemo_vbuf_session == AUDIO_VBUF_PCM_SESSION)
			{
			     rvf_send_msg (p_audio_gbl_var->vbuf_pcm_record.return_path.addr_id,
                          message);
			}


            rvf_send_msg (p_audio_gbl_var->vm_pcm_record.return_path.addr_id,
                          message);
            break;
          }
#endif//(L1_PCM_EXTRACTION)				
/*!VENKAT*/		

        #if (L1_VOICE_MEMO_AMR)
          case MMI_VM_AMR_PLAY_START_REQ:
          case MMI_VM_AMR_PLAY_STOP_REQ:
          case MMI_VM_AMR_PAUSE_REQ:
          case MMI_VM_AMR_RESUME_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->addrId,message);
          }
          break;
          case MMI_VM_AMR_RECORD_START_REQ:
          case MMI_VM_AMR_RECORD_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->addrId,message);
          }
          break;
        #endif
        #if (SPEECH_RECO)
          case MMI_SR_ENROLL_START_REQ:
          case MMI_SR_ENROLL_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->speech_reco.sr_enroll.return_path.addr_id,
                          message);
            break;
          }
          case MMI_SR_UPDATE_START_REQ:
          case MMI_SR_UPDATE_STOP_REQ:
          case MMI_SR_UPDATE_CHECK_START_REQ:
          case MMI_SR_UPDATE_CHECK_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->speech_reco.sr_update.return_path.addr_id,
                          message);
            break;
          }
          case MMI_SR_RECO_START_REQ:
          case MMI_SR_RECO_STOP_REQ:
          {
            rvf_send_msg (p_audio_gbl_var->speech_reco.sr_reco.return_path.addr_id,
                          message);
            break;
          }
        #endif
        #if (L1_GTT == 1)
          case MMI_GTT_START_REQ:
          case MMI_GTT_STOP_REQ:
          {
            rvf_send_msg (tty_env_ctrl_blk_p->return_path.addr_id,
                          message);
          }
          break;
        #endif
        #if (L1_EXT_AUDIO_MGT==1)
          case MMI_EXT_AUDIO_MGT_START_REQ:
          case MMI_EXT_AUDIO_MGT_STOP_REQ:
          
          {
            rvf_send_msg(p_audio_gbl_var->addrId,message);
            break;
          }
        #endif
        #if (FIR)
          case MMI_AUDIO_FIR_CON:
          case MMI_AUDIO_FIR_REQ:
        #endif
        #if (L1_AEC == 1)
          case MMI_AEC_CON:
          case MMI_AEC_REQ:
        #endif
        #if (L1_AEC == 2)
          case MMI_AQI_AEC_CON:
          case MMI_AQI_AEC_REQ:
        #endif		
        #if (L1_ANR == 1)
          case MMI_ANR_CON:
          case MMI_ANR_REQ:
        #endif
        #if (L1_ANR == 2)
          case MMI_AQI_ANR_CON:
          case MMI_AQI_ANR_REQ:
        #endif		
        #if (L1_ES == 1)
          case MMI_ES_CON:
          case MMI_ES_REQ:
        #endif
        #if (L1_IIR == 1)
          case MMI_IIR_CON:
          case MMI_IIR_REQ:
        #elif(L1_IIR == 2)
	   case MMI_AQI_IIR_DL_CON:
          case MMI_AQI_IIR_DL_REQ:
        #endif
		#if (L1_AGC_UL == 1)
          case MMI_AQI_AGC_UL_REQ:		
          case MMI_AQI_AGC_UL_CON :
        #endif
        #if (L1_AGC_DL == 1)
          case MMI_AQI_AGC_DL_REQ:		
          case MMI_AQI_AGC_DL_CON :
        #endif
        #if (L1_DRC == 1)
          case MMI_AQI_DRC_CON:
          case MMI_AQI_DRC_REQ:
        #endif
        #if (L1_LIMITER == 1)
          case MMI_LIMITER_CON:
          case MMI_LIMITER_REQ:
        #endif
        #if (AUDIO_MODE)
          case MMI_AUDIO_MODE_CON:
          case MMI_AUDIO_MODE_REQ:
        #endif
        case OML1_STOP_DAI_TEST_REQ:
        case OML1_STOP_DAI_TEST_CON:
        #if (AUDIO_DSP_ONOFF == 1 || L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
            case MMI_AUDIO_ONOFF_REQ:
        #endif
          {
            rvf_send_msg (p_audio_test->test_addr_id,
                          message);
            break;
          }
        #if (L1_MP3 == 1)
          case MMI_MP3_START_REQ:
          case MMI_MP3_STOP_REQ:
          case MMI_MP3_PAUSE_REQ:
          case MMI_MP3_RESUME_REQ:
          case MMI_MP3_RESTART_REQ:        
          case MMI_MP3_INFO_REQ:        
          {
            rvf_send_msg(p_audio_gbl_var->addrId,message);
            break;
          }
        #endif
        #if (L1_AAC == 1)
          case MMI_AAC_START_REQ:
          case MMI_AAC_STOP_REQ:
          case MMI_AAC_PAUSE_REQ:
          case MMI_AAC_RESUME_REQ:
          case MMI_AAC_RESTART_REQ:        
          case MMI_AAC_INFO_REQ:        
          {
            rvf_send_msg(p_audio_gbl_var->addrId,message);
            break;
          }
        #endif
	#if(L1_BT_AUDIO==1)
         case MMI_BT_CFG_REQ:
		rvf_send_msg(p_audio_gbl_var->addrId,message);
            break;
       #endif
        }
        return(RV_OK);
      #endif
    #else
      xSignalHeaderRec *p_signal;

      p_signal = (xSignalHeaderRec*)message;
      p_signal--;
      p_signal->SigP = message;
      p_signal->SignalCode = message_id;

	#if (PSP_STANDALONE != 1)
      #if (TRACE_TYPE==4)
        l1_trace_message(p_signal);
      #endif
  	//AUDIO_SEND_TRACE_PARAM("INSIDE audio_send_l1_message() L1 msg id: ", message_id,RV_TRACE_LEVEL_DEBUG_LOW);
      /* send a message in the L1 environement */
      os_send_sig(p_signal, L1C1_QUEUE);
	#endif

	  return(RV_OK);
    #endif
  }

  /********************************************************************************/
  /* Function         audio_core                                                  */
  /*                                                                              */
  /* Description      Core of the audio task, which initiliazes the audio SWE and */
  /*                  waits for messages.                                         */
  /*                                                                              */
  /********************************************************************************/
  T_RV_RET audio_core(void)
  {
    /* Declare local variables */
    BOOLEAN       error_occured = FALSE;
    T_RV_HDR      *p_message    = NULL;
    UINT16        received_event= 0x0000;
    T_RVF_MB_STATUS      status;

    AUDIO_SEND_TRACE("AUDIO_TASK started",RV_TRACE_LEVEL_DEBUG_HIGH);
    /* loop to process messages */
    while (error_occured == FALSE)
    {
      /* Wait for the necessary events (all events and no time out). */
      received_event = rvf_wait (AUDIO_ALL_EVENT_FLAGS, AUDIO_NOT_TIME_OUT);

      #if (MELODY_E1) || (MELODY_E2) || (VOICE_MEMO) || (L1_MP3) || (L1_AAC)
        /* the time out for the AUDIO FFS downloader is expired */
        if (received_event & AUDIO_FFS_TIMER_EVT_MASK)
        {
//        AUDIO_SEND_TRACE("AUDIO_TASK: Got timer event. Calling audio_ffs_downloader()",RV_TRACE_LEVEL_ERROR);
//           AUDIO_SEND_TRACE_PARAM("AUDIO PCM PLAY STATE: ", p_audio_gbl_var->vbuf_pcm_play.state,RV_TRACE_LEVEL_DEBUG_LOW);
//           AUDIO_SEND_TRACE_PARAM("AUDIO PCM RECORD STATE: ", p_audio_gbl_var->vbuf_pcm_record.state,RV_TRACE_LEVEL_DEBUG_LOW);
            audio_ffs_downloader();
        }
      #endif /* MELODY_E1 || MELODY_E2 || VOICE_MEMO || L1_MP3 || (L1_AAC)*/

      /* If an event is received, then ....*/
      if (received_event & AUDIO_TASK_MBOX_EVT_MASK)
      {
        /* Read the message in the audio mailbox */
        p_message = (T_RV_HDR *) rvf_read_mbox(AUDIO_MBOX);

        if (p_message != NULL)
        {
          p_audio_gbl_var->message_processed = FALSE;
/*VENKAT*/
        #if (MELODY_E1) || (MELODY_E2) || (VOICE_MEMO) || (L1_MP3) || (L1_AAC) || (L1_PCM_EXTRACTION)				
/*!VENKAT*/
          if ( (p_message->msg_id == AUDIO_FFS_FLASH_2_RAM_START_REQ) ||
               (p_message->msg_id == AUDIO_FFS_RAM_2_FLASH_START_REQ) ||
               (p_message->msg_id == AUDIO_FFS_STOP_REQ) )
          {
//            AUDIO_SEND_TRACE("AUDIO_TASK: Got ffs msg. Calling audio_ffs_manager()",RV_TRACE_LEVEL_ERROR);
//            AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
            /* process FFS message */
            audio_ffs_manager(p_message);
            p_audio_gbl_var->message_processed = TRUE;
          }
        #endif /* MELODY_E1 || MELODY_E2 || VOICE_MEMO || L1_MP3  || (L1_AAC)*/
           AUDIO_SEND_TRACE_PARAM("AUDIO CORE - Got the msg:", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
        // Messages added for CQ : 67192
        
        switch(p_message->msg_id)
        {
            case MMI_OUTEN_CFG_READ_CON :
            {
               T_MMI_OUTEN_CFG_READ_CON  *outen_cfg_read = (T_MMI_OUTEN_CFG_READ_CON *) p_message;
               AUDIO_SEND_TRACE_PARAM("MMI_OUTEN_CFG_READ_CON with the message", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
               BspTwl3029_cache_outen_reg(outen_cfg_read->outen1, outen_cfg_read->outen2, outen_cfg_read->outen3);
               p_audio_gbl_var->message_processed = TRUE;
               break;
            }
            case  MMI_OUTEN_CFG_CON :
            {
              AUDIO_SEND_TRACE_PARAM("MMI_OUTEN_CFG_CON with the message", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->message_processed = TRUE;
              break;
            }
         }

      #if(L1_BT_AUDIO) 
	 if ( (p_message->msg_id == AUDIO_BT_CFG_REQ) )
        {
          // T_MMI_BT_CFG_REQ *bt_cfg_req= (T_MMI_BT_CFG_REQ *)p_message;
           AUDIO_SEND_TRACE_PARAM("MMI_BT_CFG_REQ with the message", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
           audio_bt_cfg_manager (p_message);
           p_audio_gbl_var->message_processed = TRUE;
        }
	#endif
          /* Check the compatibility with the active audio task */
          if (audio_compatibilities_manager(p_message))
          {
            #if (KEYBEEP)
              if ( (p_message->msg_id == AUDIO_KEYBEEP_START_REQ) ||
                   (p_message->msg_id == AUDIO_KEYBEEP_STOP_REQ)  ||
                   (p_message->msg_id == MMI_KEYBEEP_START_CON)   ||
                   (p_message->msg_id == MMI_KEYBEEP_STOP_CON) )
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO KEYBEEP with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                /* process the keybeep manager */
                audio_keybeep_manager(p_message);
                p_audio_gbl_var->message_processed = TRUE;
              }
            #endif
            #if (TONE)
              if ( (p_message->msg_id == AUDIO_TONES_START_REQ)  ||
                   (p_message->msg_id == AUDIO_TONES_STOP_REQ)   ||
                   (p_message->msg_id == MMI_TONE_START_CON)     ||
                   (p_message->msg_id == MMI_TONE_STOP_CON) )
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO TONES with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                /* process the tones manager */
                audio_tones_manager(p_message);
                p_audio_gbl_var->message_processed = TRUE;
              }
            #endif
            #if (SPEECH_RECO)
              if ( (p_message->msg_id == AUDIO_SR_ENROLL_START_REQ)  ||
                   (p_message->msg_id == AUDIO_SR_ENROLL_STOP_REQ)   ||
                   (p_message->msg_id == MMI_SR_ENROLL_START_CON)    ||
                   (p_message->msg_id == MMI_SR_ENROLL_STOP_CON) )
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO SR ENROLL with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                audio_sr_enroll_manager(p_message);
                p_audio_gbl_var->message_processed = TRUE;
              }
              if ( (p_message->msg_id == AUDIO_SR_UPDATE_START_REQ)     ||
                   (p_message->msg_id == AUDIO_SR_UPDATE_STOP_REQ)      ||
                   (p_message->msg_id == MMI_SR_UPDATE_START_CON)       ||
                   (p_message->msg_id == MMI_SR_UPDATE_STOP_CON)        ||
                   (p_message->msg_id == MMI_SR_UPDATE_CHECK_START_CON) ||
                   (p_message->msg_id == MMI_SR_UPDATE_CHECK_STOP_CON) )
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO SR UPDATE with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                audio_sr_update_manager(p_message);
                p_audio_gbl_var->message_processed = TRUE;
              }
              if ( (p_message->msg_id == AUDIO_SR_RECO_START_REQ)  ||
                   (p_message->msg_id == AUDIO_SR_RECO_STOP_REQ)   ||
                   (p_message->msg_id == MMI_SR_RECO_START_CON)    ||
                   (p_message->msg_id == MMI_SR_RECO_STOP_CON) )
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO SR RECO with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                audio_sr_reco_manager(p_message);
                p_audio_gbl_var->message_processed = TRUE;
              }
              #ifndef _WINDOWS
                /* add the management of the background task in the audio entity */
                if ( (p_message->msg_id == L1_SRBACK_SAVE_DATA_REQ)    ||
                     (p_message->msg_id == L1_SRBACK_LOAD_MODEL_REQ)   ||
                     (p_message->msg_id == L1_SRBACK_TEMP_SAVE_DATA_REQ) )
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO SR BACKGROUND with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  audio_sr_background_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                }
              #endif
            #endif
            #if (MELODY_E1)
              /* determine which melody id is involved by this start or stop message */
              switch (audio_melody_E1_message_switch(p_message))
              {
                /* The message is for the melody number 0 */
                case AUDIO_MELODY_E1_0:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO MELODY E1.0 with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the melody E1 manager 0 */
                  audio_melody_E1_manager_0(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
                /* The message is for the melody number 1 */
                case AUDIO_MELODY_E1_1:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO MELODY E1.1 with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the melody E1 manager 1 */
                  audio_melody_E1_manager_1(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
              } /* switch */
            #endif
            #if (MELODY_E2)
              /* determine which melody id is involved by this start or stop messsage */
              switch (audio_melody_E2_message_switch(p_message))
              {
                /* The message is for the melody number 0 */
                case AUDIO_MELODY_E2_0:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO MELODY E2.0 with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the melody E2 manager 0 */
                  audio_melody_E2_manager_0(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
                /* The message is for the melody number 1 */
                case AUDIO_MELODY_E2_1:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO MELODY E2.1 with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the melody E2 manager 1 */
                  audio_melody_E2_manager_1(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
              } /* switch */
              #ifndef _WINDOWS
                /* add the management of the melody E2 background task in the audio entity */
                if ( (p_message->msg_id == L1_BACK_MELODY_E2_LOAD_INSTRUMENT_REQ) ||
                     (p_message->msg_id == L1_BACK_MELODY_E2_UNLOAD_INSTRUMENT_REQ) )
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO MELODY E2 BACKGROUND with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  audio_background_melody_e2_download_instrument_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                }
              #endif
            #endif
//#if 0
            #if (VOICE_MEMO)
              switch (audio_voice_memo_message_switch(p_message))
              {
                /* The message is for the voice memo play*/
                case AUDIO_VM_PLAY:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO VM PLAY with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo play manager */
                  audio_vm_play_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
                /* The message is for the voice memo record */
                case AUDIO_VM_RECORD:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO VM RECORD with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo record manager */
                  audio_vm_record_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
              } /* switch */
            #endif
//#endif //#if 0
			/*VENKAT*/
#if(L1_PCM_EXTRACTION) //VENKAT
              switch (audio_voice_memo_pcm_message_switch(p_message))
              {
                /* The message is for the voice memo play*/
                case AUDIO_VM_PCM_PLAY:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM PLAY with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo play manager */
                  audio_vm_pcm_play_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
                /* The message is for the voice memo record */
                case AUDIO_VM_PCM_RECORD:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM RECORD with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo record manager */
                  audio_vm_pcm_record_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
              } /* switch */

              switch (audio_vbuf_pcm_message_switch(p_message))
              {
                /* The message is for the voice memo play*/
                case AUDIO_VBUF_PCM_PLAY:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM PLAY with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo play manager */
                  audio_vbuf_pcm_play_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
                /* The message is for the voice memo record */
                case AUDIO_VBUF_PCM_RECORD:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM RECORD with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo record manager */
                  audio_vbuf_pcm_record_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                  break;
                }
              } /* switch */
#endif//(L1_PCM_EXTRACTION)				
			/*!VENKAT*/

            #if (L1_GTT == 1)
              if ( (p_message->msg_id == TTY_START_REQ) ||
                   (p_message->msg_id == TTY_STOP_REQ) ||
                   (p_message->msg_id == MMI_GTT_START_CON) ||
                   (p_message->msg_id == MMI_GTT_STOP_CON))
              {
                AUDIO_SEND_TRACE_PARAM("TTY with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                /* process the TTY manager */
                tty_manager(p_message);
                p_audio_gbl_var->message_processed = TRUE;
              }
            #endif
            #if (L1_VOICE_MEMO_AMR)
              switch (audio_voice_memo_amr_memory_message_switch(p_message))
              {
                /* The message is for the voice memo play*/
                case AUDIO_VM_AMR_PLAY:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO AMR PLAY from memory with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo play manager */
                  audio_vm_amr_play_from_memory_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                }
                break;
                /* The message is for the voice memo record */
                case AUDIO_VM_AMR_RECORD:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO AMR RECORD to memory with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo record manager */
                  audio_vm_amr_record_to_memory_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                }
                break;
              } /* switch */
            #endif

            #if (L1_EXT_AUDIO_MGT==1)
              if(audio_midi_message_switch(p_message)==AUDIO_MIDI)
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO MIDI with the msg",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
                audio_midi_manager(p_message);
                p_audio_gbl_var->message_processed = TRUE;
              }
            #endif

            #if (L1_MP3 == 1)
              if(audio_mp3_message_switch(p_message)==AUDIO_MP3)
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO MP3 with the msg",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
                audio_mp3_manager(p_message);
                p_audio_gbl_var->message_processed = TRUE;
              }
            #endif

            #if (L1_AAC == 1)
              if(audio_aac_message_switch(p_message)==AUDIO_AAC)
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO AAC with the msg",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
                audio_aac_manager(p_message);
                p_audio_gbl_var->message_processed = TRUE;
              }
            #endif

            #if (L1_AUDIO_DRIVER)
              switch (audio_driver_message_switch(p_message))
              {
                /* The message is for the voice memo play*/
                case AUDIO_DRIVER_SWITCH:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo play manager */
                  audio_driver_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                }
                break;
                case AUDIO_DRIVER_VM_AMR_RECORD_SESSION_SWITCH:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER AMR RECORD with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo record manager */
                  audio_driver_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                }
                break;
                case AUDIO_DRIVER_VM_AMR_PLAY_SESSION_SWITCH:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER AMR PLAY with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo record manager */
                  audio_driver_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                }
                break;
                case AUDIO_DRIVER_MIDI_SESSION_SWITCH:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER MIDI with the msg",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the voice memo record manager */
                  #if (L1_EXT_AUDIO_MGT == 1)
                    audio_driver_manager_for_midi(p_message);
                  #endif
                  p_audio_gbl_var->message_processed=TRUE;
                }
                break;
                case AUDIO_DRIVER_MP3_SESSION_SWITCH:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER MP3 with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the mp3 manager */
                  audio_driver_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                }
                break;
                case AUDIO_DRIVER_AAC_SESSION_SWITCH:
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER AAC with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                  /* process the aac manager */
                  audio_driver_manager(p_message);
                  p_audio_gbl_var->message_processed = TRUE;
                }
                break;

              } /* switch */
            #endif

            switch(audio_mode_message_switch(p_message))
            {
              case AUDIO_FULL_ACCESS_WRITE:
              {
                 AUDIO_SEND_TRACE_PARAM("AUDIO MODE FULL ACCESS WRITE with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                 audio_mode_full_access_write_manager(p_message);
                 p_audio_gbl_var->message_processed = TRUE;
              }
              break;
              case AUDIO_MODE_SAVE:
              {
                 AUDIO_SEND_TRACE_PARAM("AUDIO MODE SAVE with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                 audio_mode_save_manager(p_message);
                 p_audio_gbl_var->message_processed = TRUE;
              }
              break;
              case AUDIO_MODE_LOAD:
              {
                 AUDIO_SEND_TRACE_PARAM("AUDIO MODE LOAD with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                 audio_mode_load_manager(p_message);
                 p_audio_gbl_var->message_processed = TRUE;
              }
              break;
              case AUDIO_SPEAKER_VOLUME:
              {
                 AUDIO_SEND_TRACE_PARAM("AUDIO MODE SPEAKER VOLUME with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                 audio_mode_speaker_volume_manager (p_message);
                 p_audio_gbl_var->message_processed = TRUE;              
              }
              break;
              case AUDIO_STEREO_SPEAKER_VOLUME:
              {
                 AUDIO_SEND_TRACE_PARAM("AUDIO MODE STEREO SPEAKER VOLUME with the msg", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
                 audio_mode_stereo_speaker_volume_manager (p_message);
                 p_audio_gbl_var->message_processed = TRUE;
              }
              break;
            }


#ifdef _WINDOWS
          #if (L1_EXT_AUDIO_MGT == 1)
            // l1 simulator for MIDI
            if(p_message->msg_id==MMI_EXT_AUDIO_MGT_START_REQ || p_message->msg_id==MMI_EXT_AUDIO_MGT_STOP_REQ)
            {
              AUDIO_SEND_TRACE_PARAM("AUDIO MIDI L1 SIMUL with the msg",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
              audio_midi_l1_simulator(received_event,p_message);
              p_audio_gbl_var->message_processed = TRUE;
            }
          #endif
          #if (L1_VOICE_MEMO_AMR == 1)
            // l1 simulator for AMR Play
            if (p_message->msg_id==MMI_VM_AMR_PLAY_START_REQ || p_message->msg_id==MMI_VM_AMR_PLAY_STOP_REQ)
            {
              AUDIO_SEND_TRACE_PARAM("AUDIO VM AMR PLAY L1 SIMUL with the msg",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
              audio_vm_amr_play_l1_simulator(received_event,p_message);
              p_audio_gbl_var->message_processed = TRUE;
            }
            // l1 simulator for VM AMR Record
            if(p_message->msg_id==MMI_VM_AMR_RECORD_START_REQ || p_message->msg_id==MMI_VM_AMR_RECORD_STOP_REQ)
            {
              AUDIO_SEND_TRACE_PARAM("AUDIO VM AMR RECORD L1 SIMUL with the msg",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
              audio_vm_amr_record_l1_simulator(received_event,p_message);
              p_audio_gbl_var->message_processed = TRUE;
            }
          #endif
          #if (L1_MP3 == 1)
            // l1 simulator for MP3
            if(p_message->msg_id==MMI_MP3_START_REQ || p_message->msg_id==MMI_MP3_STOP_REQ || 
               p_message->msg_id==MMI_MP3_PAUSE_REQ || p_message->msg_id==MMI_MP3_RESUME_REQ ||
               p_message->msg_id==MMI_MP3_INFO_REQ )
            {
              AUDIO_SEND_TRACE_PARAM("AUDIO MP3 L1 SIMUL with the msg",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
              audio_mp3_l1_simulator(received_event,p_message);
              p_audio_gbl_var->message_processed = TRUE;
            }
          #endif
          #if (L1_AAC == 1)
            // l1 simulator for AAC
            if(p_message->msg_id==MMI_AAC_START_REQ || p_message->msg_id==MMI_AAC_STOP_REQ || 
               p_message->msg_id==MMI_AAC_PAUSE_REQ || p_message->msg_id==MMI_AAC_RESUME_REQ ||
               p_message->msg_id==MMI_AAC_INFO_REQ )
            {
              AUDIO_SEND_TRACE_PARAM("AUDIO AAC L1 SIMUL with the msg",p_message->msg_id,RV_TRACE_LEVEL_DEBUG_LOW);
              audio_aac_l1_simulator(received_event,p_message);
              p_audio_gbl_var->message_processed = TRUE;
            }
          #endif

#endif // _WINDOWS

          } /* audio_compatibilities_manager */

          if (p_audio_gbl_var->message_processed == FALSE)
          {
            AUDIO_SEND_TRACE_PARAM(" AUDIO ERROR (env). A wrong message is received ",
                           p_message->msg_id, RV_TRACE_LEVEL_ERROR);
          }
          status = (T_RVF_MB_STATUS) rvf_free_buf((T_RVF_BUFFER *)p_message);
          if (status != RVF_GREEN)
          {
            //AUDIO_SEND_TRACE(" AUDIO ERROR (env). A wrong message is deallocated ",
//                           RV_TRACE_LEVEL_ERROR);
          }
        } /* if (p_message != NULL) */
        else
        {
          AUDIO_SEND_TRACE(" AUDIO ERROR (env). Can't read the message received ",
                         RV_TRACE_LEVEL_ERROR);
        }
      } /* if (received_event & AUDIO_TASK_MBOX_EVT_MASK) */

       /* Wait timer simulation event to send notifications */
#ifdef _WINDOWS
    #if (L1_EXT_AUDIO_MGT == 1)
      if(received_event & AUDIO_MIDI_L1_SIMUL_TIMER_EVT_MASK)
        audio_midi_l1_simulator(received_event,p_message);
    #endif
    #if (L1_VOICE_MEMO_AMR == 1)
      if(received_event & AUDIO_VM_AMR_PLAY_L1_SIMUL_TIMER_EVT_MASK)
        audio_vm_amr_play_l1_simulator(received_event,p_message);
      if(received_event & AUDIO_VM_AMR_RECORD_L1_SIMUL_TIMER_EVT_MASK)
        audio_vm_amr_record_l1_simulator(received_event,p_message);
    #endif
    #if (L1_MP3 == 1)
      if(received_event & AUDIO_MP3_L1_SIMUL_TIMER_EVT_MASK)
        audio_mp3_l1_simulator(received_event,p_message);
    #endif
    #if (L1_AAC == 1)
      if(received_event & AUDIO_AAC_L1_SIMUL_TIMER_EVT_MASK)
        audio_aac_l1_simulator(received_event,p_message);
    #endif

#endif

      /* If one of the occured events is unexpected (due to an unassigned */
      /* mailbox), then report an internal error.                         */
#ifdef _WINDOWS
      if(received_event & ~(AUDIO_TASK_MBOX_EVT_MASK | AUDIO_FFS_TIMER_EVT_MASK
    #if (L1_EXT_AUDIO_MGT == 1)
                            | AUDIO_MIDI_L1_SIMUL_TIMER_EVT_MASK
    #endif
    #if (L1_VOICE_MEMO_AMR == 1)
                            | AUDIO_VM_AMR_PLAY_L1_SIMUL_TIMER_EVT_MASK | AUDIO_VM_AMR_RECORD_L1_SIMUL_TIMER_EVT_MASK
    #endif
    #if (L1_MP3 == 1)
                            | AUDIO_MP3_L1_SIMUL_TIMER_EVT_MASK
    #endif
    #if (L1_AAC == 1)
                            | AUDIO_AAC_L1_SIMUL_TIMER_EVT_MASK
    #endif
        ))
#else
      if(received_event & ~(AUDIO_TASK_MBOX_EVT_MASK | AUDIO_FFS_TIMER_EVT_MASK))
#endif
      {
        AUDIO_SEND_TRACE(" AUDIO ERROR (env). One of the occured events is unexpected ",
                           RV_TRACE_LEVEL_ERROR);

        error_occured = TRUE;
      }
    } /* end of While */
    return(RV_INTERNAL_ERR);
  } /****************************** End of audio_task function **************************/


#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x
/********************************************************************************/
/*                                                                              */
/*    Function Name:   rvf_alloc_buf_audio_class1					*/
/*                                                                              */
/*    Purpose:         This function puts traces if the flag is red or yellow   */
/*                                                                              */
/*    Note:            None                                                     */
/*                                                                              */
/*                                                                              */
/********************************************************************************/
T_RVF_MB_STATUS rvf_alloc_buf_audio_class1(UINT32 message_size,
           T_RVF_MSG_ID  msg_id, T_RVF_BUFFER ** pp_msg)
{
        T_RVF_MB_STATUS   mb_status = RVF_GREEN;

	/* rvf_get_msg_buf() function allocates a buffer for
	 * the message and assigns a message id specified in
	 * the third argument                              */
        mb_status = rvf_get_msg_buf(p_audio_gbl_var->mb_external, message_size, msg_id, (T_RVF_MSG **) (pp_msg));

        if (mb_status == RVF_RED)
        {
	        AUDIO_SEND_TRACE(" Not enough memory -- Flag : Red",RV_TRACE_LEVEL_ERROR);

        }

        if (mb_status == RVF_YELLOW)
        {

	        rvf_free_buf((T_RVF_BUFFER *)(*pp_msg));
			AUDIO_SEND_TRACE(" Not enough memory -- Flag : Yellow",RV_TRACE_LEVEL_ERROR);

        }
          return mb_status;
}

/********************************************************************************/
/*                                                                              */
/*    Function Name:   rvf_alloc_buf_audio_class2					*/
/*                                                                              */
/*    Purpose:         This function frees the database buffers, message
 *                     buffers and puts traces according to the flags           */
/*                                                                              */
/*    Note:            None                                                     */
/*                                                                              */
/*                                                                              */
/********************************************************************************/
T_RVF_MB_STATUS rvf_alloc_buf_audio_class2(UINT32 message_size,
                                      T_RVF_MSG_ID  msg_id, T_RVF_BUFFER ** pp_msg, T_RVF_BUFFER* p_database)
{
        T_RVF_MB_STATUS   mb_status = RVF_GREEN;

	/* rvf_get_msg_buf() function allocates a buffer for
	 * the message and assigns a message id specified in
	 * the third argument                              */
        mb_status = rvf_get_msg_buf(p_audio_gbl_var->mb_external, message_size, msg_id, (T_RVF_MSG **) (pp_msg));

      if (mb_status == RVF_RED)
      {
        rvf_free_buf((T_RVF_BUFFER *)p_database);
		AUDIO_SEND_TRACE(" Not enough memory -- Flag : Red",RV_TRACE_LEVEL_ERROR);
      }

      if (mb_status == RVF_YELLOW)
      {

        rvf_free_buf((T_RVF_BUFFER *)p_database);
        rvf_free_buf((T_RVF_BUFFER *)(*pp_msg));
		AUDIO_SEND_TRACE(" Not enough memory -- Flag : Yellow",RV_TRACE_LEVEL_ERROR);
      }
          return mb_status;
}


/********************************************************************************/
/*                                                                              */
/*    Function Name:   rvf_alloc_buf_audio_class3					*/
/*                                                                              */
/*    Purpose:         This function puts traces and puts delays if the flag
 *                     is red                                                   */
/*                                                                              */
/*    Note:            None                                                     */
/*                                                                              */
/*                                                                              */
/********************************************************************************/
T_RVF_MB_STATUS rvf_alloc_buf_audio_class3(UINT32 message_size,
           T_RVF_MSG_ID  msg_id, T_RVF_BUFFER ** pp_msg)
{
        T_RVF_MB_STATUS   mb_status = RVF_GREEN;

	/* rvf_get_msg_buf() function allocates a buffer for
	 * the message and assigns a message id specified in
	 * the third argument                              */
        mb_status = rvf_get_msg_buf(p_audio_gbl_var->mb_external,message_size, msg_id, (T_RVF_MSG **) (pp_msg));

      if (mb_status == RVF_RED)
      {
		  AUDIO_SEND_TRACE(" Not enough memory -- Flag : Red",RV_TRACE_LEVEL_ERROR);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
          return mb_status;

}

#endif//TI_BSP_TO_BE_BASELINED_FOR_N12N5x

#endif /* #ifdef RVM_AUDIO_MAIN_SWE */


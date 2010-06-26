/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_driver.c                                              */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            driver.                                                       */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  09 December 2002  Create                                                */
/*                                                                          */
/*  Author   Frederic Turgis                                                */
/*                                                                          */
/* (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE
#ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

  #include "l1sw.cfg"
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
  #if (L1_MP3 == 1)
    #include "audio/audio_mp3_i.h"
    #include "l1mp3_signa.h"
  #endif
  #if (L1_AAC == 1)
    #include "audio/audio_aac_i.h"
  #endif

  #ifndef _WINDOWS
    // include the usefull L1 header
    #define BOOL_FLAG
    #define CHAR_FLAG
    #include "cust_os.h"
    #include "l1audio_cust.h"
    #include "l1audio_msgty.h"
    #include "l1audio_signa.h"
    #include "l1_signa.h"
    #include "l1audio_defty.h"
  #else
    // include the usefull L1 header
    #define BOOL_FLAG
    #define CHAR_FLAG
    #include "l1audio_const.h"
    #include "l1audio_cust.h"
    #include "l1audio_defty.h"
    #include "l1audio_msgty.h"
    #include "l1audio_signa.h"
    #include "l1_const.h"
    #if (L1_MP3 == 1)
      #include "l1mp3_defty.h"
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
  #if (L1_AAC == 1)
    #include "l1aac_signa.h"
    #include "l1aac_msgty.h"
  #endif
  #ifdef _WINDOWS
    #include "l1mp3_msgty.h"
  #endif

#if (L1_AUDIO_DRIVER)
  #if (L1_VOICE_MEMO_AMR)
    extern T_AUDIO_RET audio_driver_vm_amr_play_manager (T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session);
    extern T_AUDIO_RET audio_driver_vm_amr_record_manager (T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session);
  #endif

  #if (L1_MP3)
    extern T_AUDIO_RET audio_driver_mp3_manager (T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session);
    void audio_driver_send_info_status (T_AUDIO_RET status,
                                        UINT8 status_type,
                                        UINT8 channel_id,
                                        T_L1A_MP3_INFO_CON info,
                                        T_RV_RETURN return_path);
  #endif

  #if (L1_AAC)
    extern T_AUDIO_RET audio_driver_aac_manager (T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session);
    void audio_driver_send_aac_info_status (T_AUDIO_RET status,
                                            UINT8 status_type,
                                            UINT8 channel_id,
                                            T_L1A_AAC_INFO_CON info,
                                            T_RV_RETURN return_path);
  #endif

  #if (L1_EXT_AUDIO_MGT==1)
    extern UINT16  audio_play_buffer[AUDIO_EXT_MIDI_BUFFER_SIZE*2];
    extern T_MIDI_DMA_PARAM midi_buf;
    extern T_AUDIO_RET audio_driver_midi_manager (T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session);
    void audio_driver_manager_for_midi(T_RV_HDR *p_message);
  #endif

  extern T_L1_BT_AUDIO bt_audio;
  UINT8 audio_driver_message_switch(T_RV_HDR *p_message);
  void audio_driver_send_status (T_AUDIO_RET status,
                                 UINT8 status_type,
                                 UINT8 channel_id,
                                 T_RV_RETURN return_path);
  void audio_driver_init_send_status (T_AUDIO_RET status,
                                      UINT8 session_id,
                                      UINT8 channel_id,
                                      T_RV_RETURN return_path);
  void audio_driver_manager(T_RV_HDR *p_message);
  T_AUDIO_RET audio_driver_get_play_buffer(UINT8 channel_id, UINT8 **pp_buffer);
  T_AUDIO_RET audio_driver_play_buffer(UINT8 channel_id, UINT8 *p_buffer);
  T_AUDIO_RET audio_driver_play_midi_buffer(UINT8 channel_id, UINT8 *p_buffer);
  T_AUDIO_RET audio_driver_flush_buffer(UINT8 channel_id, UINT8 *p_buffer);

  #if (L1_MP3)
    T_L1A_MP3_INFO_CON mp3_info;
  #endif

  #if (L1_AAC)
    T_L1A_AAC_INFO_CON aac_info;
  #endif


  UINT8 audio_driver_message_switch(T_RV_HDR *p_message)
  {
    switch (p_message->msg_id)
    {
      case AUDIO_DRIVER_START_SESSION:
      case AUDIO_DRIVER_STOP_SESSION:
      case AUDIO_DRIVER_FREE_SESSION:
      case AUDIO_DRIVER_PAUSE_SESSION:
      case AUDIO_DRIVER_RESUME_SESSION:
      case AUDIO_DRIVER_RESTART_SESSION:
      case AUDIO_DRIVER_INFO_SESSION:
      #if (L1_AAC == 1)
        case AUDIO_DRIVER_INFO_AAC_SESSION:
      #endif
        return(AUDIO_DRIVER_SWITCH);
    #if (L1_VOICE_MEMO_AMR)
      case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
      case MMI_VM_AMR_RECORD_START_CON:
      case MMI_VM_AMR_RECORD_STOP_CON:
        return(AUDIO_DRIVER_VM_AMR_RECORD_SESSION_SWITCH);
      case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
      case MMI_VM_AMR_PLAY_START_CON:
      case MMI_VM_AMR_PLAY_STOP_CON:
      case MMI_VM_AMR_PAUSE_CON:
      case MMI_VM_AMR_RESUME_CON:
        return(AUDIO_DRIVER_VM_AMR_PLAY_SESSION_SWITCH);
    #endif
    #if (L1_EXT_AUDIO_MGT == 1)
      case AUDIO_DRIVER_INIT_MIDI_SESSION:
      case MMI_EXT_AUDIO_MGT_START_CON:
      case MMI_EXT_AUDIO_MGT_STOP_CON:
        return(AUDIO_DRIVER_MIDI_SESSION_SWITCH);
    #endif
    #if (L1_MP3 == 1)
      case AUDIO_DRIVER_INIT_MP3_SESSION:
      case MMI_MP3_START_CON:
      case MMI_MP3_STOP_CON:
      case MMI_MP3_PAUSE_CON:
      case MMI_MP3_RESUME_CON:
      case MMI_MP3_RESTART_CON:
      case MMI_MP3_INFO_CON:
        return(AUDIO_DRIVER_MP3_SESSION_SWITCH);
    #endif
    #if (L1_AAC == 1)
      case AUDIO_DRIVER_INIT_AAC_SESSION:
      case MMI_AAC_START_CON:
      case MMI_AAC_STOP_CON:
      case MMI_AAC_PAUSE_CON:
      case MMI_AAC_RESUME_CON:
      case MMI_AAC_RESTART_CON:
      case MMI_AAC_INFO_CON:
        return(AUDIO_DRIVER_AAC_SESSION_SWITCH);
    #endif


      default:
        return(AUDIO_DRIVER_NONE);
    } // switch
  }

  void audio_driver_send_status (T_AUDIO_RET status,
                                 UINT8       status_type,
                                 UINT8       channel_id,
                                 T_RV_RETURN return_path)
  {
    T_AUDIO_DRIVER_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      // allocate the message buffer
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_DRIVER_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      // If insufficient resources, then report a memory error and abort.
      // and wait until more ressource is given
      if (mb_status == RVF_RED)
      {
        audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    // fill the header of the message + parameters
    p_send_message->os_hdr.msg_id = AUDIO_DRIVER_STATUS_MSG;
    p_send_message->status      = status;
    p_send_message->status_type = status_type;
    p_send_message->channel_id  = channel_id;

    // send message or call callback
    if (return_path.callback_func == NULL)
      rvf_send_msg (return_path.addr_id, p_send_message);
    else
    {
      (*return_path.callback_func)((void *)(p_send_message));
	    rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }


#if (L1_MP3)
  void audio_driver_send_info_status (T_AUDIO_RET status,
                                      UINT8       status_type,
                                      UINT8       channel_id,
                                      T_L1A_MP3_INFO_CON info,
                                      T_RV_RETURN return_path)
  {
    T_AUDIO_DRIVER_INFO_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      //allocate the message buffer
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_DRIVER_INFO_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      // If insufficient resources, then report a memory error and abort.
      // and wait until more ressource is given
      if (mb_status == RVF_RED)
      {
        audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    // fill the header of the message + parameters
    p_send_message->os_hdr.msg_id = AUDIO_DRIVER_STATUS_INFO_MSG;
    p_send_message->status      = status;
    p_send_message->status_type = status_type;
    p_send_message->channel_id  = channel_id;
    p_send_message->info.frequency = info.frequency;
    p_send_message->info.bitrate = info.bitrate;
    p_send_message->info.mpeg_id = info.mpeg_id;
    p_send_message->info.layer = info.layer;
    p_send_message->info.padding = info.padding;
    p_send_message->info.private = info.private;
    p_send_message->info.channel = info.channel;
    p_send_message->info.copyright = info.copyright;
    p_send_message->info.original = info.original;
    p_send_message->info.emphasis = info.emphasis;


    // send message or call callback
    if (return_path.callback_func == NULL)
      rvf_send_msg (return_path.addr_id, p_send_message);
    else
    {
      (*return_path.callback_func)((void *)(p_send_message));
	    rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }
#endif

#if (L1_AAC)
  void audio_driver_send_aac_info_status (T_AUDIO_RET status,
                                          UINT8       status_type,
                                          UINT8       channel_id,
                                          T_L1A_AAC_INFO_CON info,
                                          T_RV_RETURN return_path)
  {
    T_AUDIO_DRIVER_AAC_INFO_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      //allocate the message buffer
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_DRIVER_AAC_INFO_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      // If insufficient resources, then report a memory error and abort.
      // and wait until more ressource is given
      if (mb_status == RVF_RED)
      {
        audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    // fill the header of the message + parameters
    p_send_message->os_hdr.msg_id = AUDIO_DRIVER_STATUS_AAC_INFO_MSG;
    p_send_message->status      = status;
    p_send_message->status_type = status_type;
    p_send_message->channel_id  = channel_id;
    p_send_message->info.frequency = info.frequency;
    p_send_message->info.bitrate = info.bitrate;
    p_send_message->info.channel = info.channel;
    p_send_message->info.aac_format = info.aac_format;

    // send message or call callback
    if (return_path.callback_func == NULL)
      rvf_send_msg (return_path.addr_id, p_send_message);
    else
    {
      (*return_path.callback_func)((void *)(p_send_message));
	    rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }
#endif

  void audio_driver_init_send_status (T_AUDIO_RET status,
                                      UINT8       session_id,
                                      UINT8       channel_id,
                                      T_RV_RETURN return_path)
  {
    T_AUDIO_DRIVER_INIT_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      // allocate the message buffer
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_DRIVER_INIT_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      // If insufficient resources, then report a memory error and abort.
      // and wait until more ressource is given
      if (mb_status == RVF_RED)
      {
        audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    // fill the header of the message
    p_send_message->os_hdr.msg_id = AUDIO_DRIVER_INIT_STATUS_MSG;

    // fill the status parameters
    p_send_message->status      = status;
    p_send_message->session_id  = session_id;
    p_send_message->channel_id  = channel_id;

    // send message or call callback
    if (return_path.callback_func == NULL)
      rvf_send_msg (return_path.addr_id, p_send_message);
    else
    {
      (*return_path.callback_func)((void *)(p_send_message));
	    rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }

  void audio_driver_manager(T_RV_HDR *p_message)
  {
    T_AUDIO_DRIVER_SESSION *p_session = NULL;
    //T_AUDIO_DRIVER_PARAMETER *driver_parameter = NULL;
    T_AUDIO_DRIVER_PARAMETER driver_parameter = {0, 0}; // This added to remove lint-warnings
    T_RV_RETURN return_path;
    T_RVF_MB_STATUS mb_status;
    T_RVF_RET return_val;
    UINT8 session_id = 0, channel_id, state, j;
    UINT16  status;

    // initialize return_path to default values
    return_path.callback_func=NULL;
    return_path.addr_id=0;

    // To remove the lint-warnings thrown
    p_session = &(p_audio_gbl_var->audio_driver_session[0]);

    // Initialize driver_parameter 
    //driver_parameter = &parameter;

    // find state + extract information:channel_id or session_id + driver_parameter
    switch (p_message->msg_id)
    {
      // start/stop session messages have channel_id so we know the driver_session
      case AUDIO_DRIVER_START_SESSION:
      case AUDIO_DRIVER_STOP_SESSION:
      case AUDIO_DRIVER_FREE_SESSION:
      case AUDIO_DRIVER_PAUSE_SESSION:
      case AUDIO_DRIVER_RESUME_SESSION:
      case AUDIO_DRIVER_RESTART_SESSION:
      case AUDIO_DRIVER_INFO_SESSION:
      #if (L1_AAC == 1)
        case AUDIO_DRIVER_INFO_AAC_SESSION:
      #endif
        channel_id = ((T_AUDIO_DRIVER_HANDLE_SESSION *)p_message)->channel_id;
        p_session  = &(p_audio_gbl_var->audio_driver_session[channel_id]);
        state      = p_session->session_info.state;
        if (p_message->msg_id != AUDIO_DRIVER_STOP_SESSION)
          return_path = ((T_AUDIO_DRIVER_HANDLE_SESSION *)p_message)->return_path;
        else
          return_path = p_session->session_req.return_path;
      break;
      // messages which contain only session_id, must look for it in ACTIVE driver_session
      default:
      {
        switch (p_message->msg_id)
        {
        #if (L1_VOICE_MEMO_AMR)
          case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
            session_id       = AUDIO_VM_AMR_RECORD_SESSION_ID;
            driver_parameter = ((T_AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION *)p_message)->driver_parameter;
            return_path      = ((T_AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION *)p_message)->return_path;
          break;
          case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
            session_id       = AUDIO_VM_AMR_PLAY_SESSION_ID;
            driver_parameter = ((T_AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION *)p_message)->driver_parameter;
            return_path      = ((T_AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION *)p_message)->return_path;
          break;
          case MMI_VM_AMR_RECORD_START_CON:
          case MMI_VM_AMR_RECORD_STOP_CON:
            session_id = AUDIO_VM_AMR_RECORD_SESSION_ID;
          break;
          case MMI_VM_AMR_PLAY_START_CON:
          case MMI_VM_AMR_PLAY_STOP_CON:
          case MMI_VM_AMR_PAUSE_CON:
          case MMI_VM_AMR_RESUME_CON:
            session_id = AUDIO_VM_AMR_PLAY_SESSION_ID;
          break;
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
            session_id       = AUDIO_EXT_MIDI_SESSION_ID;
            driver_parameter = ((T_AUDIO_DRIVER_INIT_MIDI_SESSION *)p_message)->driver_parameter;
            return_path      = ((T_AUDIO_DRIVER_INIT_MIDI_SESSION *)p_message)->return_path;
          break;
          case MMI_EXT_AUDIO_MGT_START_CON:
          case MMI_EXT_AUDIO_MGT_STOP_CON:
            session_id = AUDIO_EXT_MIDI_SESSION_ID;
          break;
        #endif

        #if (L1_MP3)
          case AUDIO_DRIVER_INIT_MP3_SESSION:
            session_id       = AUDIO_MP3_SESSION_ID;
            driver_parameter = ((T_AUDIO_DRIVER_INIT_MP3_SESSION *)p_message)->driver_parameter;
            return_path      = ((T_AUDIO_DRIVER_INIT_MP3_SESSION *)p_message)->return_path;
          break;
          case MMI_MP3_START_CON:
          case MMI_MP3_STOP_CON:
          case MMI_MP3_PAUSE_CON:
          case MMI_MP3_RESUME_CON:
          case MMI_MP3_RESTART_CON:
          case MMI_MP3_INFO_CON:
            session_id = AUDIO_MP3_SESSION_ID;
          break;
        #endif

        #if (L1_AAC)
          case AUDIO_DRIVER_INIT_AAC_SESSION:
            session_id       = AUDIO_AAC_SESSION_ID;
            driver_parameter = ((T_AUDIO_DRIVER_INIT_AAC_SESSION *)p_message)->driver_parameter;
            return_path      = ((T_AUDIO_DRIVER_INIT_AAC_SESSION *)p_message)->return_path;
          break;
          case MMI_AAC_START_CON:
          case MMI_AAC_STOP_CON:
          case MMI_AAC_PAUSE_CON:
          case MMI_AAC_RESUME_CON:
          case MMI_AAC_RESTART_CON:
          case MMI_AAC_INFO_CON:
            session_id = AUDIO_AAC_SESSION_ID;
            AUDIO_SEND_TRACE_PARAM("MMI_AAC_START_CON < AUDIO_AAC_SESSION_ID", 0, RV_TRACE_LEVEL_ERROR);            
          break;
        #endif

        }

        // initialize channel_id to browse all driver channels
        channel_id = 0;
        state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;

        // look for an active session, which session_id matches the one from the message
        while ( (channel_id < AUDIO_DRIVER_MAX_CHANNEL)&&
                ((p_audio_gbl_var->audio_driver_session[channel_id].session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_INIT)||
                (p_audio_gbl_var->audio_driver_session[channel_id].session_req.session_id != session_id)) )
        {
          channel_id++;
        }
        // if channel_id < MAX_CHANNEL, we found an active channel so we can derive channel_id + state
        if (channel_id < AUDIO_DRIVER_MAX_CHANNEL)
        {
          p_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);
          state = p_session->session_info.state;
          // we get return_path from session if it was not included in the message
          if ( (return_path.addr_id == 0) && (return_path.callback_func == NULL) )
           return_path = p_session->session_req.return_path;
        }
      }
      break; // default
    }

    switch (state)
    {
      case AUDIO_DRIVER_CHANNEL_WAIT_INIT:
      {
        switch (p_message->msg_id)
        {
        #if (L1_VOICE_MEMO_AMR)
          case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
          case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
        #endif
        #if (L1_MP3 == 1)
          case AUDIO_DRIVER_INIT_MP3_SESSION:
        #endif
        #if (L1_AAC == 1)
          case AUDIO_DRIVER_INIT_AAC_SESSION:
        #endif

          {
            T_AUDIO_RET result = 0;

            // Find a free channel
            channel_id = 0;
            while ( (channel_id < AUDIO_DRIVER_MAX_CHANNEL) && 
		    (p_audio_gbl_var->audio_driver_session[channel_id].session_info.state != AUDIO_DRIVER_CHANNEL_WAIT_INIT))
              channel_id++;

            if (channel_id == AUDIO_DRIVER_MAX_CHANNEL)
            {
              AUDIO_SEND_TRACE("no driver channel available", RV_TRACE_LEVEL_DEBUG_LOW);
              audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
              return;
            }

            AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER: open channel", channel_id, RV_TRACE_LEVEL_DEBUG_LOW);

            // get session descriptor and fill REQ parameters
            p_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);
            p_session->session_req.session_id     = session_id;
            p_session->session_req.size           = driver_parameter.buffer_size << 1;// we request 16-bit words buffers
            p_session->session_req.nb_buffer      = driver_parameter.nb_buffer;
            // fill return_path parameters, may be used if next message is STOP_SESSION
            p_session->session_req.return_path.callback_func = return_path.callback_func;
            p_session->session_req.return_path.addr_id = return_path.addr_id;

            /************************************************************/
            /* the driver must allocate the RAM buffers pointer         */
            /************************************************************/
            mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                     sizeof(T_AUDIO_DRIVER_BUFFER_INFO)*p_session->session_req.nb_buffer,
                                     (T_RVF_BUFFER **) (&p_session->session_info.buffer));

            // If insufficient resources, then report a memory error and abort.
            if (mb_status == RVF_RED)
            {
              audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
              audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
              return;
            }

            #if (L1_EXT_AUDIO_MGT==1)
               p_audio_gbl_var->midi.midi_play_array[0]=midi_buf.audio_play_buffer;
               p_audio_gbl_var->midi.midi_play_array[1]=&(midi_buf.audio_play_buffer[(p_session->session_req.size)/2]);
            #endif

            /************************************************************/
            /* the driver must allocate the RAM buffers                 */
            /************************************************************/
            for (j = 0; j < p_session->session_req.nb_buffer; j++)
            {
              #if (L1_EXT_AUDIO_MGT != 1)
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_audio_ffs,
                                       p_session->session_req.size,
                                       (T_RVF_BUFFER **) (&p_session->session_info.buffer[j].p_start_pointer));

              // If insufficient resources, then report a memory error and abort.
              if (mb_status == RVF_RED)
              {
                INT8 i;
                // free already allocated buffers + buffer pointer
                if (j > 0)
                {
                  for (i = j - 1; i >= 0; i--)
                    rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer[i].p_start_pointer);
                  }
                  rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);
                  audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
                  audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
                  return;
                }
              #else
                /* L1_EXT_AUDIO_MGT : the p_start_pointer address is the address of the internal RAM array */
                /* if j=0 -> p_start_pointer = the begin of the internal RAM array                */
                /* if j=1 -> p_start_pointer = the middle of the internal RAM array                */
                p_session->session_info.buffer[j].p_start_pointer=p_audio_gbl_var->midi.midi_play_array[j];
              #endif

              // initialize parameters
              p_session->session_info.buffer[j].size = p_session->session_req.size;

              AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER: allocate buffer",
                p_session->session_info.buffer[j].p_start_pointer, RV_TRACE_LEVEL_DEBUG_LOW);
            }

            // info parameters (state must be changed before driver specific functions call
            p_session->session_info.index_l1        = 0;
            p_session->session_info.index_appli     = 0;
            p_session->session_info.stop_request    = 0;
            p_session->session_info.pause_request   = 0;
            p_session->session_info.resume_request  = 0;
            p_session->session_info.restart_request = 0;

            // conversion of parameters + prepare l1 start message
            switch(p_message->msg_id)
            {
            #if (L1_VOICE_MEMO_AMR)
              case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
                result=audio_driver_vm_amr_record_manager(p_message,p_session);
              break;
              case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
                result=audio_driver_vm_amr_play_manager(p_message,p_session);
              break;
            #endif
            #if (L1_EXT_AUDIO_MGT == 1)
              case AUDIO_DRIVER_INIT_MIDI_SESSION:
                result=audio_driver_midi_manager(p_message,p_session);
              break;
            #endif
            #if (L1_MP3 == 1)
              case AUDIO_DRIVER_INIT_MP3_SESSION:
                result=audio_driver_mp3_manager(p_message,p_session);
              break;
            #endif
            #if (L1_AAC == 1)
              case AUDIO_DRIVER_INIT_AAC_SESSION:
                result=audio_driver_aac_manager(p_message,p_session);
              break;
            #endif
            }

            // check L1 msg allocation was successfull
            if (result != AUDIO_OK)
            {
              AUDIO_SEND_TRACE("AUDIO DRIVER MANAGER: L1 msg allocation failed", RV_TRACE_LEVEL_DEBUG_LOW);
              audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
              return;
            }

            // state (must be changed after driver specific functions calls)
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_START;

            audio_driver_init_send_status(AUDIO_OK, session_id, channel_id, return_path);
          } //case AUDIO_DRIVER_INIT_..._SESSION:
          break;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_STOP_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_STOP_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_PAUSE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_PAUSE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_PAUSE_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_RESUME_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESUME_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESUME_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_RESTART_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESTART_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESTART_STATUS, channel_id, return_path);
          break;
#if (L1_MP3)
          case AUDIO_DRIVER_INFO_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
            mp3_info.bitrate  = 0;
            mp3_info.channel  = 0;
            mp3_info.copyright= FALSE;
            mp3_info.emphasis = 0 ;
            mp3_info.frequency= 0;
            mp3_info.layer    = 0;
            mp3_info.mpeg_id  = 0;
            mp3_info.original = FALSE;
            mp3_info.padding  = FALSE;
            mp3_info.private   = 0;

            audio_driver_send_info_status(AUDIO_ERROR, AUDIO_INFO_STATUS, channel_id, mp3_info, return_path);
          break;
#endif
          #if (L1_AAC)
            case AUDIO_DRIVER_AAC_INFO_SESSION:
              audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
              aac_info.bitrate  = 0;
              aac_info.channel  = 0;
              aac_info.frequency= 0;
              aac_info.aac_format = AUDIO_AAC_ADTS;

              audio_driver_send_aac_info_status(AUDIO_ERROR, AUDIO_AAC_INFO_STATUS, channel_id, aac_info, return_path);
            break;
          #endif

        } // switch (p_message->msg_id)
      } //case AUDIO_DRIVER_CHANNEL_WAIT_INIT:
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_START:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_START_SESSION:
          {
            T_AUDIO_RET result=0;

            // fill notification parameters
            p_session->session_req.return_path.callback_func = return_path.callback_func;
            p_session->session_req.return_path.addr_id = return_path.addr_id;

            // send l1 message
            switch(p_session->session_req.session_id)
            {
            #if (L1_VOICE_MEMO_AMR)
              case AUDIO_VM_AMR_RECORD_SESSION_ID:
                result=audio_driver_vm_amr_record_manager(p_message,p_session);
              break;
              case AUDIO_VM_AMR_PLAY_SESSION_ID:
                result=audio_driver_vm_amr_play_manager(p_message,p_session);
              break;
            #endif
            #if (L1_EXT_AUDIO_MGT == 1)
              case AUDIO_EXT_MIDI_SESSION_ID:
                result=audio_driver_midi_manager(p_message,p_session);
              break;
            #endif
             #if (L1_MP3 == 1)
              case AUDIO_MP3_SESSION_ID:
                result=audio_driver_mp3_manager(p_message,p_session);
              break;
            #endif
             #if (L1_AAC == 1)
              case AUDIO_AAC_SESSION_ID:
                result=audio_driver_aac_manager(p_message,p_session);
              break;
            #endif
            }

            if (result != AUDIO_OK)
            {
              audio_driver_error_trace(AUDIO_ERROR_L1_START_EVENT);
              audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
              return;
            }

            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_START_CON;
          }
          break;
          case AUDIO_DRIVER_STOP_SESSION:
          {
            #if (L1_EXT_AUDIO_MGT!=1)
              // deallocate buffers
              for(j=0; j<p_session->session_req.nb_buffer; j++)
              {
                return_val=rvf_free_buf((T_RVF_BUFFER *)(p_session->session_info.buffer[j].p_start_pointer));

                AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER MANAGER: deallocate buffer stop session",
                  j, RV_TRACE_LEVEL_DEBUG_LOW);

                if (return_val != RVF_OK)
                  AUDIO_SEND_TRACE_PARAM("can't deallocate buffer", j, RV_TRACE_LEVEL_ERROR);
              }
            #endif

            // Deallocate buffers pointer
            rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);

            // deallocate l1 message
            audio_deallocate_l1_message(p_session->session_req.p_l1_send_message);

            // send status OK
            audio_driver_send_status(AUDIO_OK,AUDIO_STOP_STATUS,channel_id,return_path);

            // change state
            p_session->session_info.state=AUDIO_DRIVER_CHANNEL_WAIT_INIT;
          }
          break;
        #if (L1_VOICE_MEMO_AMR)
          case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
          case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
        #endif
        #if (L1_MP3 == 1)
          case AUDIO_DRIVER_INIT_MP3_SESSION:
        #endif
        #if (L1_AAC == 1)
          case AUDIO_DRIVER_INIT_AAC_SESSION:
        #endif
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, 0, return_path);
          break;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_PAUSE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_PAUSE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_PAUSE_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_RESUME_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESUME_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESUME_STATUS, channel_id, return_path);
          case AUDIO_DRIVER_RESTART_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESTART_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESTART_STATUS, channel_id, return_path);
          break;
#if (L1_MP3)
          case AUDIO_DRIVER_INFO_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
            mp3_info.bitrate  = 0;
            mp3_info.channel  = 0;
            mp3_info.copyright= FALSE;
            mp3_info.emphasis = 0 ;
            mp3_info.frequency= 0;
            mp3_info.layer    = 0;
            mp3_info.mpeg_id  = 0;
            mp3_info.original = FALSE;
            mp3_info.padding  = FALSE;
            mp3_info.private   = 0;
            audio_driver_send_info_status(AUDIO_ERROR, AUDIO_INFO_STATUS, channel_id, mp3_info, return_path);
          break;
#endif
#if (L1_AAC)
          case AUDIO_DRIVER_AAC_INFO_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
            aac_info.bitrate  = 0;
            aac_info.channel  = 0;
            aac_info.frequency= 0;
            aac_info.aac_format = AUDIO_AAC_ADTS;
            audio_driver_send_aac_info_status(AUDIO_ERROR, AUDIO_AAC_INFO_STATUS, channel_id, aac_info, return_path);
          break;
#endif


        } // switch
      } //case AUDIO_DRIVER_CHANNEL_WAIT_START:
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON:
      {
        switch (p_message->msg_id)
        {


#if (L1_MP3 == 1)

      // Stop confirmation is received before Start confirmation in MP3 case
      // as DSP has not found SYNC in whole file

          case MMI_MP3_STOP_CON:
          {
	     // Deallocate buffers pointer
	        rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);
  	        status = ((T_L1A_MP3_CON *)p_message)->error_code;

            /* Other than critical failures the status flag will not be updated in L1
             * Ex : SYNC not found - warning from DSP (In this case MMI receives 0=AUDIO_OK)
             * In this special warning case, need to send AUDIO ERROR to MMI CQ :OMAPS00110012
             */
            if(status == 0)
              audio_driver_send_status (AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id,
                                           return_path);
            else
              audio_driver_send_status (status, AUDIO_STOP_STATUS, channel_id,
                                           return_path);

            AUDIO_SEND_TRACE_PARAM("MP3_STOP_CON Status", status, RV_TRACE_LEVEL_ERROR);


		   // change state
		    p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;
	      }
          break;
#endif

#if (L1_AAC == 1)
          case MMI_AAC_STOP_CON:
          {
            // Deallocate buffers pointer
            rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);
            status = ((T_L1A_AAC_CON *)p_message)->error_code;

            /* Other than critical failures the status flag will not be updated in L1
             * Ex : SYNC not found - warning from DSP (In this case MMI receives 0=AUDIO_OK)
             * In this special warning case, need to send AUDIO ERROR to MMI CQ :OMAPS00110012
             */
            if(status == 0)
              audio_driver_send_status (AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id,
                                           return_path);
            else
              audio_driver_send_status (status, AUDIO_STOP_STATUS, channel_id,
                                           return_path);

            AUDIO_SEND_TRACE_PARAM("AAC_STOP_CON Status", status, RV_TRACE_LEVEL_ERROR);

            // change state
             p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;
          }
          break;


#endif
        #if (L1_AAC == 1)
          case MMI_AAC_START_CON:
            AUDIO_SEND_TRACE_PARAM("MMI AAC_START_CON Status", 0, RV_TRACE_LEVEL_ERROR);
			p_audio_gbl_var->audio_aac.stop_aac_true = FALSE;
            
            audio_driver_send_status (AUDIO_OK, AUDIO_START_STATUS, channel_id,
                                           return_path);
           
// change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
            p_audio_gbl_var->audio_aac.aac_format = ((T_L1A_AAC_START_CON *)p_message)->aac_format;
           break;
        #endif 
        #if (L1_VOICE_MEMO_AMR)
          case MMI_VM_AMR_RECORD_START_CON:
          case MMI_VM_AMR_PLAY_START_CON:
            AUDIO_SEND_TRACE_PARAM("AMR_START_CON Status", 0, RV_TRACE_LEVEL_ERROR);
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case MMI_EXT_AUDIO_MGT_START_CON:
        #endif
        #if (L1_MP3 == 1)
          case MMI_MP3_START_CON:
        AUDIO_SEND_TRACE_PARAM("MP3_CON Status", 0, RV_TRACE_LEVEL_ERROR);
		//OMAPS00175540
	 if(bt_audio.connected_status == TRUE)
		  audio_driver_send_status (AUDIO_OK, AUDIO_START_STATUS, channel_id,
                                           return_path);
        #endif
          
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
          break;

          case AUDIO_DRIVER_STOP_SESSION:
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_STOP;
          break;
          case AUDIO_DRIVER_PAUSE_SESSION:
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_PAUSE;
          break;
        #if (L1_VOICE_MEMO_AMR)
          case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
          case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
        #endif
        #if (L1_MP3 == 1)
          case AUDIO_DRIVER_INIT_MP3_SESSION:
        #endif
        #if (L1_AAC == 1)
          case AUDIO_DRIVER_INIT_AAC_SESSION:
        #endif
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
          break;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_RESUME_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESUME_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESUME_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_RESTART_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESTART_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESTART_STATUS, channel_id, return_path);
          break;
#if (L1_MP3)
          case AUDIO_DRIVER_INFO_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
            mp3_info.bitrate  = 0;
            mp3_info.channel  = 0;
            mp3_info.copyright= FALSE;
            mp3_info.emphasis = 0 ;
            mp3_info.frequency= 0;
            mp3_info.layer    = 0;
            mp3_info.mpeg_id  = 0;
            mp3_info.original = FALSE;
            mp3_info.padding  = FALSE;
            mp3_info.private   = 0;
            audio_driver_send_info_status(AUDIO_ERROR, AUDIO_INFO_STATUS, channel_id, mp3_info, return_path);
          break;
#endif
#if (L1_AAC)
          case AUDIO_DRIVER_AAC_INFO_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
            aac_info.bitrate  = 0;
            aac_info.channel  = 0;
            aac_info.frequency= 0;
            aac_info.aac_format = AUDIO_AAC_ADTS;
            audio_driver_send_aac_info_status(AUDIO_ERROR, AUDIO_AAC_INFO_STATUS, channel_id, aac_info, return_path);
          break;
#endif
        } // switch
      } //case AUDIO_DRIVER_CHANNEL_WAIT_START_CON:
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_STOP:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_STOP_SESSION:
          {
            // 1st stop request
            if (p_session->session_info.stop_request == 0)
            {
              T_AUDIO_RET result=0;

              // send l1 message STOP_REQ
              switch(p_session->session_req.session_id)
              {
              #if (L1_VOICE_MEMO_AMR)
                case AUDIO_VM_AMR_RECORD_SESSION_ID:
                  result=audio_driver_vm_amr_record_manager(p_message,p_session);
                break;
                case AUDIO_VM_AMR_PLAY_SESSION_ID:
                  result=audio_driver_vm_amr_play_manager(p_message,p_session);
                break;
              #endif
              #if (L1_EXT_AUDIO_MGT == 1)
                case AUDIO_EXT_MIDI_SESSION_ID:
                  result=audio_driver_midi_manager(p_message,p_session);
                break;
              #endif
              #if (L1_MP3 == 1)
                case AUDIO_MP3_SESSION_ID:
                  result=audio_driver_mp3_manager(p_message,p_session);
                break;
              #endif
              #if (L1_AAC == 1)
                case AUDIO_AAC_SESSION_ID:
                  result=audio_driver_aac_manager(p_message,p_session);
                break;
              #endif
              }

              // allocate or send L1 msg went wrong
              if (result != AUDIO_OK)
              {
                audio_driver_error_trace(AUDIO_ERROR_L1_STOP_EVENT);
                audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
                return;
              }

              // STOP has been requested, no longer accept it
              p_session->session_info.stop_request = 1;
            }
            else
            {
              audio_driver_error_trace(AUDIO_ERROR_STOP_EVENT);
              audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
            }
          }
          break;

          case AUDIO_DRIVER_PAUSE_SESSION:
          {
            // 1st pause request
            if (p_session->session_info.pause_request == 0)
            {
              T_AUDIO_RET result=0;

              // send l1 message PAUSE_REQ
              switch(p_session->session_req.session_id)
              {
              #if (L1_VOICE_MEMO_AMR)
               case AUDIO_VM_AMR_PLAY_SESSION_ID:
                  result=audio_driver_vm_amr_play_manager(p_message,p_session);
                break;
              #endif
              #if (L1_MP3 == 1)
                case AUDIO_MP3_SESSION_ID:
                  result=audio_driver_mp3_manager(p_message,p_session);
                break;
              #endif
              #if (L1_AAC == 1)
                case AUDIO_AAC_SESSION_ID:
                  result=audio_driver_aac_manager(p_message,p_session);
                break;
              #endif
              }

              // allocate or send L1 msg went wrong
              if (result != AUDIO_OK)
              {
                audio_driver_error_trace(AUDIO_ERROR_L1_PAUSE_EVENT);
                audio_driver_send_status(AUDIO_ERROR, AUDIO_PAUSE_STATUS, channel_id, return_path);
                return;
              }

              // PAUSE has been requested, no longer accept it
              p_session->session_info.pause_request = 1;
              p_session->session_info.resume_request = 0;
            }
            else
            {
              audio_driver_error_trace(AUDIO_ERROR_PAUSE_EVENT);
              audio_driver_send_status(AUDIO_ERROR, AUDIO_PAUSE_STATUS, channel_id, return_path);
            }
          }
          break;
          case AUDIO_DRIVER_RESUME_SESSION:
          {
            // 1st resume request
            if (p_session->session_info.resume_request == 0)
            {
              T_AUDIO_RET result=0;

              // send l1 message RESUME_REQ
              switch(p_session->session_req.session_id)
              {
              #if (L1_VOICE_MEMO_AMR)
               case AUDIO_VM_AMR_PLAY_SESSION_ID:
                  result=audio_driver_vm_amr_play_manager(p_message,p_session);
                break;
              #endif
              #if (L1_MP3 == 1)
                case AUDIO_MP3_SESSION_ID:
                  result=audio_driver_mp3_manager(p_message,p_session);
                break;
              #endif
              #if (L1_AAC == 1)
                case AUDIO_AAC_SESSION_ID:
                  result=audio_driver_aac_manager(p_message,p_session);
                break;
              #endif
              }

              // allocate or send L1 msg went wrong
              if (result != AUDIO_OK)
              {
                audio_driver_error_trace(AUDIO_ERROR_L1_RESUME_EVENT);
                audio_driver_send_status(AUDIO_ERROR, AUDIO_RESUME_STATUS, channel_id, return_path);
                return;
              }

              // RESUME has been requested, no longer accept it
              p_session->session_info.resume_request = 1;
              p_session->session_info.pause_request = 0;
            }
            else
            {
              audio_driver_error_trace(AUDIO_ERROR_RESUME_EVENT);
              audio_driver_send_status(AUDIO_ERROR, AUDIO_RESUME_STATUS, channel_id, return_path);
            }
          }
          break;

          case AUDIO_DRIVER_RESTART_SESSION:
          {
            // 1st restart request
            if (p_session->session_info.restart_request == 0)
            {
              T_AUDIO_RET result=0;

              // send l1 message RESTART_REQ
              switch(p_session->session_req.session_id)
              {
              #if (L1_AAC == 1)
                case AUDIO_AAC_SESSION_ID:
                  result=audio_driver_aac_manager(p_message,p_session);
                break;
              #endif
              }

              // allocate or send L1 msg went wrong
              if (result != AUDIO_OK)
              {
                audio_driver_error_trace(AUDIO_ERROR_L1_RESTART_EVENT);
                audio_driver_send_status(AUDIO_ERROR, AUDIO_RESTART_STATUS, channel_id, return_path);
                return;
              }

              // RESTART has been requested, no longer accept it
              p_session->session_info.restart_request = 1;
            }
            else
            {
              audio_driver_error_trace(AUDIO_ERROR_RESTART_EVENT);
              audio_driver_send_status(AUDIO_ERROR, AUDIO_RESTART_STATUS, channel_id, return_path);
            }
          }
          break;
          case AUDIO_DRIVER_INFO_SESSION:
          {
            #if (L1_MP3 == 1)
            // 1st pause request
            if (p_session->session_info.info_request == 0)
            {
              T_AUDIO_RET result=0;

              // send l1 message INFO_REQ
              switch(p_session->session_req.session_id)
              {
                case AUDIO_MP3_SESSION_ID:
                  result=audio_driver_mp3_manager(p_message,p_session);
                break;
              }

              // allocate or send L1 msg went wrong
              if (result != AUDIO_OK)
              {
                audio_driver_error_trace(AUDIO_ERROR_L1_INFO_EVENT);
                mp3_info.bitrate  = 0;
                mp3_info.channel  = 0;
                mp3_info.copyright= FALSE;
                mp3_info.emphasis = 0 ;
                mp3_info.frequency= 0;
                mp3_info.layer    = 0;
                mp3_info.mpeg_id  = 0;
                mp3_info.original = FALSE;
                mp3_info.padding  = FALSE;
                mp3_info.private   = 0;
                audio_driver_send_info_status(AUDIO_ERROR, AUDIO_INFO_STATUS, channel_id, mp3_info, return_path);
                return;
              }
              // INFO has been requested, no longer accept it
              p_session->session_info.info_request = 1;
            }
            else
            {
              audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
              mp3_info.bitrate  = 0;
              mp3_info.channel  = 0;
              mp3_info.copyright= FALSE;
              mp3_info.emphasis = 0 ;
              mp3_info.frequency= 0;
              mp3_info.layer    = 0;
              mp3_info.mpeg_id  = 0;
              mp3_info.original = FALSE;
              mp3_info.padding  = FALSE;
              mp3_info.private   = 0;
              audio_driver_send_info_status(AUDIO_ERROR, AUDIO_INFO_STATUS, channel_id, mp3_info, return_path);
            }
          #endif

          }
          break;
      #if (L1_AAC == 1)
        case AUDIO_DRIVER_INFO_AAC_SESSION:
          {
            // 1st pause request
            if (p_session->session_info.info_request == 0)
            {
              T_AUDIO_RET result=0;

              // send l1 message INFO_REQ
              switch(p_session->session_req.session_id)
              {
                case AUDIO_AAC_SESSION_ID:
                  result=audio_driver_aac_manager(p_message,p_session);
                break;
              }

              // allocate or send L1 msg went wrong
              if (result != AUDIO_OK)
              {
                audio_driver_error_trace(AUDIO_ERROR_L1_INFO_EVENT);
                aac_info.bitrate  = 0;
                aac_info.channel  = 0;
                aac_info.frequency= 0;
                aac_info.aac_format = AUDIO_AAC_ADTS;
                audio_driver_send_aac_info_status(AUDIO_ERROR, AUDIO_AAC_INFO_STATUS, channel_id, aac_info, return_path);
                return;
              }

              // INFO has been requested, no longer accept it
              p_session->session_info.info_request = 1;
            }
            else
            {
              audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
              aac_info.bitrate  = 0;
              aac_info.channel  = 0;
              aac_info.frequency= 0;
               aac_info.aac_format = AUDIO_AAC_ADTS;
              audio_driver_send_aac_info_status(AUDIO_ERROR, AUDIO_AAC_INFO_STATUS, channel_id, aac_info, return_path);

            }
          }
          break;
        #endif
        #if (L1_VOICE_MEMO_AMR)
          case MMI_VM_AMR_RECORD_STOP_CON:
          {
            // send last notification
            T_AUDIO_DRIVER_LAST_NOTIFICATION *p_status_message;

            // Allocate the Riviera buffer
            mb_status = RVF_RED;
            while (mb_status == RVF_RED)
            {
              mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                       sizeof (T_AUDIO_DRIVER_LAST_NOTIFICATION),
                                       (T_RVF_BUFFER **) (&p_status_message));
              // If insufficient resources, then report a memory error and abort.
              if (mb_status == RVF_RED)
              {
                // the memory is insufficient to continue the non regression test
                audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
                rvf_delay(RVF_MS_TO_TICKS(1000));
              }
            }

            // Fill the message ID
            p_status_message->header.msg_id = AUDIO_DRIVER_LAST_NOTIFICATION_MSG;

            // fill parameters
            p_status_message->channel_id = channel_id;
            p_status_message->recorded_size = ((T_MMI_VM_AMR_RECORD_CON *)p_message)->recorded_size - SC_VM_AMR_END_MASK_SIZE;
            p_status_message->p_buffer   =
              (UINT16 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer;

            if (return_path.callback_func == NULL)
              rvf_send_msg (return_path.addr_id, p_status_message);
            else
            {
              // call the callback function
              (*(return_path.callback_func))((void *)(p_status_message));
              rvf_free_buf((T_RVF_BUFFER *)p_status_message);
            }

            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_FREE;
          }
          break;
        #endif // #if (L1_VOICE_MEMO_AMR)
        #if (L1_VOICE_MEMO_AMR)
          case MMI_VM_AMR_PLAY_STOP_CON:
        #endif
        #if (L1_MP3 == 1)
          case MMI_MP3_STOP_CON:
        #endif
        #if (L1_AAC == 1)
          case MMI_AAC_STOP_CON:
        #endif
          {
            #if (L1_EXT_AUDIO_MGT != 1)
              // deallocate buffers
              for (j = 0; j < p_session->session_req.nb_buffer; j++)
              {
                return_val = rvf_free_buf ((T_RVF_BUFFER *) (p_session->session_info.buffer[j].p_start_pointer));

                AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER MANAGER: deallocate buffer stop con", j, RV_TRACE_LEVEL_DEBUG_LOW);

                if (return_val != RVF_OK)
                  AUDIO_SEND_TRACE_PARAM("can't deallocate buffer", j, RV_TRACE_LEVEL_ERROR);
              }
            #endif

            // Deallocate buffers pointer
            rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);

            #if ((L1_MP3 == 1) || (L1_AAC == 1))
              #if (L1_MP3 == 1)
                status = ((T_L1A_MP3_CON *)p_message)->error_code;
              #endif
              #if (L1_AAC == 1)
                status = ((T_L1A_AAC_CON *)p_message)->error_code;
              #endif
            #else
              status = AUDIO_OK;
            #endif

            audio_driver_send_status (status, AUDIO_STOP_STATUS, channel_id,
                                      return_path);
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;
          }
          break;
        #if (L1_VOICE_MEMO_AMR)
          case MMI_VM_AMR_PAUSE_CON:
        #endif
        #if (L1_MP3 == 1)
          case MMI_MP3_PAUSE_CON:
        #endif
        #if (L1_AAC == 1)
          case MMI_AAC_PAUSE_CON:
        #endif
          {
            audio_driver_send_status (AUDIO_OK, AUDIO_PAUSE_STATUS, channel_id,
                                      return_path);
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
          }
          break;
         #if (L1_VOICE_MEMO_AMR)
          case MMI_VM_AMR_RESUME_CON:
        #endif
        #if (L1_MP3 == 1)
          case MMI_MP3_RESUME_CON:
        #endif
        #if (L1_AAC == 1)
          case MMI_AAC_RESUME_CON:
        #endif
          {
            audio_driver_send_status (AUDIO_OK, AUDIO_RESUME_STATUS, channel_id,
                                      return_path);
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
          }
          break;
        #if (L1_AAC == 1)
          case MMI_AAC_RESTART_CON:
        #endif
          {
            audio_driver_send_status (AUDIO_OK, AUDIO_RESTART_STATUS, channel_id,
                                      return_path);
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
          }
          break;

        #if (L1_MP3 == 1)
          case MMI_MP3_INFO_CON:
          {
            mp3_info.bitrate  = ((T_L1A_MP3_INFO_CON *)p_message)->bitrate;
            mp3_info.channel  = ((T_L1A_MP3_INFO_CON *)p_message)->channel;
            mp3_info.copyright= ((T_L1A_MP3_INFO_CON *)p_message)->copyright;
            mp3_info.emphasis = ((T_L1A_MP3_INFO_CON *)p_message)->emphasis;
            mp3_info.frequency= ((T_L1A_MP3_INFO_CON *)p_message)->frequency;
            mp3_info.layer    = ((T_L1A_MP3_INFO_CON *)p_message)->layer;
            mp3_info.mpeg_id  = ((T_L1A_MP3_INFO_CON *)p_message)->mpeg_id;
            mp3_info.original = ((T_L1A_MP3_INFO_CON *)p_message)->original;
            mp3_info.padding  = ((T_L1A_MP3_INFO_CON *)p_message)->padding;
            mp3_info.private   = ((T_L1A_MP3_INFO_CON *)p_message)->private;
            #ifdef _WINDOWS
              mp3_info.header.callback_func = ((T_L1A_MP3_INFO_CON *)p_message)->header.callback_func;
              mp3_info.header.dest_addr_id = ((T_L1A_MP3_INFO_CON *)p_message)->header.dest_addr_id;
              mp3_info.header.msg_id = ((T_L1A_MP3_INFO_CON *)p_message)->header.msg_id;
              mp3_info.header.src_addr_id = ((T_L1A_MP3_INFO_CON *)p_message)->header.src_addr_id;
            #endif
            audio_driver_send_info_status(AUDIO_OK, AUDIO_INFO_STATUS, channel_id, mp3_info, return_path);

            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
          }
          break;
        #endif

        #if (L1_AAC == 1)
          case MMI_AAC_INFO_CON:
          {
            aac_info.bitrate  = ((T_L1A_AAC_INFO_CON *)p_message)->bitrate;
            aac_info.channel  = ((T_L1A_AAC_INFO_CON *)p_message)->channel;
            aac_info.frequency= ((T_L1A_AAC_INFO_CON *)p_message)->frequency;
            aac_info.aac_format = ((T_L1A_AAC_INFO_CON *)p_message)->aac_format;
            #ifdef _WINDOWS
              aac_info.header.callback_func = ((T_L1A_AAC_INFO_CON *)p_message)->header.callback_func;
              aac_info.header.dest_addr_id = ((T_L1A_AAC_INFO_CON *)p_message)->header.dest_addr_id;
              aac_info.header.msg_id = ((T_L1A_AAC_INFO_CON *)p_message)->header.msg_id;
              aac_info.header.src_addr_id = ((T_L1A_AAC_INFO_CON *)p_message)->header.src_addr_id;
            #endif
            audio_driver_send_aac_info_status(AUDIO_OK, AUDIO_AAC_INFO_STATUS, channel_id, aac_info, return_path);

            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
          }
          break;
        #endif

        #if (L1_EXT_AUDIO_MGT == 1)
          case MMI_EXT_AUDIO_MGT_STOP_CON:
        #endif
          {
            // Deallocate buffers pointer
            rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);

            audio_driver_send_status (AUDIO_OK, AUDIO_STOP_STATUS, channel_id,
                                      return_path);
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;
          }
          break;
        #if (L1_VOICE_MEMO_AMR)
          case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
          case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
        #endif
        #if (L1_MP3 == 1)
          case AUDIO_DRIVER_INIT_MP3_SESSION:
        #endif
        #if (L1_AAC == 1)
          case AUDIO_DRIVER_INIT_AAC_SESSION:
        #endif
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
          break;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
          break;
        } //switch
      } //case AUDIO_DRIVER_CHANNEL_WAIT_STOP:
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_STOP:
      {
        T_AUDIO_RET result = AUDIO_OK;
        switch (p_message->msg_id)
        {
        #if (L1_VOICE_MEMO_AMR)
          case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
          case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
        #endif
        #if (L1_MP3 == 1)
          case AUDIO_DRIVER_INIT_MP3_SESSION:
        #endif
        #if (L1_AAC == 1)
          case AUDIO_DRIVER_INIT_AAC_SESSION:
        #endif
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
            return;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_STOP_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_STOP_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_PAUSE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_PAUSE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_PAUSE_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_RESUME_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESUME_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESUME_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_RESTART_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESTART_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESTART_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_INFO_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
          #if (L1_MP3)
            mp3_info.bitrate  = 0;
            mp3_info.channel  = 0;
            mp3_info.copyright= FALSE;
            mp3_info.emphasis = 0 ;
            mp3_info.frequency= 0;
            mp3_info.layer    = 0;
            mp3_info.mpeg_id  = 0;
            mp3_info.original = FALSE;
            mp3_info.padding  = FALSE;
            mp3_info.private   = 0;
            audio_driver_send_info_status(AUDIO_ERROR, AUDIO_INFO_STATUS, channel_id, mp3_info, return_path);
          #endif
            return;
//omaps00090550            break;

          #if (L1_AAC)
            case AUDIO_DRIVER_INFO_AAC_SESSION:
              audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
              aac_info.bitrate  = 0;
              aac_info.channel  = 0;
              aac_info.frequency= 0;
              aac_info.aac_format = AUDIO_AAC_ADTS;
              audio_driver_send_aac_info_status(AUDIO_ERROR, AUDIO_AAC_INFO_STATUS, channel_id, aac_info, return_path);
              return;
//omaps00090550              break;
          #endif

        #if (L1_VOICE_MEMO_AMR)
          case MMI_VM_AMR_RECORD_START_CON:
            // send L1 stop msg
            result=audio_driver_vm_amr_record_manager(p_message, p_session);
          break;
          case MMI_VM_AMR_PLAY_START_CON:
            // send L1 stop msg
            result=audio_driver_vm_amr_play_manager(p_message, p_session);
          break;
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case MMI_EXT_AUDIO_MGT_START_CON:
            // send L1 stop msg
            result=audio_driver_midi_manager(p_message,p_session);
          break;
        #endif
        #if (L1_MP3 == 1)
          case MMI_MP3_START_CON:
            // send L1 stop msg
            result=audio_driver_mp3_manager(p_message,p_session);
          break;
        #endif
        #if (L1_AAC == 1)
          case MMI_AAC_START_CON:
          // send L1 stop msg
            result=audio_driver_aac_manager(p_message,p_session);
          break;
        #endif
        }

        if (result != AUDIO_OK)
        {
          audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
          return;
        }
        else
        {
          // next state should be WAIT_STOP_CON, which is WAIT_STOP + stop_request == 1
          p_session->session_info.stop_request = 1;
          // change state
          p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
        }
      }
      break;

      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_PAUSE:
      {
        T_AUDIO_RET result = AUDIO_OK;
        switch (p_message->msg_id)
        {
        #if (L1_VOICE_MEMO_AMR)
          case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
          case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
        #endif
        #if (L1_MP3 == 1)
          case AUDIO_DRIVER_INIT_MP3_SESSION:
        #endif
        #if (L1_AAC == 1)
          case AUDIO_DRIVER_INIT_AAC_SESSION:
        #endif
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
            return;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_STOP_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_STOP_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_PAUSE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_PAUSE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_PAUSE_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_RESUME_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESUME_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESUME_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_RESTART_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESTART_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESTART_STATUS, channel_id, return_path);
            return;
          case AUDIO_DRIVER_INFO_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
          #if (L1_MP3)
            mp3_info.bitrate  = 0;
            mp3_info.channel  = 0;
            mp3_info.copyright= FALSE;
            mp3_info.emphasis = 0 ;
            mp3_info.frequency= 0;
            mp3_info.layer    = 0;
            mp3_info.mpeg_id  = 0;
            mp3_info.original = FALSE;
            mp3_info.padding  = FALSE;
            mp3_info.private  = 0;
            audio_driver_send_info_status(AUDIO_ERROR, AUDIO_INFO_STATUS, channel_id, mp3_info, return_path);
          #endif
            return;

      #if (L1_AAC)
          case AUDIO_DRIVER_INFO_AAC_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);

            aac_info.bitrate  = 0;
            aac_info.channel  = 0;
            aac_info.frequency= 0;
            aac_info.aac_format = AUDIO_AAC_ADTS;
            audio_driver_send_aac_info_status(AUDIO_ERROR, AUDIO_AAC_INFO_STATUS, channel_id, aac_info, return_path);
            return;
      #endif
         #if (L1_VOICE_MEMO_AMR == 1)
          case MMI_VM_AMR_PLAY_START_CON:
            // send L1 stop msg
            result=audio_driver_vm_amr_play_manager(p_message,p_session);
          break;
        #endif
      #if (L1_MP3 == 1)
          case MMI_MP3_START_CON:
            // send L1 stop msg
            result=audio_driver_mp3_manager(p_message,p_session);
          break;
        #endif
      #if (L1_AAC == 1)
          case MMI_AAC_START_CON:
            // send L1 stop msg
            result=audio_driver_aac_manager(p_message,p_session);
          break;
        #endif
        }

        if (result != AUDIO_OK)
        {
          audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
          return;
        }
        else
        {
          // next state should be WAIT_PAUSE_CON, which is WAIT_STOP + pause_request == 1
          p_session->session_info.pause_request = 1;
          // change state
          p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
        }
      }
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_FREE:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_FREE_SESSION:
          {
            #if (L1_EXT_AUDIO_MGT !=1)
              // deallocate buffers
              for (j = 0; j < p_session->session_req.nb_buffer; j++)
              {
                return_val = rvf_free_buf ((T_RVF_BUFFER *) (p_session->session_info.buffer[j].p_start_pointer));

                AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER MANAGER: deallocate buffer free session", j, RV_TRACE_LEVEL_DEBUG_LOW);

                if (return_val != RVF_OK)
                  AUDIO_SEND_TRACE_PARAM("can't deallocate buffer", j, RV_TRACE_LEVEL_ERROR);
              }
            #endif

            // Deallocate buffers pointer
            rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);

            audio_driver_send_status (AUDIO_OK, AUDIO_FREE_STATUS, channel_id,
                                      return_path);

            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;
          }
          break;
        #if (L1_VOICE_MEMO_AMR)
          case AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION:
          case AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION:
        #endif
        #if (L1_EXT_AUDIO_MGT == 1)
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
        #endif
        #if (L1_MP3 == 1)
          case AUDIO_DRIVER_INIT_MP3_SESSION:
        #endif
        #if (L1_AAC == 1)
          case AUDIO_DRIVER_INIT_AAC_SESSION:
        #endif
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
          break;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_STOP_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_STOP_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_PAUSE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_PAUSE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_PAUSE_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_RESUME_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESUME_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESUME_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_RESTART_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_RESTART_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_RESTART_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_INFO_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
          #if (L1_MP3)
            mp3_info.bitrate  = 0;
            mp3_info.channel  = 0;
            mp3_info.copyright= FALSE;
            mp3_info.emphasis = 0 ;
            mp3_info.frequency= 0;
            mp3_info.layer    = 0;
            mp3_info.mpeg_id  = 0;
            mp3_info.original = FALSE;
            mp3_info.padding  = FALSE;
            mp3_info.private   = 0;
            audio_driver_send_info_status(AUDIO_ERROR, AUDIO_INFO_STATUS, channel_id, mp3_info, return_path);
          #endif
          break;
       #if (L1_AAC)
          case AUDIO_DRIVER_INFO_AAC_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INFO_EVENT);
            aac_info.bitrate  = 0;
            aac_info.channel  = 0;
            aac_info.frequency= 0;
            aac_info.aac_format = AUDIO_AAC_ADTS;
            audio_driver_send_aac_info_status(AUDIO_ERROR, AUDIO_AAC_INFO_STATUS, channel_id, aac_info, return_path);
            break;
       #endif
        } // switch
      }
      break;
    }
  }

#if (L1_EXT_AUDIO_MGT==1)

  void audio_driver_manager_for_midi(T_RV_HDR *p_message)
  {
    T_AUDIO_DRIVER_SESSION *p_session=NULL;
    //T_AUDIO_DRIVER_PARAMETER *driver_parameter=NULL;
    T_AUDIO_DRIVER_PARAMETER driver_parameter = {0, 0}; // This added to remove lint-warnings
    T_RV_RETURN return_path;
    T_RVF_MB_STATUS mb_status;
    UINT8 session_id=0, channel_id, state, j;

    // initialize return_path to default values
    return_path.callback_func=NULL;
    return_path.addr_id=0;

    // To remove the lint-warnings thrown
    p_session = &(p_audio_gbl_var->audio_driver_session[0]);

    // find state + extract information:channel_id or session_id + driver_parameter
    switch (p_message->msg_id)
    {
      // start/stop session messages have channel_id so we know the driver_session
      case AUDIO_DRIVER_START_SESSION:
      case AUDIO_DRIVER_STOP_SESSION:
      case AUDIO_DRIVER_FREE_SESSION:
        channel_id = ((T_AUDIO_DRIVER_HANDLE_SESSION *)p_message)->channel_id;
        p_session  = &(p_audio_gbl_var->audio_driver_session[channel_id]);
        state      = p_session->session_info.state;
        if (p_message->msg_id != AUDIO_DRIVER_STOP_SESSION)
          return_path = ((T_AUDIO_DRIVER_HANDLE_SESSION *)p_message)->return_path;
        else
          return_path = p_session->session_req.return_path;
      break;
      // messages which contain only session_id, must look for it in ACTIVE driver_session
      default:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
            session_id       = AUDIO_EXT_MIDI_SESSION_ID;
            driver_parameter = ((T_AUDIO_DRIVER_INIT_MIDI_SESSION *)p_message)->driver_parameter;
            return_path      = ((T_AUDIO_DRIVER_INIT_MIDI_SESSION *)p_message)->return_path;
          break;
          case MMI_EXT_AUDIO_MGT_START_CON:
          case MMI_EXT_AUDIO_MGT_STOP_CON:
            session_id = AUDIO_EXT_MIDI_SESSION_ID;
          break;
        }

        // initialize channel_id to browse all driver channels
        channel_id = 0;
        state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;

        // look for an active session, which session_id matches the one from the message
        while ( (channel_id < AUDIO_DRIVER_MAX_CHANNEL)&&
                ((p_audio_gbl_var->audio_driver_session[channel_id].session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_INIT)||
                (p_audio_gbl_var->audio_driver_session[channel_id].session_req.session_id != session_id)) )
        {
          channel_id++;
        }
        // if channel_id < MAX_CHANNEL, we found an active channel so we can derive channel_id + state
        if (channel_id < AUDIO_DRIVER_MAX_CHANNEL)
        {
          p_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);
          state = p_session->session_info.state;
          // we get return_path from session if it was not included in the message
          if ( (return_path.addr_id == 0) && (return_path.callback_func == NULL) )
           return_path = p_session->session_req.return_path;
        }
      }
      break; // default
    }

    switch (state)
    {
      case AUDIO_DRIVER_CHANNEL_WAIT_INIT:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
          {
            T_AUDIO_RET result=0;

            // Find a free channel
            channel_id = 0;
            while ( (channel_id < AUDIO_DRIVER_MAX_CHANNEL) && 
		  (p_audio_gbl_var->audio_driver_session[channel_id].session_info.state != AUDIO_DRIVER_CHANNEL_WAIT_INIT))
              channel_id++;

            if (channel_id == AUDIO_DRIVER_MAX_CHANNEL)
            {
              AUDIO_SEND_TRACE("no driver channel available", RV_TRACE_LEVEL_DEBUG_LOW);
              audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
              return;
            }

            AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER: open channel", channel_id, RV_TRACE_LEVEL_DEBUG_LOW);

            // get session descriptor and fill REQ parameters
            p_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);
            p_session->session_req.session_id     = session_id;
            p_session->session_req.size           = driver_parameter.buffer_size << 1;// we request 16-bit words buffers
            p_session->session_req.nb_buffer      = driver_parameter.nb_buffer;
            // fill return_path parameters, may be used if next message is STOP_SESSION
            p_session->session_req.return_path.callback_func = return_path.callback_func;
            p_session->session_req.return_path.addr_id = return_path.addr_id;

            /************************************************************/
            /* the driver must allocate the RAM buffers pointer         */
            /************************************************************/
            mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                     sizeof(T_AUDIO_DRIVER_BUFFER_INFO)*p_session->session_req.nb_buffer,
                                     (T_RVF_BUFFER **) (&p_session->session_info.buffer));

            // If insufficient resources, then report a memory error and abort.
            if (mb_status == RVF_RED)
            {
              audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
              audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
              return;
            }
#if (L1_EXT_AUDIO_MGT==1)
            p_audio_gbl_var->midi.midi_play_array[0]=midi_buf.audio_play_buffer;
            p_audio_gbl_var->midi.midi_play_array[1]=&(midi_buf.audio_play_buffer[(p_session->session_req.size)/2]);
#endif

            /************************************************************/
            /* the driver must allocate the RAM buffers                 */
            /************************************************************/
            for (j = 0; j < p_session->session_req.nb_buffer; j++)
            {
                /* L1_EXT_AUDIO_MGT : the p_start_pointer address is the address of the internal RAM array */
                /* if j=0 -> p_start_pointer = the begin of the internal RAM array                */
                /* if j=1 -> p_start_pointer = the middle of the internal RAM array                */
                p_session->session_info.buffer[j].p_start_pointer=p_audio_gbl_var->midi.midi_play_array[j];

              // initialize parameters
              p_session->session_info.buffer[j].size = p_session->session_req.size;

              AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER: allocate buffer",
                p_session->session_info.buffer[j].p_start_pointer, RV_TRACE_LEVEL_DEBUG_LOW);
            }

            // info parameters (state must be changed before driver specific functions call
            p_session->session_info.index_l1     = 0;
            p_session->session_info.index_appli  = 0;
            p_session->session_info.stop_request = 0;

            // conversion of parameters + prepare l1 start message
            switch(p_message->msg_id)
            {
              case AUDIO_DRIVER_INIT_MIDI_SESSION:
                result=audio_driver_midi_manager(p_message,p_session);
              break;
            }

            // check L1 msg allocation was successfull
            if (result != AUDIO_OK)
            {
              AUDIO_SEND_TRACE("AUDIO DRIVER MANAGER: L1 msg allocation failed", RV_TRACE_LEVEL_DEBUG_LOW);
              audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
              return;
            }

            // state (must be changed after driver specific functions calls)
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_START;

            audio_driver_init_send_status(AUDIO_OK, session_id, channel_id, return_path);
          } //case AUDIO_DRIVER_INIT_..._SESSION:
          break;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_STOP_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_STOP_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
          break;
        } // switch (p_message->msg_id)
      } //case AUDIO_DRIVER_CHANNEL_WAIT_INIT:
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_START:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_START_SESSION:
          {
            T_AUDIO_RET result=0;

            // fill notification parameters
            p_session->session_req.return_path.callback_func = return_path.callback_func;
            p_session->session_req.return_path.addr_id = return_path.addr_id;

            // send l1 message
            switch(p_session->session_req.session_id)
            {
              case AUDIO_EXT_MIDI_SESSION_ID:
                result=audio_driver_midi_manager(p_message,p_session);
              break;
            }

            if (result != AUDIO_OK)
            {
              audio_driver_error_trace(AUDIO_ERROR_L1_START_EVENT);
              audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
              return;
            }

            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_START_CON;
          }
          break;
          case AUDIO_DRIVER_STOP_SESSION:
          {

            // Deallocate buffers pointer
            rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);

            // deallocate l1 message
            audio_deallocate_l1_message(p_session->session_req.p_l1_send_message);

            // send status OK
            audio_driver_send_status(AUDIO_OK,AUDIO_STOP_STATUS,channel_id,return_path);

            // change state
            p_session->session_info.state=AUDIO_DRIVER_CHANNEL_WAIT_INIT;
          }
          break;
        #if (L1_EXT_AUDIO_MGT == 1)
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
        #endif
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, 0, return_path);
          break;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
          break;
        } // switch
      } //case AUDIO_DRIVER_CHANNEL_WAIT_START:
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON:
      {
        switch (p_message->msg_id)
        {
          case MMI_EXT_AUDIO_MGT_START_CON:
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
          break;
          case AUDIO_DRIVER_STOP_SESSION:
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_STOP;
          break;
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
          break;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
          break;
        } // switch
      } //case AUDIO_DRIVER_CHANNEL_WAIT_START_CON:
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_STOP:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_STOP_SESSION:
          {
            // 1st stop request
            if (p_session->session_info.stop_request == 0)
            {
              T_AUDIO_RET result=0;

              // send l1 message STOP_REQ
              switch(p_session->session_req.session_id)
              {
                case AUDIO_EXT_MIDI_SESSION_ID:
                  result=audio_driver_midi_manager(p_message,p_session);
                break;
              }

              // allocate or send L1 msg went wrong
              if (result != AUDIO_OK)
              {
                audio_driver_error_trace(AUDIO_ERROR_L1_STOP_EVENT);
                audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
                return;
              }

              // STOP has been requested, no longer accept it
              p_session->session_info.stop_request = 1;
            }
            else
            {
              audio_driver_error_trace(AUDIO_ERROR_STOP_EVENT);
              audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
            }
          }
          break;
       case MMI_EXT_AUDIO_MGT_STOP_CON:
          {
            // Deallocate buffers pointer
            rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);

            audio_driver_send_status (AUDIO_OK, AUDIO_STOP_STATUS, channel_id,
                                      return_path);
            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;
          }
          break;
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
          break;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
          break;
        } //switch
      } //case AUDIO_DRIVER_CHANNEL_WAIT_STOP:
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_STOP:
      {
        T_AUDIO_RET result = AUDIO_OK;
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
            return;
//omaps00090550            break;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
            return;
//omaps00090550            break;
          case AUDIO_DRIVER_STOP_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_STOP_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
            return;
//omaps00090550            break;
          case AUDIO_DRIVER_FREE_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_FREE_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_FREE_STATUS, channel_id, return_path);
            return;
//omaps00090550            break;
        case MMI_EXT_AUDIO_MGT_START_CON:
            // send L1 stop msg
            result=audio_driver_midi_manager(p_message,p_session);
          break;
        }

        if (result != AUDIO_OK)
        {
          audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
          return;
        }
        else
        {
          // next state should be WAIT_STOP_CON, which is WAIT_STOP + stop_request == 1
          p_session->session_info.stop_request = 1;
          // change state
          p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_STOP;
        }
      }
      break;
      case AUDIO_DRIVER_CHANNEL_WAIT_FREE:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_FREE_SESSION:
          {
            // Deallocate buffers pointer
            rvf_free_buf((T_RVF_BUFFER *)p_session->session_info.buffer);

            audio_driver_send_status (AUDIO_OK, AUDIO_FREE_STATUS, channel_id,
                                      return_path);

            // change state
            p_session->session_info.state = AUDIO_DRIVER_CHANNEL_WAIT_INIT;
          }
          break;
          case AUDIO_DRIVER_INIT_MIDI_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_INIT_EVENT);
            audio_driver_init_send_status(AUDIO_ERROR, session_id, channel_id, return_path);
          break;
          case AUDIO_DRIVER_START_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_START_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_START_STATUS, channel_id, return_path);
          break;
          case AUDIO_DRIVER_STOP_SESSION:
            audio_driver_error_trace(AUDIO_ERROR_STOP_EVENT);
            audio_driver_send_status(AUDIO_ERROR, AUDIO_STOP_STATUS, channel_id, return_path);
          break;
        } // switch
      }
      break;
    }
  }
#endif

#endif // (L1_AUDIO_DRIVER)

#endif // #ifdef RVM_AUDIO_MAIN_SWE

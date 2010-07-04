/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_vm_amr_record.c                                       */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            Voice Memorization AMR record task.                           */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date          Modification                                              */
/*  ------------------------------------                                    */
/*  ?? ?? 2002 Create                                                       */
/*                                                                          */
/*  Author                                                                  */
/*     Stephanie Gerthoux                                                   */
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

  /* include the usefull L1 header */
  #include "l1_confg.h"

#if (L1_VOICE_MEMO_AMR)
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "Audio/audio_features_i.h"
#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#else
  #include "Audio/audio_ffs_i.h"
#endif
  #include "Audio/audio_api.h"
  #include "Audio/audio_structs_i.h"
  #include "Audio/audio_var_i.h"
  #include "Audio/audio_messages_i.h"
  #include "rvf/rvf_target.h"
  #include "Audio/audio_const_i.h"
  #include "Audio/audio_error_hdlr_i.h"

  /* include the usefull L1 header */
  #define BOOL_FLAG
  #define CHAR_FLAG
  #include "l1_types.h"
  #include "l1audio_cust.h"
  #include "l1audio_const.h"
  #include "l1audio_msgty.h"
  #include "l1audio_signa.h"

  #include "Audio/audio_macro_i.h"

  T_AUDIO_RET audio_convert_to_mms(UINT8 *p_buffer, UINT16 *buffer_size, UINT8 *previous_type, UINT8 *size_left);
  INT16 ram_write(T_AUDIO_VOICE_MEMO_AMR_RECORD_TO_MEM_VAR *vm_amr_record, UINT8 *src_buffer, UINT16 size);

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_amr_record_send_status                          */
  /*                                                                              */
  /*    Purpose:  This function sends the voice memorization AMR record status    */
  /*              to the entity.                                                  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        status,                                                               */
  /*        return path                                                           */
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
  void audio_vm_amr_record_send_status (T_AUDIO_RET status,
                                        UINT32 recorded_size,
                                        T_RV_RETURN return_path)
  {
    T_AUDIO_AMR_RECORD_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    /* allocate the message buffer */
    while (mb_status == RVF_RED)
    {
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_AMR_RECORD_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    /*fill the header of the message */
    p_send_message->os_hdr.msg_id = AUDIO_AMR_RECORD_TO_MEM_STATUS_MSG;

    /* fill the status parameters */
    p_send_message->status = status;
    if (recorded_size == 0)
      p_send_message->recorded_duration = 0;
    else
      p_send_message->recorded_duration = recorded_size;

    /* send message or call callback */
    if (return_path.callback_func == NULL)
    {
      rvf_send_msg (return_path.addr_id, p_send_message);
    }
    else
    {
      (*return_path.callback_func)((void *)(p_send_message));
       rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }

  void audio_vm_amr_record_to_memory_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables. */
    T_RV_RETURN my_return_path;

    /**************** audio_vm_amr_record_to_memory_manager function begins ***********************/
    switch(p_audio_gbl_var->audio_vm_amr_record.state)
    {
      case AUDIO_IDLE:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_VM_AMR_RECORD_TO_FFS_START_REQ:
          case AUDIO_VM_AMR_RECORD_TO_RAM_START_REQ:
          {
            T_AUDIO_DRIVER_VM_AMR_RECORD_PARAMETER parameter;
            T_AUDIO_DRIVER_PARAMETER driver_parameter;

            switch (p_message->msg_id)
            {
              case AUDIO_VM_AMR_RECORD_TO_FFS_START_REQ:
              {
                /* save the return path + ffs_fd */
              #if (AUDIO_RAM_MANAGER)
                p_audio_gbl_var->audio_vm_amr_record.audio_ram_fd = NULL;
              #endif
                p_audio_gbl_var->audio_vm_amr_record.return_path.callback_func =
                  ((T_AUDIO_VM_AMR_RECORD_TO_FFS_START *)p_message)->return_path.callback_func;
                p_audio_gbl_var->audio_vm_amr_record.return_path.addr_id   =
                  ((T_AUDIO_VM_AMR_RECORD_TO_FFS_START*)p_message)->return_path.addr_id;
              #if (AUDIO_NEW_FFS_MANAGER)
                p_audio_gbl_var->audio_vm_amr_record.ffs_fd =
                  ((T_AUDIO_VM_AMR_RECORD_TO_FFS_START *)p_message)->audio_ffs_fd;
              #endif

                /* fill AMR parameters and driver parameters */
                parameter.memo_duration      = ((T_AUDIO_VM_AMR_RECORD_TO_FFS_START *)p_message)->memo_duration;
                parameter.compression_mode   = ((T_AUDIO_VM_AMR_RECORD_TO_FFS_START *)p_message)->compression_mode;
                parameter.microphone_gain    = ((T_AUDIO_VM_AMR_RECORD_TO_FFS_START *)p_message)->microphone_gain;
                parameter.amr_vocoder        = ((T_AUDIO_VM_AMR_RECORD_TO_FFS_START *)p_message)->amr_vocoder;
              }
              break;
              case AUDIO_VM_AMR_RECORD_TO_RAM_START_REQ:
              {
                /* save the return path + p_buffer */
              #if (AUDIO_NEW_FFS_MANAGER)
                p_audio_gbl_var->audio_vm_amr_record.ffs_fd = -1;
              #endif
                p_audio_gbl_var->audio_vm_amr_record.return_path.callback_func =
                  ((T_AUDIO_VM_AMR_RECORD_TO_RAM_START *)p_message)->return_path.callback_func;
                p_audio_gbl_var->audio_vm_amr_record.return_path.addr_id   =
                  ((T_AUDIO_VM_AMR_RECORD_TO_RAM_START*)p_message)->return_path.addr_id;
              #if (AUDIO_RAM_MANAGER)
                p_audio_gbl_var->audio_vm_amr_record.audio_ram_fd =
                  ((T_AUDIO_VM_AMR_RECORD_TO_RAM_START *)p_message)->p_buffer;
              #endif

                /* fill AMR parameters and driver parameters */
                parameter.memo_duration      = ((T_AUDIO_VM_AMR_RECORD_TO_RAM_START *)p_message)->memo_duration;
                parameter.compression_mode   = ((T_AUDIO_VM_AMR_RECORD_TO_RAM_START *)p_message)->compression_mode;
                parameter.microphone_gain    = ((T_AUDIO_VM_AMR_RECORD_TO_RAM_START *)p_message)->microphone_gain;
                parameter.amr_vocoder        = ((T_AUDIO_VM_AMR_RECORD_TO_RAM_START *)p_message)->amr_vocoder;
              }
              break;
            } // switch (p_message->msg_id)

            driver_parameter.nb_buffer   = AUDIO_VM_AMR_RECORD_NB_BUFFER;
            driver_parameter.buffer_size = AUDIO_VM_AMR_RECORD_SIZE;

            /* return_path for driver */
            my_return_path.callback_func = NULL;
            my_return_path.addr_id       = p_audio_gbl_var->addrId;

            /* Init driver */
            audio_driver_init_vm_amr_record_session(&parameter, &driver_parameter, my_return_path);

            p_audio_gbl_var->audio_vm_amr_record.state = AUDIO_WAIT_CHANNEL_ID;
          }
          break;
          case AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ:
          case AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
            /* do not send a status message because of pre-emption issues
            An automatic stop can pre-empt a stop request. A status is sent + back in state idle
            then the stop request is received and another status is sent, which can be misinterpreted */
          }
          break;
        } // switch(p_message->msg_id)
      } // case AUDIO_IDLE:
      break;

      case AUDIO_WAIT_CHANNEL_ID:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_INIT_STATUS_MSG:
          {
            /* check init is successfull otherwise, send status AUDIO_ERROR */
            if (((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status == AUDIO_OK)
            {
              /* get channel id */
              p_audio_gbl_var->audio_vm_amr_record.channel_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              // initializations
              p_audio_gbl_var->audio_vm_amr_record.session_id = AUDIO_VM_AMR_RECORD_SESSION_ID;
              p_audio_gbl_var->audio_vm_amr_record.size = AUDIO_VM_AMR_RECORD_SIZE<<1;
              // parameters for notification handling
              p_audio_gbl_var->audio_vm_amr_record.size_left = 0;
              p_audio_gbl_var->audio_vm_amr_record.previous_type = AUDIO_VM_AMR_RXTX_SPEECH_GOOD;
              p_audio_gbl_var->audio_vm_amr_record.stop_req_allowed = TRUE;

              // initialize the return path
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              // send message
              audio_driver_start_session(p_audio_gbl_var->audio_vm_amr_record.channel_id, my_return_path);

              /* change state */
              p_audio_gbl_var->audio_vm_amr_record.state = AUDIO_WAIT_STOP_OR_NOTIFICATION;
            } // if (((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status == AUDIO_OK)
            else
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_START_EVENT);
            #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
			  if (p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= RFS_EOK)
#else
			  if (p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= EFFS_OK)
#endif
              {
#if(AS_RFS_API == 1)
			    if(rfs_close(p_audio_gbl_var->audio_vm_amr_record.ffs_fd) != RFS_EOK)
#else
				if(ffs_close(p_audio_gbl_var->audio_vm_amr_record.ffs_fd) != EFFS_OK)
#endif
                  audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              }
            #endif
              audio_vm_amr_record_send_status (AUDIO_ERROR, 0, p_audio_gbl_var->audio_vm_amr_record.return_path);
              /* change state */
              p_audio_gbl_var->audio_vm_amr_record.state = AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ:
          case AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ:
          #if (AUDIO_RAM_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_record.audio_ram_fd != NULL) )
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
            }
          #endif
          #if (AUDIO_NEW_FFS_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= EFFS_OK) )
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
            }
          #endif
            /* change state */
            p_audio_gbl_var->audio_vm_amr_record.state = AUDIO_WAIT_CHANNEL_ID_TO_STOP;
          break;
        } // switch(p_message->msg_id)
      } // case AUDIO_WAIT_CHANNEL_ID:
      break;

      case AUDIO_WAIT_STOP_OR_NOTIFICATION:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_NOTIFICATION_MSG:
          {
            UINT16 record_buffer_size;

            // default is session_req.size but can be less if we find END_MASK
            record_buffer_size = p_audio_gbl_var->audio_vm_amr_record.size;

            // convert to MMS, update record_buffer_size if END_MASK is found
            audio_convert_to_mms((UINT8 *)((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer,
                                 &record_buffer_size,
                                 &(p_audio_gbl_var->audio_vm_amr_record.previous_type),
                                 &(p_audio_gbl_var->audio_vm_amr_record.size_left));
          #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
			if(p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= RFS_EOK)
              if(rfs_write(p_audio_gbl_var->audio_vm_amr_record.ffs_fd,
                           (UINT8 *)((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer,
                           record_buffer_size)<RFS_EOK)
#else
			if(p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= EFFS_OK)
              if(ffs_write(p_audio_gbl_var->audio_vm_amr_record.ffs_fd,
                           (UINT8 *)((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer,
                           record_buffer_size)<EFFS_OK)
#endif
                AUDIO_SEND_TRACE("AUDIO AMR RECORD: FFS RECORD WRITE FAILED",RV_TRACE_LEVEL_DEBUG_LOW);
          #endif
          #if (AUDIO_RAM_MANAGER)
            if(p_audio_gbl_var->audio_vm_amr_record.audio_ram_fd != NULL)
              ram_write(&(p_audio_gbl_var->audio_vm_amr_record),
                        (UINT8 *)((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer,
                        record_buffer_size);
          #endif

            AUDIO_SEND_TRACE_PARAM("AUDIO AMR RECORD: RAM to FFS or RAM",record_buffer_size,RV_TRACE_LEVEL_DEBUG_LOW);
          }
          break;
          case AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ:
          case AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ:
          {
          #if (AUDIO_RAM_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_record.audio_ram_fd != NULL) )
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
            }
          #endif
          #if (AUDIO_NEW_FFS_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= EFFS_OK) )
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
            }
          #endif
            if(p_audio_gbl_var->audio_vm_amr_record.stop_req_allowed == TRUE)
            {
              p_audio_gbl_var->audio_vm_amr_record.stop_req_allowed = FALSE;
              audio_driver_stop_session(p_audio_gbl_var->audio_vm_amr_record.channel_id);
            }
            else
              /* do not send a status message because of pre-emption issues */
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;
          case AUDIO_DRIVER_LAST_NOTIFICATION_MSG:
          {
            UINT16 record_buffer_size;

            record_buffer_size=p_audio_gbl_var->audio_vm_amr_record.size;

            audio_convert_to_mms((UINT8 *)((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer,
                                 &record_buffer_size,
                                 &(p_audio_gbl_var->audio_vm_amr_record.previous_type),
                                 &(p_audio_gbl_var->audio_vm_amr_record.size_left));
          #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
			if(p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= RFS_EOK)
#else
			if(p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= EFFS_OK)
#endif
            {
#if(AS_RFS_API == 1)
			  if(rfs_write(p_audio_gbl_var->audio_vm_amr_record.ffs_fd,
                           (UINT8 *)((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer,
                           record_buffer_size)<RFS_EOK)
#else
			  if(ffs_write(p_audio_gbl_var->audio_vm_amr_record.ffs_fd,
                           (UINT8 *)((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer,
                           record_buffer_size)<EFFS_OK)
#endif
                AUDIO_SEND_TRACE("AUDIO AMR RECORD: FFS RECORD WRITE FAILED",RV_TRACE_LEVEL_DEBUG_LOW);
            }
          #endif
          #if (AUDIO_RAM_MANAGER)
            if(p_audio_gbl_var->audio_vm_amr_record.audio_ram_fd!=NULL)
              {
              if(ram_write(&(p_audio_gbl_var->audio_vm_amr_record),
                           (UINT8 *)((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer,
                           record_buffer_size)<0)
                AUDIO_SEND_TRACE("AUDIO AMR RECORD: RAM RECORD WRITE FAILED",RV_TRACE_LEVEL_DEBUG_LOW);
              }
          #endif

              // recorded size
              AUDIO_SEND_TRACE_PARAM("AUDIO AMR RECORD: last RAM to MEMORY",record_buffer_size,RV_TRACE_LEVEL_DEBUG_LOW);
              AUDIO_SEND_TRACE_PARAM("AUDIO AMR RECORD: size recorded",((T_AUDIO_DRIVER_LAST_NOTIFICATION *)p_message)->recorded_size,RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_vm_amr_record.recorded_size=((T_AUDIO_DRIVER_LAST_NOTIFICATION *)p_message)->recorded_size;

              // stop must no longer be accepted as it is an automatic stop
              p_audio_gbl_var->audio_vm_amr_record.stop_req_allowed = FALSE;

              /* return_path for driver */
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              audio_driver_free_session(p_audio_gbl_var->audio_vm_amr_record.channel_id, my_return_path);
            }
          break; // case AUDIO_DRIVER_LAST_NOTIFICATION_MSG:
          case AUDIO_DRIVER_STATUS_MSG:
            {
          #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
			if(p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= RFS_EOK)
#else
			if(p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= EFFS_OK)
#endif
              {
              AUDIO_SEND_TRACE("AUDIO AMR RECORD: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
#if(AS_RFS_API == 1)
				if(rfs_close(p_audio_gbl_var->audio_vm_amr_record.ffs_fd) != RFS_EOK)
#else
			  if(ffs_close(p_audio_gbl_var->audio_vm_amr_record.ffs_fd) != EFFS_OK)
#endif
                audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              }
          #endif
            audio_vm_amr_record_send_status(AUDIO_OK, p_audio_gbl_var->audio_vm_amr_record.recorded_size,
                p_audio_gbl_var->audio_vm_amr_record.return_path);
              p_audio_gbl_var->audio_vm_amr_record.state = AUDIO_IDLE;
            }
          break;
        } // switch (p_message->msg_id)
      } // case AUDIO_WAIT_STOP_OR_NOTIFICATION:
      break;
      case AUDIO_WAIT_CHANNEL_ID_TO_STOP:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_INIT_STATUS_MSG:
          {
            if (((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status == AUDIO_OK)
            {
              /* get channel_id */
              p_audio_gbl_var->audio_vm_amr_record.channel_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              audio_driver_stop_session(p_audio_gbl_var->audio_vm_amr_record.channel_id);

              /* change state */
              p_audio_gbl_var->audio_vm_amr_record.state = AUDIO_WAIT_DRIVER_STOP_CON;
            }
            else
            {
              /* close file */
            #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
			  if (p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= RFS_EOK)
#else
			  if (p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= EFFS_OK)
#endif
              {
#if(AS_RFS_API == 1)
			    if ( rfs_close(p_audio_gbl_var->audio_vm_amr_record.ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(p_audio_gbl_var->audio_vm_amr_record.ffs_fd) != EFFS_OK )
#endif
                  audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                AUDIO_SEND_TRACE("AUDIO AMR RECORD: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }
            #endif

              audio_vm_amr_record_send_status (AUDIO_OK, 0,
                p_audio_gbl_var->audio_vm_amr_record.return_path);

              /* change state */
              p_audio_gbl_var->audio_vm_amr_record.state = AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ:
          case AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ:
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
        }
      } // case AUDIO_WAIT_CHANNEL_ID_TO_STOP:
      break;
      case AUDIO_WAIT_DRIVER_STOP_CON:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_STATUS_MSG:
          {
              /* close file */
            #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
			if (p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= RFS_EOK)
#else
			if (p_audio_gbl_var->audio_vm_amr_record.ffs_fd >= EFFS_OK)
#endif
              {
#if(AS_RFS_API == 1)
				if ( rfs_close(p_audio_gbl_var->audio_vm_amr_record.ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(p_audio_gbl_var->audio_vm_amr_record.ffs_fd) != EFFS_OK )
#endif
                audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              AUDIO_SEND_TRACE("AUDIO AMR RECORD: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }
            #endif

            audio_vm_amr_record_send_status (AUDIO_OK, 0, p_audio_gbl_var->audio_vm_amr_record.return_path);
              p_audio_gbl_var->audio_vm_amr_record.state = AUDIO_IDLE;
            }
          break;
          case AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ:
          case AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ:
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
        }
      } //case AUDIO_WAIT_DRIVER_STOP_CON:
      break;
    } // switch(p_audio_gbl_var->audio_vm_amr_record.state)
  } /*********************** End of audio_vm_amr_record_to_memory_manager function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_vm_amr_record_convert_parameter             */
  /*                                                                              */
  /*    Purpose:  Convert the voice memorization record parameters from the       */
  /*              entity to the l1 parameters                                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        session_id of the voice memo record                                   */
  /*        id of the voice memo record                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        layer 1 voice memo record message                                     */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_driver_vm_amr_record_convert_parameter(T_AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION *entity_parameter,
                                             T_MMI_VM_AMR_RECORD_REQ *l1_parameter)
  {
     /* session id : specifies the customer data identification corresponding to   */
    /*              this voice memorization */
    l1_parameter->session_id      = AUDIO_VM_AMR_RECORD_SESSION_ID;
    l1_parameter->maximum_size    = entity_parameter->memo_duration + SC_VM_AMR_END_MASK_SIZE;
    l1_parameter->dtx_used        = entity_parameter->compression_mode;
    l1_parameter->record_coeff_ul = entity_parameter->microphone_gain;
    l1_parameter->amr_vocoder     = entity_parameter->amr_vocoder;
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_vm_amr_record_manager                       */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a voice memorization record   */
  /*              manager                                                         */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Message to the audio entity                                           */
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
  T_AUDIO_RET audio_driver_vm_amr_record_manager (T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session)
  {
    /**************** audio_driver_vm_amr_record_manager function begins ***********************/
    switch(p_session->session_info.state)
    {
      case AUDIO_DRIVER_CHANNEL_WAIT_INIT:
      {
        /* allocate the buffer for the message to the L1 */
        p_session->session_req.p_l1_send_message =
          audio_allocate_l1_message(sizeof(T_MMI_VM_AMR_RECORD_REQ));

        if (p_session->session_req.p_l1_send_message != NULL )
        {
          /* Convert the entity parameters to the audio L1 parameters */
          audio_driver_vm_amr_record_convert_parameter((T_AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION*)p_message,
                                         (T_MMI_VM_AMR_RECORD_REQ *)p_session->session_req.p_l1_send_message);
          return (AUDIO_OK);
        }
        else
        return (AUDIO_ERROR);
      }

      case AUDIO_DRIVER_CHANNEL_WAIT_START:
      {
        /* send the start voice memo record message to the L1 */
        audio_send_l1_message(MMI_VM_AMR_RECORD_START_REQ,
                              p_session->session_req.p_l1_send_message);
        return (AUDIO_OK);
      }

      case AUDIO_DRIVER_CHANNEL_WAIT_STOP:
      {
        /* send the stop command to the audio L1 */
        /* allocate the buffer for the message to the L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if ( p_send_message != NULL)
        {
          /* send the stop command to the audio L1 */
          audio_send_l1_message(MMI_VM_AMR_RECORD_STOP_REQ, p_send_message);
          return (AUDIO_OK);
        }
        return (AUDIO_ERROR);
      }
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_STOP:
      {
        /* only STOP is handled here */
        /* send the stop command to the audio L1 */
        /* allocate the buffer for the message to the L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if ( p_send_message != NULL)
        {
          /* send the stop command to the audio L1 */
          audio_send_l1_message(MMI_VM_AMR_RECORD_STOP_REQ, p_send_message);
          return (AUDIO_OK);
        }
        return (AUDIO_ERROR);
      }
    }
    // no corresponding message found?
    return (AUDIO_ERROR);
  } /*********************** End of audio_vm_record_manager function **********************/

  T_AUDIO_RET audio_convert_to_mms(UINT8 *p_buffer, UINT16 *buffer_size, UINT8 *previous_type, UINT8 *size_left)
  {
    UINT8 rxtx_type, frame_header, data_size = 0;
    UINT8 frame_type, quality;
    UINT8 *ptr_final, *ptr_mms;

    ptr_mms = p_buffer;
    ptr_final = ptr_mms + *buffer_size;

    /* sample is shared among the 2 buffers */
    if (*size_left > 0)
    {
      UINT8 i;

      switch (*previous_type)
      {
        case AUDIO_VM_AMR_RXTX_SID_FIRST:
          /* set data bits to 0 */
          for (i = 0; i < *size_left; i++)
            *(ptr_mms + i) = 0;
          /* set Mode Indication */
          *(ptr_mms + *size_left - 1) = AUDIO_MMS_MODE_INDICATION;
        break;
        case AUDIO_VM_AMR_RXTX_SID_UPDATE:
        //case AUDIO_VM_AMR_RXTX_SID_BAD:
          *(ptr_mms + *size_left - 1) |= AUDIO_MMS_STI_BIT | AUDIO_MMS_MODE_INDICATION;
        break;
      }
      ptr_mms += *size_left;
    }
    *size_left = 0;

    while (ptr_mms < ptr_final)
    {
      /* read header */
      frame_header = *ptr_mms;

      /* if end_mask, stop */
      if (frame_header == SC_VM_AMR_END_MASK)
      {
        *buffer_size = (ptr_mms - p_buffer);
        return AUDIO_OK;
      }

      /* reset header */
      *ptr_mms = 0;

      rxtx_type  = (frame_header & (UINT8)SC_RX_TX_TYPE_MASK);
      *previous_type = rxtx_type;
      switch (rxtx_type)
      {
        case AUDIO_VM_AMR_RXTX_SPEECH_GOOD:
        //case AUDIO_VM_AMR_RXTX_SPEECH_BAD:
        {
          /* FT + data_size */
          frame_type = frame_header & SC_CHAN_TYPE_MASK;
          switch (frame_type)
          {
            case AUDIO_VM_AMR_SPEECH_475:
              data_size = AUDIO_VM_AMR_SPEECH_475_DATA_SIZE;
            break;
            case AUDIO_VM_AMR_SPEECH_515:
              data_size = AUDIO_VM_AMR_SPEECH_515_DATA_SIZE;
            break;
            case AUDIO_VM_AMR_SPEECH_59:
              data_size = AUDIO_VM_AMR_SPEECH_590_DATA_SIZE;
            break;
            case AUDIO_VM_AMR_SPEECH_67:
              data_size = AUDIO_VM_AMR_SPEECH_670_DATA_SIZE;
            break;
            case AUDIO_VM_AMR_SPEECH_74:
              data_size = AUDIO_VM_AMR_SPEECH_740_DATA_SIZE;
            break;
            case AUDIO_VM_AMR_SPEECH_795:
              data_size = AUDIO_VM_AMR_SPEECH_795_DATA_SIZE;
            break;
            case AUDIO_VM_AMR_SPEECH_102:
              data_size = AUDIO_VM_AMR_SPEECH_102_DATA_SIZE;
            break;
            case AUDIO_VM_AMR_SPEECH_122:
              data_size = AUDIO_VM_AMR_SPEECH_122_DATA_SIZE;
            break;
          }
          /* Q */
          //if (rxtx_type == AUDIO_VM_AMR_RXTX_SPEECH_GOOD)
            quality = AUDIO_MMS_GOOD_QUALITY;
          //else
          //  quality = AUDIO_MMS_BAD_QUALITY;
        }
        break;
        case AUDIO_VM_AMR_RXTX_SID_FIRST:
        case AUDIO_VM_AMR_RXTX_SID_UPDATE:
        //case AUDIO_VM_AMR_RXTX_SID_BAD:
        {
          /* FT, data_size, Q */
          frame_type = AUDIO_MMS_SID_FRAME_TYPE;
          data_size = AUDIO_VM_AMR_SID_DATA_SIZE;
          //if ((rxtx_type == AUDIO_VM_AMR_RXTX_SID_FIRST)||
          //    (rxtx_type == AUDIO_VM_AMR_RXTX_SID_UPDATE))
          //{
            quality = AUDIO_MMS_GOOD_QUALITY;
          //}
          //else
          //  quality = AUDIO_MMS_BAD_QUALITY;

          /* data, STI, Mode indication */
          if (rxtx_type == AUDIO_VM_AMR_RXTX_SID_FIRST)
          {
            UINT8 data, i;

            /* number of bytes to set to 0 */
            data = ((ptr_final - ptr_mms) >= (data_size + 1)) ? (data_size) : (ptr_final - ptr_mms - 1);

            /* set data bits to 0 */
            for (i = 0; i < data; i++)
              *(ptr_mms + 1 + i) = 0;

            /* set Mode indication */
            if ((ptr_final - ptr_mms) >= (data_size + 1))
              *(ptr_mms + data_size) = AUDIO_MMS_MODE_INDICATION;

          }
          /* SID_UPDATE */
          else
          {
            /* set STI bit to 1 + Mode indication */
            if ((ptr_final - ptr_mms) >= (data_size + 1))
              *(ptr_mms + data_size) |= AUDIO_MMS_STI_BIT | AUDIO_MMS_MODE_INDICATION;
          }
        }
        break;
        case AUDIO_VM_AMR_RXTX_NO_DATA:
          frame_type = AUDIO_MMS_NO_DATA_FRAME_TYPE;
          data_size = AUDIO_VM_AMR_NO_DATA_DATA_SIZE;
          quality = AUDIO_MMS_GOOD_QUALITY;
        break;
        default:
        {
          AUDIO_SEND_TRACE_PARAM("AUDIO MEMORY MANAGER: convert to MMS, header not recognized", frame_header, RV_TRACE_LEVEL_DEBUG_LOW);
          return AUDIO_ERROR;
        }
      }
      /* write header */
      *(ptr_mms)++ |= (frame_type << AUDIO_MMS_FRAME_TYPE_SHIFT) | (quality << AUDIO_MMS_QUALITY_SHIFT);

      /* write data, check we are not at the end of the buffer */
      if ((ptr_final - ptr_mms) < data_size)
      {
        *size_left = data_size - (ptr_final - ptr_mms);
        data_size = ptr_final - ptr_mms;
      }
      ptr_mms += data_size;
    }
    *buffer_size = (ptr_final - p_buffer);
    return AUDIO_OK;
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_amr_record_l1_simulator                         */
  /*                                                                              */
  /*    Purpose:  This function simulates the L1 for VM AMR record                */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        event: Event that triggered the function                              */
  /*        p_msg: Message (if any) associated with the event                     */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#ifdef _WINDOWS
  void audio_vm_amr_record_l1_simulator(UINT16 event, T_RV_HDR *p_message)
  {
    T_RVF_MB_STATUS mb_status;
#if(AS_RFS_API == 1)
	static T_RFS_FD fd;	
#else
	static T_FFS_FD fd;
#endif
    static UINT16 recorded_size;

#if(AS_RFS_API == 1)
		T_WCHAR mp_uc1[50];
  		char *str1 = "/memo/amr_rec_simu";
#endif
#if(AS_RFS_API == 1)
	convert_u8_to_unicode(str1, mp_uc1);
#endif


    switch(p_audio_gbl_var->audio_vm_amr_record.l1_state)
    {
      case AUDIO_IDLE:
      {
        if(p_message->msg_id == MMI_VM_AMR_RECORD_START_REQ)
        {
          /* send START_CON message to Riviera audio driver
             + start 1s timer to send notifications */
          T_RV_HDR *p_msg;

          // open simu file
#if(AS_RFS_API == 1)
		  fd = rfs_open(mp_uc1, RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
		  fd = ffs_open("/memo/amr_rec_simu", FFS_O_RDONLY);
#endif
          recorded_size = 0;

          // start notification timer -> l1_counter shots
          rvf_start_timer(AUDIO_VM_AMR_RECORD_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1200),
                          AUDIO_VM_AMR_RECORD_L1_SIMUL_ONE_SHOT_TIMER);

          // send START_CON message
          mb_status = rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_RV_HDR),
                                (T_RVF_BUFFER **)(&p_msg));
          if(mb_status == RVF_RED)
          {
            AUDIO_SEND_TRACE("L1 has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }

          p_msg->msg_id = MMI_VM_AMR_RECORD_START_CON;
          rvf_send_msg (p_audio_gbl_var->addrId, p_msg);

          p_audio_gbl_var->audio_vm_amr_record.l1_state = AUDIO_WAIT_STOP;
        }
      }
      break;

      case AUDIO_WAIT_STOP:
      {
        T_AUDIO_DRIVER_SESSION *p_session;
        UINT8 *p_buffer;

        p_session = &p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_vm_amr_record.channel_id];
        p_buffer = (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer;

        if(event & AUDIO_VM_AMR_RECORD_L1_SIMUL_TIMER_EVT_MASK)
        {
          T_RV_RETURN *return_path;

          UINT16 size;

          return_path = &p_session->session_req.return_path;

#if(AS_RFS_API == 1)
		  size = (UINT16)rfs_read(fd, p_buffer, p_session->session_req.size);
#else
		  size = ffs_read(fd, p_buffer, p_session->session_req.size);
#endif
          recorded_size += size;

          if (size < p_session->session_req.size)
          {
            T_MMI_VM_AMR_RECORD_CON *p_msg;

            p_buffer[size] = 0xFF;
#if(AS_RFS_API == 1)
			rfs_close(fd);
#else
			ffs_close(fd);
#endif

            // allocate notification message
            mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                  sizeof(T_MMI_VM_AMR_RECORD_CON),
                                  (T_RVF_BUFFER **)(&p_msg));
            if(mb_status==RVF_RED)
            {
              AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification", RV_TRACE_LEVEL_ERROR);
              return;
            }

            p_msg->header.msg_id = MMI_VM_AMR_RECORD_STOP_CON;
            p_msg->recorded_size = recorded_size;

            rvf_send_msg(p_audio_gbl_var->addrId, p_msg);

            p_audio_gbl_var->audio_vm_amr_record.l1_state = AUDIO_IDLE;
          }
          else
          {
            T_AUDIO_DRIVER_NOTIFICATION *p_msg;

            rvf_start_timer(AUDIO_VM_AMR_RECORD_L1_SIMUL_TIMER,
                            RVF_MS_TO_TICKS(1200),
                            AUDIO_VM_AMR_RECORD_L1_SIMUL_ONE_SHOT_TIMER);

            // allocate notification message
            mb_status = rvf_get_buf(p_audio_gbl_var->mb_internal,
                                    sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                    (T_RVF_BUFFER **)(&p_msg));
            if(mb_status==RVF_RED)
            {
              AUDIO_SEND_TRACE("L1 has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
              return;
            }

            p_msg->header.msg_id = AUDIO_DRIVER_NOTIFICATION_MSG;
            p_msg->channel_id = p_audio_gbl_var->audio_vm_amr_record.channel_id;
            p_msg->p_buffer = p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer;

            p_session->session_info.index_l1++;
            if(p_session->session_info.index_l1 == p_session->session_req.nb_buffer)
              p_session->session_info.index_l1 = 0;

            if(return_path->callback_func==NULL)
              rvf_send_msg(return_path->addr_id, p_msg);
            else
            {
              (*return_path->callback_func)((void *)p_msg);
              rvf_free_buf((T_RVF_BUFFER *)p_msg);
            }
          }
        } // if(event & AUDIO_VM_AMR_RECORD_L1_SIMUL_TIMER_EVT_MASK)

        if(p_message->msg_id == MMI_VM_AMR_RECORD_STOP_REQ)
        {
          T_MMI_VM_AMR_RECORD_CON *p_msg;
          UINT16 i = 0;

          for (;i < p_session->session_req.size; i++)
            p_buffer[i] = 0xFF;
#if(AS_RFS_API == 1)
		  rfs_close(fd);
#else
		  ffs_close(fd);
#endif

          // allocate notification message
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_MMI_VM_AMR_RECORD_CON),
                                (T_RVF_BUFFER **)(&p_msg));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification", RV_TRACE_LEVEL_ERROR);
            return;
          }

          p_msg->header.msg_id = MMI_VM_AMR_RECORD_STOP_CON;
          p_msg->recorded_size = recorded_size;

          rvf_send_msg(p_audio_gbl_var->addrId, p_msg);

          p_audio_gbl_var->audio_vm_amr_record.l1_state = AUDIO_IDLE;
        } // if(p_message->msg_id == MMI_VM_AMR_RECORD_STOP_REQ)
      } // case AUDIO_WAIT_STOP:
      break;
    } // switch(p_audio_gbl_var->audio_vm_amr_record.l1_state)
  }
#endif // _WINDOWS

#endif /* VM_AMR_RECORD */
#endif /* RVM_AUDIO_MAIN_SWE */

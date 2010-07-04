/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_vm_play.c                                             */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            Voice Memorization AMR play task.                             */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
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
  #include "rfs/fscore_types.h"
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
  #include "Audio/audio_features_i.h"

  /* include the usefull L1 header */
  #define BOOL_FLAG
  #define CHAR_FLAG
  #include "l1_types.h"
  #include "l1audio_cust.h"
  #include "l1audio_msgty.h"
  #include "l1audio_signa.h"

  #include "Audio/audio_macro_i.h"

  T_AUDIO_RET audio_convert_from_mms(UINT8 *p_buffer, UINT16 buffer_size, UINT8 *previous_type, UINT8 *size_left);
  INT16 ram_read(T_AUDIO_VOICE_MEMO_AMR_PLAY_FROM_MEM_VAR *vm_amr_play, UINT8 *dest_buffer, UINT16 size);

 INT16 audio_vm_amr_ff(UINT8 *play_buffer, UINT32 Counter);
 void audio_init_vm_amr_ff_rewind();
  void audio_vm_amr_frame_index();
  T_AUDIO_RET audio_vm_amr_rw(UINT8 *play_buffer);

extern INT32 vm_amr_skip_time;

// All these global variables for Rewind implementation
UINT32 current_frame_counter = 0, seek_pointer = 0, global_file_pointer = 0;
INT32 rewind_jump_counter =0 ;
typedef struct 
{
  UINT32 frame_counter;
  UINT32 file_seek;
} frame_index_s;

frame_index_s frame_index[3]; // In init frame_index[n].frame_counter needs to be filled with 0xFFFFFFFF


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_amr_play_send_status                            */
  /*                                                                              */
  /*    Purpose:  This function sends the voice memorization AMR play status      */
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
  void audio_vm_amr_play_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_AMR_PLAY_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    /* allocate the message buffer */
    while (mb_status == RVF_RED)
    {
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_AMR_PLAY_STATUS),
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
    p_send_message->os_hdr.msg_id = AUDIO_AMR_PLAY_FROM_MEM_STATUS_MSG;

    /* fill the status parameters */
    p_send_message->status = status;

    /* send message or call callback */
    if (return_path.callback_func == NULL)
      rvf_send_msg (return_path.addr_id, p_send_message);
    else
    {
      (*return_path.callback_func)((void *)(p_send_message));
       rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_amr_play_from_memory_manager                    */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a voice memorization play     */
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
  void audio_vm_amr_play_from_memory_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    T_RV_RETURN my_return_path;

    /**************** audio_vm_amr_play_from_memory_manager function begins ***********************/
    switch(p_audio_gbl_var->audio_vm_amr_play.state)
    {
      case AUDIO_IDLE:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_VM_AMR_PLAY_FROM_FFS_START_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_START_REQ:
          {
            T_AUDIO_DRIVER_PARAMETER driver_parameter;

            switch (p_message->msg_id)
            {
              case AUDIO_VM_AMR_PLAY_FROM_FFS_START_REQ:
              {
                /* save the return path + ffs_fd */
              #if (AUDIO_RAM_MANAGER)
                p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd = NULL;
              #endif
                p_audio_gbl_var->audio_vm_amr_play.return_path.callback_func =
                  ((T_AUDIO_VM_AMR_PLAY_FROM_FFS_START *)p_message)->return_path.callback_func;
                p_audio_gbl_var->audio_vm_amr_play.return_path.addr_id   =
                  ((T_AUDIO_VM_AMR_PLAY_FROM_FFS_START*)p_message)->return_path.addr_id;
              #if (AUDIO_NEW_FFS_MANAGER)
                p_audio_gbl_var->audio_vm_amr_play.ffs_fd =
                  ((T_AUDIO_VM_AMR_PLAY_FROM_FFS_START *)p_message)->audio_ffs_fd;
              #endif
             p_audio_gbl_var->audio_vm_amr_play.start_req  = TRUE;
              }
              break;
              case AUDIO_VM_AMR_PLAY_FROM_RAM_START_REQ:
              {
                /* save the return path + ram_fd */
              #if (AUDIO_NEW_FFS_MANAGER)
                p_audio_gbl_var->audio_vm_amr_play.ffs_fd = -1;
              #endif
                p_audio_gbl_var->audio_vm_amr_play.return_path.callback_func =
                  ((T_AUDIO_VM_AMR_PLAY_FROM_RAM_START *)p_message)->return_path.callback_func;
                p_audio_gbl_var->audio_vm_amr_play.return_path.addr_id   =
                  ((T_AUDIO_VM_AMR_PLAY_FROM_RAM_START*)p_message)->return_path.addr_id;
              #if (AUDIO_RAM_MANAGER)
                p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd =
                  ((T_AUDIO_VM_AMR_PLAY_FROM_RAM_START *)p_message)->p_buffer;
                p_audio_gbl_var->audio_vm_amr_play.audio_ram_size =
                  ((T_AUDIO_VM_AMR_PLAY_FROM_RAM_START *)p_message)->buffer_size;
              #endif
              }
              break;
            } // switch (p_message->msg_id)

            /* driver parameters */
            driver_parameter.nb_buffer   = AUDIO_VM_AMR_PLAY_NB_BUFFER;
            driver_parameter.buffer_size = AUDIO_VM_AMR_PLAY_SIZE;// 16 bit words

            /* return_path for driver */
            my_return_path.callback_func = NULL;
            my_return_path.addr_id       = p_audio_gbl_var->addrId;

            /* Init driver */
            audio_driver_init_vm_amr_play_session(&driver_parameter, my_return_path);
	    	    audio_init_vm_amr_ff_rewind();


            p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_WAIT_CHANNEL_ID;
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
            /* do not send a status message because of pre-emption issues
               An automatic stop can pre-empt a stop request. A status is sent + back in state idle
               then the stop request is received and another status is sent, which can be misinterpreted */
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_PAUSE_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_RESUME_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
          break;
        } // switch(p_message->msg_id)
      } // case AUDIO_IDLE:
      break;

      case AUDIO_WAIT_CHANNEL_ID:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_DRIVER_INIT_STATUS_MSG:
          {
            UINT8 *play_buffer;
            INT16 size_read = 0;

            /* check init is successfull otherwise close file and send status AUDIO_ERROR */
            if (((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status == AUDIO_OK)
            {
              /* get channel id */
              p_audio_gbl_var->audio_vm_amr_play.channel_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              // initializations
              p_audio_gbl_var->audio_vm_amr_play.session_id       = AUDIO_VM_AMR_PLAY_SESSION_ID;
              p_audio_gbl_var->audio_vm_amr_play.size             = AUDIO_VM_AMR_PLAY_SIZE << 1;  // temporary RAM buffer size
              p_audio_gbl_var->audio_vm_amr_play.size_left        = 0;
              p_audio_gbl_var->audio_vm_amr_play.previous_type    = AUDIO_VM_AMR_RXTX_SPEECH_GOOD;
              p_audio_gbl_var->audio_vm_amr_play.stop_req_allowed = TRUE;
              p_audio_gbl_var->audio_vm_amr_play.pause_req_allowed= TRUE; 
              p_audio_gbl_var->audio_vm_amr_play.last_stop_msg    = FALSE; 
              p_audio_gbl_var->audio_vm_amr_play.last_pause_msg   = FALSE; 
              p_audio_gbl_var->audio_vm_amr_play.last_resume_msg  = FALSE;

              // fill + convert all buffers in advance
              while(audio_driver_get_play_buffer(p_audio_gbl_var->audio_vm_amr_play.channel_id, &play_buffer) == AUDIO_OK)
              {
              #if (AUDIO_NEW_FFS_MANAGER)
                // write from FLASH to RAM buffer
#if(AS_RFS_API == 1)
				if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
                {
                  size_read = (INT16)rfs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                       play_buffer,
                                       p_audio_gbl_var->audio_vm_amr_play.size);
                  if(size_read < RFS_EOK)
              {
                    AUDIO_SEND_TRACE("AUDIO AMR PLAY: FFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
                    size_read = 0;  // will fill buffer with END_MASK and stop task
                  }
              }
#else
				if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
                {
                  size_read = ffs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                       play_buffer,
                                       p_audio_gbl_var->audio_vm_amr_play.size);
                  if(size_read < EFFS_OK)
              {
                    AUDIO_SEND_TRACE("AUDIO AMR PLAY: FFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
                    size_read = 0;  // will fill buffer with END_MASK and stop task
                  }
              }
#endif
            #endif
            #if (AUDIO_RAM_MANAGER)
                // write from RAM to RAM buffer
                if(p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL)
                  size_read = ram_read(&(p_audio_gbl_var->audio_vm_amr_play), play_buffer, p_audio_gbl_var->audio_vm_amr_play.size);
            #endif

                // convert to MMS
                if(audio_convert_from_mms(play_buffer, size_read,
                                          &(p_audio_gbl_var->audio_vm_amr_play.previous_type),
                                          &(p_audio_gbl_var->audio_vm_amr_play.size_left)) != AUDIO_OK)
                {
                  AUDIO_SEND_TRACE("AUDIO AMR PLAY: wrong format", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will fill buffer with END_MASK and stop task
                }

                // last buffer already, put END_MASK
                if((UINT16)size_read < p_audio_gbl_var->audio_vm_amr_play.size)
                {
                  UINT16 i;

                  if(p_audio_gbl_var->audio_vm_amr_play.size_left != 0)
                    AUDIO_SEND_TRACE_PARAM("AUDIO AMR PLAY: MMS PLAY file incomplete", p_audio_gbl_var->audio_vm_amr_play.size_left,
                                           RV_TRACE_LEVEL_ERROR);

                  for(i = size_read; i < p_audio_gbl_var->audio_vm_amr_play.size; i++)
                    *(play_buffer + i) = SC_VM_AMR_END_MASK;
                }

                AUDIO_SEND_TRACE_PARAM("AUDIO AMR PLAY: FFS or RAM to RAM size", size_read,
                                       RV_TRACE_LEVEL_DEBUG_LOW);

                audio_driver_play_buffer(p_audio_gbl_var->audio_vm_amr_play.channel_id, play_buffer);
              }

              // initialize the return path
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              // send message
              audio_driver_start_session(p_audio_gbl_var->audio_vm_amr_play.channel_id, my_return_path);

              /* change state */
              p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION;
            } // if (((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status == AUDIO_OK)
            else
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_START_EVENT);
            #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
				if (p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
#else
			  if (p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
#endif
              {
#if(AS_RFS_API == 1)
				if(rfs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != RFS_EOK)
#else
				if(ffs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != EFFS_OK)
#endif
                  audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              }
            #endif
              audio_vm_amr_play_send_status (AUDIO_ERROR, p_audio_gbl_var->audio_vm_amr_play.return_path);
              /* change state */
              p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ:
          #if (AUDIO_RAM_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL) )
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
            }
          #endif
          #if (AUDIO_NEW_FFS_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK) )
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
            }
          #endif
            /* change state */
            p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_WAIT_CHANNEL_ID_TO_STOP;
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_PAUSE_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_RESUME_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
          break;
       
        } // switch(p_message->msg_id)
      } // case AUDIO_WAIT_CHANNEL_ID:
      break;

   
      case AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_DRIVER_NOTIFICATION_MSG:
          {
            UINT8 *play_buffer;
            INT16 size_read = 0;

            // try to get a buffer
            if(audio_driver_get_play_buffer(p_audio_gbl_var->audio_vm_amr_play.channel_id, &play_buffer) == AUDIO_OK)
          {

		if(vm_amr_skip_time > 0)
		{
		  size_read = audio_vm_amr_ff(play_buffer,((vm_amr_skip_time * 1000)/20)); 
		}
		else if(vm_amr_skip_time < 0)
		{
		  size_read = audio_vm_amr_rw(play_buffer);
		}

          /*
           * Buffer needes to be filled up only in case the forward or rewind is not called
           * Since the foward or rewind fills the buffer according to the skip time
           * the followind reading needs to be skiped.
           */

          if(vm_amr_skip_time == 0)
          {

          #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
			 if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
              {
                size_read = (INT16)rfs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_vm_amr_play.size);

                if(size_read < RFS_EOK)
            {
                  AUDIO_SEND_TRACE("AUDIO AMR PLAY: FFS PLAY READ FILED", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                }
            }
#else
			  if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
              {
                size_read = ffs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_vm_amr_play.size);

                if(size_read < EFFS_OK)
            {
                  AUDIO_SEND_TRACE("AUDIO AMR PLAY: FFS PLAY READ FILED", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                }
            }
#endif
          #endif
          #if (AUDIO_RAM_MANAGER)
              if(p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL)
                size_read = ram_read(&(p_audio_gbl_var->audio_vm_amr_play), play_buffer,
                                     p_audio_gbl_var->audio_vm_amr_play.size);
            #endif
           } // end if(vm_amr_skip_time == 0)

          vm_amr_skip_time = 0;

            if(size_read > 0)
            {

              if(audio_convert_from_mms(play_buffer,size_read,
                                        &(p_audio_gbl_var->audio_vm_amr_play.previous_type),
                                        &(p_audio_gbl_var->audio_vm_amr_play.size_left)) != AUDIO_OK)
            {
                AUDIO_SEND_TRACE("AUDIO AMR PLAY: wrong MMS format", RV_TRACE_LEVEL_ERROR);
                size_read=0;  // will fill buffer with END_MASK
            }
			  }else
            		{
                     AUDIO_SEND_TRACE("Convert MMS not need since the Buffer is over", RV_TRACE_LEVEL_ERROR);
            		}


              // last buffer, put END_MASK
              if((UINT16)size_read < p_audio_gbl_var->audio_vm_amr_play.size)
            {
                UINT16 i;

                if(p_audio_gbl_var->audio_vm_amr_play.size_left != 0)
                  AUDIO_SEND_TRACE_PARAM("AUDIO AMR PLAY: MMS PLAY file incomplete",
                                         p_audio_gbl_var->audio_vm_amr_play.size_left,RV_TRACE_LEVEL_ERROR);
                for(i=size_read; i<p_audio_gbl_var->audio_vm_amr_play.size; i++)
                  *(play_buffer+i) = SC_VM_AMR_END_MASK;
            }

              if(size_read > 0)
                AUDIO_SEND_TRACE_PARAM("AUDIO AMR PLAY: FFS or RAM to RAM size", size_read, RV_TRACE_LEVEL_DEBUG_LOW);
              else
                AUDIO_SEND_TRACE("AUDIO AMR PLAY: buffer not used",RV_TRACE_LEVEL_DEBUG_LOW);

              audio_driver_play_buffer(p_audio_gbl_var->audio_vm_amr_play.channel_id, play_buffer);
            } // if(audio_driver_get_play_buffer(channel_id,&p_buffer)==AUDIO_OK)
            else
              AUDIO_SEND_TRACE("AUDIO AMR PLAY: no buffer available", RV_TRACE_LEVEL_DEBUG_LOW);
          }
          break;  // case AUDIO_DRIVER_NOTIFICATION_MSG

          case AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ:
          {
          #if (AUDIO_RAM_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL) )
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
            }
          #endif
          #if (AUDIO_NEW_FFS_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK) )
              {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
              }
          #endif
            if(p_audio_gbl_var->audio_vm_amr_play.stop_req_allowed == TRUE)
            {
              p_audio_gbl_var->audio_vm_amr_play.stop_req_allowed = FALSE;
              p_audio_gbl_var->audio_vm_amr_play.last_stop_msg = TRUE;
              audio_driver_stop_session(p_audio_gbl_var->audio_vm_amr_play.channel_id);
            }
            else
              /* do not send a status message because of pre-emption issues */
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;
          case AUDIO_DRIVER_STATUS_MSG:
      {
      	      if(p_audio_gbl_var->audio_vm_amr_play.start_req == TRUE)
            {
              p_audio_gbl_var->audio_vm_amr_play.start_req = FALSE;
//Start Nina added for AMR play fail (second time)issue
            audio_vm_amr_play_send_status(AUDIO_OK, p_audio_gbl_var->audio_vm_amr_play.return_path);
              p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_IDLE;
//End Nina added for AMR play fail (second time)issue
            } 
      	      else if(p_audio_gbl_var->audio_vm_amr_play.last_pause_msg == TRUE)
            {
              p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_WAIT_STOP_OR_RESUME;
              p_audio_gbl_var->audio_vm_amr_play.last_pause_msg = FALSE;
            }
            else
            {
          #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
			if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
#else
			if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
#endif
        {
              AUDIO_SEND_TRACE("AUDIO AMR PLAY: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
#if(AS_RFS_API == 1)
				if(rfs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != RFS_EOK)
#else
			  if(ffs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != EFFS_OK)
#endif
                audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
            }
          #endif
            audio_vm_amr_play_send_status(AUDIO_OK, p_audio_gbl_var->audio_vm_amr_play.return_path);
              p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_IDLE;
            }
      }
          break;
          
          case AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_PAUSE_REQ:
          {  
          if(p_audio_gbl_var->audio_vm_amr_play.pause_req_allowed == TRUE)
            {
              AUDIO_SEND_TRACE("AUDIO VM AMR: PAUSE Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_vm_amr_play.pause_req_allowed = FALSE;
              p_audio_gbl_var->audio_vm_amr_play.last_pause_msg = TRUE;
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;
            
             audio_driver_pause_session(p_audio_gbl_var->audio_vm_amr_play.channel_id,my_return_path);
            }
            else
              /* do not send a status message because of pre-emption issues */
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_RESUME_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
          break;
        
        } // switch (p_message->msg_id)
      } // case AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION:
      break;
  
      case AUDIO_WAIT_STOP_OR_RESUME:
      {
        switch (p_message->msg_id)
        {
case AUDIO_DRIVER_NOTIFICATION_MSG:
          {
            UINT8 *play_buffer;
            INT16 size_read = 0;

            // try to get a buffer
            if(audio_driver_get_play_buffer(p_audio_gbl_var->audio_vm_amr_play.channel_id, &play_buffer) == AUDIO_OK)
          {
          #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
			 if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
              {
                size_read = (INT16)rfs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_vm_amr_play.size);

                if(size_read < RFS_EOK)
            {
                  AUDIO_SEND_TRACE("AUDIO AMR PLAY: FFS PLAY READ FILED", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                }
            }
#else
			  if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
              {
                size_read = ffs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_vm_amr_play.size);

                if(size_read < EFFS_OK)
            {
                  AUDIO_SEND_TRACE("AUDIO AMR PLAY: FFS PLAY READ FILED", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                }
            }
#endif
          #endif
          #if (AUDIO_RAM_MANAGER)
              if(p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL)
                size_read = ram_read(&(p_audio_gbl_var->audio_vm_amr_play), play_buffer,
                                     p_audio_gbl_var->audio_vm_amr_play.size);
            #endif
              if(audio_convert_from_mms(play_buffer,size_read,
                                        &(p_audio_gbl_var->audio_vm_amr_play.previous_type),
                                        &(p_audio_gbl_var->audio_vm_amr_play.size_left)) != AUDIO_OK)
            {
                AUDIO_SEND_TRACE("AUDIO AMR PLAY: wrong MMS format", RV_TRACE_LEVEL_ERROR);
                size_read=0;  // will fill buffer with END_MASK
            }

              // last buffer, put END_MASK
              if((UINT16)size_read < p_audio_gbl_var->audio_vm_amr_play.size)
            {
                UINT16 i;

                if(p_audio_gbl_var->audio_vm_amr_play.size_left != 0)
                  AUDIO_SEND_TRACE_PARAM("AUDIO AMR PLAY: MMS PLAY file incomplete",
                                         p_audio_gbl_var->audio_vm_amr_play.size_left,RV_TRACE_LEVEL_ERROR);
                for(i=size_read; i<p_audio_gbl_var->audio_vm_amr_play.size; i++)
                  *(play_buffer+i) = SC_VM_AMR_END_MASK;
            }

              if(size_read > 0)
                AUDIO_SEND_TRACE_PARAM("AUDIO AMR PLAY: FFS or RAM to RAM size", size_read, RV_TRACE_LEVEL_DEBUG_LOW);
              else
                AUDIO_SEND_TRACE("AUDIO AMR PLAY: buffer not used",RV_TRACE_LEVEL_DEBUG_LOW);

              audio_driver_play_buffer(p_audio_gbl_var->audio_vm_amr_play.channel_id, play_buffer);
            } // if(audio_driver_get_play_buffer(channel_id,&p_buffer)==AUDIO_OK)
            else
              AUDIO_SEND_TRACE("AUDIO AMR PLAY: no buffer available", RV_TRACE_LEVEL_DEBUG_LOW);
          }
          break;  // case AUDIO_DRIVER_NOTIFICATION_MSG
         

          case AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ:
          {
          #if (AUDIO_RAM_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL) )
            {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
            }
          #endif
          #if (AUDIO_NEW_FFS_MANAGER)
            if ( (p_message->msg_id == AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ)
              && (p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK) )
              {
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
              return;
              }
          #endif
            if(p_audio_gbl_var->audio_vm_amr_play.stop_req_allowed == TRUE)
            {
              p_audio_gbl_var->audio_vm_amr_play.stop_req_allowed = FALSE;
              p_audio_gbl_var->audio_vm_amr_play.last_stop_msg = TRUE;
              audio_driver_stop_session(p_audio_gbl_var->audio_vm_amr_play.channel_id);
            }
            else
              /* do not send a status message because of pre-emption issues */
              audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;
          case AUDIO_DRIVER_STATUS_MSG:
      {

            if(p_audio_gbl_var->audio_vm_amr_play.last_resume_msg == TRUE)
            {
              p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION;
              p_audio_gbl_var->audio_vm_amr_play.last_resume_msg = FALSE;
            }
            
            else
            {
            #if (AUDIO_NEW_FFS_MANAGER)
              #if(AS_RFS_API == 1)
		          	if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
              #else
                if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
              #endif
                 {
                     AUDIO_SEND_TRACE("AUDIO AMR PLAY: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              #if(AS_RFS_API == 1)
			         if(rfs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != RFS_EOK)
              #else
               if(ffs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != EFFS_OK)
              #endif
                audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
            }
            #endif
             audio_vm_amr_play_send_status(AUDIO_OK, p_audio_gbl_var->audio_vm_amr_play.return_path);
             p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_IDLE;
             p_audio_gbl_var->audio_vm_amr_play.last_stop_msg = FALSE;  
            }
            
      }
            break;
          
          case AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_PAUSE_REQ:
          {  
           /* do not send a status message because of pre-emption issues */
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_RESUME_REQ:
          {
             AUDIO_SEND_TRACE("AUDIO VM AMR: RESUME Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
             p_audio_gbl_var->audio_vm_amr_play.last_resume_msg   = TRUE; 
             p_audio_gbl_var->audio_vm_amr_play.pause_req_allowed = TRUE; 
             my_return_path.callback_func = NULL;
             my_return_path.addr_id       = p_audio_gbl_var->addrId;

             audio_driver_resume_session(p_audio_gbl_var->audio_vm_amr_play.channel_id, my_return_path);
        
           
          }
          break;
        
        } // switch (p_message->msg_id)
      } // case AUDIO_WAIT_STOP_OR_RESUME:
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
              p_audio_gbl_var->audio_vm_amr_play.channel_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              audio_driver_stop_session(p_audio_gbl_var->audio_vm_amr_play.channel_id);

              /* change state */
              p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_WAIT_DRIVER_STOP_CON;
            }
            else
            {
              /* close file */
            #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
				if (p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)				
#else
			  if (p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
#endif
              {
#if(AS_RFS_API == 1)
				if ( rfs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != EFFS_OK )
#endif
                  audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                AUDIO_SEND_TRACE("AUDIO AMR PLAY: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }
            #endif

              audio_vm_amr_play_send_status (AUDIO_OK, p_audio_gbl_var->audio_vm_amr_play.return_path);

              /* change state */
              p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ:
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_PAUSE_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_RESUME_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
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
			if (p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
#else
			if (p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
#endif
              {
#if(AS_RFS_API == 1)
				if ( rfs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != RFS_EOK )
#else
				if ( ffs_close(p_audio_gbl_var->audio_vm_amr_play.ffs_fd) != EFFS_OK )
#endif
                audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              AUDIO_SEND_TRACE("AUDIO AMR PLAY: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }
            #endif

            audio_vm_amr_play_send_status (AUDIO_OK, p_audio_gbl_var->audio_vm_amr_play.return_path);
              p_audio_gbl_var->audio_vm_amr_play.state = AUDIO_IDLE;
            }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ:
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_PAUSE_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ:
          case AUDIO_VM_AMR_PLAY_FROM_RAM_RESUME_REQ:
          {
            audio_voice_memo_amr_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
          break;
        }
      } //case AUDIO_WAIT_DRIVER_STOP_CON:
      break;
    } // switch(p_audio_gbl_var->audio_vm_amr_play.state)
  } /*********************** End of audio_vm_amr_play_from_memory_manager function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_vm_amr_play_manager                         */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a voice memorization AMR play */
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
  T_AUDIO_RET audio_driver_vm_amr_play_manager (T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session)
  {
    /**************** audio_driver_vm_amr_play_manager function begins ***********************/
    switch(p_session->session_info.state)
    {
      case AUDIO_DRIVER_CHANNEL_WAIT_INIT:
      {
        /* init buffer index, layer1 not valid until 1st buffer is filled */
        /* index_l1 will be set to 0 when get_play_buffer() is called in WAIT_START state */
        p_session->session_info.index_l1    = 0xFF;
        p_session->session_info.index_appli = 0;
        p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;

        /* allocate the buffer for the message to the L1 */
        p_session->session_req.p_l1_send_message =
          audio_allocate_l1_message(sizeof(T_MMI_VM_AMR_PLAY_REQ));
        ((T_MMI_VM_AMR_PLAY_REQ *)(p_session->session_req.p_l1_send_message))->session_id =
          AUDIO_VM_AMR_PLAY_SESSION_ID;

        if (p_session->session_req.p_l1_send_message != NULL )
          return (AUDIO_OK);
        else
          return (AUDIO_ERROR);
      }
//omaps00090550        break;

      case AUDIO_DRIVER_CHANNEL_WAIT_START:
      {
        /* send the start voice memo play message to the L1 */
        audio_send_l1_message(MMI_VM_AMR_PLAY_START_REQ,
                              p_session->session_req.p_l1_send_message);
        return (AUDIO_OK);
      }
//omaps00090550        break;

      case AUDIO_DRIVER_CHANNEL_WAIT_STOP:
      {
        /* send the stop command to the audio L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if ( p_send_message != NULL)
        {
         if (p_audio_gbl_var->audio_vm_amr_play.last_stop_msg == TRUE)
          {
          /* send the stop command to the audio L1 */
          audio_send_l1_message(MMI_VM_AMR_PLAY_STOP_REQ, p_send_message);
         
          }
        
        else
          {
            if(p_audio_gbl_var->audio_vm_amr_play.last_pause_msg == TRUE)
            {
              /* send the pause command to the audio L1 */
              audio_send_l1_message(MMI_VM_AMR_PAUSE_REQ, p_send_message);
            }
            else
            {
              if(p_audio_gbl_var->audio_vm_amr_play.last_resume_msg == TRUE)
                 /* send the resume command to the audio L1 */
                audio_send_l1_message(MMI_VM_AMR_RESUME_REQ, p_send_message);
              
            }
          }

          return (AUDIO_OK);
        }
        return (AUDIO_ERROR);
      }
//omaps00090550        break;
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_STOP:
      {
        /* send the stop command to the audio L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if ( p_send_message != NULL)
        {
          /* send the stop command to the audio L1 */
          audio_send_l1_message(MMI_VM_AMR_PLAY_STOP_REQ, p_send_message);
          return (AUDIO_OK);
        }
        return (AUDIO_ERROR);
      }
//omaps00090550        break;
      
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_PAUSE:
      {
        /* send the pause command to the audio L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if ( p_send_message != NULL)
        {
          /* send the pause command to the audio L1 */
          audio_send_l1_message(MMI_VM_AMR_PAUSE_REQ, p_send_message);
          return (AUDIO_OK);
        }
        return (AUDIO_ERROR);
      }
    }
	// no corresponding message found?
	return (AUDIO_ERROR);
  } /*********************** End of audio_vm_play_manager function **********************/

  T_AUDIO_RET audio_convert_from_mms(UINT8 *p_buffer, UINT16 buffer_size, UINT8 *previous_type, UINT8 *size_left)
  {
    UINT8 frame_header, data_size = 0; //OMAPS00090550
    UINT8 frame_type, quality;
    UINT8 *ptr_final, *ptr_mms;

    ptr_mms = p_buffer;
    ptr_final = ptr_mms + buffer_size;

    /* a sample is split between 2 RAM buffers */
    if (*size_left > 0)
    {
      /* if SID sample, remove STI and mode indication */
      if (*previous_type == AUDIO_MMS_SID_FRAME_TYPE)
      {
        *(ptr_mms + *size_left - 1) &= (~(AUDIO_MMS_STI_BIT | AUDIO_MMS_MODE_INDICATION));
      }
      ptr_mms += *size_left;
      *size_left = 0;
    }

    while (ptr_mms < ptr_final)
    {
      /* read header */
      frame_header = *ptr_mms;

      /* reset header */
      *ptr_mms = 0;

      /* FT and Q */
      frame_type = (frame_header & AUDIO_MMS_FRAME_TYPE_MASK) >> AUDIO_MMS_FRAME_TYPE_SHIFT;
      quality    = (frame_header & AUDIO_MMS_QUALITY_MASK) >> AUDIO_MMS_QUALITY_SHIFT;
      *previous_type = frame_type;

      /* Identify sample */
      if (frame_type < AUDIO_MMS_SID_FRAME_TYPE)
      {
        /* speech good or bad */
        *ptr_mms |= frame_type;
        if (quality == AUDIO_MMS_GOOD_QUALITY)
          *ptr_mms |= AUDIO_VM_AMR_RXTX_SPEECH_GOOD;
        else
          *ptr_mms |= AUDIO_VM_AMR_RXTX_SPEECH_BAD;

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
      }
      else if (frame_type == AUDIO_MMS_SID_FRAME_TYPE)
      {
        data_size = AUDIO_VM_AMR_SID_DATA_SIZE;
        /* SID_BAD */
        if (quality == AUDIO_MMS_BAD_QUALITY)
          *ptr_mms |= AUDIO_VM_AMR_RXTX_SID_BAD;
        /* SID_FIRST or SID_UPDATE */
        else
        {
          if (*previous_type == AUDIO_MMS_NO_DATA_FRAME_TYPE)
            *ptr_mms |= AUDIO_VM_AMR_RXTX_SID_UPDATE;
          else
            *ptr_mms |= AUDIO_VM_AMR_RXTX_SID_FIRST;
          /* try to remove STI + mode indication if sample not split between 2 buffers */
          if ((ptr_final - ptr_mms) >= (data_size + 1))
            *(ptr_mms + data_size) &= (~(AUDIO_MMS_STI_BIT | AUDIO_MMS_MODE_INDICATION));
        }
      }
      else if (frame_type == AUDIO_MMS_NO_DATA_FRAME_TYPE)
      {
        data_size = AUDIO_VM_AMR_NO_DATA_DATA_SIZE;
        *ptr_mms |= AUDIO_VM_AMR_RXTX_NO_DATA;
      }
      else
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MEMORY MANAGER: convert from MMS, header not recognized", frame_header, RV_TRACE_LEVEL_DEBUG_LOW);
        return AUDIO_ERROR;
      }

      /* pass header */
      ptr_mms++;

      /* write data, check we are not at the end of the buffer */
      if ((ptr_final - ptr_mms) < data_size)
      {
        *size_left = data_size - (ptr_final - ptr_mms);
        global_file_pointer += (data_size+1);

        data_size = ptr_final - ptr_mms;
      }
     else
       global_file_pointer += (data_size+1);

	  
      ptr_mms += data_size;
      current_frame_counter++;
      audio_vm_amr_frame_index();

    }
    return AUDIO_OK;
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_amr_play_l1_simulator                           */
  /*                                                                              */
  /*    Purpose:  This function simulates the L1 for VM AMR PLAY                  */
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
  void audio_vm_amr_play_l1_simulator(UINT16 event, T_RV_HDR *p_message)
  {
    T_RVF_MB_STATUS mb_status;
#if(AS_RFS_API == 1)
	static T_RFS_FD fd;
#else
	static T_FFS_FD fd;
#endif
    static UINT8 align;

#if(AS_RFS_API == 1)
		T_WCHAR mp_uc1[50];
  		char *str1 = "/memo/amr_play_simu";
#endif
#if(AS_RFS_API == 1)
	convert_u8_to_unicode(str1, mp_uc1);
#endif

    switch(p_audio_gbl_var->audio_vm_amr_play.l1_state)
    {
      case AUDIO_IDLE:
      {
        if(p_message->msg_id == MMI_VM_AMR_PLAY_START_REQ)
        {
          T_RV_HDR *p_msg;

          align = 0;

          // open result file
#if(AS_RFS_API == 1)
		  fd = rfs_open(mp_uc1, RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
		  fd = ffs_open("/memo/amr_play_simu", FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif

          // start notification timer -> l1_counter shots
          rvf_start_timer(AUDIO_VM_AMR_PLAY_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1200),
                          AUDIO_VM_AMR_PLAY_L1_SIMUL_ONE_SHOT_TIMER);

          // send START_CON message
          mb_status = rvf_get_buf(p_audio_gbl_var->mb_internal,
                                  sizeof(T_RV_HDR),
                                  (T_RVF_BUFFER **)(&p_msg));
          if(mb_status == RVF_RED)
          {
            AUDIO_SEND_TRACE("L1 has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }

          p_msg->msg_id = MMI_VM_AMR_PLAY_START_CON;
          rvf_send_msg (p_audio_gbl_var->addrId, p_msg);

          p_audio_gbl_var->audio_vm_amr_play.l1_state = AUDIO_WAIT_STOP;
        }
      }
      break;

      case AUDIO_WAIT_STOP:
      {
        T_AUDIO_DRIVER_SESSION *p_session;
        p_session = &p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_vm_amr_play.channel_id];

        if(event & AUDIO_VM_AMR_PLAY_L1_SIMUL_TIMER_EVT_MASK)
        {
          T_RV_RETURN *return_path;
          UINT16 i = 0;

          T_AUDIO_DRIVER_NOTIFICATION *p_msg;
          UINT8 *p_buffer;

          return_path = &p_session->session_req.return_path;
          p_buffer = (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer;

          // realign buffer
          i += align;

          // parse buffer to reach auto stop or next buffer
          while ( (p_buffer[i] != 0xFF) && (i < p_session->session_req.size) )
          {
            switch(p_buffer[i] & (7<<3))
            {
              case (0<<3):
              case (1<<3):
              case (3<<3):
              {
                switch(p_buffer[i] & 7)
                {
                  case 0: i += 13; break; case 1: i += 14; break; case 2: i += 16; break;
                  case 3: i += 18; break; case 4: i += 20; break; case 5: i += 21; break;
                  case 6: i += 27; break; case 7: i += 32; break;
                }
              }
              break;
              case (2<<3): case (7<<3): i += 1; break;
              case (4<<3): case (5<<3): case (6<<3): i += 6; break;
            }
          }

          // AUTO STOP
          if (i < p_session->session_req.size)
          {
            T_RV_HDR *p_msg;

#if(AS_RFS_API == 1)
			rfs_write (fd, p_buffer, i);
            rfs_close(fd);
#else
			ffs_write (fd, p_buffer, i);
            ffs_close(fd);
#endif

            // allocate notification message
            mb_status = rvf_get_buf(p_audio_gbl_var->mb_internal,
                                    sizeof(T_RV_HDR),
                                    (T_RVF_BUFFER **)(&p_msg));
            if(mb_status == RVF_RED)
            {
              AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification", RV_TRACE_LEVEL_ERROR);
              return;
            }

            p_msg->msg_id = MMI_VM_AMR_PLAY_STOP_CON;

            rvf_send_msg(p_audio_gbl_var->addrId, p_msg);

            p_audio_gbl_var->audio_vm_amr_play.l1_state = AUDIO_IDLE;
          }
          else
          {
            rvf_start_timer(AUDIO_VM_AMR_PLAY_L1_SIMUL_TIMER,
                            RVF_MS_TO_TICKS(1200),
                            AUDIO_VM_AMR_PLAY_L1_SIMUL_ONE_SHOT_TIMER);

#if(AS_RFS_API == 1)
			rfs_write (fd, p_buffer, p_session->session_req.size);
#else
			ffs_write (fd, p_buffer, p_session->session_req.size);
#endif
            align = i - p_session->session_req.size;

            p_session->session_info.index_l1++;
            if(p_session->session_info.index_l1 == p_session->session_req.nb_buffer)
              p_session->session_info.index_l1 = 0;

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
            p_msg->channel_id = p_audio_gbl_var->audio_vm_amr_play.channel_id;
            p_msg->p_buffer = NULL;

            if(return_path->callback_func==NULL)
              rvf_send_msg(return_path->addr_id, p_msg);
            else
            {
              (*return_path->callback_func)((void *)p_msg);
              rvf_free_buf((T_RVF_BUFFER *)p_msg);
            }
          }
        } // if(event & AUDIO_VM_AMR_PLAY_L1_SIMUL_TIMER_EVT_MASK)

        if(p_message->msg_id == MMI_VM_AMR_PLAY_STOP_REQ)
        {
          T_RV_HDR *p_msg;

#if(AS_RFS_API == 1)
		  rfs_write (fd, (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer,(T_RFS_SIZE)align);
          rfs_close(fd);
#else
		  ffs_write (fd, (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer, align);
          ffs_close(fd);
#endif

          // allocate notification message
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_RV_HDR),
                                (T_RVF_BUFFER **)(&p_msg));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification", RV_TRACE_LEVEL_ERROR);
            return;
          }

          p_msg->msg_id = MMI_VM_AMR_PLAY_STOP_CON;

          rvf_send_msg(p_audio_gbl_var->addrId, p_msg);

          p_audio_gbl_var->audio_vm_amr_play.l1_state = AUDIO_IDLE;
        } // if(p_message->msg_id == MMI_VM_AMR_PLAY_STOP_REQ)
        if(p_message->msg_id == MMI_VM_AMR_PAUSE_REQ)
        {
          T_RV_HDR *p_msg;
#if(AS_RFS_API == 1)
		  rfs_write (fd, (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer,(T_RFS_SIZE)align);
          rfs_close(fd);
#else
          ffs_write (fd, (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer, align);
          ffs_close(fd);
#endif

          // allocate notification message
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_RV_HDR),
                                (T_RVF_BUFFER **)(&p_msg));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification", RV_TRACE_LEVEL_ERROR);
            return;
          }

          p_msg->msg_id = MMI_VM_AMR_PAUSE_CON;

          rvf_send_msg(p_audio_gbl_var->addrId, p_msg);

          p_audio_gbl_var->audio_vm_amr_play.l1_state = AUDIO_STOP;
        }// if(p_message->msg_id == MMI_VM_AMR_PAUSE_REQ)

        if(p_message->msg_id == MMI_VM_AMR_RESUME_REQ)
        {
          T_RV_HDR *p_msg;
#if(AS_RFS_API == 1)
		  rfs_write (fd, (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer,(T_RFS_SIZE)align);
          rfs_close(fd);
#else
          ffs_write (fd, (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer, align);
          ffs_close(fd);
#endif
          // allocate notification message
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_RV_HDR),
                                (T_RVF_BUFFER **)(&p_msg));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification", RV_TRACE_LEVEL_ERROR);
            return;
          }

          p_msg->msg_id = MMI_VM_AMR_RESUME_CON;

          rvf_send_msg(p_audio_gbl_var->addrId, p_msg);

          p_audio_gbl_var->audio_vm_amr_play.l1_state = AUDIO_STOP;
        }// if(p_message->msg_id == MMI_VM_AMR_RESUME_REQ)
      } // case AUDIO_WAIT_STOP:
      break;
    } // switch(p_audio_gbl_var->audio_vm_amr_play.l1_state)
  }
#endif // _WINDOWS


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_amr_ff                                          */
  /*                                                                              */
  /*    Purpose:  Function to forward the vm amr file                             */
  /*                                                                              */
  /*                                                                              */
  /********************************************************************************/


INT16 audio_vm_amr_ff(UINT8 *play_buffer, UINT32 counter)
{

  INT16 size_read = 0, size_remaining = 0;
  UINT8 frame_type, frame_header, data_size = 0, size_left_temp = 0;
  UINT8 *ptr_final, *ptr_mms;
  UINT8 last_buffer, skip_found;
  // Split frame cache
  UINT8 split_frame [AUDIO_VM_AMR_SPEECH_122_DATA_SIZE], split_frame_cached = 0, split_previous_type = 0;

  AUDIO_SEND_TRACE_PARAM("vm_amr_ff: skip time:", vm_amr_skip_time, RV_TRACE_LEVEL_DEBUG_LOW);



  // Read data to RAM Buffer

#if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
 {
  size_read = (INT16)rfs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_vm_amr_play.size);
 }

#else
 if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
 {
  size_read = ffs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_vm_amr_play.size);
 }
#endif
#endif

#if (AUDIO_RAM_MANAGER)
    // write from RAM to RAM buffer
 if(p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL)
 size_read = ram_read(&(p_audio_gbl_var->audio_vm_amr_play), play_buffer, p_audio_gbl_var->audio_vm_amr_play.size);
#endif

  ptr_mms = play_buffer;
  ptr_final = ptr_mms + size_read;

  /* a sample is split between 2 RAM buffers */
  if (p_audio_gbl_var->audio_vm_amr_play.size_left > 0)
  {
    // Cache the splited frame
    split_frame_cached = p_audio_gbl_var->audio_vm_amr_play.size_left;
    split_previous_type = p_audio_gbl_var->audio_vm_amr_play.previous_type;
    memcpy(split_frame, play_buffer, p_audio_gbl_var->audio_vm_amr_play.size_left);

    ptr_mms += p_audio_gbl_var->audio_vm_amr_play.size_left;
    p_audio_gbl_var->audio_vm_amr_play.size_left = 0;
  }


	if( vm_amr_skip_time < 0)
	{
          //ffs_seek(seek_pointer);
              /* seek to the position specified for Forward */
	#if(AS_RFS_API == 1)
	  if(rfs_lseek (p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                           seek_pointer,
                           RFS_SEEK_SET) < RFS_EOK )
	 #else
              if(ffs_seek ( p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                            seek_pointer,
                            FFS_SEEK_SET) < EFFS_OK )
         #endif
              {
                  AUDIO_SEND_TRACE("AUDIO VM AMR: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;
		  return AUDIO_ERROR;
              }

          size_read = ffs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                       play_buffer,
                                       p_audio_gbl_var->audio_vm_amr_play.size);
          ptr_mms = play_buffer;
          ptr_final = ptr_mms + size_read;
	}


  last_buffer = 0;
  skip_found  = 0;

// Skip the frame till counter reaches 0 or end of file

  while((counter > 0) && (skip_found == 0))
  {
    // Process the Last buffer and come out
    if(last_buffer==1)
      skip_found = 1;

    if((ptr_mms < ptr_final) && (p_audio_gbl_var->audio_vm_amr_play.size_left == 0))
    {
      frame_header = *ptr_mms;
      frame_type = (frame_header & AUDIO_MMS_FRAME_TYPE_MASK) >> AUDIO_MMS_FRAME_TYPE_SHIFT;

      p_audio_gbl_var->audio_vm_amr_play.previous_type = frame_type;

      /* Identify sample */
      if (frame_type < AUDIO_MMS_SID_FRAME_TYPE)
      {
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
      }
      else if (frame_type == AUDIO_MMS_SID_FRAME_TYPE)
      {
        data_size = AUDIO_VM_AMR_SID_DATA_SIZE;
      }
      else if (frame_type == AUDIO_MMS_NO_DATA_FRAME_TYPE)
      {
        data_size = AUDIO_VM_AMR_NO_DATA_DATA_SIZE;
      }
      else
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MEMORY MANAGER: convert from MMS, header not recognized", frame_header, RV_TRACE_LEVEL_DEBUG_LOW);
        return AUDIO_ERROR;
      }

     /* write data, check we are not at the end of the buffer */
     if ((ptr_final - ptr_mms) <= data_size)
     {
      // size_left_temp =
	   p_audio_gbl_var->audio_vm_amr_play.size_left = data_size - (ptr_final - ptr_mms) + 1;
     }
     else
       ptr_mms += (data_size + 1);

	   //current_frame_counter++;
	     global_file_pointer += (data_size + 1);
             current_frame_counter++;
             audio_vm_amr_frame_index();


     // reduce the skip time counter
     counter --;

     }
     else // If the current RAM buffer get over
     {
       AUDIO_SEND_TRACE("VM AMR FF - Nxt Buff", RV_TRACE_LEVEL_DEBUG_LOW);

#if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
  if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
	   size_read = (INT16)rfs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                       play_buffer,
                                       p_audio_gbl_var->audio_vm_amr_play.size);
#else
  if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
	   size_read = ffs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                       play_buffer,
                                       p_audio_gbl_var->audio_vm_amr_play.size);
#endif
#endif
#if (AUDIO_RAM_MANAGER)
    // write from RAM to RAM buffer
 if(p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL)
 size_read = ram_read(&(p_audio_gbl_var->audio_vm_amr_play), play_buffer, p_audio_gbl_var->audio_vm_amr_play.size);
#endif


       if((size_read < EFFS_OK) )
       {
         AUDIO_SEND_TRACE("AUDIO AMR PLAY: FFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
         size_read = 0;  
         skip_found = 1;
       }
       else
       {
         ptr_mms = play_buffer;
         ptr_final = ptr_mms + size_read;

         /* a sample is split between 2 RAM buffers */
         if (p_audio_gbl_var->audio_vm_amr_play.size_left > 0)
         {
           /* if SID sample, remove STI and mode indication */
           if (p_audio_gbl_var->audio_vm_amr_play.previous_type == AUDIO_MMS_SID_FRAME_TYPE)
           {
             *(ptr_mms + p_audio_gbl_var->audio_vm_amr_play.size_left - 1) &= (~(AUDIO_MMS_STI_BIT | AUDIO_MMS_MODE_INDICATION));
           }
           ptr_mms += p_audio_gbl_var->audio_vm_amr_play.size_left;
           p_audio_gbl_var->audio_vm_amr_play.size_left = 0;
         }

         // Check for Last buffer (For the loop to close)
         if(size_read < p_audio_gbl_var->audio_vm_amr_play.size)
         {
           AUDIO_SEND_TRACE("Reached Last Buffer", RV_TRACE_LEVEL_ERROR);
           last_buffer = 1;
         }
       } // if(size_read < EFS_OK)
     } // If the current buffer get over
  } // end while


  AUDIO_SEND_TRACE_PARAM("Counter :", counter, RV_TRACE_LEVEL_ERROR);

// Need to retain the size left for convert_mms function
//  p_audio_gbl_var->audio_vm_amr_play.size_left = size_left_temp;

  // Copy the split frame for frame sent to L1 before forward operation
  if (split_frame_cached > 0)
  {
    memcpy(play_buffer, split_frame, split_frame_cached);
    p_audio_gbl_var->audio_vm_amr_play.size_left =  split_frame_cached;
    p_audio_gbl_var->audio_vm_amr_play.previous_type = split_previous_type;
    if(split_frame_cached > (ptr_mms - play_buffer))
    {
      AUDIO_SEND_TRACE("split frame cached more than the skip frame", RV_TRACE_LEVEL_ERROR);
#if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
#else
if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
#endif
 {

 #if(AS_RFS_API == 1)
	  if(rfs_lseek (p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                           -(ptr_final - ptr_mms),
                           RFS_SEEK_CUR) < RFS_EOK )
 #else
      if(ffs_seek (p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                           -(ptr_final - ptr_mms),
                           FFS_SEEK_CUR) < EFFS_OK )
 #endif
      {
        AUDIO_SEND_TRACE("AUDIO MP3: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
		size_read = split_frame_cached;
      }
 }
#endif
#if (AUDIO_RAM_MANAGER)
      if(p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL)
	    p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd -= (ptr_final - ptr_mms);
#endif
      // In this case do not copy the remaining data
      ptr_mms = ptr_final; // forcing not to copy the remaining data
    }
  }
    else
     p_audio_gbl_var->audio_vm_amr_play.size_left =  0;	


  // Do fast forward only if the end of file is not reached
  if(counter == 0)
  {
    // remaining data in ram buffer after skipping forward frames
    size_remaining = ptr_final - ptr_mms;
    if(size_remaining > 0)
    {
      memcpy((play_buffer + split_frame_cached), ptr_mms, size_remaining);
    }
    else
      size_remaining = 0;

    if((p_audio_gbl_var->audio_vm_amr_play.size - (size_remaining + split_frame_cached)) > 0)
    {
#if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= RFS_EOK)
	  size_read = (INT16)rfs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                  (play_buffer + (size_remaining + split_frame_cached)),
                                  (p_audio_gbl_var->audio_vm_amr_play.size - (size_remaining + split_frame_cached)));
#else
 if(p_audio_gbl_var->audio_vm_amr_play.ffs_fd >= EFFS_OK)
	  size_read = ffs_read(p_audio_gbl_var->audio_vm_amr_play.ffs_fd,
                                  (play_buffer + (size_remaining + split_frame_cached)),
                                  (p_audio_gbl_var->audio_vm_amr_play.size - (size_remaining + split_frame_cached)));
#endif
#endif
#if (AUDIO_RAM_MANAGER)
    // write from RAM to RAM buffer
     if(p_audio_gbl_var->audio_vm_amr_play.audio_ram_fd != NULL)
     size_read = ram_read(&(p_audio_gbl_var->audio_vm_amr_play), 
      								(play_buffer + (size_remaining + split_frame_cached)), 
      								(p_audio_gbl_var->audio_vm_amr_play.size) - (size_remaining + split_frame_cached));
#endif


      if(size_read < EFFS_OK)
      {
        int i = 0;
        AUDIO_SEND_TRACE("AUDIO AMR PLAY: FFS PLAY READ failed at vm_amr_ff", RV_TRACE_LEVEL_ERROR);
          size_read = size_remaining + split_frame_cached;

      }
      else if(size_read < (p_audio_gbl_var->audio_vm_amr_play.size - (size_remaining + split_frame_cached)))
      { // size_read less than requested size
        int i = 0;
          size_read = size_remaining + split_frame_cached;

      }
      else // size_read is fine
        size_read += size_remaining + split_frame_cached;
    }
    else
    {
      size_read = size_remaining + split_frame_cached;
    }
  } // if(counter == 0)
  else
  {
    int i =0;
    AUDIO_SEND_TRACE("VM_AMR_FF : end of file Reached ", RV_TRACE_LEVEL_ERROR);
    // Fill the RAM Buffer with end of MASK
   

	size_read = split_frame_cached;

  }

  return size_read;

} // end of function void audio_vm_amr_ff


/********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_amr_rw                                          */
  /*                                                                              */
  /*    Purpose:  This function is called to rewind vm amr file playback          */
  /*                                                                              */
  /*                                                                              */
/********************************************************************************/

void audio_init_vm_amr_ff_rewind()
{
  current_frame_counter = 0;
  seek_pointer = 0;
  global_file_pointer = 0;
  rewind_jump_counter = 0;

  frame_index[0].frame_counter = 0;
  frame_index[0].file_seek = 0; // 0 or 1 I need to check
  frame_index[1].frame_counter = 0xFFFFFFFF;
  frame_index[1].file_seek = 0; 
  frame_index[2].frame_counter = 0xFFFFFFFF;
  frame_index[2].file_seek = 0; 
}

void audio_vm_amr_frame_index()
{
 if (current_frame_counter == 500)
 {
  frame_index[0].frame_counter = current_frame_counter;
  frame_index[0].file_seek = global_file_pointer; 
 }
 else if (current_frame_counter == 1500)
 {
  frame_index[1].frame_counter = current_frame_counter;
  frame_index[1].file_seek = global_file_pointer; 
 }
 else if (current_frame_counter == 2500)
 {
  frame_index[2].frame_counter = current_frame_counter;
  frame_index[2].file_seek = global_file_pointer; 
 }
}

// current_frame_counter, seek_pointer and all variables should be initialized as part of 
//
T_AUDIO_RET audio_vm_amr_rw(UINT8 *play_buffer)
{
  INT32 n = 2;
  UINT32 forward_frame_counter = 0;
  AUDIO_SEND_TRACE_PARAM("Inside audio_vm_amr_rw :", current_frame_counter, RV_TRACE_LEVEL_ERROR);
  rewind_jump_counter = current_frame_counter + (vm_amr_skip_time * 1000/20);

  if( rewind_jump_counter > 0 )
  {
    while( n>=0 && rewind_jump_counter < frame_index[n].frame_counter)
    {
      n--;
    }

    if(n >= 0)
    {
      // seek_pointer should be frame_index[n].file_seek
      seek_pointer = frame_index[n].file_seek;
      current_frame_counter = frame_index[n].frame_counter;
    }
    else
    {
      /* if(mms) // should go to 6th or 7th byte
      	{
      seek_pointer = 6;
      current_frame_counter = 0;
      	}
    else */
    	{
      seek_pointer = 0; // Should go to the beginning of the file
      current_frame_counter = 0;
    	}
    }

    global_file_pointer = seek_pointer;
    forward_frame_counter = rewind_jump_counter - current_frame_counter;

    return audio_vm_amr_ff(play_buffer, forward_frame_counter);
  }
  else
  {
    // start stop request to L1
    return AUDIO_ERROR;
  }

}


#endif /* VM_AMR_PLAY */
#endif /* RVM_AUDIO_MAIN_SWE */

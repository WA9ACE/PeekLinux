/****************************************************************************/
/*                                                                          */
/*  Name        audio_ffs.c                                                 */
/*                                                                          */
/*  Function    this file contains the  AUDIO ffs function:                 */
/*									                                        */
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
  #if (MELODY_E1) || (MELODY_E2) || (VOICE_MEMO) || (L1_PCM_EXTRACTION)
    #include "rvf/rvf_api.h"
    #include "rv/rv_general.h"
    #include "rvm/rvm_gen.h"
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

    /* include the usefull L1 header */
    #define BOOL_FLAG
    #define CHAR_FLAG
    #include "l1audio_cust.h"
    #include "l1audio_const.h"
    #include "l1audio_msgty.h"
    #include "l1audio_signa.h"



#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
  #include "rfs/fscore_types.h"
#else
  #include "ffs/ffs_api.h"
#endif

#if (L1_PCM_EXTRACTION)
#define DEBUG_LIMIT_MAX 300

UINT16	debug_downloader_counter = 0;
UINT16 debug_downloader_play_counter = 0;
UINT16 debug_downloader_record_counter = 0;
UINT16 debug_buff_no_switch_counter=0;
UINT16 debug_write_ptr_counter = 0;
UINT16 debug_play_ptr_counter = 0;
UINT32 debug_play_file_reset_counter = 0;

UINT16 debug_downloader_counter_bkup = 0;
UINT16 debug_downloader_play_counter_bkup = 0;
UINT16 debug_downloader_record_counter_bkup = 0;
UINT16 debug_buff_no_switch_counter_bkup = 0;
UINT16 debug_write_ptr_counter_bkup = 0;
UINT16 debug_play_ptr_counter_bkup = 0;

#if(AS_RFS_API == 1)
T_RFS_SIZE debug_write_ptr[DEBUG_LIMIT_MAX];
T_RFS_SIZE debug_play_ptr[DEBUG_LIMIT_MAX];
#else
T_FFS_SIZE debug_write_ptr[DEBUG_LIMIT_MAX];
T_FFS_SIZE debug_play_ptr[DEBUG_LIMIT_MAX];
#endif
UINT32 debug_downloader_play_tick_count[DEBUG_LIMIT_MAX];
UINT32 debug_downloader_write_tick_count[DEBUG_LIMIT_MAX];
UINT32 debug_downloader_tick_count[DEBUG_LIMIT_MAX];

UINT8 debug_open_channel_PLAY[DEBUG_LIMIT_MAX];
UINT8   debug_open_channel_RECORD[DEBUG_LIMIT_MAX];

T_AUDIO_FFS_SESSION *debug_p_session_addr_PLAY[DEBUG_LIMIT_MAX];
T_AUDIO_FFS_SESSION *debug_p_session_addr_RECORD[DEBUG_LIMIT_MAX];

UINT8 debug_p_session_id_PLAY[DEBUG_LIMIT_MAX];
UINT8 debug_p_session_id_RECORD[DEBUG_LIMIT_MAX];

UINT8 debug_p_ffs_index_PLAY[DEBUG_LIMIT_MAX];
UINT8 debug_p_ffs_index_RECORD[DEBUG_LIMIT_MAX];

UINT8 debug_play_init = 0;
UINT8 debug_record_init = 0;
#endif


    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_ffs_manager                                        */
    /*                                                                              */
    /*    Purpose:  This function is called to manage the FFS request from the      */
    /*              audio entity                                                    */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        message from the audio entity                                         */
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
    void audio_ffs_manager (T_RV_HDR *p_message)
    {
      UINT8 j, active_task, index_ffs, index_l1, *p_buffer, channel_id;
      T_AUDIO_FFS_SESSION *p_session;
      T_RV_HDR            *p_send_message;
      T_RVF_MB_STATUS     mb_status;
      BOOLEAN             loop_mode;
      #ifndef _WINDOWS
        UINT16              voice_memo_size, *p_scan;
      #else
        UINT16              i;
        UINT8               *p_mem;
      #endif
      UINT16              buffer_size;
#if(AS_RFS_API == 1)
		T_RFS_SIZE          size;
#else
      T_FFS_SIZE          size;
#endif
      T_RVF_MB_STATUS            status;
      UINT32 debug_i = 0, debug_j = 0;

	  int ffs_error;


      switch (p_message->msg_id)
      {
        case AUDIO_FFS_FLASH_2_RAM_START_REQ:
        {
#if (L1_PCM_EXTRACTION)
		if(debug_play_init == 0)
		{
			for (debug_i = 0; debug_i<DEBUG_LIMIT_MAX;debug_i++)
			{
				debug_open_channel_PLAY[debug_i] = 0xff;
				debug_p_session_addr_PLAY[debug_i] =(T_AUDIO_FFS_SESSION *) 0xffffffff;
				debug_downloader_play_tick_count[debug_i] = 0xffffffff;
				debug_play_ptr[debug_i] = (T_FFS_SIZE)0xffffffff;
				debug_p_session_id_PLAY[debug_i] = 0xff;
			}

			debug_downloader_play_counter = 0;
			debug_play_ptr_counter = 0;
			debug_play_file_reset_counter = 0;
			debug_play_init = 1;

		}
#endif //(L1_PCM_EXTRACTION)

          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: flash to RAM session_id",
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_message)->session_id, RV_TRACE_LEVEL_DEBUG_LOW);

          /* Find a free channel */
          channel_id = 0;
          while ( (channel_id < AUDIO_FFS_MAX_CHANNEL) &&
		  (p_audio_gbl_var->audio_ffs_session[channel_id].session_req.valid_channel))
          {
            channel_id++;
            if(channel_id == AUDIO_FFS_MAX_CHANNEL)//OMAPS00090550
            break;
          }

	   if(channel_id >= AUDIO_FFS_MAX_CHANNEL)
	   {
   	          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: WRONG CHANNEL ID GOT:!!!!!!!!!!!!!!!",
          									  channel_id, RV_TRACE_LEVEL_DEBUG_LOW);
		return;
	   }
          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: open channel for PLAY",
            channel_id, RV_TRACE_LEVEL_DEBUG_LOW);

          p_session =
            &(p_audio_gbl_var->audio_ffs_session[channel_id]);

          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: p_session addr  for PLAY",
            p_session, RV_TRACE_LEVEL_DEBUG_LOW);

          /* fill the request structure corresponding to the session id */
          p_session->session_req.audio_ffs_fd =
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_message)->audio_ffs_fd;
          p_session->session_req.size =
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_message)->initial_size;
          p_session->session_req.loop_mode =
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_message)->loop;
          p_session->session_req.session_id =
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_message)->session_id;

          /************************************************************/
          /* the FFS must download the 2 first buffers to the RAM     */
          /************************************************************/
          for (j=0; j<2; j++)
          {
            /* allocate the first buffer */
            p_session->session_info.buffer[j].size =
            p_session->session_req.size;

            mb_status = rvf_get_buf (p_audio_gbl_var->mb_audio_ffs,
                                     p_session->session_info.buffer[j].size,
                                     (T_RVF_BUFFER **) (&p_session->session_info.buffer[j].p_start_pointer));

            /* If insufficient resources, then report a memory error and abort.               */
            if (mb_status == RVF_RED)
            {
              audio_ffs_error_trace(AUDIO_ENTITY_NO_MEMORY);
            }

            AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: allocate buffer",
                j, RV_TRACE_LEVEL_DEBUG_LOW);

            /* intialize the stop pointer */
            p_session->session_info.buffer[j].p_stop_pointer =
              p_session->session_info.buffer[j].p_start_pointer +
              (p_session->session_info.buffer[j].size);

            /* Fill the buffer j while it isn't full in case of the loop back mode activated */
            loop_mode = TRUE;
            buffer_size = p_session->session_info.buffer[j].size;
            p_buffer = (UINT8 *)p_session->session_info.buffer[j].p_start_pointer;
		/*VENKAT*/
		#if (L1_PCM_EXTRACTION)
		if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY)
		{
#if (AS_RFS_API == 1)

				debug_play_ptr[debug_play_ptr_counter++] = rfs_lseek(p_session->session_req.audio_ffs_fd,
		              p_audio_gbl_var->vbuf_pcm_play.ffs_read,
            			RFS_SEEK_SET);
  	             AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER........................: debug_play_ptr:",
                            debug_play_ptr, RV_TRACE_LEVEL_DEBUG_LOW);

#else

		      debug_play_ptr[debug_play_ptr_counter++] = ffs_seek(p_session->session_req.audio_ffs_fd,
		              p_audio_gbl_var->vbuf_pcm_play.ffs_read,
            			FFS_SEEK_SET);
  	             AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER........................: debug_play_ptr:",
                            debug_play_ptr, RV_TRACE_LEVEL_DEBUG_LOW);

#endif
			if(debug_play_ptr_counter >= DEBUG_LIMIT_MAX)
				debug_play_ptr_counter =0;

		}
		#endif
		/*!VENKAT*/
            while ( (p_buffer < p_session->session_info.buffer[j].p_stop_pointer) &&
                    (loop_mode) )
            {
              loop_mode = p_session->session_req.loop_mode;

              #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				size = rfs_read(p_session->session_req.audio_ffs_fd,
                                p_buffer,
                                buffer_size);
#else
                size = ffs_read(p_session->session_req.audio_ffs_fd,
                                p_buffer,
                                buffer_size);
#endif
              #else
                size = buffer_size;
                p_mem = p_buffer;
                for (i=0; i<size; i++)
                {
                  *p_mem = (UINT8)i;
                  p_mem++;
                }
              #endif

              AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: ffs_read size",
                size, RV_TRACE_LEVEL_DEBUG_LOW);

              buffer_size -= size;
              p_buffer += size;

#if (L1_PCM_EXTRACTION)
		if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY)
		{
		   p_audio_gbl_var->vbuf_pcm_play.size_read_from_flash +=size;
		}
		else if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_PLAY)
		{
		   p_audio_gbl_var->vm_pcm_play.size_read_from_flash +=size;
		}
#endif// (L1_PCM_EXTRACTION)

		/*VENKAT - Removed since it was resulting in more repetitions while PCM playback.
		There is not impact observed on Voice Memo. Need to investigate further.*/

        /*
           Commented code is creating problem in case of Melody so this part
           needs to be uncommented only for Melody
        */

        switch(p_session->session_req.session_id)
        {
          case AUDIO_FFS_SESSION_MELODY_E1_0 :
          case AUDIO_FFS_SESSION_MELODY_E1_1 :
          case AUDIO_FFS_SESSION_MELODY_E2_0 :
          case AUDIO_FFS_SESSION_MELODY_E2_1 :
                  if (buffer_size != 0)
                  {
#ifndef _WINDOWS
#if(AS_RFS_API == 1)
			rfs_lseek(p_session->session_req.audio_ffs_fd,
								0,
            					RFS_SEEK_SET);
#else
                    /* reset the FFS pointer */
            ffs_seek(p_session->session_req.audio_ffs_fd,
                             0,
                             FFS_SEEK_SET);
#endif
#endif
                  }
           break;
        } // end switch
            } /* while */
			/*VENKAT*/
#if (L1_PCM_EXTRACTION)
	     if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY)
	     {

#if(AS_RFS_API == 1)

			p_audio_gbl_var->vbuf_pcm_play.ffs_read = rfs_lseek(p_session->session_req.audio_ffs_fd,
										                     		      0,
            												                RFS_SEEK_CUR);

#else
			p_audio_gbl_var->vbuf_pcm_play.ffs_read = ffs_seek(p_session->session_req.audio_ffs_fd,
										                     		      0,
            												                FFS_SEEK_CUR);
#endif

	     }
#endif
			/*!VENKAT*/

          } /* for (j=0; j<2; j++) */

          /* initialize the cust_get_pointer state machine */
          p_session->session_info.cust_get_pointer_state = AUDIO_CUST_GET_POINTER_INIT;

          /* inform the L1 to use the buffer 0 */
          p_session->session_info.index_l1 = 0;

          /* inform the FFS downloader to fill the buffer 0 when the L1 doesn't used */
          p_session->session_info.index_ffs = 0;

          p_session->session_req.session_mode =
            AUDIO_FFS_FLASH_2_RAM_SESSION;

          /* a new session is valid now */
          p_session->session_req.valid_channel = TRUE;

          /* Active the downloader if it is not already activated */
          active_task = 0;
          for (j=0; j<AUDIO_FFS_MAX_CHANNEL; j++)
          {
            if ( p_audio_gbl_var->audio_ffs_session[j].session_req.valid_channel )
            {
              active_task++;
            }
          }
          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: active session",
            active_task, RV_TRACE_LEVEL_DEBUG_LOW);
          if (active_task == 1)
          {
            AUDIO_SEND_TRACE("AUDIO FFS MANAGER: start FFS DOWNLOADER", RV_TRACE_LEVEL_DEBUG_LOW);
//            AUDIO_SEND_TRACE("audio_ffs_manager(): Start timer with time out value AUDIO_FFS_ACTIVE_NOW", RV_TRACE_LEVEL_ERROR);
            /* Active asap the FFS downloader */
            rvf_start_timer(AUDIO_FFS_TIMER, AUDIO_FFS_ACTIVE_NOW, AUDIO_FFS_ONE_SHOT_TIMER);
          }

          /* Send the message to confirm that the first buffer is downloaded */
          /* allocate the message buffer */
          mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                   sizeof (T_AUDIO_FFS_INIT),
                                   (T_RVF_BUFFER **) (&p_send_message));

          /* If insufficient resources, then report a memory error and abort.               */
          if (mb_status == RVF_RED)
          {
            audio_ffs_error_trace(AUDIO_ENTITY_NO_MEMORY);
          }

          /* fill the header of the message */
          ((T_AUDIO_FFS_INIT*)(p_send_message))->os_hdr.msg_id = AUDIO_FFS_INIT_DONE;

          /* fill the status parameters */
          ((T_AUDIO_FFS_INIT *)(p_send_message))->session_id =
            ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_message)->session_id;
          ((T_AUDIO_FFS_INIT *)(p_send_message))->channel_id =
            channel_id;


          /* send the message to the AUDIO entity */
          rvf_send_msg (p_audio_gbl_var->addrId,
                        p_send_message);
          break;
        }
        case AUDIO_FFS_RAM_2_FLASH_START_REQ:
        {
#if (L1_PCM_EXTRACTION)
		if(debug_record_init == 0)
		{
			for (debug_i = 0; debug_i<DEBUG_LIMIT_MAX;debug_i++)
			{
				debug_open_channel_RECORD[debug_i] = 0xFF;
				debug_p_session_addr_RECORD[debug_i] = (T_AUDIO_FFS_SESSION *)0xFFFFFFFF;
#if(AS_RFS_API == 1)
				debug_write_ptr[debug_i] = (T_RFS_SIZE)0xffffffff;
#else
				debug_write_ptr[debug_i] = (T_FFS_SIZE)0xffffffff;
#endif
				debug_open_channel_RECORD[debug_i] = 0xff;
				debug_p_session_addr_RECORD[debug_i] =(T_AUDIO_FFS_SESSION *) 0xffffffff;
				debug_p_session_id_RECORD[debug_i] =(UINT8) 0xff;
			}
			debug_record_init = 1;
			debug_downloader_play_counter = 0;
		}
#endif// (L1_PCM_EXTRACTION)
          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: RAM to flash session_id",
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_message)->session_id, RV_TRACE_LEVEL_DEBUG_LOW);

          /* Find a free channel */
          channel_id = 0;
          while ( (channel_id < AUDIO_FFS_MAX_CHANNEL) &&
		(p_audio_gbl_var->audio_ffs_session[channel_id].session_req.valid_channel))
            channel_id++;

	  if(channel_id >= AUDIO_FFS_MAX_CHANNEL)
		  return;

	  AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: open channel for RECORD",
            channel_id, RV_TRACE_LEVEL_DEBUG_LOW);

          p_session =
            &(p_audio_gbl_var->audio_ffs_session[channel_id]);

          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: p_session addr for RECORD",
            p_session, RV_TRACE_LEVEL_DEBUG_LOW);


          /* fill the request structure corresponding to the session id */
          p_session->session_req.size =
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_message)->initial_size;
          p_session->session_req.loop_mode = FALSE;
          p_session->session_req.audio_ffs_fd =
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_message)->audio_ffs_fd;
          p_session->session_req.session_mode =
            AUDIO_FFS_RAM_2_FLASH_SESSION;
          p_session->session_req.session_id =
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_message)->session_id;

          /********************* TO BE COMPLETED **********************/
          /* the FFS must allocate the first buffer to the RAM        */
          /************************************************************/
          for (j=0; j<2; j++)
          {
            mb_status = rvf_get_buf (p_audio_gbl_var->mb_audio_ffs,
                                     p_session->session_req.size,
                                    (T_RVF_BUFFER **) (&p_session->session_info.buffer[j].p_start_pointer));

            /* If insufficient resources, then report a memory error and abort.               */
            if (mb_status == RVF_RED)
            {
              audio_ffs_error_trace(AUDIO_ENTITY_NO_MEMORY);
            }

//            AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: allocate buffer",
//                j, RV_TRACE_LEVEL_DEBUG_LOW);

            /* Copy the initial size */
            p_session->session_info.buffer[j].size = p_session->session_req.size;
          }

          /* initialize the cust_get_pointer state machine */
          p_session->session_info.cust_get_pointer_state = AUDIO_CUST_GET_POINTER_INIT;

          /* inform the L1 to use the buffer 0 */
          p_session->session_info.index_l1 = 0;

          /* inform the FFS downloader to read the buffer 0 when the L1 doesn't used */
          p_session->session_info.index_ffs = 0;

          /* a new session is valid now */
          p_session->session_req.valid_channel = TRUE;

          /* Active the downloader if it is not already activated */
          active_task = 0;
          for (j=0; j<AUDIO_FFS_MAX_CHANNEL; j++)
          {
            if ( p_audio_gbl_var->audio_ffs_session[j].session_req.valid_channel)
            {
              active_task++;
            }
          }
          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: active session",
            active_task, RV_TRACE_LEVEL_DEBUG_LOW);

          if (active_task == 1)
          {
            AUDIO_SEND_TRACE("AUDIO FFS MANAGER: start FFS DOWNLOADER", RV_TRACE_LEVEL_DEBUG_LOW);

            /* Active asap the FFS downloader */
//            AUDIO_SEND_TRACE("audio_ffs_manager(): Start timer with time out value AUDIO_FFS_ACTIVE_NOW", RV_TRACE_LEVEL_ERROR);
            rvf_start_timer(AUDIO_FFS_TIMER, AUDIO_FFS_ACTIVE_NOW, AUDIO_FFS_ONE_SHOT_TIMER);
          }
          /* Send the message to confirm that the first buffer is allocated */
          /* allocate the message buffer */
          mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                   sizeof (T_AUDIO_FFS_INIT),
                                   (T_RVF_BUFFER **) (&p_send_message));

          /* If insufficient resources, then report a memory error and abort.               */
          if (mb_status == RVF_RED)
          {
            audio_ffs_error_trace(AUDIO_ENTITY_NO_MEMORY);
          }

          /* fill the header of the message */
          ((T_AUDIO_FFS_INIT*)(p_send_message))->os_hdr.msg_id = AUDIO_FFS_INIT_DONE;

          /* fill the status parameters */
          ((T_AUDIO_FFS_INIT *)(p_send_message))->session_id =
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_message)->session_id;
          ((T_AUDIO_FFS_INIT *)(p_send_message))->channel_id =
            channel_id;

          /* send the message to the AUDIO entity */
          rvf_send_msg (p_audio_gbl_var->addrId,
                        p_send_message);

          break;
        }

        case AUDIO_FFS_STOP_REQ:
        {
          /* Find a channel corresponding to this session */
          channel_id = 0;
          while ( (channel_id < AUDIO_FFS_MAX_CHANNEL) &&
		 (p_audio_gbl_var->audio_ffs_session[channel_id].session_req.session_id
                    != ((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id))
            channel_id++;

	  if(channel_id >= AUDIO_FFS_MAX_CHANNEL)
		  return;

          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: channel to STOP",
            channel_id, RV_TRACE_LEVEL_DEBUG_LOW);

          p_session = &(p_audio_gbl_var->audio_ffs_session[channel_id]);

          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: p_session addr for FFS STOP",
            p_session, RV_TRACE_LEVEL_DEBUG_LOW);

          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: stop session_id",
            ((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id, RV_TRACE_LEVEL_DEBUG_LOW);
          AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: close channel",
            channel_id, RV_TRACE_LEVEL_DEBUG_LOW);

          /* the task is stopped */
          p_session->session_req.valid_channel = FALSE;

          /* the stop process depends on the session_mode and sometimes the session_id */
          #if (VOICE_MEMO)||(L1_PCM_EXTRACTION)
            if ( (p_session->session_req.session_mode == AUDIO_FFS_RAM_2_FLASH_SESSION) &&
                 ((((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_VM_RECORD) ||
                 (((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_VM_PCM_RECORD) ||
                 (((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)))
            {
              index_l1 = p_session->session_info.index_l1;
              index_ffs = p_session->session_info.index_ffs;

/*VENKAT*/
#if (L1_PCM_EXTRACTION)
		if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
		{
#if(AS_RFS_API == 1)
			  debug_write_ptr[debug_write_ptr_counter++] = rfs_lseek(p_session->session_req.audio_ffs_fd,
		             												 p_audio_gbl_var->vbuf_pcm_record.ffs_write,
												            			RFS_SEEK_SET);
#else
		      debug_write_ptr[debug_write_ptr_counter++] = ffs_seek(p_session->session_req.audio_ffs_fd,
		             												 p_audio_gbl_var->vbuf_pcm_record.ffs_write,
												            			FFS_SEEK_SET);
#endif
//    	             AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER........................: debug_WRITE_ptr:",
//                            debug_write_ptr, RV_TRACE_LEVEL_DEBUG_LOW);
			if(debug_write_ptr_counter >= DEBUG_LIMIT_MAX)
				debug_write_ptr_counter =0;

		}
#endif
/*!VENKAT*/

              if (index_ffs != index_l1)
              /* There's two buffers to save: one full (index_ffs) and one not full (index_l1) */
              {
                AUDIO_SEND_TRACE("<<<<AUDIO FFS MANAGER: end of VM record session with index_l1<>index_ffs>>>>>", RV_TRACE_LEVEL_DEBUG_LOW);

                #ifndef _WINDOWS

#if(AS_RFS_API == 1)

			 /* save the full buffer */
			ffs_error =	rfs_write (p_session->session_req.audio_ffs_fd,
                                  p_session->session_info.buffer[index_ffs].p_start_pointer,
                                  p_session->session_req.size);
                  if ((ffs_error) < RFS_EOK)
                  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_SAVED);
			AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: VBUF Record-FileSaveError:", ffs_error, RV_TRACE_LEVEL_DEBUG_LOW);
                  }
#else
                  /* save the full buffer */
			ffs_error =	ffs_write (p_session->session_req.audio_ffs_fd,
                                  p_session->session_info.buffer[index_ffs].p_start_pointer,
                                  p_session->session_req.size);
                  if ((ffs_error) < EFFS_OK)
                  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_SAVED);
			AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: VBUF Record-FileSaveError:", ffs_error, RV_TRACE_LEVEL_DEBUG_LOW);
                  }
#endif


#if (L1_PCM_EXTRACTION)
                  else
		    {
			    if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
			   {
			   	p_audio_gbl_var->vbuf_pcm_record.size_copied_to_flash += p_session->session_req.size;
	                 }
			   else if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_RECORD)
			   {
				p_audio_gbl_var->vm_pcm_record.size_copied_to_flash += p_session->session_req.size;
			   }

		    }
#endif//(L1_PCM_EXTRACTION)



#if (L1_PCM_EXTRACTION)
		   if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
		   {
		   	voice_memo_size = ((p_audio_gbl_var->vbuf_pcm_record.recorded_size) * 2) - p_audio_gbl_var->vbuf_pcm_record.size_copied_to_flash;
                 }
		   else if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_RECORD)
		   {
   		   	voice_memo_size = ((p_audio_gbl_var->vm_pcm_record.recorded_size) * 2) - p_audio_gbl_var->vm_pcm_record.size_copied_to_flash;
		   }
		   else
   	   	   {
#endif//(L1_PCM_EXTRACTION)
                  /* save a part of the buffer pointed by the L1 */
                  voice_memo_size = 2;
                  p_scan = (UINT16 *)(p_session->session_info.buffer[index_l1].p_start_pointer);
                  while ( (*p_scan++) != SC_VM_END_MASK )
                  {
                    voice_memo_size += 2;
                  }
#if (L1_PCM_EXTRACTION)
		   }
#endif//(L1_PCM_EXTRACTION)

#if(AS_RFS_API == 1)

			ffs_error = rfs_write  (p_session->session_req.audio_ffs_fd,
                                   p_session->session_info.buffer[index_l1].p_start_pointer,
                                   voice_memo_size);

                  if ((ffs_error) < RFS_EOK)
                  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_SAVED);
			AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: VBUF Record-FileSaveError:", ffs_error, RV_TRACE_LEVEL_DEBUG_LOW);
                  }

#else

			ffs_error = ffs_write  (p_session->session_req.audio_ffs_fd,
                                   p_session->session_info.buffer[index_l1].p_start_pointer,
                                   voice_memo_size);

                  if ((ffs_error) < EFFS_OK)
                  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_SAVED);
			AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: VBUF Record-FileSaveError:", ffs_error, RV_TRACE_LEVEL_DEBUG_LOW);
                  }
#endif

#if (L1_PCM_EXTRACTION)
		    else
		    {
	  		   if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
			   {
	                        p_audio_gbl_var->vbuf_pcm_record.size_copied_to_flash += (voice_memo_size);
	                 }
			   else if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_RECORD)
			   {
				p_audio_gbl_var->vm_pcm_record.size_copied_to_flash += (voice_memo_size);
			   }

		    	    /* used in voice memo pcm or VM buffering instances    */

		    }
#endif//(L1_PCM_EXTRACTION)
                #endif
              }
              else
              /* 1 buffer (not full) needs to be saved */
              {
                AUDIO_SEND_TRACE("++++AUDIO FFS MANAGER: end of VM record session with index_l1==index_ffs+++++", RV_TRACE_LEVEL_DEBUG_LOW);
                #ifndef _WINDOWS

#if (L1_PCM_EXTRACTION)
		   if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
		   {
		   	 voice_memo_size = ((p_audio_gbl_var->vbuf_pcm_record.recorded_size)*2) - p_audio_gbl_var->vbuf_pcm_record.size_copied_to_flash;
                 }
		   else if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_RECORD)
		   {
   		   	voice_memo_size = ((p_audio_gbl_var->vm_pcm_record.recorded_size)*2) - p_audio_gbl_var->vm_pcm_record.size_copied_to_flash;
		   }
		   else
		   {
#endif//(L1_PCM_EXTRACTION)
                  voice_memo_size = 2;
                  p_scan = (UINT16*)(p_session->session_info.buffer[index_l1].p_start_pointer);

                  while ( (*p_scan++) != SC_VM_END_MASK )
                  {
                    voice_memo_size += 2;
                  }
#if (L1_PCM_EXTRACTION)
		    }
#endif //(L1_PCM_EXTRACTION)

#if(AS_RFS_API == 1)
			ffs_error =rfs_write (p_session->session_req.audio_ffs_fd,
                                  p_session->session_info.buffer[index_l1].p_start_pointer,
                                  voice_memo_size);

                  if ((ffs_error) < RFS_EOK)
                  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_SAVED);
			AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: VBUF Record-FileSaveError:", ffs_error, RV_TRACE_LEVEL_DEBUG_LOW);
                  }

#else

		ffs_error =ffs_write (p_session->session_req.audio_ffs_fd,
                                  p_session->session_info.buffer[index_l1].p_start_pointer,
                                  voice_memo_size);

                  if ((ffs_error) < EFFS_OK)
                  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_SAVED);
			AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: VBUF Record-FileSaveError:", ffs_error, RV_TRACE_LEVEL_DEBUG_LOW);
                  }
#endif

#if (L1_PCM_EXTRACTION)
		    else
		    {
    	  		   if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
			   {
	                        p_audio_gbl_var->vbuf_pcm_record.size_copied_to_flash += (voice_memo_size);
	                 }
			   else if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_RECORD)
			   {
				p_audio_gbl_var->vm_pcm_record.size_copied_to_flash += (voice_memo_size);
			   }

		    }
#endif //(L1_PCM_EXTRACTION)
                #endif
              } /* index_ffs != index_l1 */

/*VENKAT*/
#if (L1_PCM_EXTRACTION)
          if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
	     {
#if(AS_RFS_API == 1)

		    p_audio_gbl_var->vbuf_pcm_record.ffs_write = rfs_lseek(p_session->session_req.audio_ffs_fd,
										                     		      0,
            												                RFS_SEEK_CUR);

#else
			p_audio_gbl_var->vbuf_pcm_record.ffs_write = ffs_seek(p_session->session_req.audio_ffs_fd,
										                     		      0,
            												                FFS_SEEK_CUR);
#endif
	     }
#endif
/*!VENKAT*/

            }
          #endif /* VOICE_MEMO || L1_PCM_EXTRACTION*/

          /* deallocate the buffers */
          for (j=0; j<AUDIO_MAX_FFS_BUFFER_PER_SESSION; j++)
          {
            status = (T_RVF_MB_STATUS)rvf_free_buf ( (T_RVF_BUFFER *)(p_session->session_info.buffer[j].p_start_pointer) );
            if (status != RVF_GREEN)
            {
              AUDIO_SEND_TRACE(" wrong buffer deallocated ",
                               RV_TRACE_LEVEL_ERROR);
            }
            AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: deallocate buffer",
                j, RV_TRACE_LEVEL_DEBUG_LOW);
          }

          /* Close the FFS file */
          #ifndef _WINDOWS
		  /*VENKAT*/
#if (L1_PCM_EXTRACTION)
		 if ((((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)&&
		 	(p_audio_gbl_var->vbuf_pcm_record.delete_after_play == TRUE))

		 {
		 	AUDIO_SEND_TRACE("VBUF RECORD STOP - File not closed. To be closed after play complete", RV_TRACE_LEVEL_DEBUG_LOW);
		 }
		 else
	 	{
  #endif //(L1_PCM_EXTRACTION)
#if(AS_RFS_API == 1)
			if ( rfs_close(p_session->session_req.audio_ffs_fd) != RFS_EOK )
            {
              audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
            }

#else
            if ( ffs_close(p_session->session_req.audio_ffs_fd) != EFFS_OK )
            {
              audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
            }
#endif

#if (L1_PCM_EXTRACTION)
	 	}
#endif //(L1_PCM_EXTRACTION)
		 /*!VENKAT*/
#endif //_WINDOWS

          /* Send the message to confirm that the session is stopped */
          /* allocate the message buffer */
          mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                   sizeof (T_AUDIO_FFS_STOP_CON),
                                   (T_RVF_BUFFER **)(&p_send_message));

          /* If insufficient resources, then report a memory error and abort.               */
          if (mb_status == RVF_RED)
          {
            audio_ffs_error_trace(AUDIO_ENTITY_NO_MEMORY);
          }

#if (L1_PCM_EXTRACTION)
	debug_downloader_counter_bkup = debug_downloader_counter;
	debug_downloader_play_counter_bkup = debug_downloader_play_counter;
	debug_downloader_record_counter_bkup = debug_downloader_record_counter;
	debug_buff_no_switch_counter_bkup = debug_buff_no_switch_counter;
	debug_write_ptr_counter_bkup = debug_write_ptr_counter;
	debug_play_ptr_counter_bkup = debug_play_ptr_counter;

  AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_downloader_counter",
            debug_downloader_counter, RV_TRACE_LEVEL_DEBUG_LOW);

  AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_downloader_play_counter",
            debug_downloader_play_counter, RV_TRACE_LEVEL_DEBUG_LOW);

  AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_downloader_record_counter",
            debug_downloader_record_counter, RV_TRACE_LEVEL_DEBUG_LOW);

  AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_buff_no_switch_counter",
            debug_buff_no_switch_counter, RV_TRACE_LEVEL_DEBUG_LOW);

  AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_write_ptr_counter",
            debug_write_ptr_counter, RV_TRACE_LEVEL_DEBUG_LOW);

    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_play_ptr_counter",
            debug_play_ptr_counter, RV_TRACE_LEVEL_DEBUG_LOW);

    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_play_file_reset_counter",
            debug_play_file_reset_counter, RV_TRACE_LEVEL_DEBUG_LOW);


	for (debug_j = 0; debug_j<=debug_downloader_play_counter;debug_j++)
	{
	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_open_channel_PLAY[debug_downloader_play_counter]",
            debug_open_channel_PLAY[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}
	for (debug_j = 0; debug_j<=debug_downloader_play_counter;debug_j++)
	{
	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_p_session_addr_PLAY[debug_downloader_play_counter]",
            debug_p_session_addr_PLAY[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}
	for (debug_j = 0; debug_j<=debug_downloader_play_counter;debug_j++)
	{
	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_p_session_id_PLAY[debug_downloader_play_counter]",
		debug_p_session_id_PLAY[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}
	for (debug_j = 0; debug_j<=debug_downloader_play_counter;debug_j++)
	{
	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_p_ffs_index_PLAY[debug_downloader_play_counter] = index_ffs;",
            debug_p_ffs_index_PLAY[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}
	for (debug_j = 0; debug_j<=debug_downloader_play_counter;debug_j++)
	{
	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_downloader_play_tick_count[debug_downloader_play_counter]",
            debug_downloader_play_tick_count[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);

	}

	for (debug_j = 0; debug_j<=debug_downloader_record_counter;debug_j++)
	{
	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_open_channel_RECORD[debug_downloader_play_counter]",
            debug_open_channel_RECORD[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}
	for (debug_j = 0; debug_j<=debug_downloader_record_counter;debug_j++)
	{

	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_p_session_addr_RECORD[debug_downloader_play_counter]",
            debug_p_session_addr_RECORD[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}
	for (debug_j = 0; debug_j<=debug_downloader_record_counter;debug_j++)
	{

	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_p_session_id_RECORD[debug_downloader_play_counter]",
		debug_p_session_id_RECORD[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}
	for (debug_j = 0; debug_j<=debug_downloader_record_counter;debug_j++)
	{

	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_p_ffs_index_RECORD[debug_downloader_play_counter] = index_ffs;",
            debug_p_ffs_index_RECORD[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}
	for (debug_j = 0; debug_j<=debug_downloader_record_counter;debug_j++)
	{

	    AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_downloader_write_tick_count[debug_downloader_play_counter]",
            debug_downloader_write_tick_count[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);

	}

	for (debug_j = 0; debug_j<=debug_write_ptr_counter;debug_j++)
	{

		AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_write_ptr[debug_downloader_play_counter]",
	            debug_write_ptr[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}

	for (debug_j = 0; debug_j<=debug_play_ptr_counter;debug_j++)
	{
		AUDIO_SEND_TRACE_PARAM("AUDIO FFS MANAGER: debug_play_ptr[debug_downloader_play_counter]",
				debug_play_ptr[debug_j], RV_TRACE_LEVEL_DEBUG_LOW);
	}

	debug_downloader_counter = 0;
	debug_downloader_play_counter = 0;
	debug_downloader_record_counter = 0;
	debug_buff_no_switch_counter=0;
	debug_write_ptr_counter = 0;
	debug_play_ptr_counter = 0;

#endif// (L1_PCM_EXTRACTION)

          /* fill the header of the message */
          ((T_AUDIO_FFS_STOP_CON*)(p_send_message))->os_hdr.msg_id = AUDIO_FFS_STOP_CON;

          /* fill the status parameters */
          ((T_AUDIO_FFS_STOP_CON*)(p_send_message))->session_id =
            ((T_AUDIO_FFS_STOP_REQ *)p_message)->session_id;

          /* send the message to the AUDIO entity */
          rvf_send_msg (p_audio_gbl_var->addrId,
                        p_send_message);
          break;
        }
      } /* switch (p_message) */
    }

    /********************************************************************************/
    /*                                                                              */
    /*    Function Name:   audio_ffs_downloader                                     */
    /*                                                                              */
    /*    Purpose:  This function is called to download the melody, voice memo data */
    /*              between the RAM and the FLASH.                                  */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        None.                                                                 */
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
    void audio_ffs_downloader(void)
    {
      UINT8                index_ffs, index_l1, *p_buffer;
       int  i, ffs_error;
      T_AUDIO_FFS_SESSION *p_session;
      UINT16              buffer_size;
#if(AS_RFS_API == 1)
		T_RFS_SIZE          size = 0;
#else
      T_FFS_SIZE          size = 0;
#endif
      BOOLEAN             loop_mode, active_session;
	BOOLEAN  pcm_vm_vbuf_timer = 0;


      /* Scan all session in order to know which is valid */
      active_session = FALSE;
#if (L1_PCM_EXTRACTION)
	     debug_downloader_counter++;
		if(debug_downloader_counter >= DEBUG_LIMIT_MAX)
			debug_downloader_counter =0;
	 	debug_downloader_tick_count[debug_downloader_counter] =  rvf_get_tick_count();

//     AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER:...... debug_downloader_tick_count:",
//              							debug_downloader_tick_count, RV_TRACE_LEVEL_DEBUG_LOW);

//     debug_valid_channel_counter = 0;
#endif// (L1_PCM_EXTRACTION)
      //for (i=0; i<AUDIO_FFS_MAX_CHANNEL; i++)
      for (i = (AUDIO_FFS_MAX_CHANNEL -1) ; i >=0 ; i--)
      {
       p_session = &(p_audio_gbl_var->audio_ffs_session[i]);

	 if(i >= AUDIO_FFS_MAX_CHANNEL)
	   {
   	          AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: WRONG CHANNEL ID GOT:!!!!!!!!!!!!!!!",
          									  i, RV_TRACE_LEVEL_DEBUG_LOW);

	   }

       if (p_session->session_req.valid_channel)
       {
          /* a session is valid */
          active_session = TRUE;
		  if((p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD) ||
			(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_RECORD) ||
			(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY) ||
			(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_PLAY))
		  {
			pcm_vm_vbuf_timer = 1;
		  }

          index_l1 = p_session->session_info.index_l1;
          index_ffs = p_session->session_info.index_ffs;


          if (index_l1 != index_ffs)
          /* It's time to download a new buffer for the L1 */
          {
          #if 0
          	if(debug_buff_no_switch_counter != 0)
		{
		AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: debug_play_buff_no_switch_counter:",
              							debug_buff_no_switch_counter, RV_TRACE_LEVEL_DEBUG_LOW);
		AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: debug_downloader_counter:",
              							debug_downloader_counter, RV_TRACE_LEVEL_DEBUG_LOW);

			debug_buff_no_switch_counter = 0;
			debug_downloader_counter = 0;
		}
	   #endif //#if 0


//           AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: index_l1",
//              index_l1, RV_TRACE_LEVEL_DEBUG_LOW);

//            AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: index_ffs",
//              index_ffs, RV_TRACE_LEVEL_DEBUG_LOW);

            switch (p_session->session_req.session_mode)
            {
              case AUDIO_FFS_FLASH_2_RAM_SESSION:
              {
#if (L1_PCM_EXTRACTION)
//                AUDIO_SEND_TRACE("AUDIO FFS DOWNLOADER: FLASH to RAM", RV_TRACE_LEVEL_DEBUG_LOW);
		   debug_downloader_play_counter++;
		   if(debug_downloader_play_counter >= DEBUG_LIMIT_MAX)
		   	debug_downloader_play_counter =0;

		   debug_open_channel_PLAY[debug_downloader_play_counter] = i;
		   debug_p_session_addr_PLAY[debug_downloader_play_counter] = p_session;
		   debug_p_session_id_PLAY[debug_downloader_play_counter] = p_session->session_req.session_id;
		  debug_p_ffs_index_PLAY[debug_downloader_play_counter] = index_ffs;
  	 	debug_downloader_play_tick_count[debug_downloader_play_counter] =  rvf_get_tick_count();

#endif //(L1_PCM_EXTRACTION)

//   	        AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: open channel for PLAY",
//            									i, RV_TRACE_LEVEL_DEBUG_LOW);

//		  AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: p_session addr for PLAY",
//		            p_session, RV_TRACE_LEVEL_DEBUG_LOW);

                /* Fill the buffer 0 while it isn't full in case of the loop back mode activated */
                loop_mode = TRUE;
                buffer_size = p_session->session_info.buffer[index_ffs].size;
                p_buffer = (UINT8 *)p_session->session_info.buffer[index_ffs].p_start_pointer;

//		  AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: index_ffs Play buffer pointer:",
//              p_buffer, RV_TRACE_LEVEL_DEBUG_LOW);

/*VENKAT*/
#if (L1_PCM_EXTRACTION)
		if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY)
		{
#if(AS_RFS_API == 1)
			  debug_play_ptr[debug_play_ptr_counter++] = rfs_lseek(p_session->session_req.audio_ffs_fd,
		              p_audio_gbl_var->vbuf_pcm_play.ffs_read,
            			RFS_SEEK_SET);
#else
		      debug_play_ptr[debug_play_ptr_counter++] = ffs_seek(p_session->session_req.audio_ffs_fd,
		              p_audio_gbl_var->vbuf_pcm_play.ffs_read,
            			FFS_SEEK_SET);
#endif


//	             AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: debug_play_ptr:",
//                            debug_play_ptr, RV_TRACE_LEVEL_DEBUG_LOW);
			if(debug_play_ptr_counter >= DEBUG_LIMIT_MAX)
				debug_play_ptr_counter =0;
		}
#endif
/*!VENKAT*/
                while ( (p_buffer < p_session->session_info.buffer[index_ffs].p_stop_pointer) &&
                        (loop_mode) )
                {
                  loop_mode = p_session->session_req.loop_mode;

                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					size = rfs_read(p_session->session_req.audio_ffs_fd,
                                    p_buffer,
                                    buffer_size);
#else
                    size = ffs_read(p_session->session_req.audio_ffs_fd,
                                    p_buffer,
                                    buffer_size);
#endif
                  #endif

                  AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: ffs_read size",
                    size, RV_TRACE_LEVEL_DEBUG_LOW);

                  buffer_size -= size;
                  p_buffer += size;
#if (L1_PCM_EXTRACTION)
		   if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY)
                  {
		     p_audio_gbl_var->vbuf_pcm_play.size_read_from_flash +=size;

		     if(  p_audio_gbl_var->vbuf_pcm_play.size_read_from_flash >=
			 	p_audio_gbl_var->vbuf_pcm_record.size_copied_to_flash)
		     	{
		     			if(size < AUDIO_VBUF_PCM_PLAY_SIZE)
		     			{
		     			   AUDIO_SEND_TRACE_PARAM("size < AUDIO_VBUF_PCM_PLAY_SIZE", buffer_size, RV_TRACE_LEVEL_DEBUG_LOW);
					   memset(p_buffer, 0x00, buffer_size);
		     			}
		     	            break;
		     	}
		  }
		   else if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_PLAY)
		  {
		     p_audio_gbl_var->vm_pcm_play.size_read_from_flash +=size;

		     if(  p_audio_gbl_var->vm_pcm_play.size_read_from_flash >=
			 	p_audio_gbl_var->vm_pcm_record.size_copied_to_flash)
			 break;
		  }
#endif //(L1_PCM_EXTRACTION)

                  if (buffer_size != 0)
                  {
/*VENKAT*/
/*Commented out since it was cuasing too many repetitions in PCM voice buffering. No impact foundon voice memo feature.
Needs to investigate more*/
                    #ifndef _WINDOWS
                      /* reset the FFS pointer */
        /*
           Commented code is creating problem in case of Melody so this part
           needs to be uncommented only for Melody
        */
                      switch(p_session->session_req.session_id)
                     {
                       case AUDIO_FFS_SESSION_MELODY_E1_0 :
                       case AUDIO_FFS_SESSION_MELODY_E1_1 :
                       case AUDIO_FFS_SESSION_MELODY_E2_0 :
                       case AUDIO_FFS_SESSION_MELODY_E2_1 :
#if(AS_RFS_API == 1)
                                 rfs_lseek(p_session->session_req.audio_ffs_fd, 0, RFS_SEEK_SET);
#else
                                 ffs_seek(p_session->session_req.audio_ffs_fd, 0, FFS_SEEK_SET);
#endif
                       break;
                     } // end switch
                    #endif
#if (L1_PCM_EXTRACTION)
			debug_play_file_reset_counter++;
			if(debug_play_file_reset_counter >= DEBUG_LIMIT_MAX)
				debug_play_file_reset_counter =0;
#endif// (L1_PCM_EXTRACTION)
                  }
		    else
 	  	   {
// 	  	   	debug_play_file_reset_counter++;
		   }

                } /* while */
				/*VENKAT*/
#if (L1_PCM_EXTRACTION)
		if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY)
		{
#if(AS_RFS_API == 1)
			  p_audio_gbl_var->vbuf_pcm_play.ffs_read = rfs_lseek(p_session->session_req.audio_ffs_fd,
													              0,
											            			RFS_SEEK_CUR);
#else
		      p_audio_gbl_var->vbuf_pcm_play.ffs_read = ffs_seek(p_session->session_req.audio_ffs_fd,
													              0,
											            			FFS_SEEK_CUR);
#endif
		}
#endif //(L1_PCM_EXTRACTION)
		/*!VENKAT*/
                break;
              }

              case AUDIO_FFS_RAM_2_FLASH_SESSION:
              {

//                AUDIO_SEND_TRACE("AUDIO FFS DOWNLOADER: RAM to FLASH", RV_TRACE_LEVEL_DEBUG_LOW);

                /* save the full buffer */
                #ifndef _WINDOWS
				/*VENKAT*/
#if (L1_PCM_EXTRACTION)
  	  	      debug_downloader_record_counter++;
		  if(debug_downloader_record_counter >= DEBUG_LIMIT_MAX)
		  	debug_downloader_record_counter =0;

		   debug_open_channel_RECORD[debug_downloader_record_counter] = i;
		   debug_p_session_addr_RECORD[debug_downloader_record_counter] = p_session;
		   debug_p_session_id_RECORD[debug_downloader_record_counter] = p_session->session_req.session_id;
		  debug_p_ffs_index_RECORD[debug_downloader_record_counter] = index_ffs;
  	 	debug_downloader_write_tick_count[debug_downloader_record_counter] =  rvf_get_tick_count();
#endif //(L1_PCM_EXTRACTION)
//		  AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: open channel for RECORD",
//            									i, RV_TRACE_LEVEL_DEBUG_LOW);

//		  AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: p_session addr for RECORD",
//		            p_session, RV_TRACE_LEVEL_DEBUG_LOW);


#if (L1_PCM_EXTRACTION)
		if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
		{

#if(AS_RFS_API == 1)

			  debug_write_ptr[debug_write_ptr_counter++] = rfs_lseek(p_session->session_req.audio_ffs_fd,
		              p_audio_gbl_var->vbuf_pcm_record.ffs_write,
            			RFS_SEEK_SET);
#else
		      debug_write_ptr[debug_write_ptr_counter++] = ffs_seek(p_session->session_req.audio_ffs_fd,
		              p_audio_gbl_var->vbuf_pcm_record.ffs_write,
            			FFS_SEEK_SET);
#endif


//  	             AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: debug_write_ptr:",
//                            debug_write_ptr, RV_TRACE_LEVEL_DEBUG_LOW);
			if(debug_write_ptr_counter >= DEBUG_LIMIT_MAX)
				debug_write_ptr_counter =0;

		}
#endif
		/*!VENKAT*/
//				  AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: index_ffs Record buffer pointer:",
//p_session->session_info.buffer[index_ffs].p_start_pointer, RV_TRACE_LEVEL_DEBUG_LOW);

#if(AS_RFS_API == 1)
			ffs_error = rfs_write  (p_session->session_req.audio_ffs_fd,
                                   p_session->session_info.buffer[index_ffs].p_start_pointer,
                                   p_session->session_req.size);
                  if (ffs_error < RFS_EOK)
                  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_SAVED);
			AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: VBUF Record-FileSaveError:", ffs_error, RV_TRACE_LEVEL_DEBUG_LOW);
                  }

#else
			ffs_error = ffs_write  (p_session->session_req.audio_ffs_fd,
                                   p_session->session_info.buffer[index_ffs].p_start_pointer,
                                   p_session->session_req.size);
                  if (ffs_error < EFFS_OK)
                  {
                    audio_ffs_error_trace(AUDIO_ENTITY_FILE_NO_SAVED);
			AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: VBUF Record-FileSaveError:", ffs_error, RV_TRACE_LEVEL_DEBUG_LOW);
                  }
#endif

#if (L1_PCM_EXTRACTION)
		    else
		    {
		  	    if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
			   {
			   	p_audio_gbl_var->vbuf_pcm_record.size_copied_to_flash += p_session->session_req.size;
	                 }
			   else if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_RECORD)
			   {
				p_audio_gbl_var->vm_pcm_record.size_copied_to_flash += p_session->session_req.size;
			   }
			   AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: VBUF Record-buffer save success:", 0, RV_TRACE_LEVEL_DEBUG_LOW);

		    }
#endif//(L1_PCM_EXTRACTION)
                #endif
				/*VENKAT*/
#if (L1_PCM_EXTRACTION)
		if(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD)
		{

#if(AS_RFS_API == 1)
			  p_audio_gbl_var->vbuf_pcm_record.ffs_write = rfs_lseek(p_session->session_req.audio_ffs_fd,
														              0,
												            			RFS_SEEK_CUR);
#else
		      p_audio_gbl_var->vbuf_pcm_record.ffs_write = ffs_seek(p_session->session_req.audio_ffs_fd,
														              0,
												            			FFS_SEEK_CUR);
#endif
		}
#endif//(L1_PCM_EXTRACTION)
		/*!VENKAT*/
                break;
              }
            } /* switch (p_session->session_req.session_mode) */


            /* update the ffs buffer index */
            p_session->session_info.index_ffs++;
            if (p_session->session_info.index_ffs == AUDIO_MAX_FFS_BUFFER_PER_SESSION)
            {
              p_session->session_info.index_ffs = 0;
            }
          } /* (p_session->session_info.index_l1 != p_session->session_info.index_l1) */
#if (L1_PCM_EXTRACTION)
	   else
	   {
	   	debug_buff_no_switch_counter++;
		if(debug_buff_no_switch_counter >= DEBUG_LIMIT_MAX)
			debug_buff_no_switch_counter =0;
	   }
#endif// (L1_PCM_EXTRACTION)
        } /* valid session */
      } /* for (i=0; i<AUDIO_FFS_MAX_CHANNEL; i++) */

      /* Activate or not the Timer the next time */
      if (active_session)
      {

#if (L1_PCM_EXTRACTION)

/*		if((p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_RECORD) ||
			(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_RECORD) ||
			(p_session->session_req.session_id == AUDIO_FFS_SESSION_VBUF_PCM_PLAY) ||
			(p_session->session_req.session_id == AUDIO_FFS_SESSION_VM_PCM_PLAY))*/
			if(pcm_vm_vbuf_timer == 1)
			{
//   	                 AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: p_session->session_req.session_id:",
//                            p_session->session_req.session_id, RV_TRACE_LEVEL_DEBUG_LOW);

			    AUDIO_SEND_TRACE("audio_ffs_downloader(): Start timer with time out value AUDIO_FFS_TIME_OUT_PCM", RV_TRACE_LEVEL_ERROR);
			        rvf_start_timer(AUDIO_FFS_TIMER, AUDIO_FFS_TIME_OUT_PCM, AUDIO_FFS_ONE_SHOT_TIMER);
			}
			else
#endif//(L1_PCM_EXTRACTION)
			{
			    // AUDIO_SEND_TRACE("audio_ffs_downloader(): Start timer with time out value AUDIO_FFS_TIME_OUT", RV_TRACE_LEVEL_ERROR);
        rvf_start_timer(AUDIO_FFS_TIMER, AUDIO_FFS_TIME_OUT, AUDIO_FFS_ONE_SHOT_TIMER);
      }
//   	                 AUDIO_SEND_TRACE_PARAM("AUDIO FFS DOWNLOADER: p_session->session_req.session_id:",
//                            p_session->session_req.session_id, RV_TRACE_LEVEL_DEBUG_LOW);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO FFS DOWNLOADER: stop", RV_TRACE_LEVEL_DEBUG_LOW);
        /* Stop asap the FFS downloader */
        rvf_stop_timer(AUDIO_FFS_TIMER);
      }
    }
  #endif /* MELODY_E1 || MELODY_E2 || VOICE_MEMO */


#if (AUDIO_RAM_MANAGER)
  /* try to copy "size" bytes from audio_ram_fd to dest_buffer, returns bytes copied (0 to size) */
  INT16 ram_read(T_AUDIO_VOICE_MEMO_AMR_PLAY_FROM_MEM_VAR *vm_amr_play, UINT8 *dest_buffer, UINT16 size)
  {
    UINT16 i;

    // check how many bytes there are om audio_ram_fd
    if(size > vm_amr_play->audio_ram_size)
      size = vm_amr_play->audio_ram_size;

    // copy byte to byte
    for (i = 0; i < size; i++)
      *dest_buffer++ = *(vm_amr_play->audio_ram_fd)++;

    // update audio_ram_fd size
    vm_amr_play->audio_ram_size-=size;

    return size;
  }

  /* copy "size" bytes from src_buffer to audio_ram_fd, does not check size */
  INT16 ram_write(T_AUDIO_VOICE_MEMO_AMR_RECORD_TO_MEM_VAR *vm_amr_record, UINT8 *src_buffer, UINT16 size)
  {
    UINT16 i;

    // copy byte by byte
    for (i = 0; i < size; i++)
      *(vm_amr_record->audio_ram_fd)++ = *src_buffer++;

    return size;
  }
#endif

#endif /* RVM_AUDIO_MAIN_SWE */


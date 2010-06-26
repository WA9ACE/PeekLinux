/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_aac.c                                                 */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            AAC task.                                                     */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date          Modification                                              */
/*  ------------------------------------                                    */
/*  10 January 2005   Create                                                */
/*                                                                          */
/*  Author                                                                  */
/*     Stephanie Levieil                                                    */
/*                                                                          */
/* (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/


#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE
#ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
#endif

#include "as/as_api.h"
#include "as/as_i.h"
#include "as/as_message.h"
#include "as/as_midi.h"
  /* include the usefull L1 header */
  #include "l1_confg.h"
  #include "l1sw.cfg"

#if (L1_AAC == 1)
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"



  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_features_i.h"

  #if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
  #include "rfs/fscore_types.h"
#else
  #include "audio/audio_ffs_i.h"
#endif

  /* include the usefull L1 header */
  #define BOOL_FLAG
  #define CHAR_FLAG
  #include "l1_types.h"
  #include "l1audio_cust.h"
  #include "l1audio_msgty.h"
  #include "l1audio_signa.h"

  #include "l1aac_signa.h"
  #include "audio/audio_aac_i.h"

  #include "audio/audio_macro_i.h"
  #include "l1aac_defty.h"




  #define AAC_NO_OF_SAMPLES 1024
  #define AAC_FRAME_CONST  768000 // Check this  1000

 extern T_AAC_MCU_DSP *aac_ndb;
 static  UINT32  u32TotalTimePlayed = 0;

  void audio_aac_statusBar ( T_AUDIO_RET status,T_RV_RETURN return_path );

  void audio_send_data_to_MCU(void);

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_aac_message_switch	                              */
  /*                                                                              */
  /*    Purpose:  Manage the message                                              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        start or stop message from the aac features                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        index of the manager                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  UINT8 audio_aac_message_switch (T_RV_HDR *p_message)
  {
    switch (p_message->msg_id)
    {
      case AUDIO_AAC_START_REQ:
      case AUDIO_AAC_STOP_REQ:
      case AUDIO_AAC_PAUSE_REQ:
      case AUDIO_AAC_RESUME_REQ:
      case AUDIO_AAC_RESTART_REQ:
      case AUDIO_AAC_INFO_REQ:
        return(AUDIO_AAC);
//omaps00090550        break;

      /* driver init => check session_id is AAC */
      case AUDIO_DRIVER_INIT_STATUS_MSG:
      {
        UINT8 session_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->session_id;

        /* session_id is AAC */
        if ( (session_id == AUDIO_AAC_SESSION_ID)&&
             (p_audio_gbl_var->audio_aac.state != AUDIO_IDLE))
        {
          return (AUDIO_AAC);
        }
        return(AUDIO_AAC_NONE);
      }
//omaps00090550        break;

      case AUDIO_DRIVER_NOTIFICATION_MSG:
      {
        UWORD8 channel_id;
        channel_id = ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id;

        if( (channel_id == p_audio_gbl_var->audio_aac.channel_id)
          &&(p_audio_gbl_var->audio_aac.state > AUDIO_WAIT_CHANNEL_ID) )
        {
          return(AUDIO_AAC);
        }

        return(AUDIO_AAC_NONE);
      }
//omaps00090550        break;

      case AUDIO_DRIVER_STATUS_MSG:
      case AUDIO_DRIVER_STATUS_AAC_INFO_MSG:
      {
        UWORD8 channel_id;
        channel_id = ((T_AUDIO_DRIVER_STATUS *)p_message)->channel_id;
        if( (channel_id == p_audio_gbl_var->audio_aac.channel_id)
         && (p_audio_gbl_var->audio_aac.state > AUDIO_WAIT_CHANNEL_ID) )
        {
          return(AUDIO_AAC);
        }
        return(AUDIO_AAC_NONE);
      }
//omaps00090550        break;

      default:
        return(AUDIO_AAC_NONE);
//omaps00090550        break;
    } /* switch */
  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_aac_send_status                                    */
  /*                                                                              */
  /*    Purpose:  This function sends the AAC status to the entity.               */
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
  void audio_aac_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_AAC_STATUS *p_send_message;
    T_RVF_MB_STATUS    mb_status = RVF_RED;

    /* allocate the message buffer */
    while (mb_status == RVF_RED)
    {
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_AAC_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    /*fill the header of the message */
    p_send_message->os_hdr.msg_id = AUDIO_AAC_STATUS_MSG;

    /* fill the status parameters */
    p_send_message->status = status;

    /* fill the aac format parameters */
    p_send_message->aac_format = p_audio_gbl_var->audio_aac.aac_format;

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
  /*    Function Name:   audio_aac_send_info_status                               */
  /*                                                                              */
  /*    Purpose:  This function sends the AAC Info status to the entity.          */
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
  void audio_aac_send_info_status (T_AUDIO_RET status, T_L1A_AAC_INFO_CON info, T_RV_RETURN return_path)
  {
    T_AUDIO_AAC_INFO_STATUS *p_send_message;
    T_RVF_MB_STATUS    mb_status = RVF_RED;

    /* allocate the message buffer */
    while (mb_status == RVF_RED)
    {
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_AAC_INFO_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    /*fill the header of the message */
    p_send_message->os_hdr.msg_id = AUDIO_AAC_STATUS_INFO_MSG;

    /* fill the status parameters */
    p_send_message->status        = status;
    p_send_message->info.bitrate  = info.bitrate;
    p_send_message->info.channel  = info.channel;
    p_send_message->info.aac_format= info.aac_format;
    p_send_message->info.frequency= info.frequency;
    #ifdef _WINDOWS
      p_send_message->info.header   = info.header;
    #endif

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
  /*    Function Name:   audio_aac_manager                                        */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a manager                     */
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
  void audio_aac_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    T_RV_RETURN my_return_path;
    T_L1A_AAC_INFO_CON info;

    AUDIO_SEND_TRACE_PARAM("AAC_S : ", p_audio_gbl_var->audio_aac.state, RV_TRACE_LEVEL_DEBUG_LOW);

    /**************** audio_aac_manager function begins ***********************/
    switch(p_audio_gbl_var->audio_aac.state)
    {
      case AUDIO_IDLE:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_AAC_START_REQ:
          {
            T_AUDIO_DRIVER_PARAMETER driver_parameter;
	     AUDIO_SEND_TRACE("AUDIO AAC: AUDIO_IDLE -AUDIO_AAC_START_REQ", RV_TRACE_LEVEL_DEBUG_LOW);

           /* save the return path + ffs_fd */
            p_audio_gbl_var->audio_aac.return_path.callback_func =
              ((T_AUDIO_AAC_START *)p_message)->return_path.callback_func;
            p_audio_gbl_var->audio_aac.return_path.addr_id   =
              ((T_AUDIO_AAC_START*)p_message)->return_path.addr_id;
            p_audio_gbl_var->audio_aac.ffs_fd =
              ((T_AUDIO_AAC_START *)p_message)->audio_ffs_fd;
            p_audio_gbl_var->audio_aac.start_req = TRUE;
            p_audio_gbl_var->audio_aac.size_file_start =
              ((T_AUDIO_AAC_START *)p_message)->aac_parameter.size_file_start;
            p_audio_gbl_var->audio_aac.audio_filesize =
              ((T_AUDIO_AAC_START *)p_message)->audio_filesize;
            p_audio_gbl_var->audio_aac.play_bar_on =
              ((T_AUDIO_AAC_START *)p_message)->play_bar_on;
            p_audio_gbl_var->audio_aac.aac_mono_stereo_mmi_command =
              ((T_AUDIO_AAC_START *)p_message)->aac_parameter.mono_stereo;
			 p_audio_gbl_var->audio_aac.stop_aac_true= TRUE;

            /* driver parameters */
            driver_parameter.nb_buffer   = AUDIO_AAC_NB_BUFFER;
            driver_parameter.buffer_size = AUDIO_AAC_SIZE;

            /* return_path for driver */
            my_return_path.callback_func = NULL;
            my_return_path.addr_id       = p_audio_gbl_var->addrId;

            /* Init driver */


            audio_driver_init_aac_session(&driver_parameter, my_return_path);
		u32TotalTimePlayed = 0;

            p_audio_gbl_var->audio_aac.state = AUDIO_WAIT_CHANNEL_ID;
          }
          break;
          case AUDIO_AAC_STOP_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_STOP_EVENT);
            /* do not send a status message because of pre-emption issues
               An automatic stop can pre-empt a stop request. A status is sent + back in state idle
               then the stop request is received and another status is sent, which can be misinterpreted */
          }
          break;
          case AUDIO_AAC_PAUSE_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_AAC_RESUME_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
          break;
          case AUDIO_AAC_INFO_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_INFO_EVENT);
          }
          case AUDIO_AAC_RESTART_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_RESTART_EVENT);
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
            INT16 size_read=0;

            T_AUDIO_DRIVER_SESSION *p_session;

            /* check init is successfull otherwise close file and send status AUDIO_ERROR */
            if (((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status == AUDIO_OK)
            {
              /* get channel id */
              p_audio_gbl_var->audio_aac.channel_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              /* initializations */
              p_audio_gbl_var->audio_aac.session_id       = AUDIO_AAC_SESSION_ID;
              p_audio_gbl_var->audio_aac.size             = AUDIO_AAC_SIZE <<1 ;  // temporary RAM buffer size
              p_audio_gbl_var->audio_aac.size_left        = 0;
              p_audio_gbl_var->audio_aac.stop_req_allowed = TRUE;
              p_audio_gbl_var->audio_aac.pause_req_allowed = TRUE;
              p_audio_gbl_var->audio_aac.last_stop_msg    = FALSE;
              p_audio_gbl_var->audio_aac.last_pause_msg   = FALSE;
              p_audio_gbl_var->audio_aac.last_resume_msg  = FALSE;
              p_audio_gbl_var->audio_aac.last_restart_msg = FALSE;
              p_audio_gbl_var->audio_aac.last_info_msg    = FALSE;
              p_audio_gbl_var->audio_aac.buff_notify_msg  = FALSE;

              /* seek to the position specified in the AAC start API */
#if(AS_RFS_API == 1)
			  if(rfs_lseek ( p_audio_gbl_var->audio_aac.ffs_fd,
                            p_audio_gbl_var->audio_aac.size_file_start,
                            RFS_SEEK_SET) < RFS_EOK )
#else

              if(ffs_seek ( p_audio_gbl_var->audio_aac.ffs_fd,
                            p_audio_gbl_var->audio_aac.size_file_start,
                            FFS_SEEK_SET) < EFFS_OK )
#endif
              {
#if(AS_RFS_API == 1)
				  AUDIO_SEND_TRACE("AUDIO AAC: RFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
#else
                  AUDIO_SEND_TRACE("AUDIO AAC: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
#endif
                  size_read = 0;
              }

              p_audio_gbl_var->audio_aac.size_played = p_audio_gbl_var->audio_aac.size_file_start;
              /* fill + convert all buffers in advance */
              while(audio_driver_get_play_buffer(p_audio_gbl_var->audio_aac.channel_id, &play_buffer) == AUDIO_OK)
              {

#if(AS_RFS_API == 1)

/* write from FLASH to RAM buffer */
                if(p_audio_gbl_var->audio_aac.ffs_fd >= RFS_EOK)
                {
                  size_read = (INT16)rfs_read( p_audio_gbl_var->audio_aac.ffs_fd,
                                        play_buffer,
                                        p_audio_gbl_var->audio_aac.size);
                  if(size_read < RFS_EOK)
                  {
                    AUDIO_SEND_TRACE("AUDIO AAC: RFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
                    size_read = 0;  // will fill buffer with END_MASK and stop task
                  }
                }

#else
                /* write from FLASH to RAM buffer */
                if(p_audio_gbl_var->audio_aac.ffs_fd >= EFFS_OK)
                {
                  size_read = ffs_read( p_audio_gbl_var->audio_aac.ffs_fd,
                                        play_buffer,
                                        p_audio_gbl_var->audio_aac.size);
                  if(size_read < EFFS_OK)
                  {
                    AUDIO_SEND_TRACE("AUDIO AAC: FFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
                    size_read = 0;  // will fill buffer with END_MASK and stop task
                  }
                }
#endif

                /* nb of bytes read in the AAC file */
                p_audio_gbl_var->audio_aac.size_played += size_read;

                /* last buffer already, call flush buffer */
                if((UINT16)size_read < p_audio_gbl_var->audio_aac.size)
                {
                  UINT16 i;

                  for(i = size_read; i < p_audio_gbl_var->audio_aac.size; i++)
                    *(play_buffer + i) = 0x00;

                  AUDIO_SEND_TRACE_PARAM("AUDIO AAC: FFS to RAM size", size_read,
                                           RV_TRACE_LEVEL_DEBUG_LOW);

                  audio_driver_flush_buffer(p_audio_gbl_var->audio_aac.channel_id, play_buffer);
                  p_audio_gbl_var->audio_aac.size_played = 0;
                }
                else
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO AAC: FFS to RAM size", size_read,
                                         RV_TRACE_LEVEL_DEBUG_LOW);
                  audio_driver_play_buffer(p_audio_gbl_var->audio_aac.channel_id, play_buffer);
                }

                /* save the read size */
                p_session = &(p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_aac.channel_id]);
                p_session->session_info.buffer_size = size_read;
              }

              /* initialize the return path */
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              /* send message */
              audio_driver_start_session(p_audio_gbl_var->audio_aac.channel_id, my_return_path);

              /* change state */
              p_audio_gbl_var->audio_aac.state = AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION;
            } // if (((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status == AUDIO_OK)
            else
            {
              audio_aac_error_trace(AUDIO_ERROR_START_EVENT);
#if(AS_RFS_API == 1)
			  if (p_audio_gbl_var->audio_aac.ffs_fd >= RFS_EOK)
#else
			  if (p_audio_gbl_var->audio_aac.ffs_fd >= EFFS_OK)
#endif
              {
#if(AS_RFS_API == 1)
				if(rfs_close(p_audio_gbl_var->audio_aac.ffs_fd) != RFS_EOK)
#else
				if(ffs_close(p_audio_gbl_var->audio_aac.ffs_fd) != EFFS_OK)
#endif
                  audio_aac_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              }
              audio_aac_send_status (AUDIO_ERROR, p_audio_gbl_var->audio_aac.return_path);
              /* change state */
              p_audio_gbl_var->audio_aac.state = AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_AAC_STOP_REQ:
            /* change state */
            p_audio_gbl_var->audio_aac.state = AUDIO_WAIT_CHANNEL_ID_TO_STOP;
          break;
          case AUDIO_AAC_PAUSE_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_AAC_RESUME_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
          break;
          case AUDIO_AAC_INFO_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_INFO_EVENT);
          }
          break;
          case AUDIO_AAC_RESTART_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_RESTART_EVENT);
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
			// added.
			  if (p_audio_gbl_var->audio_aac.play_bar_on == 1)
			    audio_aac_statusBar(AUDIO_OK, p_audio_gbl_var->audio_aac.return_path);

            audio_send_data_to_MCU();
          } // case AUDIO_DRIVER_NOTIFICATION_MSG
          break;
          case AUDIO_AAC_STOP_REQ:
		  {
		    if(p_audio_gbl_var->audio_aac.stop_req_allowed == TRUE)
		    {
		      AUDIO_SEND_TRACE("AUDIO AAC: STOP Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
		      p_audio_gbl_var->audio_aac.stop_req_allowed = FALSE;
		      p_audio_gbl_var->audio_aac.last_stop_msg = TRUE;
		      audio_driver_stop_session(p_audio_gbl_var->audio_aac.channel_id);
		    }
		    else
		      /* do not send a status message because of pre-emption issues */
              audio_aac_error_trace(AUDIO_ERROR_STOP_EVENT);
	      }
          break;
          case AUDIO_AAC_PAUSE_REQ:
          {
            if(p_audio_gbl_var->audio_aac.pause_req_allowed == TRUE)
            {
              AUDIO_SEND_TRACE("AUDIO AAC: PAUSE Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_aac.pause_req_allowed = FALSE;
              p_audio_gbl_var->audio_aac.last_pause_msg = TRUE;
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              audio_driver_pause_session(p_audio_gbl_var->audio_aac.channel_id, my_return_path);
            }
            else
              /* do not send a status message because of pre-emption issues */
              audio_aac_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;
          case AUDIO_AAC_INFO_REQ:
          {
              AUDIO_SEND_TRACE("AUDIO AAC: INFO Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_aac.last_info_msg = TRUE;
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              audio_driver_info_aac_session(p_audio_gbl_var->audio_aac.channel_id, my_return_path);
          }
          break;

          case AUDIO_DRIVER_STATUS_MSG:
          {
            if(p_audio_gbl_var->audio_aac.start_req == TRUE)
            {
              p_audio_gbl_var->audio_aac.start_req = FALSE;
            }      
            else if(p_audio_gbl_var->audio_aac.last_pause_msg == TRUE)
            {
              p_audio_gbl_var->audio_aac.state = AUDIO_WAIT_STOP_OR_RESUME_OR_RESTART;
              p_audio_gbl_var->audio_aac.last_pause_msg = FALSE;
		#if(L1_BT_AUDIO==1)
		as_send_pause_ind(AUDIO_OK,FALSE, &as_ctrl_blk->client_rp);
		#endif
            }
            else
            {
#if(AS_RFS_API == 1)
				if(p_audio_gbl_var->audio_aac.ffs_fd >= RFS_EOK)
#else
			   if(p_audio_gbl_var->audio_aac.ffs_fd >= EFFS_OK)
#endif
                {
                  AUDIO_SEND_TRACE("AUDIO AAC: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
#if(AS_RFS_API == 1)
				  if(rfs_close(p_audio_gbl_var->audio_aac.ffs_fd) != RFS_EOK)
#else
				  if(ffs_close(p_audio_gbl_var->audio_aac.ffs_fd) != EFFS_OK)
#endif
                    audio_aac_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }

					 if(((T_AUDIO_DRIVER_STATUS *)p_message)->status != AUDIO_OK)
				{
                  audio_aac_send_status(((T_AUDIO_DRIVER_STATUS *)p_message)->status, p_audio_gbl_var->audio_aac.return_path);
                }
                else
                 audio_aac_send_status(AUDIO_OK, p_audio_gbl_var->audio_aac.return_path);

                p_audio_gbl_var->audio_aac.state = AUDIO_IDLE;
            }
          }
          break;

          case AUDIO_DRIVER_STATUS_AAC_INFO_MSG:
          {
             if(p_audio_gbl_var->audio_aac.last_info_msg == TRUE)
              {
                info.bitrate  = ((T_AUDIO_DRIVER_AAC_INFO_STATUS *)p_message)->info.bitrate;
                info.channel  = ((T_AUDIO_DRIVER_AAC_INFO_STATUS *)p_message)->info.channel;
                info.frequency= ((T_AUDIO_DRIVER_AAC_INFO_STATUS *)p_message)->info.frequency;
                info.aac_format = ((T_AUDIO_DRIVER_AAC_INFO_STATUS *)p_message)->info.aac_format;
                #ifdef _WINDOWS
                  info.header.callback_func = ((T_AUDIO_DRIVER_AAC_INFO_STATUS *)p_message)->info.header.callback_func;
                  info.header.dest_addr_id = ((T_AUDIO_DRIVER_AAC_INFO_STATUS *)p_message)->info.header.dest_addr_id;
                  info.header.msg_id = ((T_AUDIO_DRIVER_AAC_INFO_STATUS *)p_message)->info.header.msg_id;
                  info.header.src_addr_id = ((T_AUDIO_DRIVER_AAC_INFO_STATUS *)p_message)->info.header.src_addr_id;
                #endif

                p_audio_gbl_var->audio_aac.state = AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION;
                p_audio_gbl_var->audio_aac.last_info_msg = FALSE;
                audio_aac_send_info_status(AUDIO_OK,info,p_audio_gbl_var->audio_aac.return_path);
              }
             else
              /* do not send a status message because of pre-emption issues */
              audio_aac_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;

          case AUDIO_AAC_RESUME_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
          break;
          case AUDIO_AAC_RESTART_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_RESTART_EVENT);
          }
          break;
        } // switch (p_message->msg_id)
      } // case AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION:
      break;

      case AUDIO_WAIT_STOP_OR_RESUME_OR_RESTART:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_AAC_STOP_REQ:
          {
            if(p_audio_gbl_var->audio_aac.stop_req_allowed == TRUE)
            {
              AUDIO_SEND_TRACE("AUDIO AAC: STOP Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_aac.stop_req_allowed = FALSE;
              p_audio_gbl_var->audio_aac.last_stop_msg = TRUE;
              audio_driver_stop_session(p_audio_gbl_var->audio_aac.channel_id);
            }
            else
              /* do not send a status message because of pre-emption issues */
              audio_aac_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;
          case AUDIO_DRIVER_NOTIFICATION_MSG :
		  {
		    AUDIO_SEND_TRACE("NOTIFICATION_MSG recvd in STOP_OR_RESUME_OR_RESTART", RV_TRACE_LEVEL_DEBUG_LOW);
		    audio_send_data_to_MCU();
		  }
		  break;
          case AUDIO_AAC_RESUME_REQ:
          {
             AUDIO_SEND_TRACE("AUDIO AAC: RESUME Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
             p_audio_gbl_var->audio_aac.last_resume_msg   = TRUE;
             p_audio_gbl_var->audio_aac.pause_req_allowed = TRUE;
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

             audio_driver_resume_session(p_audio_gbl_var->audio_aac.channel_id,my_return_path);
          }
          break;
          case AUDIO_AAC_RESTART_REQ:
          {
              UINT8 *play_buffer;
              INT16 size_read;
              T_AUDIO_DRIVER_SESSION *p_session;

              AUDIO_SEND_TRACE("AUDIO AAC: RESTART Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_aac.last_restart_msg   = TRUE;
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              p_audio_gbl_var->audio_aac.size_file_start =
                ((T_AUDIO_AAC_RESTART *)p_message)->aac_restart_parameter.size_file_start;

              AUDIO_SEND_TRACE_PARAM("p_audio_gbl_var->audio_aac.size_file_start aac",p_audio_gbl_var->audio_aac.size_file_start, RV_TRACE_LEVEL_ERROR);

#if(AS_RFS_API == 1)

/* seek to the position specified in the AAC restart API */
             	if(rfs_lseek ( p_audio_gbl_var->audio_aac.ffs_fd,
                            p_audio_gbl_var->audio_aac.size_file_start,
                            RFS_SEEK_SET) < RFS_EOK )
              {
                  AUDIO_SEND_TRACE("AUDIO AAC: RFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;
              }

#else
              /* seek to the position specified in the AAC restart API */
             	if(ffs_seek ( p_audio_gbl_var->audio_aac.ffs_fd,
                            p_audio_gbl_var->audio_aac.size_file_start,
                            FFS_SEEK_SET) < EFFS_OK )
              {
                  AUDIO_SEND_TRACE("AUDIO AAC: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;
              }
#endif
              p_audio_gbl_var->audio_aac.size_played = p_audio_gbl_var->audio_aac.size_file_start;

              /* fill + convert all buffers in advance */

              /* give new buffer = index_l1++ % nb_buffer */
              p_session = &(p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_aac.channel_id]);
              /* all the buffers will be refilled - so index appli == 0( first buffer), and index_l1= 1 ( next buffer); */
              p_session->session_info.index_l1    = 0xFF;
              p_session->session_info.index_appli = 0;
              p_audio_gbl_var->audio_aac.index_last_buffer = 0xFF;

              while(audio_driver_get_play_buffer(p_audio_gbl_var->audio_aac.channel_id, &play_buffer) == AUDIO_OK)
              {

                /* 1st time, index_l1 = 0xFF. At 1st play, layer1 becomes valid so we put 0
                This way, appli can't circle round buffers when in WAIT_START state */
                if (p_session->session_info.index_l1 == 0xFF)
                {
                    p_session->session_info.index_l1 =0;
                }

#if(AS_RFS_API == 1)

/* write from FLASH to RAM buffer */
                if(p_audio_gbl_var->audio_aac.ffs_fd >= RFS_EOK)
                {
                  size_read = (INT16)rfs_read( p_audio_gbl_var->audio_aac.ffs_fd,
                                        play_buffer,
                                        p_audio_gbl_var->audio_aac.size);
                  AUDIO_SEND_TRACE_PARAM("p_audio_gbl_var->audio_aac.ffs_fd after get buf",p_audio_gbl_var->audio_aac.ffs_fd, RV_TRACE_LEVEL_ERROR);

                  if(size_read < RFS_EOK)
                  {
                    AUDIO_SEND_TRACE("AUDIO AAC: RFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
                    size_read = 0;
                  }
                }

#else

                /* write from FLASH to RAM buffer */
                if(p_audio_gbl_var->audio_aac.ffs_fd >= EFFS_OK)
                {
                  size_read = ffs_read( p_audio_gbl_var->audio_aac.ffs_fd,
                                        play_buffer,
                                        p_audio_gbl_var->audio_aac.size);
                  AUDIO_SEND_TRACE_PARAM("p_audio_gbl_var->audio_aac.ffs_fd after get buf",p_audio_gbl_var->audio_aac.ffs_fd, RV_TRACE_LEVEL_ERROR);

                  if(size_read < EFFS_OK)
                  {
                    AUDIO_SEND_TRACE("AUDIO AAC: FFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
                    size_read = 0;
                  }
                }
#endif

                /* nb of bytes read in the AAC file */
                p_audio_gbl_var->audio_aac.size_played += size_read;
                AUDIO_SEND_TRACE_PARAM("nb of bytes read in the AAC file",p_audio_gbl_var->audio_aac.size_played, RV_TRACE_LEVEL_ERROR);


                /* last buffer already, call flush buffer */
                if((UINT16)size_read < p_audio_gbl_var->audio_aac.size)
                {
                  UINT16 i;

                  p_audio_gbl_var->audio_aac.pause_req_allowed = FALSE;
                  p_audio_gbl_var->audio_aac.stop_req_allowed  = TRUE;

                  for(i = size_read; i < p_audio_gbl_var->audio_aac.size; i++)
                    *(play_buffer + i) = 0x00;

                  AUDIO_SEND_TRACE_PARAM("AUDIO AAC: FFS to RAM size", size_read,
                                           RV_TRACE_LEVEL_DEBUG_LOW);

                  audio_driver_flush_buffer(p_audio_gbl_var->audio_aac.channel_id, play_buffer);
                  p_audio_gbl_var->audio_aac.size_played = 0;
                }
                else
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO AAC: FFS to RAM size", size_read,
                                         RV_TRACE_LEVEL_DEBUG_LOW);
                  audio_driver_play_buffer(p_audio_gbl_var->audio_aac.channel_id, play_buffer);
                }

                /* save the read size */
                p_session = &(p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_aac.channel_id]);
                p_session->session_info.buffer_size = size_read;
              }

             AUDIO_SEND_TRACE_PARAM("before audio_driver_restart_session",p_audio_gbl_var->audio_aac.last_restart_msg, RV_TRACE_LEVEL_ERROR);

             audio_driver_restart_session(p_audio_gbl_var->audio_aac.channel_id,my_return_path);
             AUDIO_SEND_TRACE("after audio_driver_restart_session", RV_TRACE_LEVEL_ERROR);

          }
          break;
          case AUDIO_DRIVER_STATUS_MSG:
          {
            AUDIO_SEND_TRACE_PARAM("AUDIO_DRIVER_STATUS_MSG",p_audio_gbl_var->audio_aac.last_restart_msg, RV_TRACE_LEVEL_ERROR);

            /* We receive the Audio_DRIVER_STATUS_MSG corresponding to the RESUME */
            if(p_audio_gbl_var->audio_aac.last_resume_msg == TRUE)
            {
              p_audio_gbl_var->audio_aac.state = AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION;
              p_audio_gbl_var->audio_aac.last_resume_msg = FALSE;
            }
            else
            {
              /* We receive the Audio_DRIVER_STATUS_MSG corresponding to the RESTART */
              if(p_audio_gbl_var->audio_aac.last_restart_msg == TRUE)
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO_DRIVER_STATUS_MSG",p_audio_gbl_var->audio_aac.last_restart_msg, RV_TRACE_LEVEL_ERROR);
                p_audio_gbl_var->audio_aac.state = AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION;
                p_audio_gbl_var->audio_aac.last_restart_msg = FALSE;
              }
              else
              {
#if(AS_RFS_API == 1)

				if(p_audio_gbl_var->audio_aac.ffs_fd >= RFS_EOK)
                {
                  AUDIO_SEND_TRACE("AUDIO AAC: close RFS file", RV_TRACE_LEVEL_DEBUG_LOW);
                  if(rfs_close(p_audio_gbl_var->audio_aac.ffs_fd) != RFS_EOK)
                    audio_aac_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
                audio_aac_send_status(AUDIO_OK, p_audio_gbl_var->audio_aac.return_path);
                  p_audio_gbl_var->audio_aac.state = AUDIO_IDLE;
                p_audio_gbl_var->audio_aac.last_stop_msg = FALSE;

#else

                if(p_audio_gbl_var->audio_aac.ffs_fd >= EFFS_OK)
                {
                  AUDIO_SEND_TRACE("AUDIO AAC: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
                  if(ffs_close(p_audio_gbl_var->audio_aac.ffs_fd) != EFFS_OK)
                    audio_aac_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
                audio_aac_send_status(AUDIO_OK, p_audio_gbl_var->audio_aac.return_path);
                  p_audio_gbl_var->audio_aac.state = AUDIO_IDLE;
                p_audio_gbl_var->audio_aac.last_stop_msg = FALSE;

#endif
              }
            }
          }
          break;
          case AUDIO_AAC_INFO_REQ:
          {
             AUDIO_SEND_TRACE("AUDIO AAC: INFO Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
             p_audio_gbl_var->audio_aac.last_info_msg   = TRUE;
             my_return_path.callback_func = NULL;
             my_return_path.addr_id       = p_audio_gbl_var->addrId;

             audio_driver_info_aac_session(p_audio_gbl_var->audio_aac.channel_id, my_return_path);
          }
          break;
          case AUDIO_AAC_PAUSE_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_AAC_START_REQ:
          {
            audio_aac_error_trace(AUDIO_ERROR_START_EVENT);
          }
          break;

        } // switch (p_message->msg_id)
      } // case AUDIO_WAIT_STOP_OR_RESUME_OR_RESTART:
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
              p_audio_gbl_var->audio_aac.channel_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              audio_driver_stop_session(p_audio_gbl_var->audio_aac.channel_id);

              /* change state */
              p_audio_gbl_var->audio_aac.state = AUDIO_WAIT_DRIVER_STOP_CON;
            }
            else
            {
#if(AS_RFS_API == 1)

/* close file */
              if (p_audio_gbl_var->audio_aac.ffs_fd >= RFS_EOK)
              {
                if ( rfs_close(p_audio_gbl_var->audio_aac.ffs_fd) != RFS_EOK )
                  audio_aac_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                AUDIO_SEND_TRACE("AUDIO AAC: close RFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }

              audio_aac_send_status (AUDIO_OK, p_audio_gbl_var->audio_aac.return_path);

              /* change state */
              p_audio_gbl_var->audio_aac.state = AUDIO_IDLE;

#else
              /* close file */
              if (p_audio_gbl_var->audio_aac.ffs_fd >= EFFS_OK)
              {
                if ( ffs_close(p_audio_gbl_var->audio_aac.ffs_fd) != EFFS_OK )
                  audio_aac_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                AUDIO_SEND_TRACE("AUDIO AAC: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }

              audio_aac_send_status (AUDIO_OK, p_audio_gbl_var->audio_aac.return_path);

              /* change state */
              p_audio_gbl_var->audio_aac.state = AUDIO_IDLE;
#endif
            }
          }
          break;
          case AUDIO_AAC_STOP_REQ:
            audio_aac_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
          case AUDIO_AAC_PAUSE_REQ:
            audio_aac_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          break;
          case AUDIO_AAC_RESUME_REQ:
            audio_aac_error_trace(AUDIO_ERROR_RESUME_EVENT);
          break;
          case AUDIO_AAC_RESTART_REQ:
            audio_aac_error_trace(AUDIO_ERROR_RESTART_EVENT);
          break;
          case AUDIO_AAC_INFO_REQ:
            audio_aac_error_trace(AUDIO_ERROR_INFO_EVENT);
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
#if(AS_RFS_API == 1)

 /* close file */
            if (p_audio_gbl_var->audio_aac.ffs_fd >= RFS_EOK)
              {
                if ( rfs_close(p_audio_gbl_var->audio_aac.ffs_fd) != RFS_EOK )
                audio_aac_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              AUDIO_SEND_TRACE("AUDIO AAC: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }

#else

              /* close file */
            if (p_audio_gbl_var->audio_aac.ffs_fd >= EFFS_OK)
              {
                if ( ffs_close(p_audio_gbl_var->audio_aac.ffs_fd) != EFFS_OK )
                audio_aac_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              AUDIO_SEND_TRACE("AUDIO AAC: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }

#endif

            audio_aac_send_status (AUDIO_OK, p_audio_gbl_var->audio_aac.return_path);
              p_audio_gbl_var->audio_aac.state = AUDIO_IDLE;
            }
          break;
          case AUDIO_AAC_STOP_REQ:
            audio_aac_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
          case AUDIO_AAC_PAUSE_REQ:
            audio_aac_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          break;
          case AUDIO_AAC_RESUME_REQ:
            audio_aac_error_trace(AUDIO_ERROR_RESUME_EVENT);
          break;
          case AUDIO_AAC_RESTART_REQ:
            audio_aac_error_trace(AUDIO_ERROR_RESTART_EVENT);
          break;
          case AUDIO_AAC_INFO_REQ:
            audio_aac_error_trace(AUDIO_ERROR_INFO_EVENT);
          break;

        }
      } //case AUDIO_WAIT_DRIVER_STOP_CON:
      break;
    } // switch(p_audio_gbl_var->audio_aac.state)
  } /*********************** End of audio_aac_manager function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_aac_manager                                 */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a AAC manager                 */
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
  T_AUDIO_RET audio_driver_aac_manager (T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session)
  {
    /**************** audio_driver_aac_manager function begins ***********************/
    switch(p_session->session_info.state)
    {
      case AUDIO_DRIVER_CHANNEL_WAIT_INIT:
      {
        /* init buffer index, layer1 not valid until 1st buffer is filled */
        /* index_l1 will be set to 0 when get_play_buffer() is called in WAIT_START state */
        p_session->session_info.index_l1    = 0xFF;
        p_session->session_info.index_appli = 0;
        p_audio_gbl_var->audio_aac.index_last_buffer = 0xFF;

        p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;

        /* allocate the buffer for the message to the L1 */
        p_session->session_req.p_l1_send_message =
          audio_allocate_l1_message(sizeof(T_MMI_AAC_REQ));
        ((T_MMI_AAC_REQ *)(p_session->session_req.p_l1_send_message))->session_id = AUDIO_AAC_SESSION_ID;
        ((T_MMI_AAC_REQ *)(p_session->session_req.p_l1_send_message))->loopback = 0;
        ((T_MMI_AAC_REQ *)(p_session->session_req.p_l1_send_message))->stereo     = ((T_AUDIO_AAC_PARAMETER *)p_message)->mono_stereo;
        ((T_MMI_AAC_REQ *)(p_session->session_req.p_l1_send_message))->dma_channel_number = AUDIO_DMA_CHANNEL;


        if(p_session->session_req.p_l1_send_message!=NULL)
          return(AUDIO_OK);
        else
          return(AUDIO_ERROR);

      }
      //omaps00090550        break;

      case AUDIO_DRIVER_CHANNEL_WAIT_START:
      {
        /* send the start AAC message to the L1 */
       audio_send_l1_message(MMI_AAC_START_REQ,
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
          if (p_audio_gbl_var->audio_aac.last_stop_msg == TRUE)
          {
            /* send the stop command to the audio L1 */
            audio_send_l1_message(MMI_AAC_STOP_REQ, p_send_message);
          }
          else
          {
            if(p_audio_gbl_var->audio_aac.last_pause_msg == TRUE)
            {
              /* send the pause command to the audio L1 */
              audio_send_l1_message(MMI_AAC_PAUSE_REQ, p_send_message);
            }
            else
            {
              if(p_audio_gbl_var->audio_aac.last_resume_msg == TRUE)
              {
                /* send the resume command to the audio L1 */
                audio_send_l1_message(MMI_AAC_RESUME_REQ, p_send_message);
              }
              else
              {
                if(p_audio_gbl_var->audio_aac.last_info_msg == TRUE)
                {
                  /* send the info command to the audio L1 */
                  audio_send_l1_message(MMI_AAC_INFO_REQ, p_send_message);
                }
                else
                {
                  if(p_audio_gbl_var->audio_aac.last_restart_msg == TRUE)
                  {
                    /* send the restart command to the audio L1 */
                    audio_send_l1_message(MMI_AAC_RESTART_REQ, p_send_message);
                  }
                }
              }
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
          audio_send_l1_message(MMI_AAC_STOP_REQ, p_send_message);
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
          audio_send_l1_message(MMI_AAC_PAUSE_REQ, p_send_message);
          return (AUDIO_OK);
        }
        return (AUDIO_ERROR);
      }
      //omaps00090550        break;
    }
	/* no corresponding message found? */
	return (AUDIO_ERROR);
  } /*********************** End of audio_aac_manager function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_aac_l1_simulator                                   */
  /*                                                                              */
  /*    Purpose:  This function simulates the L1 for AAC                          */
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
  void audio_aac_l1_simulator(UINT16 event, T_RV_HDR *p_message)
  {
#ifdef _WINDOWS
    enum { WAIT_START_REQ, WAIT_STOP };

    T_RVF_MB_STATUS mb_status;
    T_RV_RETURN     *return_path=&(p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_aac.channel_id].session_req.return_path);
    T_L1A_AAC_INFO_CON info;
    UINT8           status;
    UINT16          size;

    switch(p_audio_gbl_var->audio_aac.l1_state)
    {
      case WAIT_START_REQ:
        if(p_message->msg_id==MMI_AAC_START_REQ)
        {
          rvf_start_timer(AUDIO_AAC_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_AAC_L1_SIMUL_ONE_SHOT_TIMER);
          p_audio_gbl_var->audio_aac.counter=10;

          /* send MMI_AAC_START_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_AAC_START_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_aac.channel_id;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }

          p_audio_gbl_var->audio_aac.l1_state=WAIT_STOP;
          return;
        }
      break;

      case WAIT_STOP:
        if(event & AUDIO_AAC_L1_SIMUL_TIMER_EVT_MASK)
        {
          T_AUDIO_DRIVER_SESSION *p_session;
          UINT16 *p_buffer;

          p_session = &p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_aac.channel_id];

          p_buffer = (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer;

          p_audio_gbl_var->audio_aac.counter--;

          status = Cust_get_pointer_next_buffer(&p_buffer, &size,AUDIO_AAC_SESSION_ID);
          Cust_get_pointer_notify(AUDIO_AAC_SESSION_ID);

          /* check if we're done with the simulation */
          if(p_audio_gbl_var->audio_aac.counter==0)
          {
            rvf_stop_timer(AUDIO_AAC_L1_SIMUL_TIMER);

            /* send MMI_AAC_STOP_CON message to the Riviera audio entity */
            mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                  sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                  (T_RVF_BUFFER **)(&p_message));
            if(mb_status==RVF_RED)
            {
              AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
              return;
            }
            ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_AAC_STOP_CON;
            ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_aac.channel_id;
            ((T_L1A_AAC_CON *)p_message)->error_code = AUDIO_OK;
            if(return_path->callback_func==NULL)
              rvf_send_msg(return_path->addr_id, p_message);
            else
            {
              (*return_path->callback_func)((void *)(p_message));
              rvf_free_buf((T_RVF_BUFFER *)p_message);
            }

            p_audio_gbl_var->audio_aac.l1_state=WAIT_START_REQ;
            return;
          }
          rvf_start_timer(AUDIO_AAC_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_AAC_L1_SIMUL_ONE_SHOT_TIMER);
        }
        if(p_message->msg_id==MMI_AAC_STOP_REQ)
        {
          rvf_stop_timer(AUDIO_AAC_L1_SIMUL_TIMER);

          /* send MMI_AAC_STOP_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_AAC_STOP_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_aac.channel_id;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }

          p_audio_gbl_var->audio_aac.l1_state=WAIT_START_REQ;
          return;
        }

        if(p_message->msg_id==MMI_AAC_PAUSE_REQ)
        {
          rvf_stop_timer(AUDIO_AAC_L1_SIMUL_TIMER);

          /* send MMI_AAC_PAUSE_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_AAC_PAUSE_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_aac.channel_id;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }
          p_audio_gbl_var->audio_aac.l1_state=WAIT_STOP;
          return;
        }

        if(p_message->msg_id==MMI_AAC_RESUME_REQ)
        {
          rvf_start_timer(AUDIO_AAC_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_AAC_L1_SIMUL_ONE_SHOT_TIMER);

          /* send MMI_AAC_RESUME_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_AAC_RESUME_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_aac.channel_id;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }
          p_audio_gbl_var->audio_aac.l1_state=WAIT_STOP;
          return;
        }

        if(p_message->msg_id==MMI_AAC_RESTART_REQ)
        {
          rvf_start_timer(AUDIO_AAC_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_AAC_L1_SIMUL_ONE_SHOT_TIMER);

          /* send MMI_AAC_RESTART_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_AAC_RESTART_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_aac.channel_id;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }
          p_audio_gbl_var->audio_aac.l1_state=WAIT_STOP;
          return;
        }

        if(p_message->msg_id==MMI_AAC_INFO_REQ)
        {
          rvf_start_timer(AUDIO_AAC_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_AAC_L1_SIMUL_ONE_SHOT_TIMER);

          /* send MMI_AAC_INFO_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                (sizeof(T_AUDIO_DRIVER_NOTIFICATION)+sizeof(T_L1A_AAC_INFO_CON)),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_AAC_INFO_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_aac.channel_id;
          ((T_L1A_AAC_INFO_CON *)p_message)->bitrate  = 0;
          ((T_L1A_AAC_INFO_CON *)p_message)->channel  = 1;
          ((T_L1A_AAC_INFO_CON *)p_message)->frequency= 2;
          ((T_L1A_AAC_INFO_CON *)p_message)->aac_format = 0;
          ((T_L1A_AAC_INFO_CON *)p_message)->header.callback_func = 0x00;
          ((T_L1A_AAC_INFO_CON *)p_message)->header.dest_addr_id = 0x04;
          ((T_L1A_AAC_INFO_CON *)p_message)->header.msg_id = MMI_AAC_INFO_CON;
          ((T_L1A_AAC_INFO_CON *)p_message)->header.src_addr_id = 0x00;


          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }
          p_audio_gbl_var->audio_aac.l1_state=WAIT_STOP;
          return;
        }
    }
#endif /* _WINDOWS */
  }

  void audio_send_data_to_MCU(void)
  {
            UINT8 *play_buffer;
            INT16 size_read=0;
 		      T_AUDIO_DRIVER_SESSION *p_session;


            /* try to get a buffer */
            if(audio_driver_get_play_buffer(p_audio_gbl_var->audio_aac.channel_id, &play_buffer) == AUDIO_OK)
            {
#if(AS_RFS_API == 1)

			if(p_audio_gbl_var->audio_aac.ffs_fd >= RFS_EOK)
              {
                size_read = (INT16)rfs_read(p_audio_gbl_var->audio_aac.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_aac.size);

                if(size_read < RFS_EOK)
                {
                  AUDIO_SEND_TRACE("AUDIO AAC: FFS PLAY READ FILED", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;
                }
              }

#else
              if(p_audio_gbl_var->audio_aac.ffs_fd >= EFFS_OK)
              {
                size_read = ffs_read(p_audio_gbl_var->audio_aac.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_aac.size);

                if(size_read < EFFS_OK)
                {
                  AUDIO_SEND_TRACE("AUDIO AAC: FFS PLAY READ FILED", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;
                }
              }
#endif

				    // size_read has to updated  - Fix for OMAPS00133930
				    p_session = &(p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_aac.channel_id]);
                p_session->session_info.buffer_size = size_read;
		          p_session->session_info.buffer[p_session->session_info.index_appli].size = size_read;

                /* nb of bytes read in the AAC file */
                p_audio_gbl_var->audio_aac.size_played += size_read;

                /* last buffer already, call flush buffer */
                if((UINT16)size_read < p_audio_gbl_var->audio_aac.size)
                {
                  UINT16 i;


                  for(i = size_read; i < p_audio_gbl_var->audio_aac.size; i++)
                    *(play_buffer + i) = 0x00;

                  audio_driver_flush_buffer(p_audio_gbl_var->audio_aac.channel_id, play_buffer);
                  p_audio_gbl_var->audio_aac.size_played = 0;
        p_audio_gbl_var->audio_aac.pause_req_allowed = FALSE;
        AUDIO_SEND_TRACE("Last Buffer", RV_TRACE_LEVEL_DEBUG_LOW);
                }
                else
                {
                  audio_driver_play_buffer(p_audio_gbl_var->audio_aac.channel_id, play_buffer);
                }

              if(size_read > 0)
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO AAC: FFS to RAM size", size_read, RV_TRACE_LEVEL_DEBUG_LOW);
              }
              else
                AUDIO_SEND_TRACE("AUDIO AAC: buffer not used",RV_TRACE_LEVEL_DEBUG_LOW);
            } // if(audio_driver_get_play_buffer(channel_id,&p_buffer)==AUDIO_OK)
            else
              AUDIO_SEND_TRACE("AUDIO AAC: no buffer available", RV_TRACE_LEVEL_DEBUG_LOW);

            p_audio_gbl_var->audio_aac.buff_notify_msg = FALSE;
          }

                    /********************************************************************************/
		    /*                                                                              */
		    /*    Function Name:   audio_aac_statusBar                                      */
		    /*                                                                              */
		    /*    Purpose:  This function estimates the amount of the acc played            */
		    /*                                                                              */
		    /*    Input Parameters:                                                         */
		    /*                                                                              */
		    /*    Note:                                                                     */
		    /*        None.                                                                 */
		    /*                                                                              */
		    /*    Revision History:                                                         */
		    /*        None.                                                                 */
		    /*                                                                              */
		    /********************************************************************************/
		    void audio_aac_statusBar ( T_AUDIO_RET status,T_RV_RETURN return_path )
		    {
		  	   T_AUDIO_AAC_STATUS_BAR  *p_send_message;
		  	   T_RVF_MB_STATUS    mb_status = RVF_RED;
                           UWORD32 aac_frequency=0,u32Temp =0,u32Temp1 =0;
                           UWORD32 aac_bitrate;
                           static UINT16 u16totalTimeEst=0;
                           UINT16 framelength_aac;

                           UINT16 u16totalTime=0,u16TimePlayed = 0;
                           static UINT16 u16Noofframesread=0;

	              //    INT16 s16TimePlayedEst = 0;




                          UWORD16 freq_index=(aac_ndb->d_aac_api_frequency_index);
                           aac_bitrate   =(aac_ndb->d_aac_api_bitrate[0]<<16)+aac_ndb->d_aac_api_bitrate[1];



                           u16Noofframesread++;

                          aac_frequency = l1aac_get_frequencyrate(freq_index);
                           u32Temp = AAC_FRAME_CONST * aac_bitrate;

	   	           if ( u32Temp1 != 0)
	                     framelength_aac = (u32Temp /aac_frequency ) ;

                             /* If it is to be converted into time estimate the whole time for the first time */
                         if ( aac_bitrate != 0)
                              u16totalTime =  (p_audio_gbl_var->audio_aac.size * 8000) / aac_bitrate ;// bitrate from the first decoding


                        // Amount of file played
                           u16TimePlayed =  (framelength_aac * 1024 ) ;
	                 if ( aac_frequency != 0)
	                   u16TimePlayed /=  aac_frequency;

                   if (u16Noofframesread == 1)
	                  u32TotalTimePlayed += (u16totalTime*2) ; /* To account for the buffer already played */
                   else
                     u32TotalTimePlayed += (u16totalTime) ;


                           /* Total time to be played */
                        if (u16Noofframesread == 1)
                             u16totalTimeEst = ( u16totalTime * p_audio_gbl_var->audio_aac.audio_filesize )/ 4096 ;









		       #if 0
		       AUDIO_SEND_TRACE_PARAM("AAC-Total : u16totalTimeEst- \n", u16totalTimeEst , RV_TRACE_LEVEL_DEBUG_LOW);
		       AUDIO_SEND_TRACE_PARAM("AAC-Status : u32TotalTimePlayed - \n",u32TotalTimePlayed , RV_TRACE_LEVEL_DEBUG_LOW);

               #else

		     /* allocate the message buffer */
		      while (mb_status == RVF_RED)
		      {
		        mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
		                                 sizeof (T_AUDIO_AAC_STATUS_BAR),
		                                 (T_RVF_BUFFER **) (&p_send_message));

		        /* If insufficient resources, then report a memory error and abort.               */
		        /* and wait until more ressource is given */
		        if (mb_status == RVF_RED)
		        {
		          audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
		          rvf_delay(RVF_MS_TO_TICKS(1000));
		        }
		      }

		      /*fill the header of the message */
		      p_send_message->os_hdr.msg_id = AUDIO_AAC_STATUS_BAR;

		      /* fill the status parameters */
		      p_send_message->status        = status;
		      p_send_message->u32TotalTimePlayed  = u32TotalTimePlayed;
		      p_send_message->u16totalTimeEst  = u16totalTimeEst;



		      /* send message or call callback */
		      if (return_path.callback_func == NULL)
		        rvf_send_msg (return_path.addr_id, p_send_message);
		      else
		      {
		        (*return_path.callback_func)((void *)(p_send_message));
		         rvf_free_buf((T_RVF_BUFFER *)p_send_message);
		      }

            #endif

		  /* Reset the value of the variable in case of last buffer */
		  if ((  p_audio_gbl_var->audio_aac.audio_filesize  - p_audio_gbl_var->audio_aac.size_played) < 4096 )
		  {
		  	AUDIO_SEND_TRACE("AAC:- : End of file  - \n",  RV_TRACE_LEVEL_DEBUG_LOW);
		  		u32TotalTimePlayed = 0;
		  		u16Noofframesread = 0;
		  }



		    }






 #endif /* AAC */
#endif /* RVM_AUDIO_MAIN_SWE */

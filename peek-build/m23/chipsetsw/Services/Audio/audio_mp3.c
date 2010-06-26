/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_mp3.c                                                 */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            MP3 task.                                                     */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date          Modification                                              */
/*  ------------------------------------                                    */
/*  1 June 2004   Create                                                    */
/*                                                                          */
/*  Author                                                                  */
/*     Stephanie Levieil                                                    */
/*                                                                          */
/* (C) Copyright 2004 by Texas Instruments Incorporated, All Rights Reserved*/
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

#if (L1_MP3 == 1)
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_features_i.h"

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
  #include "l1audio_defty.h"

  #include "l1mp3_signa.h"
  #include "audio/audio_mp3_i.h"
  #include "audio/audio_api.h"


  #include "audio/audio_macro_i.h"

  #include "l1mp3_defty.h"
  #include "l1mp3_proto.h"
  #define MP3_NO_OF_SAMPLES 1152
  #define MP3_LAYRE1_FAMRE_CONST 12000
  #define MP3_LAYER2_3_FRAME_CONST 144000
  #define  MP3_LAYER1_2_NOT_SUPPORTED -11
extern WORD32 mp3_skip_time;//Mp3-FR - glb var set by audio_api.c- fr/rw request

static void audio_mp3_send_data_to_MCU(void);

UWORD32 mp3_forwardrewind_reset =0;//Mp3-FR-global- used for DSP codec reset
extern T_L1_BT_AUDIO bt_audio;
extern T_MP3_DMA_PARAM *mp3_dma;
extern UWORD8	d_mp3_dma_current_buffer_id;
// MP3 NDB API
extern T_MP3_MCU_DSP *mp3_ndb;//Mp3-FR

// UINT8 mp3_statusBar ();
void mp3_statusBar ( T_AUDIO_RET status,T_RV_RETURN return_path );
static INT32  check_mp3_header(UINT8 * buffer, INT32 Bytes);
static UINT32 corner_head = 0x00000000;
static UINT8  corner = 0;
static UINT32  u32TotalTimePlayed = 0;
static UINT16  u16NextreportingTime = 1;
static UINT16 u16Noofframesread=0;
INT32 failed_skip_time = 0;


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_message_switch	                              */
  /*                                                                              */
  /*    Purpose:  Manage the message                                              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        start or stop message from the mp3 features                           */
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
  UINT8 audio_mp3_message_switch (T_RV_HDR *p_message)
  {
    switch (p_message->msg_id)
    {
      case AUDIO_MP3_START_REQ:
      case AUDIO_MP3_STOP_REQ:
      case AUDIO_MP3_PAUSE_REQ:
      case AUDIO_MP3_RESUME_REQ:
      case AUDIO_MP3_INFO_REQ:
        return(AUDIO_MP3);

      /* driver init => check session_id is MP3 */
      case AUDIO_DRIVER_INIT_STATUS_MSG:
      {
        UINT8 session_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->session_id;

        /* session_id is MP3 */
        if ( (session_id == AUDIO_MP3_SESSION_ID)&&
             (p_audio_gbl_var->audio_mp3.state != AUDIO_IDLE))
        {
          return (AUDIO_MP3);
        }
        return(AUDIO_MP3_NONE);
      }

      case AUDIO_DRIVER_NOTIFICATION_MSG:
      {
        UWORD8 channel_id;
        channel_id = ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id;

        if( (channel_id == p_audio_gbl_var->audio_mp3.channel_id)
          &&(p_audio_gbl_var->audio_mp3.state > AUDIO_WAIT_CHANNEL_ID) )
        {
          return(AUDIO_MP3);
        }

        return(AUDIO_MP3_NONE);
      }

      case AUDIO_DRIVER_STATUS_MSG:
      case AUDIO_DRIVER_STATUS_INFO_MSG:
      {
        UWORD8 channel_id;
        channel_id = ((T_AUDIO_DRIVER_STATUS *)p_message)->channel_id;
        if( (channel_id == p_audio_gbl_var->audio_mp3.channel_id)
         && (p_audio_gbl_var->audio_mp3.state > AUDIO_WAIT_CHANNEL_ID) )
        {
          return(AUDIO_MP3);
        }
        return(AUDIO_MP3_NONE);
      }

      default:
        return(AUDIO_MP3_NONE);
    } /* switch */
  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_send_status                                    */
  /*                                                                              */
  /*    Purpose:  This function sends the MP3 status to the entity.               */
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
  void audio_mp3_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_MP3_STATUS *p_send_message;
    T_RVF_MB_STATUS    mb_status = RVF_RED;

    /* allocate the message buffer */
    while (mb_status == RVF_RED)
    {
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_MP3_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    /*fill the header of the message */
    p_send_message->os_hdr.msg_id = AUDIO_MP3_STATUS_MSG;

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
  /*    Function Name:   audio_mp3_send_info_status                               */
  /*                                                                              */
  /*    Purpose:  This function sends the MP3 Info status to the entity.          */
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
  void audio_mp3_send_info_status (T_AUDIO_RET status, T_L1A_MP3_INFO_CON info, T_RV_RETURN return_path)
  {
    T_AUDIO_MP3_INFO_STATUS *p_send_message;
    T_RVF_MB_STATUS    mb_status = RVF_RED;

    /* allocate the message buffer */
    while (mb_status == RVF_RED)
    {
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_MP3_INFO_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    /*fill the header of the message */
    p_send_message->os_hdr.msg_id = AUDIO_MP3_STATUS_INFO_MSG;

    /* fill the status parameters */
    p_send_message->status        = status;
    p_send_message->info.bitrate  = info.bitrate;
    p_send_message->info.channel  = info.channel;
    p_send_message->info.copyright= info.copyright;
    p_send_message->info.emphasis = info.emphasis;
    p_send_message->info.frequency= info.frequency;
    #ifdef _WINDOWS
      p_send_message->info.header   = info.header;
    #endif
    p_send_message->info.layer    = info.layer;
    p_send_message->info.mpeg_id  = info.mpeg_id;
    p_send_message->info.original = info.original;
    p_send_message->info.padding  = info.padding;
    p_send_message->info.private   = info.private;

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
  /*    Function Name:   audio_mp3_manager                                        */
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
  void audio_mp3_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    T_RV_RETURN my_return_path;
    T_L1A_MP3_INFO_CON info;

    /* added as a test */
    UINT8 percent;
    UINT8 string[50];


    /**************** audio_mp3_manager function begins ***********************/
    switch(p_audio_gbl_var->audio_mp3.state)
    {
      case AUDIO_IDLE:
      {
        switch(p_message->msg_id)
        {
          case AUDIO_MP3_START_REQ:
          {
            T_AUDIO_DRIVER_PARAMETER driver_parameter;

           /* save the return path + ffs_fd */
            p_audio_gbl_var->audio_mp3.return_path.callback_func =
              ((T_AUDIO_MP3_START *)p_message)->return_path.callback_func;
            p_audio_gbl_var->audio_mp3.return_path.addr_id   =
              ((T_AUDIO_MP3_START*)p_message)->return_path.addr_id;
            p_audio_gbl_var->audio_mp3.ffs_fd =
              ((T_AUDIO_MP3_START *)p_message)->audio_ffs_fd;
            p_audio_gbl_var->audio_mp3.size_file_start =
              ((T_AUDIO_MP3_START *)p_message)->mp3_parameter.size_file_start;
            p_audio_gbl_var->audio_mp3.filesize =
              ((T_AUDIO_MP3_START *)p_message)->audio_filesize;
	    p_audio_gbl_var->audio_mp3.play_bar_on =
			   ((T_AUDIO_MP3_START *)p_message)->play_bar_on;
		p_audio_gbl_var->audio_mp3.mp3_mono_stereo_mmi_command =
			    (BOOL)(((T_AUDIO_MP3_START *)p_message)->mp3_parameter.mono_stereo); /* 0-mono 1-stereo */

	  if(bt_audio.connected_status == TRUE)
	    p_audio_gbl_var->audio_mp3.start_req = TRUE;
            /* driver parameters */
            driver_parameter.nb_buffer   = AUDIO_MP3_NB_BUFFER;
            driver_parameter.buffer_size = AUDIO_MP3_SIZE;

            /* return_path for driver */
            my_return_path.callback_func = NULL;
            my_return_path.addr_id       = p_audio_gbl_var->addrId;

            /* Init driver */
            audio_driver_init_mp3_session(&driver_parameter, my_return_path);

	    u32TotalTimePlayed = 0;
            u16NextreportingTime = 1;

            p_audio_gbl_var->audio_mp3.state = AUDIO_WAIT_CHANNEL_ID;
          }
          break;
          case AUDIO_MP3_STOP_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_STOP_EVENT);
            /* do not send a status message because of pre-emption issues
               An automatic stop can pre-empt a stop request. A status is sent + back in state idle
               then the stop request is received and another status is sent, which can be misinterpreted */
          }
          break;
          case AUDIO_MP3_PAUSE_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_MP3_RESUME_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
          break;
          case AUDIO_MP3_INFO_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_INFO_EVENT);
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
            UINT8  *play_buffer, hc_once = 1,last_buffer = 0,id3v2check=0;
            INT16 size_read = 0;//OMAPS00090550
            INT32  indx;
            T_AUDIO_DRIVER_SESSION *p_session;

            corner_head = 0x00000000;
            corner = 0;

            mp3_skip_time =0;
            /* check init is successfull otherwise close file and send status AUDIO_ERROR */
            if (((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status == AUDIO_OK)
            {
              /* get channel id */
              p_audio_gbl_var->audio_mp3.channel_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              /* initializations */
              p_audio_gbl_var->audio_mp3.session_id       = AUDIO_MP3_SESSION_ID;
              p_audio_gbl_var->audio_mp3.size             = AUDIO_MP3_SIZE <<1 ;  // temporary RAM buffer size
              //p_audio_gbl_var->audio_mp3.size             = AUDIO_MP3_SIZE;
              p_audio_gbl_var->audio_mp3.size_left        = 0;
              p_audio_gbl_var->audio_mp3.stop_req_allowed = TRUE;
              p_audio_gbl_var->audio_mp3.pause_req_allowed = TRUE;
              p_audio_gbl_var->audio_mp3.last_stop_msg    = FALSE;
              p_audio_gbl_var->audio_mp3.last_pause_msg   = FALSE;
              p_audio_gbl_var->audio_mp3.last_resume_msg  = FALSE;
              p_audio_gbl_var->audio_mp3.last_info_msg    = FALSE;
              p_audio_gbl_var->audio_mp3.buff_notify_msg  = FALSE;


#if(AS_RFS_API == 1)

//seek to the position specified in the MP3 start API
             	if(rfs_lseek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            p_audio_gbl_var->audio_mp3.size_file_start,
                            RFS_SEEK_SET) < RFS_EOK )

#else
			  //seek to the position specified in the MP3 start API
             	if(ffs_seek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            p_audio_gbl_var->audio_mp3.size_file_start,
                            FFS_SEEK_SET) < EFFS_OK )
#endif
              {
                  AUDIO_SEND_TRACE("AUDIO MP3: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;
              }

              AUDIO_SEND_TRACE_PARAM("start size : ", p_audio_gbl_var->audio_mp3.size_file_start, RV_TRACE_LEVEL_ERROR);
			  AUDIO_SEND_TRACE_PARAM("RFS size : ", p_audio_gbl_var->audio_mp3.size, RV_TRACE_LEVEL_ERROR);
              p_audio_gbl_var->audio_mp3.size_played = p_audio_gbl_var->audio_mp3.size_file_start;
              /* fill + convert all buffers in advance */
              while(audio_driver_get_play_buffer(p_audio_gbl_var->audio_mp3.channel_id, &play_buffer) == AUDIO_OK)
              {
#if(AS_RFS_API == 1)

				// write from FLASH to RAM buffer
                if(p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
                {
                  size_read = (INT16)rfs_read( p_audio_gbl_var->audio_mp3.ffs_fd,
                                        play_buffer,
                                        p_audio_gbl_var->audio_mp3.size);
				  AUDIO_SEND_TRACE_PARAM("RFS size : ", p_audio_gbl_var->audio_mp3.size, RV_TRACE_LEVEL_ERROR);
                  if(size_read < RFS_EOK)
                  {
                    AUDIO_SEND_TRACE("AUDIO MP3: FFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
                    size_read = 0;  // will fill buffer with END_MASK and stop task
                  }
                }

#else
                // write from FLASH to RAM buffer
                if(p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
                {
                  size_read = ffs_read( p_audio_gbl_var->audio_mp3.ffs_fd,
                                        play_buffer,
                                        p_audio_gbl_var->audio_mp3.size);
                  if(size_read < EFFS_OK)
                  {
                    AUDIO_SEND_TRACE("AUDIO MP3: FFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
                    size_read = 0;  // will fill buffer with END_MASK and stop task
                  }
                }
#endif
				//ID3v2 check starts here.

			if(id3v2check==0)
			{

				    UINT32 id3;
				    UINT32 temp,sec;
				    UINT32 size;
                    id3v2check=1;
					//AUDIO_SEND_TRACE_PARAM("id3v2check: ",id3v2check, RV_TRACE_LEVEL_ERROR);
					//Id3v2 check
				     id3=play_buffer[0];
				     id3=id3<<16;
				     sec=play_buffer[1];
				     temp=sec<<8;
				     id3|=temp;
				     id3|=play_buffer[2];

				if(id3==0x494433)
				{
				     AUDIO_SEND_TRACE_PARAM("ID3V2 Found: ",id3, RV_TRACE_LEVEL_ERROR);

				     size=play_buffer[6];
					 size=size<<21;
					 temp=play_buffer[7];
					 temp=temp<<14;
					 size|=temp;
					 temp=play_buffer[8];
					 temp=temp<<7;
					 size|=temp;
					 size|=play_buffer[9];//got the size
					 //Now Change the file start variable to point to the end of ID3V2
					 p_audio_gbl_var->audio_mp3.size_file_start=size;
					 AUDIO_SEND_TRACE_PARAM("size of ID3 ",p_audio_gbl_var->audio_mp3.size_file_start, RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)

					 //seek to the position specified in the MP3 start variable
						if(rfs_lseek ( p_audio_gbl_var->audio_mp3.ffs_fd,
												 p_audio_gbl_var->audio_mp3.size_file_start,
												 RFS_SEEK_SET) < RFS_EOK )

#else
								   //seek to the position specified in the MP3 start API
						if(ffs_seek ( p_audio_gbl_var->audio_mp3.ffs_fd,
												 p_audio_gbl_var->audio_mp3.size_file_start,
												 FFS_SEEK_SET) < EFFS_OK )
#endif
					     {
                               AUDIO_SEND_TRACE("AUDIO MP3: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                                     size_read = 0;
                         }

					    audio_driver_get_play_buffer(p_audio_gbl_var->audio_mp3.channel_id, &play_buffer);

#if(AS_RFS_API == 1)

									 // write from FLASH to RAM buffer
						 if(p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
							{
							  size_read = (INT16)rfs_read( p_audio_gbl_var->audio_mp3.ffs_fd,
															 play_buffer,
															 p_audio_gbl_var->audio_mp3.size);
									  // AUDIO_SEND_TRACE_PARAM("RFS size : ", p_audio_gbl_var->audio_mp3.size, RV_TRACE_LEVEL_ERROR);

								if(size_read < RFS_EOK)
								 {
								   AUDIO_SEND_TRACE("AUDIO MP3: FFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
										 size_read = 0;  // will fill buffer with END_MASK and stop task
								  }
							 }

#else
									 // write from FLASH to RAM buffer
						   if(p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
							  {
								 size_read = ffs_read( p_audio_gbl_var->audio_mp3.ffs_fd,
															 play_buffer,
															 p_audio_gbl_var->audio_mp3.size);

                  if(size_read < EFFS_OK)
                  {
                    AUDIO_SEND_TRACE("AUDIO MP3: FFS PLAY READ failed at init", RV_TRACE_LEVEL_ERROR);
                    size_read = 0;  // will fill buffer with END_MASK and stop task
                  }
                }
#endif

				}

			}
                // MP3 Heaader Consistency Check
                while(hc_once)
                {
                  indx = check_mp3_header(play_buffer, size_read);
                   if (indx == MP3_LAYER1_2_NOT_SUPPORTED)
				    {
					  AUDIO_SEND_TRACE_PARAM("MP3 Layer 1 & Layer 2 Not Supported Invalid Mp3", size_read, RV_TRACE_LEVEL_DEBUG_LOW);
					  audio_mp3_send_status (AUDIO_MP3_NOT_SUPP, p_audio_gbl_var->audio_mp3.return_path);
					  audio_mp3_error_trace(AUDIO_ERROR_START_EVENT);
					  size_read = 0;  // will put END_MASK in whole buffer so stops play
					  AUDIO_SEND_TRACE_PARAM(" check for traces here",size_read, RV_TRACE_LEVEL_ERROR);
                      hc_once = 0;
				    }  	     		 	
							
                  // Once Last buffer is reached exit from Header consistency check
                  if(last_buffer == 1) hc_once = 0;

                  if(indx != -1)
                  {
                    int i;
                    for(i = 0; i < indx; i++)
                      *(play_buffer + i) = 0x00;

                    hc_once = 0;
                  }
                  else
                  {
                    AUDIO_SEND_TRACE("Header Consistency Miss", RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)
					if(p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
#else
					if(p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
#endif
                    {
                      AUDIO_SEND_TRACE_PARAM("Corner : ", *(play_buffer + (size_read - 4)), RV_TRACE_LEVEL_DEBUG_LOW);
#if(AS_RFS_API == 1)
					  if(rfs_lseek ( p_audio_gbl_var->audio_mp3.ffs_fd, -3, RFS_SEEK_CUR) < RFS_EOK )
#else
					  if(ffs_seek ( p_audio_gbl_var->audio_mp3.ffs_fd, -3, FFS_SEEK_CUR) < EFFS_OK )
#endif
                      {
                        AUDIO_SEND_TRACE("AUDIO MP3: FFS SEEK Corner - Error", RV_TRACE_LEVEL_ERROR);
                        size_read = 0;
                      }

#if(AS_RFS_API == 1)
					  size_read = (INT16)rfs_read(p_audio_gbl_var->audio_mp3.ffs_fd,
                                              play_buffer,
                                              p_audio_gbl_var->audio_mp3.size);
#else
					  size_read = ffs_read(p_audio_gbl_var->audio_mp3.ffs_fd,
                                              play_buffer,
                                              p_audio_gbl_var->audio_mp3.size);
#endif
                      AUDIO_SEND_TRACE_PARAM("After Miss: ", *(play_buffer + 0), RV_TRACE_LEVEL_DEBUG_LOW);
                      AUDIO_SEND_TRACE_PARAM("Size Read11 ", size_read, RV_TRACE_LEVEL_DEBUG_LOW);

               	      AUDIO_SEND_TRACE("MP3 Corrupted File Received So Pls Stop It.....!", RV_TRACE_LEVEL_ERROR);
                      // Check for Last buffer
                      if (size_read < p_audio_gbl_var->audio_mp3.size)
                      {
                        last_buffer = 1;
                        AUDIO_SEND_TRACE_PARAM("Last Buffer ", size_read, RV_TRACE_LEVEL_DEBUG_LOW);
                      }


                     if(size_read <= RFS_EOK)
                      {
                         AUDIO_SEND_TRACE("MP3-FR: FFS PLAY READ FAILED........!", RV_TRACE_LEVEL_ERROR);
                         size_read = 0;  // will put END_MASK in whole buffer so stops play
                         hc_once = 0;
                      }

                      if(size_read <= EFFS_OK)
                      {
                         AUDIO_SEND_TRACE("MP3-FR: FFS PLAY READ FAILED........!", RV_TRACE_LEVEL_ERROR);
                         size_read = 0;  // will put END_MASK in whole buffer so stops play
                         hc_once = 0;
                      }
                    }
                  } // end else
                } // end while

                /* nb of bytes read in the MP3 file */
                p_audio_gbl_var->audio_mp3.size_played += size_read;

                /* last buffer already, call flush buffer */
                if((UINT16)size_read < p_audio_gbl_var->audio_mp3.size)
                {
                  UINT16 i;

                  for(i = size_read; i < p_audio_gbl_var->audio_mp3.size; i++)
                    *(play_buffer + i) = 0x00;

                  AUDIO_SEND_TRACE_PARAM("AUDIO MP3: FFS to RAM size", size_read,
                                           RV_TRACE_LEVEL_DEBUG_LOW);

                  audio_driver_flush_buffer(p_audio_gbl_var->audio_mp3.channel_id, play_buffer);
                  p_audio_gbl_var->audio_mp3.size_played = 0;
                }
                else
                {
                  AUDIO_SEND_TRACE_PARAM("AUDIO MP3: FFS to RAM size", size_read,
                                         RV_TRACE_LEVEL_DEBUG_LOW);
                  audio_driver_play_buffer(p_audio_gbl_var->audio_mp3.channel_id, play_buffer);
                }

                /* save the read size */
                p_session = &(p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_mp3.channel_id]);
                p_session->session_info.buffer_size = size_read;
              }

              /* initialize the return path */
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              /* send message */
              audio_driver_start_session(p_audio_gbl_var->audio_mp3.channel_id, my_return_path);

              /* change state */
              p_audio_gbl_var->audio_mp3.state = AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION;
            } // if (((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->status == AUDIO_OK)
            else
            {
              audio_mp3_error_trace(AUDIO_ERROR_START_EVENT);
#if(AS_RFS_API == 1)

			if (p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
              {
                if(rfs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != RFS_EOK)
                  audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              }

#else
              if (p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
              {
                if(ffs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != EFFS_OK)
                  audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              }
#endif

              audio_mp3_send_status (AUDIO_ERROR, p_audio_gbl_var->audio_mp3.return_path);
              /* change state */
              p_audio_gbl_var->audio_mp3.state = AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_MP3_STOP_REQ:
            /* change state */
            p_audio_gbl_var->audio_mp3.state = AUDIO_WAIT_CHANNEL_ID_TO_STOP;
          break;
          case AUDIO_MP3_PAUSE_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_MP3_RESUME_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }
          break;
          case AUDIO_MP3_INFO_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_INFO_EVENT);
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
           audio_mp3_send_data_to_MCU();
          }
          break;  // case AUDIO_DRIVER_NOTIFICATION_MSG

          case AUDIO_MP3_STOP_REQ:
          {
            if(p_audio_gbl_var->audio_mp3.stop_req_allowed == TRUE)
            {
              AUDIO_SEND_TRACE("AUDIO MP3: STOP Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_mp3.stop_req_allowed = FALSE;
              p_audio_gbl_var->audio_mp3.last_stop_msg = TRUE;
              audio_driver_stop_session(p_audio_gbl_var->audio_mp3.channel_id);
            }
            else
              /* do not send a status message because of pre-emption issues */
              audio_mp3_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;
          case AUDIO_MP3_PAUSE_REQ:
          {
            if(p_audio_gbl_var->audio_mp3.pause_req_allowed == TRUE)
            {
              AUDIO_SEND_TRACE("AUDIO MP3: PAUSE Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_mp3.pause_req_allowed = FALSE;
              p_audio_gbl_var->audio_mp3.last_pause_msg = TRUE;
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              audio_driver_pause_session(p_audio_gbl_var->audio_mp3.channel_id, my_return_path);
            }
            else
              /* do not send a status message because of pre-emption issues */
              audio_mp3_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;
          case AUDIO_MP3_INFO_REQ:
          {
              AUDIO_SEND_TRACE("AUDIO MP3: INFO Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_mp3.last_info_msg = TRUE;
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

              audio_driver_info_session(p_audio_gbl_var->audio_mp3.channel_id, my_return_path);
          }
          break;

          case AUDIO_DRIVER_STATUS_MSG:
          {

			  T_AUDIO_DRIVER_SESSION *p_session;
			  p_session = &(p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_mp3.channel_id]);
  
		
		if(p_audio_gbl_var->audio_mp3.start_req == TRUE)
            	{
             		 p_audio_gbl_var->audio_mp3.start_req = FALSE;
				   if(bt_audio.connected_status == TRUE)
                     audio_mp3_send_status(AUDIO_OK , p_audio_gbl_var->audio_mp3.return_path);
               }   
 		else if((p_audio_gbl_var->audio_mp3.last_pause_msg == TRUE) && (p_session->session_info.state != AUDIO_DRIVER_CHANNEL_WAIT_INIT) )
            {
              p_audio_gbl_var->audio_mp3.state = AUDIO_WAIT_STOP_OR_RESUME;
              p_audio_gbl_var->audio_mp3.last_pause_msg = FALSE;
		#if(L1_BT_AUDIO==1)
		as_send_pause_ind(AUDIO_OK,FALSE, &as_ctrl_blk->client_rp);
		#endif
            }
            else
            {

#if(AS_RFS_API == 1)
				if(p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
                {
							if((bt_audio.connected_status == TRUE) )
							  {
							bt_audio.pcm_data_end=1;
				//				   return L1_PCM_MEDIA_ENDED;
				
						AUDIO_SEND_TRACE("AUDIO MP3:2 error nand file", RV_TRACE_LEVEL_DEBUG_LOW);
							  l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
						l1mp3_apihisr_reset_buffer((UWORD16 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
				
						  }
                  AUDIO_SEND_TRACE("AUDIO MP3: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
                  if(rfs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != RFS_EOK)
                    audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }

#else
               if(p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
                {
                  AUDIO_SEND_TRACE("AUDIO MP3: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
                  if(ffs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != EFFS_OK)
                    audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                }
#endif
                if(((T_AUDIO_DRIVER_STATUS *)p_message)->status != AUDIO_OK)
                  audio_mp3_send_status(((T_AUDIO_DRIVER_STATUS *)p_message)->status, p_audio_gbl_var->audio_mp3.return_path);
                else
                  audio_mp3_send_status(AUDIO_OK, p_audio_gbl_var->audio_mp3.return_path);
                p_audio_gbl_var->audio_mp3.state = AUDIO_IDLE;
            }
          }
          break;

          case AUDIO_DRIVER_STATUS_INFO_MSG:
          {
             if(p_audio_gbl_var->audio_mp3.last_info_msg == TRUE)
              {
                info.bitrate  = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.bitrate;
                info.channel  = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.channel;
                info.copyright= ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.copyright;
                info.emphasis = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.emphasis;
                info.frequency= ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.frequency;
                info.layer    = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.layer;
                info.mpeg_id  = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.mpeg_id;
                info.original = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.original;
                info.padding  = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.padding;
                info.private  = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.private;
                #ifdef _WINDOWS
                  info.header.callback_func = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.header.callback_func;
                  info.header.dest_addr_id = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.header.dest_addr_id;
                  info.header.msg_id = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.header.msg_id;
                  info.header.src_addr_id = ((T_AUDIO_DRIVER_INFO_STATUS *)p_message)->info.header.src_addr_id;
                #endif

                p_audio_gbl_var->audio_mp3.state = AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION;
                p_audio_gbl_var->audio_mp3.last_info_msg = FALSE;
                audio_mp3_send_info_status(AUDIO_OK,info,p_audio_gbl_var->audio_mp3.return_path);
              }
             else
              /* do not send a status message because of pre-emption issues */
              audio_mp3_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;

          case AUDIO_MP3_RESUME_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_RESUME_EVENT);
          }

          break;
        } // switch (p_message->msg_id)


      } // case AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION:
      break;

      case AUDIO_WAIT_STOP_OR_RESUME:
      {
        switch (p_message->msg_id)
        {
          case AUDIO_MP3_STOP_REQ:
          {
            if(p_audio_gbl_var->audio_mp3.stop_req_allowed == TRUE)
            {
              AUDIO_SEND_TRACE("AUDIO MP3: STOP Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
              p_audio_gbl_var->audio_mp3.stop_req_allowed = FALSE;
              p_audio_gbl_var->audio_mp3.last_stop_msg = TRUE;
              audio_driver_stop_session(p_audio_gbl_var->audio_mp3.channel_id);
            }
            else
              /* do not send a status message because of pre-emption issues */
              audio_mp3_error_trace(AUDIO_ERROR_STOP_EVENT);
          }
          break;

          /* MP3 pause and resume Crash Fix */
          case AUDIO_DRIVER_NOTIFICATION_MSG:
          {
		   AUDIO_SEND_TRACE("NOTIFICATION_MSG recvd in STOP_OR_RESUME_OR_RESTART", RV_TRACE_LEVEL_DEBUG_LOW);
           audio_mp3_send_data_to_MCU();
          }
          break;

          case AUDIO_MP3_RESUME_REQ:
          {
             AUDIO_SEND_TRACE("AUDIO MP3: RESUME Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
             p_audio_gbl_var->audio_mp3.last_resume_msg   = TRUE;
             p_audio_gbl_var->audio_mp3.pause_req_allowed = TRUE;
              my_return_path.callback_func = NULL;
              my_return_path.addr_id       = p_audio_gbl_var->addrId;

             audio_driver_resume_session(p_audio_gbl_var->audio_mp3.channel_id,my_return_path);
          }
          break;
          case AUDIO_DRIVER_STATUS_MSG:
          {
            /* We receive the Audio_DRIVER_STATUS_MSG corresponding to the RESUME */
            if(p_audio_gbl_var->audio_mp3.last_resume_msg == TRUE)
            {
              p_audio_gbl_var->audio_mp3.state = AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION;
              p_audio_gbl_var->audio_mp3.last_resume_msg = FALSE;
            }
            else
            {
#if(AS_RFS_API == 1)
				if(p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
              {
								  if((bt_audio.connected_status == TRUE) )
									{
								  bt_audio.pcm_data_end=1;
					  //				 return L1_PCM_MEDIA_ENDED;
					  
							  AUDIO_SEND_TRACE("AUDIO MP3:3 error nand file", RV_TRACE_LEVEL_DEBUG_LOW);
									l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
							  l1mp3_apihisr_reset_buffer((UWORD16 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
					  
								}
                AUDIO_SEND_TRACE("AUDIO MP3: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
                if(rfs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != RFS_EOK)
                  audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              }

#else
              if(p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
              {
                AUDIO_SEND_TRACE("AUDIO MP3: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
                if(ffs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != EFFS_OK)
                  audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              }
#endif
              if(bt_audio.connected_status == TRUE)
              	{
                if(((T_AUDIO_DRIVER_STATUS *)p_message)->status != AUDIO_OK)
                  audio_mp3_send_status(((T_AUDIO_DRIVER_STATUS *)p_message)->status, p_audio_gbl_var->audio_mp3.return_path);
              	}
                else
              audio_mp3_send_status(AUDIO_OK, p_audio_gbl_var->audio_mp3.return_path);
                p_audio_gbl_var->audio_mp3.state = AUDIO_IDLE;
              p_audio_gbl_var->audio_mp3.last_stop_msg = FALSE;
            }
          }
          break;
          case AUDIO_MP3_INFO_REQ:
          {
             AUDIO_SEND_TRACE("AUDIO MP3: INFO Request received ", RV_TRACE_LEVEL_DEBUG_LOW);
             p_audio_gbl_var->audio_mp3.last_info_msg   = TRUE;
             my_return_path.callback_func = NULL;
             my_return_path.addr_id       = p_audio_gbl_var->addrId;

             audio_driver_info_session(p_audio_gbl_var->audio_mp3.channel_id, my_return_path);
          }
          break;
          case AUDIO_MP3_PAUSE_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          }
          break;
          case AUDIO_MP3_START_REQ:
          {
            audio_mp3_error_trace(AUDIO_ERROR_START_EVENT);
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
              p_audio_gbl_var->audio_mp3.channel_id = ((T_AUDIO_DRIVER_INIT_STATUS *)p_message)->channel_id;

              audio_driver_stop_session(p_audio_gbl_var->audio_mp3.channel_id);

              /* change state */
              p_audio_gbl_var->audio_mp3.state = AUDIO_WAIT_DRIVER_STOP_CON;
            }
            else
            {
#if(AS_RFS_API == 1)
				/* close file */
              if (p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
              {
                if ( rfs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != RFS_EOK )
                  audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                AUDIO_SEND_TRACE("AUDIO MP3: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }

#else
              /* close file */
              if (p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
              {
                if ( ffs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != EFFS_OK )
                  audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                AUDIO_SEND_TRACE("AUDIO MP3: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }
#endif

              audio_mp3_send_status (AUDIO_OK, p_audio_gbl_var->audio_mp3.return_path);

              /* change state */
              p_audio_gbl_var->audio_mp3.state = AUDIO_IDLE;
            }
          }
          break;
          case AUDIO_MP3_STOP_REQ:
            audio_mp3_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
          case AUDIO_MP3_PAUSE_REQ:
            audio_mp3_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          break;
          case AUDIO_MP3_RESUME_REQ:
            audio_mp3_error_trace(AUDIO_ERROR_RESUME_EVENT);
          break;
          case AUDIO_MP3_INFO_REQ:
            audio_mp3_error_trace(AUDIO_ERROR_INFO_EVENT);
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
            if (p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
              {
								  if((bt_audio.connected_status == TRUE) )
									{
								  bt_audio.pcm_data_end=1;
					  //				 return L1_PCM_MEDIA_ENDED;
					  
							  AUDIO_SEND_TRACE("AUDIO MP31: error nand file", RV_TRACE_LEVEL_DEBUG_LOW);
									l1_audio_manager((UWORD8 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
							  l1mp3_apihisr_reset_buffer((UWORD16 *)&mp3_dma->a_mp3_dma_input_buffer[d_mp3_dma_current_buffer_id][0],C_MP3_OUTPUT_BUFFER_SIZE);
					  
								}
                if ( rfs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != RFS_EOK )
                audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              AUDIO_SEND_TRACE("AUDIO MP3: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }

#else
              /* close file */
            if (p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
              {
                if ( ffs_close(p_audio_gbl_var->audio_mp3.ffs_fd) != EFFS_OK )
                audio_mp3_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
              AUDIO_SEND_TRACE("AUDIO MP3: close FFS file", RV_TRACE_LEVEL_DEBUG_LOW);
              }
#endif


            audio_mp3_send_status (AUDIO_OK, p_audio_gbl_var->audio_mp3.return_path);
              p_audio_gbl_var->audio_mp3.state = AUDIO_IDLE;
            }
          break;
          case AUDIO_MP3_STOP_REQ:
            audio_mp3_error_trace(AUDIO_ERROR_STOP_EVENT);
          break;
          case AUDIO_MP3_PAUSE_REQ:
            audio_mp3_error_trace(AUDIO_ERROR_PAUSE_EVENT);
          break;
          case AUDIO_MP3_RESUME_REQ:
            audio_mp3_error_trace(AUDIO_ERROR_RESUME_EVENT);
          break;
          case AUDIO_MP3_INFO_REQ:
            audio_mp3_error_trace(AUDIO_ERROR_INFO_EVENT);
          break;

        }
      } //case AUDIO_WAIT_DRIVER_STOP_CON:
      break;
    } // switch(p_audio_gbl_var->audio_mp3.state)
  } /*********************** End of audio_mp3_manager function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_mp3_manager                                 */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a MP3 manager                 */
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
  T_AUDIO_RET audio_driver_mp3_manager (T_RV_HDR *p_message, T_AUDIO_DRIVER_SESSION *p_session)
  {
    /**************** audio_driver_mp3_manager function begins ***********************/
    switch(p_session->session_info.state)
    {
      case AUDIO_DRIVER_CHANNEL_WAIT_INIT:
      {
        T_MMI_MP3_REQ *l1_message;
        /* init buffer index, layer1 not valid until 1st buffer is filled */
        /* index_l1 will be set to 0 when get_play_buffer() is called in WAIT_START state */
        p_session->session_info.index_l1    = 0xFF;
        p_session->session_info.index_appli = 0;
        p_audio_gbl_var->audio_mp3.index_last_buffer = 0xFF;

        p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;

        /* allocate the buffer for the message to the L1 */
        l1_message                     = (T_MMI_MP3_REQ *) audio_allocate_l1_message(sizeof(T_MMI_MP3_REQ));
        l1_message->session_id         = AUDIO_MP3_SESSION_ID;
        l1_message->loopback           = 0;
        l1_message->stereo             = p_audio_gbl_var->audio_mp3.mp3_mono_stereo_mmi_command;
        l1_message->dma_channel_number = AUDIO_MP3_DMA_CHANNEL;

        p_session->session_req.p_l1_send_message = (DummyStruct *)l1_message;





        if(p_session->session_req.p_l1_send_message!=NULL)
          return(AUDIO_OK);
        else
          return(AUDIO_ERROR);

      }

      case AUDIO_DRIVER_CHANNEL_WAIT_START:
      {
        /* send the start MP3 message to the L1 */
        audio_send_l1_message(MMI_MP3_START_REQ,
                              p_session->session_req.p_l1_send_message);
        return (AUDIO_OK);
      }

      case AUDIO_DRIVER_CHANNEL_WAIT_STOP:
      {
        /* send the stop command to the audio L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if ( p_send_message != NULL)
        {
          if (p_audio_gbl_var->audio_mp3.last_stop_msg == TRUE)
          {
            /* send the stop command to the audio L1 */
            audio_send_l1_message(MMI_MP3_STOP_REQ, p_send_message);
          }
          else
          {
            if(p_audio_gbl_var->audio_mp3.last_pause_msg == TRUE)
            {
              /* send the pause command to the audio L1 */
              audio_send_l1_message(MMI_MP3_PAUSE_REQ, p_send_message);
            }
            else
            {
              if(p_audio_gbl_var->audio_mp3.last_resume_msg == TRUE)
              {
                /* send the resume command to the audio L1 */
                audio_send_l1_message(MMI_MP3_RESUME_REQ, p_send_message);
              }
              else
              {
                if(p_audio_gbl_var->audio_mp3.last_info_msg == TRUE)
                {
                  /* send the info command to the audio L1 */
                  audio_send_l1_message(MMI_MP3_INFO_REQ, p_send_message);
                }
              }
            }
          }
          return (AUDIO_OK);
        }
        return (AUDIO_ERROR);
      }
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_STOP:
      {
        /* send the stop command to the audio L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if ( p_send_message != NULL)
        {
          /* send the stop command to the audio L1 */
          audio_send_l1_message(MMI_MP3_STOP_REQ, p_send_message);
          return (AUDIO_OK);
        }
        return (AUDIO_ERROR);
      }
      case AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_PAUSE:
      {
        /* send the pause command to the audio L1 */
        DummyStruct *p_send_message = audio_allocate_l1_message(0);
        if ( p_send_message != NULL)
        {
          /* send the pause command to the audio L1 */
          audio_send_l1_message(MMI_MP3_PAUSE_REQ, p_send_message);
          return (AUDIO_OK);
        }
        return (AUDIO_ERROR);
      }
    }
	// no corresponding message found?
	return (AUDIO_ERROR);
  } /*********************** End of audio_mp3_manager function **********************/


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_l1_simulator                                   */
  /*                                                                              */
  /*    Purpose:  This function simulates the L1 for MP3                          */
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
  void audio_mp3_l1_simulator(UINT16 event, T_RV_HDR *p_message)
  {
#ifdef _WINDOWS
    enum { WAIT_START_REQ, WAIT_STOP };

    T_RVF_MB_STATUS mb_status;
    T_RV_RETURN     *return_path=&(p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_mp3.channel_id].session_req.return_path);
    T_L1A_MP3_INFO_CON info;
    UINT8           status;
    UINT16          size;

    switch(p_audio_gbl_var->audio_mp3.l1_state)
    {
      case WAIT_START_REQ:
        if(p_message->msg_id==MMI_MP3_START_REQ)
        {
          rvf_start_timer(AUDIO_MP3_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_MP3_L1_SIMUL_ONE_SHOT_TIMER);
          p_audio_gbl_var->audio_mp3.counter=10;

          /* send MMI_MP3_START_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_MP3_START_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_mp3.channel_id;
          //((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }

          p_audio_gbl_var->audio_mp3.l1_state=WAIT_STOP;
          return;
        }
      break;

      case WAIT_STOP:
        if(event & AUDIO_MP3_L1_SIMUL_TIMER_EVT_MASK)
        {
          T_AUDIO_DRIVER_SESSION *p_session;
          UINT16 *p_buffer;

          p_session = &p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_mp3.channel_id];

          p_buffer = (UINT8 *)p_session->session_info.buffer[p_session->session_info.index_l1].p_start_pointer;

          p_audio_gbl_var->audio_mp3.counter--;

          status = Cust_get_pointer_next_buffer(&p_buffer, &size,AUDIO_MP3_SESSION_ID);
          Cust_get_pointer_notify(AUDIO_MP3_SESSION_ID);

          /* switch buffer */
//          {
//            T_AUDIO_DRIVER_SESSION *p=&p_audio_gbl_var->audio_driver_session[p_audio_gbl_var->audio_mp3.channel_id];
//            p->session_info.index_l1++;
//            if(p->session_info.index_l1==p->session_req.nb_buffer) p->session_info.index_l1=0;
//          }

          /* send notification message to the Riviera audio entity */
//          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
//                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
//                                (T_RVF_BUFFER **)(&p_message));
//          if(mb_status==RVF_RED)
//          {
//            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
//            return;
//          }
//          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=AUDIO_DRIVER_NOTIFICATION_MSG;
//          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_mp3.channel_id;
//          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
//          if(return_path->callback_func==NULL)
//            rvf_send_msg(return_path->addr_id, p_message);
//          else
//          {
//            (*return_path->callback_func)((void *)(p_message));
//            rvf_free_buf((T_RVF_BUFFER *)p_message);
//          }

          /* check if we're done with the simulation */
          if(p_audio_gbl_var->audio_mp3.counter==0)
          {
            rvf_stop_timer(AUDIO_MP3_L1_SIMUL_TIMER);

            /* send MMI_MP3_STOP_CON message to the Riviera audio entity */
            mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                  sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                  (T_RVF_BUFFER **)(&p_message));
            if(mb_status==RVF_RED)
            {
              AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
              return;
            }
            ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_MP3_STOP_CON;
            ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_mp3.channel_id;
            ((T_L1A_MP3_CON *)p_message)->error_code = AUDIO_OK;
            //((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
            if(return_path->callback_func==NULL)
              rvf_send_msg(return_path->addr_id, p_message);
            else
            {
              (*return_path->callback_func)((void *)(p_message));
              rvf_free_buf((T_RVF_BUFFER *)p_message);
            }

            p_audio_gbl_var->audio_mp3.l1_state=WAIT_START_REQ;
            return;
          }
          rvf_start_timer(AUDIO_MP3_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_MP3_L1_SIMUL_ONE_SHOT_TIMER);
        }
        if(p_message->msg_id==MMI_MP3_STOP_REQ)
        {
          rvf_stop_timer(AUDIO_MP3_L1_SIMUL_TIMER);

          /* send MMI_MP3_STOP_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_MP3_STOP_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_mp3.channel_id;
          //((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }

          p_audio_gbl_var->audio_mp3.l1_state=WAIT_START_REQ;
          return;
        }

        if(p_message->msg_id==MMI_MP3_PAUSE_REQ)
        {
          rvf_stop_timer(AUDIO_MP3_L1_SIMUL_TIMER);

          /* send MMI_MP3_PAUSE_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_MP3_PAUSE_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_mp3.channel_id;
          //((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }
          p_audio_gbl_var->audio_mp3.l1_state=WAIT_STOP;
          return;
        }

        if(p_message->msg_id==MMI_MP3_RESUME_REQ)
        {
          rvf_start_timer(AUDIO_MP3_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_MP3_L1_SIMUL_ONE_SHOT_TIMER);

          /* send MMI_MP3_RESUME_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                sizeof(T_AUDIO_DRIVER_NOTIFICATION),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_MP3_RESUME_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_mp3.channel_id;
          //((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }
          p_audio_gbl_var->audio_mp3.l1_state=WAIT_STOP;
          return;
        }

        if(p_message->msg_id==MMI_MP3_INFO_REQ)
        {
          rvf_start_timer(AUDIO_MP3_L1_SIMUL_TIMER,
                          RVF_MS_TO_TICKS(1000),
                          AUDIO_MP3_L1_SIMUL_ONE_SHOT_TIMER);

          /* send MMI_MP3_INFO_CON message to the Riviera audio entity */
          mb_status=rvf_get_buf(p_audio_gbl_var->mb_internal,
                                (sizeof(T_AUDIO_DRIVER_NOTIFICATION)+sizeof(T_L1A_MP3_INFO_CON)),
                                (T_RVF_BUFFER **)(&p_message));
          if(mb_status==RVF_RED)
          {
            AUDIO_SEND_TRACE("AUDIO entity has no memory for driver notification",RV_TRACE_LEVEL_ERROR);
            return;
          }
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->header.msg_id=MMI_MP3_INFO_CON;
          ((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->channel_id=p_audio_gbl_var->audio_mp3.channel_id;
          //((T_AUDIO_DRIVER_NOTIFICATION *)p_message)->p_buffer=NULL;
          ((T_L1A_MP3_INFO_CON *)p_message)->bitrate  = 0;
          ((T_L1A_MP3_INFO_CON *)p_message)->channel  = 1;
          ((T_L1A_MP3_INFO_CON *)p_message)->copyright= TRUE;
          ((T_L1A_MP3_INFO_CON *)p_message)->emphasis = 2;
          ((T_L1A_MP3_INFO_CON *)p_message)->frequency= 3;
          ((T_L1A_MP3_INFO_CON *)p_message)->layer    = 4;
          ((T_L1A_MP3_INFO_CON *)p_message)->mpeg_id  = 5;
          ((T_L1A_MP3_INFO_CON *)p_message)->original = TRUE;
          ((T_L1A_MP3_INFO_CON *)p_message)->padding  = TRUE ;
          ((T_L1A_MP3_INFO_CON *)p_message)->private  = 6;
          ((T_L1A_MP3_INFO_CON *)p_message)->header.callback_func = 0x00;
          ((T_L1A_MP3_INFO_CON *)p_message)->header.dest_addr_id = 0x04;
          ((T_L1A_MP3_INFO_CON *)p_message)->header.msg_id = MMI_MP3_INFO_CON;
          ((T_L1A_MP3_INFO_CON *)p_message)->header.src_addr_id = 0x00;


          if(return_path->callback_func==NULL)
            rvf_send_msg(return_path->addr_id, p_message);
          else
          {
            (*return_path->callback_func)((void *)(p_message));
            rvf_free_buf((T_RVF_BUFFER *)p_message);
          }
          p_audio_gbl_var->audio_mp3.l1_state=WAIT_STOP;
          return;
        }
    }
#endif /* _WINDOWS */
  }

// MP3 Header consistency Check

//bitrates for mpeg-2/1/2.5
const int BITRATE_INDEX[3][16] = {
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160, 128},
	{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320, 128},
	{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160, 128}
};

//sampling frequecies for mpeg-2/1/2.5
const long S_FREQ_INDEX[3][4] = {
	{22050, 24000, 16000, 0},
	{44100, 48000, 32000, 0},
	{11025, 12000, 8000, 0}
};

const unsigned long HEADERMASK = 0xfffe0c00;

INT32 check_mp3_header(UINT8 * buffer, INT32 Bytes)
{
  UINT32 i, c1, c2, ver_id;
  UINT32 tmp;
  INT32  bit_rate, sam_freq, bit_rate1, sam_freq1;
  INT32 nSlots;
  INT32 found=0;
  INT32 skipData=0;

  UINT32 header1, header2, indx = 0, f_indx = 0;

  i=0;

  while(1)
  {
    if(Bytes <= (indx + 3))
    {
      AUDIO_SEND_TRACE_PARAM("EOB ", indx,  RV_TRACE_LEVEL_ERROR);
      return -1;
    }

    tmp = buffer[indx++];
    tmp = tmp<<8 | (0xFF & buffer[indx++]);

    // SYNC WORD checker
    if(((tmp & 0xFFE0) == 0xFFE0))
    {

      f_indx = indx;

      ver_id   = ((tmp >>3) & 3);

      //c1 used temporarily
      c1         = ~ver_id;

      c1         = c1 & 2;
      ver_id     = ver_id & 1;
      ver_id     = ver_id | c1;

      if(ver_id == 3)  // Version Id (01) Reserved Skip
      {
        indx = f_indx - 1;
        continue;
      }

      // Store 1st part of frame header
      header1     = tmp;

      // grab next 16 bits
      tmp = buffer[indx++];
      tmp = tmp<<8 | (0xFF & buffer[indx++]);

      // Complete 32 bit header
      header1 = (header1<<16)  | (0x0000ffff & tmp);
      //mask header 2 for comparing constant part.
      header1 = header1 & HEADERMASK;
	  	{
			int layer;
			layer = (header1 >> 17) & 0x3;
			 AUDIO_SEND_TRACE_PARAM(" Layer1 & Layer 2 files Not Supported",header1, RV_TRACE_LEVEL_ERROR);
			if (layer != 1)
				return  MP3_LAYER1_2_NOT_SUPPORTED;
      	 }

      // extract bit rate and samp freq index from bitstream
      bit_rate = tmp>>12;
      sam_freq = ((tmp>>10) & 3);

      //check for invalid bit_rate and sampling frequencies
      if(bit_rate==15 || sam_freq==3 || bit_rate == 0)
      {  //Wrong SYNC picked up,  check for next sync word
        indx = f_indx - 1;
        continue;
      }

      if(corner != 1) // Corner Check
      {
        // calculate frame byte-length
        nSlots = (int)(
            (long)((long)144000.0 * (long)BITRATE_INDEX[ver_id][bit_rate]) /
              S_FREQ_INDEX[ver_id][sam_freq]);

        if ( ver_id != 1)  //if not mpeg1
            //if(!notLSF)
        {
          nSlots = nSlots / 2;
        }

        //add with padding bits
        nSlots += ( (tmp>>9) & 1);

        skipData = nSlots;
        // frame_len = skipData * 8;
        indx += (skipData - 4);

        if(Bytes < (indx + 3))
        {
          corner = 1;
          corner_head = header1;
          AUDIO_SEND_TRACE_PARAM("EOB - Indx", indx,  RV_TRACE_LEVEL_ERROR);
          AUDIO_SEND_TRACE_PARAM("EOB - Corner Header ", corner_head,  RV_TRACE_LEVEL_ERROR);
          return -1;
        }

        // grab 1st 16 bits
        tmp = buffer[indx++];

        tmp = tmp<<8 | (0xFF & buffer[indx++]);
        header2 = tmp;

        // grab next 16 bits
        tmp = buffer[indx++];
        tmp = tmp<<8 | (0xFF & buffer[indx++]);
        header2 = (header2<<16)  | (0x0000ffff & tmp);


        bit_rate1 = tmp>>12;
        sam_freq1 = ((tmp>>10) & 3);

        //found variable temporarly used for getting protection bits
        found = 16*((header2 & 0x10000) == 0);

        //mask header 2 for comparing constant part.
        header2 = header2 & HEADERMASK;
      }
      else // For Last Buffer Corner
      {
        header2 = corner_head;
        corner = 0;
       AUDIO_SEND_TRACE_PARAM("CH/Header2 ", header2,  RV_TRACE_LEVEL_ERROR);
      }

      // Check whether correct Sync found or not??
      found = (header1==header2);

      if(found)
      {
         AUDIO_SEND_TRACE_PARAM("Header Match", f_indx,  RV_TRACE_LEVEL_ERROR);
         f_indx -= 2;
         return f_indx;
      }

        indx = f_indx - 1;

    } // if SYNC Word
    else
    {
       indx--;
    }

  }
}

/********************************************************************************/
  /*                                                                              */
  /*    Function Name:   mp3_statusBar                                            */
  /*                                                                              */
  /*    Purpose:  This function estimates the amount of the mp3 played            */
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


  void mp3_statusBar ( T_AUDIO_RET status,T_RV_RETURN return_path )
  {
	  T_AUDIO_MP3_STATUS_BAR  *p_send_message;
	   T_RVF_MB_STATUS    mb_status = RVF_RED;
	  UINT16 u16totalTime=0,u16TimePlayed = 0;
	  INT16 s16TimePlayedEst = 0;
	  static  UINT32 u32totalTimeEst=0,mp3_forward_rew;
	  static UINT16 u16Noofframesread=0;
	  UWORD32 mp3_frequency=0,mp3_bitrate=0,framelength_mp3=0, header=0;
	  UWORD32 layer=0,mp3_padding=0,u32Temp =0,u32Temp1 =0;
	  WORD32 mp3_skip_frame= 0;
	  WORD32 mp3_skip_buffer=0,temp_cal3=0,temp_cal4=0;
	  UINT8  u8statuspercentage = 0;


	  INT32 s32TimeForRew=0;
         static INT32 size_played_prev = 0; /* Total time played is 1 because it plays before comming here */



	  // Assuming this function is called every mp3 frame
	  u16Noofframesread++;



      /* Calculating the mp3 information parameters */

       header=(mp3_ndb->d_mp3_api_header[0]<<16)+mp3_ndb->d_mp3_api_header[1];
	   mp3_frequency = l1mp3_get_frequencyrate(header) ;
	   mp3_bitrate = l1mp3_get_bitrate(header);
	   layer= l1mp3_get_layer(header);
	   mp3_padding = l1mp3_get_padding(header);

	   if(layer==1)//for MP3 layer1
	   {
	   	          u32Temp = MP3_LAYRE1_FAMRE_CONST * mp3_bitrate;
	   	          u32Temp1 = mp3_frequency + mp3_padding;
	   	           if ( u32Temp1 != 0)
	              framelength_mp3 = ((u32Temp * 4000 )/ u32Temp1);
	   }
	   else//layer-2 & 3
	   {
	   	          u32Temp = MP3_LAYER2_3_FRAME_CONST * mp3_bitrate;
	   	          u32Temp1 = mp3_frequency + mp3_padding;
	   	           if ( u32Temp1 != 0)
	   	          framelength_mp3 = (u32Temp * 1000 ) / u32Temp1;
       }





      if ( (p_audio_gbl_var->audio_mp3.filesize - p_audio_gbl_var->audio_mp3.size_played) < 4096 ) //last
      {

		  u16totalTime =  ((p_audio_gbl_var->audio_mp3.filesize - p_audio_gbl_var->audio_mp3.size_played) * 8) /  mp3_bitrate ;
	  }

      else
      {

	   if ( mp3_bitrate != 0)
	           u16totalTime =  (p_audio_gbl_var->audio_mp3.size * 8) /  mp3_bitrate ;// bitrate from the first decoding
      }


      // Amount of file played
         u16TimePlayed =  (framelength_mp3 * 1152) ;
      // u32Temp = (mp3_frequency + mp3_padding);
	  	 if ( mp3_frequency != 0)
	     u16TimePlayed /=  mp3_frequency;






if (u16Noofframesread == 1)
   {
       u32totalTimeEst = ( u16totalTime * p_audio_gbl_var->audio_mp3.filesize )/ 4096 ;
       u32TotalTimePlayed = (u16totalTime*2);

   }


if( (mp3_forward_rew == 1) || (failed_skip_time == 1))
{
    if(p_audio_gbl_var->audio_mp3.size_played > size_played_prev)
		{
	      s32TimeForRew = (((p_audio_gbl_var->audio_mp3.size_played - size_played_prev))* 8) /mp3_bitrate;
              u16NextreportingTime += (s32TimeForRew /1000) ;

	    }
	    else
	    {

			//compute the skip frame number from skip time..
		   mp3_skip_frame = mp3_skip_time * (mp3_frequency/MP3_NO_OF_SAMPLES);
			//calculate the buffer to be skipped
		  mp3_skip_buffer  = mp3_skip_frame * framelength_mp3;
					  //added error handling.-rewind case when song just starting.rewind is pressed.MFR
		  temp_cal3 = mp3_skip_buffer + (WORD32)p_audio_gbl_var->audio_mp3.size_played ;
// p_audio_gbl_var->audio_mp3.size == 4096

		  s32TimeForRew = (((size_played_prev - p_audio_gbl_var->audio_mp3.size_played ))* 8 ) / mp3_bitrate ;
		   if(temp_cal3 < 0 )
              s32TimeForRew =- s32TimeForRew - 2; // Remember we have to go back also.
	      else
	         s32TimeForRew =- s32TimeForRew - 1;

             u16NextreportingTime += (s32TimeForRew /1000) ;

        }
         s16TimePlayedEst = s32TimeForRew;
         mp3_forward_rew = 0;
	  failed_skip_time = 0;
}
else
      s16TimePlayedEst = u16totalTime;


if(mp3_skip_time !=0)
   mp3_forward_rew = 1;


        u32TotalTimePlayed += s16TimePlayedEst ; // u16TimePlayed;


if ( u32totalTimeEst != 0)
		u8statuspercentage = (UWORD8)( (u32TotalTimePlayed * 100) / u32totalTimeEst );

// For wrong estimate of the total file length and prevent overshoot.

if (u32TotalTimePlayed > u32totalTimeEst)
{
	u32totalTimeEst = u32TotalTimePlayed;
	u32TotalTimePlayed = 0;
}



if (p_audio_gbl_var->audio_mp3.size_played == p_audio_gbl_var->audio_mp3.filesize)
{
        AUDIO_SEND_TRACE("MP3- : End of file  - \n",  RV_TRACE_LEVEL_DEBUG_LOW);
        u32TotalTimePlayed = 0;
		u16Noofframesread = 0;
}


#if  0


       AUDIO_SEND_TRACE_PARAM("MP3- :  p_audio_gbl_var->audio_mp3.size_played - \n", p_audio_gbl_var->audio_mp3.size_played , RV_TRACE_LEVEL_DEBUG_LOW);

       AUDIO_SEND_TRACE_PARAM("MP3- :  u16Noofframesread - \n", u16Noofframesread , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- :  mp3_padding - \n", mp3_padding , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- :  layer - \n", layer , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- :  mp3_bitrate - \n", mp3_bitrate , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- : s32TimeForRew   - \n", s32TimeForRew , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- : mp3_skip_time   - \n", mp3_skip_time , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- :  GlobalsizePlayed  - \n", p_audio_gbl_var->audio_mp3.size_played, RV_TRACE_LEVEL_DEBUG_LOW);

       AUDIO_SEND_TRACE_PARAM("MP3- : u16FileSize - \n", p_audio_gbl_var->audio_mp3.filesize , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- : s16TimePlayedEst - \n", s16TimePlayedEst , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- : u16TimePlayed - ", u16TimePlayed , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- : mp3_freq - \n", mp3_frequency , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- : u8statuspercentage - \n", u8statuspercentage , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3- : File total- \n", p_audio_gbl_var->audio_mp3.size, RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3-Framelength  : - \n", framelength_mp3 , RV_TRACE_LEVEL_DEBUG_LOW);

 #endif


       AUDIO_SEND_TRACE_PARAM("MP3-Total : u32totalTimeEst- \n", u32totalTimeEst , RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("MP3-Status : u32TotalTimePlayed - \n",u32TotalTimePlayed , RV_TRACE_LEVEL_DEBUG_LOW);


 if(u32TotalTimePlayed >= (u16NextreportingTime * 1000 ))
	{
   /* allocate the message buffer */
    while (mb_status == RVF_RED)
    {
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_MP3_STATUS_BAR),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    /*fill the header of the message */
    p_send_message->os_hdr.msg_id = AUDIO_MP3_STATUS_BAR; // AUDIO_MP3_STATUS_INFO_MSG;

    /* fill the status parameters */
    p_send_message->status        = status;
    p_send_message->u32TotalTimePlayed  = u32TotalTimePlayed;
    p_send_message->u32totalTimeEst  = u32totalTimeEst;



    /* send message or call callback */
    if (return_path.callback_func == NULL)
      rvf_send_msg (return_path.addr_id, p_send_message);
    else
    {
      (*return_path.callback_func)((void *)(p_send_message));
       rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }

   u16NextreportingTime++;
	}



	   size_played_prev = p_audio_gbl_var->audio_mp3.size_played;


	//   return u8statuspercentage;


  }


void audio_mp3_send_data_to_MCU(void)
{
            UINT8 *play_buffer;
            INT16 size_read = 0;
//MP3_FORWARD_REWIND.................implementation starts
           WORD32 mp3_skip_frame= 0;
           UWORD32 mp3_skip_buffer_end =0;
           WORD32 mp3_skip_buffer  = 0,temp_cal3=0,temp_cal4=0;
	   UWORD32 layer=0,mp3_padding=0,temp_cal1=0,temp_cal2=0;
	   UWORD32 mp3_frequency=0,mp3_bitrate=0,framelength_mp3=0,mp3_sync_adjust=0, header=0;
//.....................................................

      if (p_audio_gbl_var->audio_mp3.play_bar_on == 1)
          mp3_statusBar(AUDIO_OK, p_audio_gbl_var->audio_mp3.return_path);

//	  AUDIO_SEND_TRACE_PARAM("mp3_skip_time in data_to_mcu %x", mp3_skip_time,  RV_TRACE_LEVEL_ERROR);

            // try to get a buffer
            if(audio_driver_get_play_buffer(p_audio_gbl_var->audio_mp3.channel_id, &play_buffer) == AUDIO_OK)
            {

//MP3_FORWARD_REWIND.................
	   if(mp3_skip_time !=0)
	     { //calculate the skip buffer
		AUDIO_SEND_TRACE_PARAM("MP3-FR: REQUEST RECEIVED FOR FORWARD REWIND, skip time-", mp3_skip_time, RV_TRACE_LEVEL_DEBUG_LOW);
		  header=(mp3_ndb->d_mp3_api_header[0]<<16)+mp3_ndb->d_mp3_api_header[1];
	          mp3_frequency = l1mp3_get_frequencyrate(header) ;
	          mp3_bitrate = l1mp3_get_bitrate(header);
	          layer= l1mp3_get_layer(header);
	          mp3_padding = l1mp3_get_padding(header);
		  //RESET the DSP checking gobal var in mp3_apihisr
	          mp3_forwardrewind_reset=1;//set global variable.--set to 1
	          if(layer==1)//for MP3 layer1
                  {
	          temp_cal1 = MP3_LAYRE1_FAMRE_CONST * mp3_bitrate;
	          temp_cal2 = mp3_frequency + mp3_padding;
                  framelength_mp3 = (temp_cal1 / temp_cal2) * 4 ;
                  }
                  else//layer-2 & 3
                  {
	          temp_cal1 = MP3_LAYER2_3_FRAME_CONST * mp3_bitrate;
	          temp_cal2 = mp3_frequency + mp3_padding;
	          framelength_mp3 = temp_cal1 / temp_cal2;
                  }
		  //compute the skip frame number from skip time..
		  mp3_skip_frame = mp3_skip_time * (mp3_frequency/MP3_NO_OF_SAMPLES);
		  ///calculate the buffer to be skipped
                  mp3_skip_buffer  = mp3_skip_frame * framelength_mp3;
		  //added error handling.-rewind case when song just starting.rewind is pressed.MFR
		 temp_cal3 = mp3_skip_buffer + (WORD32)p_audio_gbl_var->audio_mp3.size_played ;
                 if(temp_cal3 < 0 )
		 {
#if(AS_RFS_API == 1)
			 //start from the begining...
			if(rfs_lseek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_file_start),
                            RFS_SEEK_SET) < RFS_EOK )

#else
			 //start from the begining...
			if(ffs_seek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_file_start),
                            FFS_SEEK_SET) < EFFS_OK )
#endif
                       {
                         AUDIO_SEND_TRACE("MP3-FR: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                         size_read = 0;
			 temp_cal4 =1; //some file access related error- set to begining..
                       }
			AUDIO_SEND_TRACE("MP3-FR: START OF SONG.....STARTING FROM BEGINING", RV_TRACE_LEVEL_ERROR);
			p_audio_gbl_var->audio_mp3.size_played = p_audio_gbl_var->audio_mp3.size_file_start;
			mp3_skip_buffer=0;
		 }
		 else
		 {
#if(AS_RFS_API == 1)

			if(rfs_lseek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_played + mp3_skip_buffer),
                            RFS_SEEK_SET) < RFS_EOK )

#else
			if(ffs_seek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_played + mp3_skip_buffer),
                            FFS_SEEK_SET) < EFFS_OK )
#endif
                    {
                     AUDIO_SEND_TRACE("MP3-FR: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                     size_read = 0;
		     temp_cal4 =1; //some file access related error- set to begining
                    }
			 AUDIO_SEND_TRACE("MP3-FR: SONG LENGTH OK", RV_TRACE_LEVEL_ERROR);

		 }


	       }//brace close for if mp3_skip_time !=0
	       else
		   	mp3_skip_buffer_end = 1;

    //end MP3_FORWARD_REWIND.....................................................
  //..........outside loop.....read for normal case and forward-rewind case........................................
               //pointer is set now read.........accordingly

#if(AS_RFS_API == 1)
			if(p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
                {
		  size_read = (INT16)rfs_read(p_audio_gbl_var->audio_mp3.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_mp3.size);

                  if(size_read < RFS_EOK)
                  {
                  AUDIO_SEND_TRACE("MP3-FR: FFS PLAY READ FAILED........!", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                  }
                 }

#else
		   if(p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
                {
		  size_read = ffs_read(p_audio_gbl_var->audio_mp3.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_mp3.size);

                  if(size_read < EFFS_OK)
                  {
                  AUDIO_SEND_TRACE("MP3-FR: FFS PLAY READ FAILED........!", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                  }
                 }
#endif
//...................................................................
////MP3_FORWARD_REWIND.................
////adjust the buffer till sync-- if forward/rewind forward- later it should be 1.............##
// //check the next sync in the play buffer.
           if(mp3_skip_time !=0)
	     {
//added by chandra

	if(mp3_skip_buffer_end == 1)
		{
         	 header=(mp3_ndb->d_mp3_api_header[0]<<16)+mp3_ndb->d_mp3_api_header[1];
	          mp3_frequency = l1mp3_get_frequencyrate(header) ;
	          mp3_bitrate = l1mp3_get_bitrate(header);
	          layer= l1mp3_get_layer(header);
	          mp3_padding = l1mp3_get_padding(header);
		  //RESET the DSP checking gobal var in mp3_apihisr
	          mp3_forwardrewind_reset=1;//set global variable.--set to 1
	          if(layer==1)//for MP3 layer1
                  {
	          temp_cal1 = MP3_LAYRE1_FAMRE_CONST * mp3_bitrate;
	          temp_cal2 = mp3_frequency + mp3_padding;
                  framelength_mp3 = (temp_cal1 / temp_cal2) * 4 ;
                  }
                  else//layer-2 & 3
                  {
	          temp_cal1 = MP3_LAYER2_3_FRAME_CONST * mp3_bitrate;
	          temp_cal2 = mp3_frequency + mp3_padding;
	          framelength_mp3 = temp_cal1 / temp_cal2;
                  }
		  //compute the skip frame number from skip time..
		  mp3_skip_frame = mp3_skip_time * (mp3_frequency/MP3_NO_OF_SAMPLES);
		  ///calculate the buffer to be skipped
                  mp3_skip_buffer_end  = mp3_skip_frame * framelength_mp3;

		     mp3_skip_buffer = mp3_skip_buffer_end;

  //added error handling.-rewind case when song just starting.rewind is pressed.MFR
		 temp_cal3 = mp3_skip_buffer + (WORD32)p_audio_gbl_var->audio_mp3.size_played ;
                 if(temp_cal3 < 0 )
		 {
#if(AS_RFS_API == 1)
			 //start from the begining...
			if(rfs_lseek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_file_start),
                            RFS_SEEK_SET) < RFS_EOK )

#else
			 //start from the begining...
			if(ffs_seek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_file_start),
                            FFS_SEEK_SET) < EFFS_OK )
#endif
                       {
                         AUDIO_SEND_TRACE("MP3-FR: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                         size_read = 0;
			 temp_cal4 =1; //some file access related error- set to begining..
                       }
			AUDIO_SEND_TRACE("MP3-FR: START OF SONG.....STARTING FROM BEGINING", RV_TRACE_LEVEL_ERROR);
			p_audio_gbl_var->audio_mp3.size_played = p_audio_gbl_var->audio_mp3.size_file_start;
			mp3_skip_buffer=0;


		 }
		 else
		 {
#if(AS_RFS_API == 1)

			if(rfs_lseek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_played + mp3_skip_buffer),
                            RFS_SEEK_SET) < RFS_EOK )

#else
			if(ffs_seek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_played + mp3_skip_buffer),
                            FFS_SEEK_SET) < EFFS_OK )
#endif
                    {
                     AUDIO_SEND_TRACE("MP3-FR: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                     size_read = 0;
		     temp_cal4 =1; //some file access related error- set to begining
                    }
			 AUDIO_SEND_TRACE("MP3-FR: SONG LENGTH OK", RV_TRACE_LEVEL_ERROR);

		 }


		 #if(AS_RFS_API == 1)
			if(p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
                {
		  size_read = (INT16)rfs_read(p_audio_gbl_var->audio_mp3.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_mp3.size);

                  if(size_read < RFS_EOK)
                  {
                  AUDIO_SEND_TRACE("MP3-FR: FFS PLAY READ FAILED........!", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                  }
                 }

#else
		   if(p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
                {
		  size_read = ffs_read(p_audio_gbl_var->audio_mp3.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_mp3.size);

                  if(size_read < EFFS_OK)
                  {
                  AUDIO_SEND_TRACE("MP3-FR: FFS PLAY READ FAILED........!", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                  }
                 }
#endif


		     mp3_skip_buffer_end = 0;
		    failed_skip_time = 1;
		}
//added by chandra

		//forward case gone out of file size....or some error has happened during file access
		if(((UINT16)size_read < p_audio_gbl_var->audio_mp3.size)|| temp_cal4 ==1)
                {
#if(AS_RFS_API == 1)

			//start from the begining...
			if(rfs_lseek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_file_start),
                            RFS_SEEK_SET) < RFS_EOK )

#else
			 //start from the begining...
			if(ffs_seek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_file_start),
                            FFS_SEEK_SET) < EFFS_OK )
#endif

                       {
                         AUDIO_SEND_TRACE("MP3-FR: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                         size_read = 0;
                       }
			AUDIO_SEND_TRACE("MP3-FR: FORWARD- SONG ENDED.....", RV_TRACE_LEVEL_ERROR);
			p_audio_gbl_var->audio_mp3.size_played = p_audio_gbl_var->audio_mp3.size_file_start;
			mp3_skip_buffer =0;

		}
		///sync search starts..
		for(mp3_sync_adjust=0; mp3_sync_adjust < p_audio_gbl_var->audio_mp3.size; mp3_sync_adjust++)
  		{
      		  if((play_buffer[mp3_sync_adjust] & 0xFF) == 0xFF)
	  	    {
	      	      if((play_buffer[mp3_sync_adjust+1] | 0x1F) == 0xFF)
	      		{
		          break;
	                }
	           }
                }
                AUDIO_SEND_TRACE_PARAM("MP3-FR: SYNC Loctaion at new buffer- ", mp3_sync_adjust, RV_TRACE_LEVEL_DEBUG_LOW);
		mp3_skip_buffer = mp3_skip_buffer + mp3_sync_adjust;//adjust the buffer to the next sync loc


#if(AS_RFS_API == 1)
		if(rfs_lseek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_played + mp3_skip_buffer),
                            RFS_SEEK_SET) < RFS_EOK )

#else
		if(ffs_seek ( p_audio_gbl_var->audio_mp3.ffs_fd,
                            (p_audio_gbl_var->audio_mp3.size_played + mp3_skip_buffer),
                            FFS_SEEK_SET) < EFFS_OK )
#endif
                    {
                     AUDIO_SEND_TRACE("MP3-FR: FFS SEEK - Error", RV_TRACE_LEVEL_ERROR);
                     size_read = 0;
                    }

#if(AS_RFS_API == 1)

		//over write buffer..
	      if(p_audio_gbl_var->audio_mp3.ffs_fd >= RFS_EOK)
              {
		size_read = (INT16)rfs_read(p_audio_gbl_var->audio_mp3.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_mp3.size);

                if(size_read < RFS_EOK)
                {
                  AUDIO_SEND_TRACE("MP3-FR: FFS PLAY READ FAILED", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                }
              }

#else

		//over write buffer..
	      if(p_audio_gbl_var->audio_mp3.ffs_fd >= EFFS_OK)
              {
		size_read = ffs_read(p_audio_gbl_var->audio_mp3.ffs_fd,
                                     play_buffer,
                                     p_audio_gbl_var->audio_mp3.size);

                if(size_read < EFFS_OK)
                {
                  AUDIO_SEND_TRACE("MP3-FR: FFS PLAY READ FAILED", RV_TRACE_LEVEL_ERROR);
                  size_read = 0;  // will put END_MASK in whole buffer so stops play
                }
              }
#endif
              	AUDIO_SEND_TRACE_PARAM("MP3-FR: FORWARD REWIND REQUEST PROCESSED-skip buffer=", mp3_skip_buffer, RV_TRACE_LEVEL_DEBUG_LOW);
		mp3_skip_time =0;//processed- reset it again
	     }

//...............................................................................
              //now update how much is read & played actually..
              p_audio_gbl_var->audio_mp3.size_played = p_audio_gbl_var->audio_mp3.size_played + size_read + mp3_skip_buffer;
//...........................................................................................................
               mp3_skip_buffer=0;//reset skip buffer

//end MP3_FORWARD_REWIND update.....................................................
                /* last buffer already, call flush buffer */
                if((UINT16)size_read < p_audio_gbl_var->audio_mp3.size)
                {
                  UINT16 i;
	          if (p_audio_gbl_var->audio_mp3.play_bar_on == 1)
                     mp3_statusBar(AUDIO_OK, p_audio_gbl_var->audio_mp3.return_path);

                  for(i = size_read; i < p_audio_gbl_var->audio_mp3.size; i++)
                    *(play_buffer + i) = 0x00;

                  audio_driver_flush_buffer(p_audio_gbl_var->audio_mp3.channel_id, play_buffer);
                  p_audio_gbl_var->audio_mp3.size_played = 0;
		  mp3_skip_buffer=0;
	          mp3_skip_time =0;//reset the frame no - commented for testing purpose
                }
                else
                {
                  audio_driver_play_buffer(p_audio_gbl_var->audio_mp3.channel_id, play_buffer);
                }
              if(size_read > 0)
              {
                AUDIO_SEND_TRACE_PARAM("AUDIO MP3: FFS to RAM size", size_read, RV_TRACE_LEVEL_DEBUG_LOW);
              }
              else
              	{
              	 DummyStruct *p_send_message = audio_allocate_l1_message(0);
                AUDIO_SEND_TRACE("AUDIO MP3: buffer not used",RV_TRACE_LEVEL_DEBUG_LOW);
		     
                if ( p_send_message != NULL)
                   {
                                      audio_send_l1_message(MMI_MP3_STOP_REQ, p_send_message);
                    }
              	}
            } // if(audio_driver_get_play_buffer(channel_id,&p_buffer)==AUDIO_OK)
            else
              AUDIO_SEND_TRACE("AUDIO MP3: no buffer available", RV_TRACE_LEVEL_DEBUG_LOW);

            p_audio_gbl_var->audio_mp3.buff_notify_msg = FALSE;
}


 #endif /* MP3 */
#endif /* RVM_AUDIO_MAIN_SWE */


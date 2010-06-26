/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_vbuf_pcm_record.c                                           */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            Voice Memorization recording task.                            */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date          Modification                                              */
/*  ------------------------------------                                    */
/*  1 August 2001 Create                                                    */
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

 #if(L1_PCM_EXTRACTION)	
  #include <math.h>
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_features_i.h"

  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_macro_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_error_hdlr_i.h"

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

extern  UINT8 ptt_send_size_to_l1_flag;
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vbuf_pcm_record_send_status                              */
  /*                                                                              */
  /*    Purpose:  This function sends the voice memorization record status        */
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
  void audio_vbuf_pcm_record_send_status ( T_AUDIO_RET status,
                                     UINT32 recorded_size,
                                     T_RV_RETURN return_path)
  {
    T_AUDIO_VBUF_PCM_RECORD_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VBUF_PCM_RECORD_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    /*fill the header of the message */
    ((T_AUDIO_VBUF_PCM_RECORD_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_VBUF_PCM_RECORD_STATUS_MSG;

    /* fill the status parameters */
    ((T_AUDIO_VBUF_PCM_RECORD_STATUS *)p_send_message)->status = status;
    if (recorded_size == 0)
    {
      ((T_AUDIO_VBUF_PCM_RECORD_STATUS *)p_send_message)->recorded_duration = 0;
       AUDIO_SEND_TRACE_PARAM("audio_vbuf_pcm_record_send_status: RECORDED SIZE", ((T_AUDIO_VBUF_PCM_RECORD_STATUS *)p_send_message)->recorded_duration, RV_TRACE_LEVEL_DEBUG_LOW);
    }
    else
    {
      ((T_AUDIO_VBUF_PCM_RECORD_STATUS *)p_send_message)->recorded_duration = (UINT16)(recorded_size / 8000);
      AUDIO_SEND_TRACE_PARAM("audio_vbuf_pcm_record_send_status: RECORDED SIZE", ((T_AUDIO_VBUF_PCM_RECORD_STATUS *)p_send_message)->recorded_duration, RV_TRACE_LEVEL_DEBUG_LOW);
    }

    if (return_path.callback_func == NULL)
    {
      /* send the message to the entity */
      rvf_send_msg (return_path.addr_id, p_send_message);
    }
    else
    {
      /* call the callback function */
      (*return_path.callback_func)((void *)p_send_message);
	  rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vbuf_pcm_record_convert_parameter                        */
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
void audio_vbuf_pcm_record_convert_parameter( UINT8 channel_id,
                                          T_AUDIO_VBUF_PCM_RECORD_START *entity_parameter,
                                          T_MMI_PCM_UPLOAD_START_REQ *l1_parameter)
  {
    /* session id : specifies the customer data identification corresponding to   */
    /*              this PCM voice memorization */
    l1_parameter->session_id = channel_id;
    l1_parameter->maximum_size =((entity_parameter->memo_duration)*AUDIO_PCM_SAMPLES_PER_SEC);
    l1_parameter->upload_ul_gain = (UINT8)(entity_parameter->upload_ul_gain);
    l1_parameter->upload_dl_gain = (UINT8)(entity_parameter->upload_dl_gain);
//    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM L1 PARAM session_id: ", l1_parameter->session_id, RV_TRACE_LEVEL_DEBUG_LOW);
//    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM L1 PARAM maximum_size: ", l1_parameter->maximum_size, RV_TRACE_LEVEL_DEBUG_LOW);
//    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM L1 PARAM: upload_ul_gain", l1_parameter->upload_ul_gain, RV_TRACE_LEVEL_DEBUG_LOW);
//    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM L1 PARAM: upload_dl_gain", l1_parameter->upload_dl_gain, RV_TRACE_LEVEL_DEBUG_LOW);
   }



  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vbuf_pcm_record_manager                                  */
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
  void audio_vbuf_pcm_record_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    T_RVF_MB_STATUS mb_status;

    /**************** audio_vbuf_pcm_record_manager function begins ***********************/
  AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM RECORD MANAGER: STATE", p_audio_gbl_var->vbuf_pcm_record.state, RV_TRACE_LEVEL_DEBUG_LOW);
    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM RECORD MANAGER: RECVD MSG", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
    switch(p_audio_gbl_var->vbuf_pcm_record.state)
    {
  
      case AUDIO_VBUF_PCM_RECORD_IDLE:
        {
	    	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: STATE -  AUDIO_VBUF_PCM_RECORD_IDLE",
					             RV_TRACE_LEVEL_ERROR);

          switch(p_message->msg_id)
          {
             case AUDIO_VBUF_PCM_RECORD_START_REQ:
              {
            T_AUDIO_FFS_RAM_2_FLASH_START *p_send_message;
	  	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_START_REQ",
					             RV_TRACE_LEVEL_ERROR);

                /* save the task id of the entity */
                p_audio_gbl_var->vbuf_pcm_record.task_id = p_message->src_addr_id;

                /* save the return path */
                p_audio_gbl_var->vbuf_pcm_record.return_path.callback_func =
                  ((T_AUDIO_VBUF_PCM_RECORD_START*)(p_message))->return_path.callback_func;
                p_audio_gbl_var->vbuf_pcm_record.return_path.addr_id   =
                  ((T_AUDIO_VBUF_PCM_RECORD_START*)(p_message))->return_path.addr_id;
#if(AS_RFS_API == 1)
			wstrcpy(p_audio_gbl_var->vbuf_pcm_record.memo_name, ((T_AUDIO_VBUF_PCM_RECORD_START*)(p_message))->memo_name);//VENKAT
#else
		  strcpy(p_audio_gbl_var->vbuf_pcm_record.memo_name, ((T_AUDIO_VBUF_PCM_RECORD_START*)(p_message))->memo_name);//VENKAT
#endif
 		  p_audio_gbl_var->vbuf_pcm_record.vbuf_ffs_fd = ((T_AUDIO_VBUF_PCM_RECORD_START*)(p_message))->audio_ffs_fd;

#if(AS_RFS_API == 1)
			p_audio_gbl_var->vbuf_pcm_record.ffs_write = rfs_lseek( ((T_AUDIO_VBUF_PCM_RECORD_START*)(p_message))->audio_ffs_fd,
										                     		      0,
            												                RFS_SEEK_SET);
#else
			 p_audio_gbl_var->vbuf_pcm_record.ffs_write = ffs_seek( ((T_AUDIO_VBUF_PCM_RECORD_START*)(p_message))->audio_ffs_fd,
										                     		      0,
            												                FFS_SEEK_SET);
#endif

                /* Send the Start message to the FFS */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_RAM_2_FLASH_START),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_RAM_2_FLASH_START_REQ;

                /* fill the parameters */
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_send_message)->audio_ffs_fd =
                  ((T_AUDIO_VBUF_PCM_RECORD_START*)(p_message))->audio_ffs_fd;
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_send_message)->initial_size =
                  AUDIO_VBUF_PCM_RECORD_SIZE;
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_send_message)->session_id = AUDIO_FFS_SESSION_VBUF_PCM_RECORD;

                /* allocate the buffer for the message to the L1 */
                p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message =
                  audio_allocate_l1_message(sizeof(T_MMI_PCM_UPLOAD_START_REQ));

                if ( p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message != NULL )
                {
                  /* Convert the entity parameters to the audio L1 parameters */
                  audio_vbuf_pcm_record_convert_parameter( ((T_AUDIO_FFS_RAM_2_FLASH_START *)(p_send_message))->session_id,
                          ((T_AUDIO_VBUF_PCM_RECORD_START*)(p_message)),
                          ((T_MMI_PCM_UPLOAD_START_REQ *)(p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message)));

                }

                /* send the message to the entity */
                rvf_send_msg ( p_audio_gbl_var->audio_ffs_addrId,
                               p_send_message);
                AUDIO_SEND_TRACE("  p_audio_gbl_var->vmemo_vbuf_session = AUDIO_VBUF_PCM_SESSION; %%%%",
					             RV_TRACE_LEVEL_ERROR);
		  p_audio_gbl_var->vmemo_vbuf_session = AUDIO_VBUF_PCM_SESSION;

                /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_INIT_FFS */
                p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_INIT_FFS;
                break;
          } /* case AUDIO_VBUF_PCM_RECORD_START_REQ: */

              /* A stop request is sent during a start connection */
              /* event error - send an error message, the recorded size is null  */
              case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
              {
		  	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_STOP_REQ",
					             RV_TRACE_LEVEL_ERROR);
                 audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                              ((T_AUDIO_VBUF_PCM_RECORD_STOP *)(p_message))->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                 break;
              } /* case AUDIO_VBUF_PCM_RECORD_STOP_REQ */
        } /* switch(p_message->msg_id) */
        break;
        } /* case AUDIO_VBUF_PCM_RECORD_IDLE */

      case AUDIO_VBUF_PCM_RECORD_WAIT_INIT_FFS:
        {
    	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: STATE -  AUDIO_VBUF_PCM_RECORD_WAIT_INIT_FFS",
					             RV_TRACE_LEVEL_ERROR);

          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
              {

 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_FFS_INIT_DONE",
					             RV_TRACE_LEVEL_ERROR);	  	               
                /* set the channel_id */
                ((T_MMI_PCM_UPLOAD_START_REQ   *)(p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message))->session_id =
                  ((T_AUDIO_FFS_INIT *)p_message)->channel_id;

    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM L1 PARAM session_id: ",((T_MMI_PCM_UPLOAD_START_REQ   *)(p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message))->session_id, RV_TRACE_LEVEL_DEBUG_LOW);
    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM L1 PARAM maximum_size: ", ((T_MMI_PCM_UPLOAD_START_REQ   *)(p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message))->maximum_size, RV_TRACE_LEVEL_DEBUG_LOW);
    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM L1 PARAM: upload_ul_gain", ((T_MMI_PCM_UPLOAD_START_REQ   *)(p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message))->upload_ul_gain, RV_TRACE_LEVEL_DEBUG_LOW);
    AUDIO_SEND_TRACE_PARAM("AUDIO VBUF PCM L1 PARAM: upload_dl_gain", ((T_MMI_PCM_UPLOAD_START_REQ   *)(p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message))->upload_dl_gain, RV_TRACE_LEVEL_DEBUG_LOW);

		 AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: L1 MSG SENT -  MMI_PCM_UPLOAD_START_REQ",
                       RV_TRACE_LEVEL_ERROR);

                /* send the start voice memo record message to the L1 */
                audio_send_l1_message( MMI_PCM_UPLOAD_START_REQ, p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message);
                /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_START_CONFIRMATION */
                p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_START_CON;
                break;
              } // case AUDIO_FFS_INIT_DONE:

             case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
              {
			  	 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_STOP_REQ",
					             RV_TRACE_LEVEL_ERROR);
                /*.Before stopping this task, control that            */
                /* stop task id caller = Voice Memorization task id   */
                   if ( p_audio_gbl_var->vbuf_pcm_record.task_id == p_message->src_addr_id)
                   {
                     /* Deallocate the L1 message previously allocated */
                     audio_deallocate_l1_message(p_audio_gbl_var->vbuf_pcm_record.p_l1_send_message);

                     /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_INIT_DONE */
                     p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_INIT_DONE;
                   }
                   else
                   {
                     /* A stop request from an other task is sent during a start connection */
                     /* event error - send an error message, the recorded size is null  */
                     audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VBUF_PCM_RECORD_STOP *)(p_message))->return_path);
                     audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                   }
                break;
              } /* case AUDIO_VBUF_PCM_RECORD_STOP_REQ */

             case AUDIO_VBUF_PCM_RECORD_START_REQ:
              {
                  /* event error - send an error message, the recorded size is null*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
				   	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_START_REQ",
					             RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VBUF_PCM_RECORD_START_REQ:

          } /* switch */
        break;
        } /* case AUDIO_VBUF_PCM_RECORD_E1_WAIT_INIT_FFS */

      case AUDIO_VBUF_PCM_RECORD_WAIT_INIT_DONE:
        {
    	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: STATE - AUDIO_VBUF_PCM_RECORD_WAIT_INIT_DONE",
					             RV_TRACE_LEVEL_ERROR);

          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
          {
            T_AUDIO_FFS_STOP_REQ *p_send_message;
 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_FFS_INIT_DONE",
					             RV_TRACE_LEVEL_ERROR);
                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_STOP_REQ),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
            ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
            ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VBUF_PCM_RECORD;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_STOP_FFS */
                p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_STOP_FFS;
              break;
          } // case AUDIO_FFS_INIT_DONE:

             case AUDIO_VBUF_PCM_RECORD_START_REQ:
              {
                  /* event error - send an error message, the recorded size is null*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
				   	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_START_REQ",
					             RV_TRACE_LEVEL_ERROR);
#if(AS_AUDIO_RFS_FLAg == 1)
				if ( rfs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
			  if ( ffs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                    ((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VBUF_PCM_RECORD_START_REQ:

             case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
              {
			  	 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG - AUDIO_VBUF_PCM_RECORD_STOP_REQ",
					             RV_TRACE_LEVEL_ERROR);
                  /* event error - send an error message, the recorded size is null*/
                  audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                        ((T_AUDIO_VBUF_PCM_RECORD_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
          } // case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VBUF_PCM_RECORD_WAIT_INIT_DONE*/

      case AUDIO_VBUF_PCM_RECORD_WAIT_START_CON:
        {
 	 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: STATE -  AUDIO_VBUF_PCM_RECORD_WAIT_START_CON",
					             RV_TRACE_LEVEL_ERROR);
          switch(p_message->msg_id)
          {
             case MMI_PCM_UPLOAD_START_CON:
              {
  	 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: delete aftyer play = true MSG -  MMI_PCM_UPLOAD_START_CON",
					             RV_TRACE_LEVEL_ERROR);
		   
		   p_audio_gbl_var->vbuf_pcm_record.delete_after_play = TRUE;
		   
                /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_STOP_COMMAND */
                p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_STOP_COMMAND;
                break;
          } // case MMI_VBUF_RECORD_START_CON:

             case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
          {
            /*.Before stopping this task, control that   */
               /* stop task id caller = voice memo task id  */
			 AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG - AUDIO_VBUF_PCM_RECORD_STOP_REQ",
					             RV_TRACE_LEVEL_ERROR);
                if ( p_audio_gbl_var->vbuf_pcm_record.task_id == p_message->src_addr_id)
                {
                   /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CONFIRMATION */
                  p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_START_CON_TO_STOP;
                }
                else
                {
                   /* A stop request from an other task is sent during a start connection */
                   /* event error - send an error message, the recorded size is null  */
                   audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VBUF_PCM_RECORD_STOP *)(p_message))->return_path);
                   audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                }
               break;
          } // case AUDIO_VBUF_PCM_RECORD_STOP_REQ:

             case AUDIO_VBUF_PCM_RECORD_START_REQ:
             {
               /* event error - send an error message, the recorded size is null*/
               /* Close the FFS file previously open by the audio start API*/
               #ifndef _WINDOWS
			    	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_START_REQ",
					             RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
			  if ( ffs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                 {
                   audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                 }
               #endif
               audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                       ((T_AUDIO_VBUF_PCM_RECORD_START *)p_message)->return_path);
               audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
               break;
          } // case AUDIO_VBUF_PCM_RECORD_START_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VBUF_PCM_RECORD_WAIT_START_CON */

       case AUDIO_VBUF_PCM_RECORD_WAIT_START_CON_TO_STOP:
        {
		AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: STATE - AUDIO_VBUF_PCM_RECORD_WAIT_START_CON_TO_STOP",
					             RV_TRACE_LEVEL_ERROR);
          switch(p_message->msg_id)
          {
             case MMI_PCM_UPLOAD_START_CON:
              {
            DummyStruct *p_send_message;
 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  MMI_PCM_UPLOAD_START_CON",
					             RV_TRACE_LEVEL_ERROR);
                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(0);
                if ( p_send_message != NULL) /*VENKAT: Check. When will this be non-NULL). Seems to be wrong.*/
                {
                  		 AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: L1 MSG SENT -  MMI_PCM_UPLOAD_STOP_REQ",
                       RV_TRACE_LEVEL_ERROR);
                  /* send the stop command to the audio L1 */
                  audio_send_l1_message(MMI_PCM_UPLOAD_STOP_REQ, p_send_message);

                }

                /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CON */
                p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CON;
               break;
          } // case MMI_VBUF_RECORD_START_CON:

             case AUDIO_VBUF_PCM_RECORD_START_REQ:
              {
                  /* event error - send an error message the recorded size is null*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
				   	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG - AUDIO_VBUF_PCM_RECORD_START_REQ",
					             RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                          ((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VBUF_PCM_RECORD_START_REQ:

             case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
              {
			AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG - AUDIO_VBUF_PCM_RECORD_STOP_REQ",
					             RV_TRACE_LEVEL_ERROR);
                  /* event error - send an error message, the recorded size is null*/
                  audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VBUF_PCM_RECORD_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
          } // case AUDIO_VBUF_PCM_RECORD_STOP_REQ:

        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VBUF_PCM_RECORD_WAIT_START_CON_TO_STOP */

       case AUDIO_VBUF_PCM_RECORD_WAIT_STOP_COMMAND:
        {
			 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: STATE - AUDIO_VBUF_PCM_RECORD_WAIT_STOP_COMMAND",
					             RV_TRACE_LEVEL_ERROR);
          switch(p_message->msg_id)
          {
             case MMI_PCM_UPLOAD_STOP_CON:
               {
            T_AUDIO_FFS_STOP_REQ *p_send_message;
		DummyStruct *p_send_message_to_l1;
 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG - MMI_PCM_UPLOAD_STOP_CON",
					             RV_TRACE_LEVEL_ERROR);
                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_STOP_REQ),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
            ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
            ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VBUF_PCM_RECORD;

                /* fill the recorded size */
                p_audio_gbl_var->vbuf_pcm_record.recorded_size = ((T_MMI_PCM_UPLOAD_STOP_CON *)p_message)->uploaded_size;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_STOP_FFS */
                p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_STOP_FFS;
/* send the voice memo play stop message to the L1 */
/* allocate the buffer for the message to the L1 */
p_send_message_to_l1 = audio_allocate_l1_message(sizeof(T_MMI_PCM_DOWNLOAD_STOP_REQ));
if ( p_send_message_to_l1 != NULL)
{
      AUDIO_SEND_TRACE(" AUDIO VBUF PCM PLAY nekkareb_NEW_1: L1 MSG SENT -  MMI_PCM_DOWNLOAD_STOP_REQ",RV_TRACE_LEVEL_ERROR);
	/* Convert the entity parameters to the audio L1 parameters */
	((T_MMI_PCM_DOWNLOAD_STOP_REQ *)p_send_message_to_l1)->maximum_size =p_audio_gbl_var->vbuf_pcm_record.recorded_size;

      AUDIO_SEND_TRACE_PARAM(" L1 maximum_size nekkareb_NEW_1: MMI_PCM_DOWNLOAD_STOP_REQ",((T_MMI_PCM_DOWNLOAD_STOP_REQ *)p_send_message_to_l1)->maximum_size ,RV_TRACE_LEVEL_ERROR);
	/* send the start command to the audio L1 */
	audio_send_l1_message(MMI_PCM_DOWNLOAD_STOP_REQ, p_send_message_to_l1);
	ptt_send_size_to_l1_flag = TRUE;
}
      AUDIO_SEND_TRACE(" AUDIO VBUF PCM PLAY nekkareb_NEW_1: L1 MSG already SENT -  MMI_PCM_DOWNLOAD_STOP_REQ",RV_TRACE_LEVEL_ERROR);



			
               break;
          } // case MMI_VBUF_RECORD_STOP_CON:

             case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
          {
		  	 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_STOP_REQ",
					             RV_TRACE_LEVEL_ERROR);
            /*.Before stopping this task, control that stop task id caller = Voice task id                    */
               if ( p_audio_gbl_var->vbuf_pcm_record.task_id == p_message->src_addr_id)
               {
              DummyStruct *p_send_message;

                   /* send the stop command to the audio L1 */
                   /* allocate the buffer for the message to the L1 */
                   p_send_message = audio_allocate_l1_message(0);
                  if (p_send_message != NULL)
                  {
                  	   		 AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: L1 MSG SENT -  MMI_PCM_UPLOAD_STOP_REQ",
                       RV_TRACE_LEVEL_ERROR);
                     /* send the stop command to the audio L1 */
                     audio_send_l1_message(MMI_PCM_UPLOAD_STOP_REQ, p_send_message);

                   }

                  /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CONFIRMATION */
                   p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CON;
               }
               else
               {
                 /* A stop request from an other task is sent during a start connection */
                 /* event error - send an error message, the recorded size is null  */
                 audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VBUF_PCM_RECORD_STOP *)(p_message))->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
               }
             break;
          } // case AUDIO_VBUF_PCM_RECORD_STOP_REQ:

             case AUDIO_VBUF_PCM_RECORD_START_REQ:
               {
                 /* A start request from an other task is sent during a start connection */
                 /* event error - send an error message, the recorded size is null */
                  /* Close the FFS file previously open by the audio start API*/
                 #ifndef _WINDOWS
				  	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG - AUDIO_VBUF_PCM_RECORD_START_REQ",
					             RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				   if ( ffs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                   {
                     audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                   }
                 #endif
                 audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                        ((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
          } // case AUDIO_VBUF_PCM_RECORD_START_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VBUF_PCM_RECORD_WAIT_STOP_COMMAND */

        case AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CON:
        {
			 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: STATE -  AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CON",
					             RV_TRACE_LEVEL_ERROR);
          switch(p_message->msg_id)
          {
             case MMI_PCM_UPLOAD_STOP_CON:
              {
            T_AUDIO_FFS_STOP_REQ *p_send_message;
              DummyStruct *p_send_message_to_l1;
 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  MMI_PCM_UPLOAD_STOP_CON",
					             RV_TRACE_LEVEL_ERROR);
                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_STOP_REQ),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_STOP_REQ *)(p_send_message))->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)(p_send_message))->session_id = AUDIO_FFS_SESSION_VBUF_PCM_RECORD;

                /* fill the recorded size */
                p_audio_gbl_var->vbuf_pcm_record.recorded_size = ((T_MMI_PCM_UPLOAD_STOP_CON *)p_message)->uploaded_size;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_VBUF_PCM_RECORD_WAIT_STOP_FFS */
                 p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_STOP_FFS;
/* send the voice memo play stop message to the L1 */
/* allocate the buffer for the message to the L1 */
p_send_message_to_l1 = audio_allocate_l1_message(sizeof(T_MMI_PCM_DOWNLOAD_STOP_REQ));
if ( p_send_message_to_l1 != NULL)
{
      AUDIO_SEND_TRACE(" AUDIO VBUF PCM PLAY nekkareb_NEW_2: L1 MSG SENT -  MMI_PCM_DOWNLOAD_STOP_REQ",RV_TRACE_LEVEL_ERROR);
	/* Convert the entity parameters to the audio L1 parameters */
	((T_MMI_PCM_DOWNLOAD_STOP_REQ *)p_send_message_to_l1)->maximum_size =p_audio_gbl_var->vbuf_pcm_record.recorded_size;

      AUDIO_SEND_TRACE_PARAM(" L1 maximum_size nekkareb_NEW_1: MMI_PCM_DOWNLOAD_STOP_REQ",((T_MMI_PCM_DOWNLOAD_STOP_REQ *)p_send_message_to_l1)->maximum_size ,RV_TRACE_LEVEL_ERROR);
	/* send the start command to the audio L1 */
	audio_send_l1_message(MMI_PCM_DOWNLOAD_STOP_REQ, p_send_message_to_l1);
			ptt_send_size_to_l1_flag = TRUE;
}
      AUDIO_SEND_TRACE(" AUDIO VBUF PCM PLAY nekkareb_NEW_2: L1 MSG already SENT -  MMI_PCM_DOWNLOAD_STOP_REQ",RV_TRACE_LEVEL_ERROR);



               break;
          } // case MMI_VBUF_RECORD_STOP_CON:

              case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
                {
					 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_STOP_REQ",
					             RV_TRACE_LEVEL_ERROR);
                  /* event error - send an error message, the recorded size is null*/
                  audio_vbuf_pcm_record_send_status (AUDIO_ERROR, 0,
                        ((T_AUDIO_VBUF_PCM_RECORD_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
          } // case AUDIO_VBUF_PCM_RECORD_STOP_REQ:

              case AUDIO_VBUF_PCM_RECORD_START_REQ:
                {
                  /* event error - send an error message, the recorded size is null*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
				   	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_START_REQ",
					             RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VBUF_PCM_RECORD_START_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CON */

        case AUDIO_VBUF_PCM_RECORD_WAIT_STOP_FFS:
        {
		AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: STATE - AUDIO_VBUF_PCM_RECORD_WAIT_STOP_FFS",
					             RV_TRACE_LEVEL_ERROR);
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_STOP_CON:
              {
	  	 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_FFS_STOP_CON",
					             RV_TRACE_LEVEL_ERROR);
		  	if(p_audio_gbl_var->vbuf_pcm_record.delete_after_play == FALSE)
			{
	  	 	     AUDIO_SEND_TRACE(" VBUF PCM RCD MGR: FFS_STOP_CON file removed",
					             RV_TRACE_LEVEL_ERROR);			
#if(AS_RFS_API == 1)
				rfs_remove(p_audio_gbl_var->vbuf_pcm_record.memo_name);
#else
				 ffs_remove(p_audio_gbl_var->vbuf_pcm_record.memo_name);
#endif
              	        p_audio_gbl_var->vbuf_pcm_record.recorded_size = 0;
		                audio_vbuf_pcm_record_send_status ( AUDIO_OK,
              	        p_audio_gbl_var->vbuf_pcm_record.recorded_size,
                     	 p_audio_gbl_var->vbuf_pcm_record.return_path);
				p_audio_gbl_var->vmemo_vbuf_session = 0;
		                /* change to the state AUDIO_VBUF_PCM_RECORD_IDLE */
              		  p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_IDLE;
             		}
			else
			{
	  	 	     AUDIO_SEND_TRACE(" VBUF PCM RCD MGR: FFS_STOP_CON file NOT removed",
					             RV_TRACE_LEVEL_ERROR);			
			
				p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_WAIT_STOP_PLAY;
			}
	              break;
              } // case AUDIO_FFS_STOP_CON:

             case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
              {
			  	 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG - AUDIO_VBUF_PCM_RECORD_STOP_REQ",
					             RV_TRACE_LEVEL_ERROR);
                /* event error - send an error message, the recorded size is null*/
                audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                                ((T_AUDIO_VBUF_PCM_RECORD_STOP *)(p_message))->return_path);
                audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
          } // case AUDIO_VBUF_PCM_RECORD_STOP_REQ:

             case AUDIO_VBUF_PCM_RECORD_START_REQ:
              {
                /* event error - send an error message, the recorded size is null*/
                /* Close the FFS file previously open by the audio start API*/
                #ifndef _WINDOWS
				 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG - AUDIO_VBUF_PCM_RECORD_START_REQ",
					             RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				  if ( ffs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                  {
                    audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                  }
                #endif
                audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                          ((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->return_path);
                audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VBUF_PCM_RECORD_START_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CON */
	case AUDIO_VBUF_PCM_RECORD_WAIT_STOP_PLAY:
	{
		 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: STATE - AUDIO_VBUF_PCM_RECORD_WAIT_STOP_PLAY",
					             RV_TRACE_LEVEL_ERROR);
		          switch(p_message->msg_id)
		          	{
		          		case AUDIO_VBUF_PCM_PLAY_STOP_CON:
					{
						
#if(AS_RFS_API == 1)
					T_RFS_RET fclose_ret;
					T_RFS_RET frem_ret;
#else
						T_FFS_RET fclose_ret;
						T_FFS_RET frem_ret;
#endif
						
						
						
		 	                     AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_PLAY_STOP_CON",
					             RV_TRACE_LEVEL_ERROR);

#if(AS_RFS_API == 1)
						fclose_ret=rfs_close(p_audio_gbl_var->vbuf_pcm_record.vbuf_ffs_fd);
#else
						fclose_ret=ffs_close(p_audio_gbl_var->vbuf_pcm_record.vbuf_ffs_fd);
#endif
						
			                     AUDIO_SEND_TRACE_PARAM("AUDIO_VBUF_PCM_PLAY_STOP_CON  fclose_ret :",
					            fclose_ret, RV_TRACE_LEVEL_ERROR);	
								 
#if(AS_RFS_API == 1)
						frem_ret =rfs_remove(p_audio_gbl_var->vbuf_pcm_record.memo_name);
#else
						frem_ret =ffs_remove(p_audio_gbl_var->vbuf_pcm_record.memo_name);
#endif
		 	                     AUDIO_SEND_TRACE_PARAM("AUDIO_VBUF_PCM_PLAY_STOP_CON  frem_ret:",
					            frem_ret, RV_TRACE_LEVEL_ERROR);				
			   
						p_audio_gbl_var->vbuf_pcm_record.recorded_size = 0;
						p_audio_gbl_var->vbuf_pcm_record.vbuf_ffs_fd = NULL;
				              audio_vbuf_pcm_record_send_status ( AUDIO_OK,
		              				        p_audio_gbl_var->vbuf_pcm_record.recorded_size,
			              		       	 p_audio_gbl_var->vbuf_pcm_record.return_path);
						  p_audio_gbl_var->vmemo_vbuf_session = 0;
		              		  /* change to the state AUDIO_VBUF_PCM_RECORD_IDLE */
		              		  p_audio_gbl_var->vbuf_pcm_record.state = AUDIO_VBUF_PCM_RECORD_IDLE;
						  break;
	          			}
					case AUDIO_VBUF_PCM_RECORD_START_REQ:
			              {
				                /* event error - send an error message, the recorded size is null*/
				                /* Close the FFS file previously open by the audio start API*/
				                #ifndef _WINDOWS
										 	   AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG -  AUDIO_VBUF_PCM_RECORD_START_REQ",
					             RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)
								if ( rfs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
								  if ( ffs_close(((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
				                  {
					                    audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
				                  }
				                #endif
				                audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
				                          ((T_AUDIO_VBUF_PCM_RECORD_START *)(p_message))->return_path);
				                audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
				                break;
				      } // case AUDIO_VBUF_PCM_RECORD_START_REQ:
				      case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
			              {
						 AUDIO_SEND_TRACE(" AUDIO VBUF PCM RECORD MANAGER: MSG - AUDIO_VBUF_PCM_RECORD_STOP_REQ",
					             RV_TRACE_LEVEL_ERROR);
				                /* event error - send an error message, the recorded size is null*/
				                audio_vbuf_pcm_record_send_status ( AUDIO_ERROR, 0,
                            			    ((T_AUDIO_VBUF_PCM_RECORD_STOP *)(p_message))->return_path);
				                audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
				                break;
				        } // case AUDIO_VBUF_PCM_RECORD_STOP_REQ:
				      				
		          	}
			break;
	}
    } /* switch(p_audio_gbl_var->vbuf_pcm_record.state) */
  } /*********************** End of audio_vbuf_pcm_record_manager function **********************/

  #endif /*  L1_PCM_EXTRACTION */
#endif /* RVM_AUDIO_MAIN_SWE */

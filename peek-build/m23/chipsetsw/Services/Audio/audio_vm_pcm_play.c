/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_vm_pcm_play.c                                             */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            Voice Memorization play.                                      */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  24 July 2001 Create                                                     */
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

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_pcm_play_send_status                                */
  /*                                                                              */
  /*    Purpose:  This function sends the voice memorization play status          */
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
  void audio_vm_pcm_play_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_VM_PCM_PLAY_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PCM_PLAY_STATUS),
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
    ((T_AUDIO_VM_PCM_PLAY_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_VM_PCM_PLAY_STATUS_MSG;

    /* fill the status parameters */
    ((T_AUDIO_VM_PCM_PLAY_STATUS *)p_send_message)->status = status;

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
  /*    Function Name:   audio_vm_pcm_play_convert_parameter                          */
  /*                                                                              */
  /*    Purpose:  Convert the voice memorization play parameters from the         */
  /*              entity to the l1 parameters                                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        session_id of the voice memo play                                     */
  /*        id of the voice memo play                                             */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        layer 1 voice memo play message                                       */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_vm_pcm_play_convert_parameter( UINT8 channel_id,
					     T_AUDIO_VM_PCM_PLAY_START *entity_parameter,
                                        T_MMI_PCM_DOWNLOAD_START_REQ *l1_parameter)
  {
    /* session id : specifies the customer data identification corresponding to   */
    /*              this PCM voice memorization */
    l1_parameter->session_id = channel_id;
    l1_parameter->maximum_size = AUDIO_PCM_SAMPLES_PER_SEC * (entity_parameter->memo_duration);
    l1_parameter->download_ul_gain = (UINT8)(entity_parameter->download_ul_gain);
    l1_parameter->download_dl_gain = (UINT8)(entity_parameter->download_dl_gain);
//    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM L1 PARAM session_id: ", l1_parameter->session_id, RV_TRACE_LEVEL_DEBUG_LOW);
//    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM L1 PARAM maximum_size: ", l1_parameter->maximum_size, RV_TRACE_LEVEL_DEBUG_LOW);
//    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM L1 PARAM: download_ul_gain", l1_parameter->download_ul_gain, RV_TRACE_LEVEL_DEBUG_LOW);
//    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM L1 PARAM: download_dl_gain", l1_parameter->download_dl_gain, RV_TRACE_LEVEL_DEBUG_LOW);

  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_pcm_play_manager                                    */
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
  void audio_vm_pcm_play_manager(T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    T_RVF_MB_STATUS mb_status;

    /**************** audio_vm_pcm_play_manager function begins ***********************/
    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM PLAY MANAGER: STATE", p_audio_gbl_var->vm_pcm_play.state, RV_TRACE_LEVEL_DEBUG_LOW);
    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM PLAY MANAGER: RECVD MSG", p_message->msg_id, RV_TRACE_LEVEL_DEBUG_LOW);
    switch(p_audio_gbl_var->vm_pcm_play.state)
    {
      case AUDIO_VM_PCM_PLAY_IDLE:
        {
	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: STATE -  AUDIO_VM_PCM_PLAY_IDLE",
                       RV_TRACE_LEVEL_ERROR);
          switch(p_message->msg_id)
          {
             case AUDIO_VM_PCM_PLAY_START_REQ:
              {
                T_AUDIO_FFS_FLASH_2_RAM_START *p_send_message;
	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_START_REQ",
                       RV_TRACE_LEVEL_ERROR);

                /* save the task id of the entity */
                p_audio_gbl_var->vm_pcm_play.task_id = p_message->src_addr_id;

                /* save the return path */
                p_audio_gbl_var->vm_pcm_play.return_path.callback_func =
                  ((T_AUDIO_VM_PCM_PLAY_START*)(p_message))->return_path.callback_func;
                p_audio_gbl_var->vm_pcm_play.return_path.addr_id   =
                  ((T_AUDIO_VM_PCM_PLAY_START*)(p_message))->return_path.addr_id;

		
                /* Send the Start message to the FFS */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                         sizeof (T_AUDIO_FFS_FLASH_2_RAM_START),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_FLASH_2_RAM_START_REQ;

                /* fill the parameters */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->audio_ffs_fd =
                  ((T_AUDIO_VM_PCM_PLAY_START*)p_message)->audio_ffs_fd;
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->initial_size = AUDIO_VM_PCM_PLAY_SIZE;
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_PCM_PLAY;

                /* no loop with a voice memo */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->loop = FALSE;

	       /* allocate the buffer for the message to the L1 */
                p_audio_gbl_var->vm_pcm_play.p_l1_send_message =
                  audio_allocate_l1_message(sizeof(T_MMI_PCM_DOWNLOAD_START_REQ));

                if ( p_audio_gbl_var->vm_pcm_play.p_l1_send_message != NULL )
                {
                  /* Convert the entity parameters to the audio L1 parameters */
                  audio_vm_pcm_play_convert_parameter( ((T_AUDIO_FFS_RAM_2_FLASH_START *)(p_send_message))->session_id,
                          ((T_AUDIO_VM_PCM_PLAY_START*)(p_message)),
                          ((T_MMI_PCM_DOWNLOAD_START_REQ *)(p_audio_gbl_var->vm_pcm_play.p_l1_send_message)));

                }
		else
		{
		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: audio_allocate_l1_message() - FAILED",
                       RV_TRACE_LEVEL_ERROR);

		}

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

		  p_audio_gbl_var->vmemo_vbuf_session = AUDIO_VMEMO_PCM_SESSION;

                /* change to the state AUDIO_VM_PCM_PLAY_WAIT_INIT_FFS */
                p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_INIT_FFS;
                break;
              }
              case AUDIO_VM_PCM_PLAY_STOP_REQ:
              {
		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_STOP_REQ",
                       RV_TRACE_LEVEL_ERROR);
                 audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_STOP *)(p_message))->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                 break;
              } /* case AUDIO_VM_PCM_PLAY_STOP_REQ */
        }
        break;
        } /* case AUDIO_VM_PCM_PLAY_IDLE */

      case AUDIO_VM_PCM_PLAY_WAIT_INIT_FFS:
        {
	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: STATE -  AUDIO_VM_PCM_PLAY_WAIT_INIT_FFS",
                       RV_TRACE_LEVEL_ERROR);
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
              {
	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_FFS_INIT_DONE:",
                       RV_TRACE_LEVEL_ERROR);
		    ((T_MMI_PCM_DOWNLOAD_START_REQ   *)(p_audio_gbl_var->vm_pcm_play.p_l1_send_message))->session_id =
                  ((T_AUDIO_FFS_INIT *)p_message)->channel_id;

    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM L1 PARAM session_id: ",((T_MMI_PCM_DOWNLOAD_START_REQ   *)(p_audio_gbl_var->vm_pcm_play.p_l1_send_message))->session_id, RV_TRACE_LEVEL_DEBUG_LOW);
    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM L1 PARAM maximum_size: ", ((T_MMI_PCM_DOWNLOAD_START_REQ   *)(p_audio_gbl_var->vm_pcm_play.p_l1_send_message))->maximum_size, RV_TRACE_LEVEL_DEBUG_LOW);
    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM L1 PARAM: download_ul_gain", ((T_MMI_PCM_DOWNLOAD_START_REQ   *)(p_audio_gbl_var->vm_pcm_play.p_l1_send_message))->download_ul_gain, RV_TRACE_LEVEL_DEBUG_LOW);
    AUDIO_SEND_TRACE_PARAM("AUDIO VM PCM L1 PARAM: download_dl_gain", ((T_MMI_PCM_DOWNLOAD_START_REQ   *)(p_audio_gbl_var->vm_pcm_play.p_l1_send_message))->download_dl_gain, RV_TRACE_LEVEL_DEBUG_LOW);

		  AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: L1 MSG SENT -  MMI_PCM_DOWNLOAD_START_REQ",
                       RV_TRACE_LEVEL_ERROR);
		  
                /* send the start voice memo PLAY message to the L1 */
                audio_send_l1_message( MMI_PCM_DOWNLOAD_START_REQ, p_audio_gbl_var->vm_pcm_play.p_l1_send_message);

#if 0		
                DummyStruct *p_send_message;

                /* send the start voice memo play message to the L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(sizeof(T_MMI_PCM_DOWNLOAD_START_REQ));
                if ( p_send_message != NULL)
                {
                  /* Convert the entity parameters to the audio L1 parameters */
                  audio_vm_pcm_play_convert_parameter(((T_AUDIO_FFS_INIT *)p_message)->channel_id,
                  (T_AUDIO_VM_PCM_PLAY_START *)p_message,
                                                    ((T_MMI_PCM_DOWNLOAD_START_REQ *)p_send_message));

                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_PCM_DOWNLOAD_START_REQ, p_send_message);
                }
#endif
                /* change to the state AUDIO_VM_PCM_PLAY_WAIT_START_CONFIRMATION */
                p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_START_CON;
                break;
             }

             case AUDIO_VM_PCM_PLAY_STOP_REQ:
              {
			  		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_STOP_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                /*.Before stopping this task, control that            */
                /* stop task id caller = Voice Memorization task id   */
                   if ( p_audio_gbl_var->vm_pcm_play.task_id == p_message->src_addr_id)
                   {
                     /* change to the state AUDIO_VM_PCM_PLAY_WAIT_INIT_DONE */
                     p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_INIT_DONE;
                   }
                   else
                   {
                     /* A stop request from an other task is sent during a start connection */
                     /* event error - send an error message  */
                     audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_STOP *)(p_message))->return_path);
                     audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                   }
                break;
              } /* case AUDIO_VM_PCM_PLAY_STOP_REQ */
             case AUDIO_VM_PCM_PLAY_START_REQ:
              {
			  		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_START_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
					{
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }

          } /* switch */
        break;
        } /* case AUDIO_VM_PCM_PLAY_E1_WAIT_INIT_FFS */

      case AUDIO_VM_PCM_PLAY_WAIT_INIT_DONE:
        {
	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: STATE -  AUDIO_VM_PCM_PLAY_WAIT_INIT_DONE",
                       RV_TRACE_LEVEL_ERROR);
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
              {
                T_AUDIO_FFS_STOP_REQ *p_send_message;
	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_FFS_INIT_DONE:",
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
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_PCM_PLAY;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VM_PCM_PLAY_WAIT_STOP_FFS */
                p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_STOP_FFS;
              break;
              }
             case AUDIO_VM_PCM_PLAY_START_REQ:
              {
			  		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_START_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
					{
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
             case AUDIO_VM_PCM_PLAY_STOP_REQ:
              {
  		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_STOP_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                  /* event error - send an error message*/
                  audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_VM_PCM_PLAY_WAIT_INIT_DONE*/

      case AUDIO_VM_PCM_PLAY_WAIT_START_CON:
        {
  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: STATE -  AUDIO_VM_PCM_PLAY_WAIT_START_CON",
                       RV_TRACE_LEVEL_ERROR);			
          switch(p_message->msg_id)
          {
             case MMI_PCM_DOWNLOAD_START_CON:
              {
  		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  MMI_PCM_DOWNLOAD_START_CON:",
                       RV_TRACE_LEVEL_ERROR);
                /* change to the state AUDIO_VM_PCM_PLAY_WAIT_STOP_COMMAND */
                p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_STOP_COMMAND;
                break;
              }
             case AUDIO_VM_PCM_PLAY_STOP_REQ:
             { /*.Before stopping this task, control that   */
               /* stop task id caller = voice memo task id  */
			  AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_STOP_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                if ( p_audio_gbl_var->vm_pcm_play.task_id == p_message->src_addr_id)
                {
                   /* change to the state AUDIO_VM_PCM_PLAY_WAIT_STOP_CONFIRMATION */
                  p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_START_CON_TO_STOP;
                }
                else
                {
                   /* A stop request from an other task is sent during a start connection */
                   /* event error - send an error message  */
                   audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_STOP *)(p_message))->return_path);
                   audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                }
               break;
             }
             case AUDIO_VM_PCM_PLAY_START_REQ:
             {
               /* event error - send an error message*/
               /* Close the FFS file previously open by the audio start API*/
			   	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_START_REQ:",
                       RV_TRACE_LEVEL_ERROR);
               #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				 if ( ffs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
				 {
                   audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                 }
               #endif
               audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->return_path);
               audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
               break;
             }
          }
        break;
        } /* case AUDIO_VM_PCM_PLAY_WAIT_START_CON */

       case AUDIO_VM_PCM_PLAY_WAIT_START_CON_TO_STOP:
        {
  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: STATE -  AUDIO_VM_PCM_PLAY_WAIT_START_CON_TO_STOP",
                       RV_TRACE_LEVEL_ERROR);			
          switch(p_message->msg_id)
          {
             case MMI_PCM_DOWNLOAD_START_CON:
              {
                DummyStruct *p_send_message;
	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  MMI_PCM_DOWNLOAD_START_CON:",
                       RV_TRACE_LEVEL_ERROR);
                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(sizeof(T_MMI_PCM_DOWNLOAD_STOP_REQ)); //nekkareb_June_16
                if ( p_send_message != NULL)
                {
                  		  AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: L1 MSG SENT -  MMI_PCM_DOWNLOAD_STOP_REQ",
                       RV_TRACE_LEVEL_ERROR);

			 /* Convert the entity parameters to the audio L1 parameters to stop immediately */
			((T_MMI_PCM_DOWNLOAD_STOP_REQ *)p_send_message)->maximum_size =0; //nekkareb_June_16
						  
                  /* send the stop command to the audio L1 */
                  audio_send_l1_message(MMI_PCM_DOWNLOAD_STOP_REQ, p_send_message);

                }

                /* change to the state AUDIO_VM_PCM_PLAY_WAIT_STOP_CON */
                p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_STOP_CON;
               break;
              }
             case AUDIO_VM_PCM_PLAY_START_REQ:
              {
			  AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_START_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PCM_PLAY_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_PCM_PLAY_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
					{
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_START *)p_message)->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
             case AUDIO_VM_PCM_PLAY_STOP_REQ:
              {
			  		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_STOP_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                  /* event error - send an error message*/
                  audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              }

          }
        break;
        } /* case AUDIO_VM_PCM_PLAY_WAIT_START_CON_TO_STOP */

       case AUDIO_VM_PCM_PLAY_WAIT_STOP_COMMAND:
        {
  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: STATE -  AUDIO_VM_PCM_PLAY_WAIT_STOP_COMMAND",
                       RV_TRACE_LEVEL_ERROR);			
          switch(p_message->msg_id)
          {
             case MMI_PCM_DOWNLOAD_STOP_CON:
               {
                T_AUDIO_FFS_STOP_REQ *p_send_message;
	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  MMI_PCM_DOWNLOAD_STOP_CON:",
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
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_PCM_PLAY;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VM_PCM_PLAY_WAIT_STOP_FFS */
                p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_STOP_FFS;
               break;
              }

             case AUDIO_VM_PCM_PLAY_STOP_REQ:
             { /*.Before stopping this task, control that stop task id caller = Voice task id                    */
 		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_STOP_REQ:",
                       RV_TRACE_LEVEL_ERROR);
               if ( p_audio_gbl_var->vm_pcm_play.task_id == p_message->src_addr_id)
               {
                   DummyStruct *p_send_message;

                   /* send the stop command to the audio L1 */
                   /* allocate the buffer for the message to the L1 */
                   p_send_message = audio_allocate_l1_message(sizeof(T_MMI_PCM_DOWNLOAD_STOP_REQ)); //nekkareb_June_16
                  if (p_send_message != NULL)
                  {
                 	 AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: L1 MSG SENT -  MMI_PCM_DOWNLOAD_STOP_REQ",
					 	                       RV_TRACE_LEVEL_ERROR);

			 /* Convert the entity parameters to the audio L1 parameters to stop immediately */
			((T_MMI_PCM_DOWNLOAD_STOP_REQ *)p_send_message)->maximum_size =0; //nekkareb_June_16
					 
                     /* send the stop command to the audio L1 */
                     audio_send_l1_message(MMI_PCM_DOWNLOAD_STOP_REQ, p_send_message);

                   }

                  /* change to the state AUDIO_VM_PCM_PLAY_WAIT_STOP_CONFIRMATION */
                   p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_STOP_CON;
               }
               else
               {
                 /* A stop request from an other task is sent during a start connection */
                 /* event error - send an error message  */
                 audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_STOP *)p_message)->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
               }
             break;
             }
             case AUDIO_VM_PCM_PLAY_START_REQ:
               {
			  AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_START_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                 /* A start request from an other task is sent during a start connection */
                 /* event error - send an error message  */
                 /* Close the FFS file previously open by the audio start API*/
                 #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PCM_PLAY_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
				   if ( ffs_close(((T_AUDIO_VM_PCM_PLAY_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
				   {
                     audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                   }
                 #endif
                 audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_START *)p_message)->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
               }
          }
        break;
        } /* case AUDIO_VM_PCM_PLAY_WAIT_STOP_COMMAND */

        case AUDIO_VM_PCM_PLAY_WAIT_STOP_CON:
        {
  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: STATE -  AUDIO_VM_PCM_PLAY_WAIT_STOP_CON",
                       RV_TRACE_LEVEL_ERROR);			
          switch(p_message->msg_id)
          {
             case MMI_PCM_DOWNLOAD_STOP_CON:
              {
                T_AUDIO_FFS_STOP_REQ *p_send_message;
	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  MMI_PCM_DOWNLOAD_STOP_CON:",
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
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_PCM_PLAY;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_VM_PCM_PLAY_WAIT_STOP_FFS */
                 p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_WAIT_STOP_FFS;
               break;
              }
              case AUDIO_VM_PCM_PLAY_STOP_REQ:
                {
                  /* event error - send an error message*/
			 	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_STOP_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                  audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_STOP *)p_message)->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
                }
              case AUDIO_VM_PCM_PLAY_START_REQ:
                {
                  /* event error - send an error message*/
                 /* Close the FFS file previously open by the audio start API*/
			 	  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_START_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PCM_PLAY_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_PCM_PLAY_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
					{
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_START *)p_message)->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
                }

          }
        break;
        } /* case AUDIO_VM_PCM_PLAY_WAIT_STOP_CON */

        case AUDIO_VM_PCM_PLAY_WAIT_STOP_FFS:
        {
  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: STATE -  AUDIO_VM_PCM_PLAY_WAIT_STOP_FFS",
                       RV_TRACE_LEVEL_ERROR);			
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_STOP_CON:
              {
			  		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_FFS_STOP_CON:",
                       RV_TRACE_LEVEL_ERROR);
                audio_vm_pcm_play_send_status (AUDIO_OK, p_audio_gbl_var->vm_pcm_play.return_path);

		  p_audio_gbl_var->vmemo_vbuf_session = 0;
                /* change to the state AUDIO_VM_PCM_PLAY_IDLE */
                p_audio_gbl_var->vm_pcm_play.state = AUDIO_VM_PCM_PLAY_IDLE;

                break;
              }
             case AUDIO_VM_PCM_PLAY_STOP_REQ:
              {
			  		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_STOP_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                /* event error - send an error message*/
                audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_STOP *)(p_message))->return_path);
                audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              }
             case AUDIO_VM_PCM_PLAY_START_REQ:
              {
  		  	AUDIO_SEND_TRACE(" AUDIO VM PCM PLAY MANAGER: MSG -  AUDIO_VM_PCM_PLAY_START_REQ:",
                       RV_TRACE_LEVEL_ERROR);
                /* event error - send an error message*/
                /* Close the FFS file previously open by the audio start API*/
                #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				  if ( ffs_close(((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
				  {
                    audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                  }
                #endif
                audio_vm_pcm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PCM_PLAY_START *)(p_message))->return_path);
                audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_VM_PCM_PLAY_WAIT_STOP_CON */
    } /* switch(p_audio_gbl_var->vm_pcm_play.state) */
  } /*********************** End of audio_vm_pcm_play_manager function **********************/

  #endif /*  L1_PCM_EXTRACTION */
#endif /* RVM_AUDIO_MAIN_SWE */

/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_vm_play.c                                             */
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

  #if (VOICE_MEMO)
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
  /*    Function Name:   audio_vm_play_send_status                                */
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
  void audio_vm_play_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x
    audio_tones_speaker_send_status_common (status, return_path, AUDIO_VM_PLAY_STATUS_MSG);
#else
    T_AUDIO_VM_PLAY_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PLAY_STATUS),
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
    ((T_AUDIO_VM_PLAY_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_VM_PLAY_STATUS_MSG;

    /* fill the status parameters */
    ((T_AUDIO_VM_PLAY_STATUS *)p_send_message)->status = status;

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
#endif
  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_play_convert_parameter                          */
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
  void audio_vm_play_convert_parameter( UINT8 channel_id,
                                        T_MMI_VM_PLAY_REQ *l1_parameter)
  {
    /* session id : specifies the customer data identification corresponding to   */
    /*              this voice memorization */
    l1_parameter->session_id = channel_id;
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_play_wait_init_stop_common                      */
  /*                                                                              */
  /*    Purpose:  This function is called to handle cases						  */
  /*              AUDIO_VM_PLAY_WAIT_INIT_DONE                                    */
  /*              AUDIO_VM_PLAY_WAIT_STOP_CON                                     */
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

#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x//line 75
//new function for clone code removal
static void audio_vm_play_wait_init_stop_common(T_RV_HDR *p_message, T_RVF_MB_STATUS *mb_status)
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
			 case MMI_VM_PLAY_STOP_CON:
              {
                T_AUDIO_FFS_STOP_REQ *p_send_message;

                *mb_status = rvf_alloc_buf_audio_class1 (sizeof (T_AUDIO_FFS_STOP_REQ),
					 AUDIO_FFS_STOP_REQ,
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_PLAY;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VM_PLAY_WAIT_STOP_FFS */
                p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_STOP_FFS;
              break;
              }
             case AUDIO_VM_PLAY_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
                    if ( ffs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
             case AUDIO_VM_PLAY_STOP_REQ:
              {
                  /* event error - send an error message*/
                  audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              }
          }
        }
#endif

#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x//line 129
	extern void audio_ffs_stop_send (T_RVF_MB_STATUS *, UINT8);
#endif


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_play_manager                                    */
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
  void audio_vm_play_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    T_RVF_MB_STATUS mb_status;

    /**************** audio_vm_play_manager function begins ***********************/

    switch(p_audio_gbl_var->vm_play.state)
    {
      case AUDIO_VM_PLAY_IDLE:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_VM_PLAY_START_REQ:
              {
                T_AUDIO_FFS_FLASH_2_RAM_START *p_send_message;

                /* save the task id of the entity */
                p_audio_gbl_var->vm_play.task_id = p_message->src_addr_id;

                /* save the return path */
                p_audio_gbl_var->vm_play.return_path.callback_func =
                  ((T_AUDIO_VM_PLAY_START*)(p_message))->return_path.callback_func;
                p_audio_gbl_var->vm_play.return_path.addr_id   =
                  ((T_AUDIO_VM_PLAY_START*)(p_message))->return_path.addr_id;

#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x
                mb_status = rvf_alloc_buf_audio_class1 (sizeof (T_AUDIO_FFS_FLASH_2_RAM_START),
					 AUDIO_FFS_FLASH_2_RAM_START_REQ,
                                         (T_RVF_BUFFER **) (&p_send_message));
#else
                /* Send the Start message to the FFS */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                                         sizeof (T_AUDIO_FFS_FLASH_2_RAM_START),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_FLASH_2_RAM_START_REQ;
#endif

                /* fill the parameters */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->audio_ffs_fd =
                  ((T_AUDIO_VM_PLAY_START*)p_message)->audio_ffs_fd;
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->initial_size = AUDIO_VM_PLAY_SIZE;
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_PLAY;

                /* no loop with a voice memo */
                ((T_AUDIO_FFS_FLASH_2_RAM_START *)p_send_message)->loop = FALSE;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VM_PLAY_WAIT_INIT_FFS */
                p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_INIT_FFS;
                break;
              }
              case AUDIO_VM_PLAY_STOP_REQ:
              {
                 audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_STOP *)(p_message))->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                 break;
              } /* case AUDIO_VM_PLAY_STOP_REQ */
        }
        break;
        } /* case AUDIO_VM_PLAY_IDLE */

      case AUDIO_VM_PLAY_WAIT_INIT_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
              {
                DummyStruct *p_send_message;

                /* send the start voice memo play message to the L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(sizeof(T_MMI_VM_PLAY_REQ));
                if ( p_send_message != NULL)
                {
                  /* Convert the entity parameters to the audio L1 parameters */
                  audio_vm_play_convert_parameter(((T_AUDIO_FFS_INIT *)p_message)->channel_id,
                                                    ((T_MMI_VM_PLAY_REQ *)p_send_message));

                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_VM_PLAY_START_REQ, p_send_message);
                }

                /* change to the state AUDIO_VM_PLAY_WAIT_START_CONFIRMATION */
                p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_START_CON;
                break;
             }

             case AUDIO_VM_PLAY_STOP_REQ:
              {
                /*.Before stopping this task, control that            */
                /* stop task id caller = Voice Memorization task id   */
                   if ( p_audio_gbl_var->vm_play.task_id == p_message->src_addr_id)
                   {
                     /* change to the state AUDIO_VM_PLAY_WAIT_INIT_DONE */
                     p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_INIT_DONE;
                   }
                   else
                   {
                     /* A stop request from an other task is sent during a start connection */
                     /* event error - send an error message  */
                     audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_STOP *)(p_message))->return_path);
                     audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                   }
                break;
              } /* case AUDIO_VM_PLAY_STOP_REQ */
             case AUDIO_VM_PLAY_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }

          } /* switch */
        break;
        } /* case AUDIO_VM_PLAY_E1_WAIT_INIT_FFS */

      case AUDIO_VM_PLAY_WAIT_INIT_DONE:
        {
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x//line 75
          audio_vm_play_wait_init_stop_common(p_message, &mb_status);
#else
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
              {
                T_AUDIO_FFS_STOP_REQ *p_send_message;

                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
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
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_PLAY;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VM_PLAY_WAIT_STOP_FFS */
                p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_STOP_FFS;
              break;
              }
             case AUDIO_VM_PLAY_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
             case AUDIO_VM_PLAY_STOP_REQ:
              {
                  /* event error - send an error message*/
                  audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              }
          }
#endif//TI_BSP_TO_BE_BASELINED_FOR_N12N5x
        break;
        } /* case AUDIO_VM_PLAY_WAIT_INIT_DONE*/

      case AUDIO_VM_PLAY_WAIT_START_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_VM_PLAY_START_CON:
              {
                /* change to the state AUDIO_VM_PLAY_WAIT_STOP_COMMAND */
                p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_STOP_COMMAND;
                break;
              }
             case AUDIO_VM_PLAY_STOP_REQ:
             { /*.Before stopping this task, control that   */
               /* stop task id caller = voice memo task id  */
                if ( p_audio_gbl_var->vm_play.task_id == p_message->src_addr_id)
                {
                   /* change to the state AUDIO_VM_PLAY_WAIT_STOP_CONFIRMATION */
                  p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_START_CON_TO_STOP;
                }
                else
                {
                   /* A stop request from an other task is sent during a start connection */
                   /* event error - send an error message  */
                   audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_STOP *)(p_message))->return_path);
                   audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                }
               break;
             }
             case AUDIO_VM_PLAY_START_REQ:
             {
               /* event error - send an error message*/
               /* Close the FFS file previously open by the audio start API*/
               #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				 if ( ffs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                 {
                   audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                 }
               #endif
               audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_START *)(p_message))->return_path);
               audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
               break;
             }
          }
        break;
        } /* case AUDIO_VM_PLAY_WAIT_START_CON */

       case AUDIO_VM_PLAY_WAIT_START_CON_TO_STOP:
        {
          switch(p_message->msg_id)
          {
             case MMI_VM_PLAY_START_CON:
              {
                DummyStruct *p_send_message;

                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(0);
                if ( p_send_message != NULL)
                {
                  /* send the stop command to the audio L1 */
                  audio_send_l1_message(MMI_VM_PLAY_STOP_REQ, p_send_message);
                }

                /* change to the state AUDIO_VM_PLAY_WAIT_STOP_CON */
                p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_STOP_CON;
               break;
              }
             case AUDIO_VM_PLAY_START_REQ:
              {
                  /* event error - send an error message*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PLAY_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_PLAY_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_START *)p_message)->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
             case AUDIO_VM_PLAY_STOP_REQ:
              {
                  /* event error - send an error message*/
                  audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              }

          }
        break;
        } /* case AUDIO_VM_PLAY_WAIT_START_CON_TO_STOP */

       case AUDIO_VM_PLAY_WAIT_STOP_COMMAND:
        {
          switch(p_message->msg_id)
          {
             case MMI_VM_PLAY_STOP_CON:
               {
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x//line 129
				audio_ffs_stop_send (&mb_status, AUDIO_FFS_SESSION_VM_PLAY);
#else
                T_AUDIO_FFS_STOP_REQ *p_send_message;

                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
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
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_PLAY;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VM_PLAY_WAIT_STOP_FFS */
                p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_STOP_FFS;
#endif//TI_BSP_TO_BE_BASELINED_FOR_N12N5x
               break;
              }

             case AUDIO_VM_PLAY_STOP_REQ:
             { /*.Before stopping this task, control that stop task id caller = Voice task id                    */
               if ( p_audio_gbl_var->vm_play.task_id == p_message->src_addr_id)
               {
                   DummyStruct *p_send_message;

                   /* send the stop command to the audio L1 */
                   /* allocate the buffer for the message to the L1 */
                   p_send_message = audio_allocate_l1_message(0);
                  if (p_send_message != NULL)
                  {
                     /* send the stop command to the audio L1 */
                     audio_send_l1_message(MMI_VM_PLAY_STOP_REQ, p_send_message);
                   }

                  /* change to the state AUDIO_VM_PLAY_WAIT_STOP_CONFIRMATION */
                   p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_STOP_CON;
               }
               else
               {
                 /* A stop request from an other task is sent during a start connection */
                 /* event error - send an error message  */
                 audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_STOP *)p_message)->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
               }
             break;
             }
             case AUDIO_VM_PLAY_START_REQ:
               {
                 /* A start request from an other task is sent during a start connection */
                 /* event error - send an error message  */
                 /* Close the FFS file previously open by the audio start API*/
                 #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PLAY_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
				   if ( ffs_close(((T_AUDIO_VM_PLAY_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                   {
                     audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                   }
                 #endif
                 audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_START *)p_message)->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
               }
          }
        break;
        } /* case AUDIO_VM_PLAY_WAIT_STOP_COMMAND */

        case AUDIO_VM_PLAY_WAIT_STOP_CON:
        {
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x//line 75
          audio_vm_play_wait_init_stop_common(p_message, &mb_status);
#else
          switch(p_message->msg_id)
          {
             case MMI_VM_PLAY_STOP_CON:
              {
                T_AUDIO_FFS_STOP_REQ *p_send_message;

                /* Send FFS stop command */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
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
                ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_PLAY;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_VM_PLAY_WAIT_STOP_FFS */
                 p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_STOP_FFS;
               break;
              }
              case AUDIO_VM_PLAY_STOP_REQ:
                {
                  /* event error - send an error message*/
                  audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_STOP *)p_message)->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
                }
              case AUDIO_VM_PLAY_START_REQ:
                {
                  /* event error - send an error message*/
                 /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_PLAY_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_PLAY_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_START *)p_message)->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
                }

          }
#endif//TI_BSP_TO_BE_BASELINED_FOR_N12N5x
        break;
        } /* case AUDIO_VM_PLAY_WAIT_STOP_CON */

        case AUDIO_VM_PLAY_WAIT_STOP_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_STOP_CON:
              {
                audio_vm_play_send_status (AUDIO_OK, p_audio_gbl_var->vm_play.return_path);

                /* change to the state AUDIO_VM_PLAY_IDLE */
                p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_IDLE;

                break;
              }
             case AUDIO_VM_PLAY_STOP_REQ:
              {
                /* event error - send an error message*/
                audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_STOP *)(p_message))->return_path);
                audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              }
             case AUDIO_VM_PLAY_START_REQ:
              {
                /* event error - send an error message*/
                /* Close the FFS file previously open by the audio start API*/
                #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				if ( rfs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				  if ( ffs_close(((T_AUDIO_VM_PLAY_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                  {
                    audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                  }
                #endif
                audio_vm_play_send_status (AUDIO_ERROR, ((T_AUDIO_VM_PLAY_START *)(p_message))->return_path);
                audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_VM_PLAY_WAIT_STOP_CON */
    } /* switch(p_audio_gbl_var->vm_play.state) */
  } /*********************** End of audio_vm_play_manager function **********************/

  #endif /* VM_PLAY */
#endif /* RVM_AUDIO_MAIN_SWE */



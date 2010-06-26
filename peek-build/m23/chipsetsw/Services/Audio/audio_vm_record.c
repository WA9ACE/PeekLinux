/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_vm_record.c                                           */
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

  #if (VOICE_MEMO)
  #include <math.h>
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
  /*    Function Name:   audio_vm_record_send_status                              */
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
  extern void audio_vm_vm_amr_record_send_status (T_AUDIO_RET status,
                                        UINT32 recorded_size,
                                        T_RV_RETURN return_path, UINT16 hdr_msg_id);

  void audio_vm_record_send_status ( T_AUDIO_RET status,
                                     UINT32 recorded_size,
                                     T_RV_RETURN return_path)
  {
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x
    audio_vm_vm_amr_record_send_status (status, recorded_size, return_path, AUDIO_VM_RECORD_STATUS_MSG);
#else
    T_AUDIO_VM_RECORD_STATUS *p_send_message;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_RECORD_STATUS),
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
    ((T_AUDIO_VM_RECORD_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_VM_RECORD_STATUS_MSG;

    /* fill the status parameters */
    ((T_AUDIO_VM_RECORD_STATUS *)p_send_message)->status = status;
    if (recorded_size == 0)
    {
      ((T_AUDIO_VM_RECORD_STATUS *)p_send_message)->recorded_duration = 0;
    }
    else
    {
      ((T_AUDIO_VM_RECORD_STATUS *)p_send_message)->recorded_duration = (UINT16)((recorded_size - 1) / 1000);
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
#endif
  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_record_convert_parameter                        */
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
void audio_vm_record_convert_parameter( UINT8 channel_id,
                                          T_AUDIO_VM_RECORD_START *entity_parameter,
                                          T_MMI_VM_RECORD_REQ *l1_parameter)
  {
    double frequency_index, frequency_beep, amplitude_beep, amplitude, amplitude_index;

    /* session id : specifies the customer data identification corresponding to   */
    /*              this voice memorization */
    l1_parameter->session_id = channel_id;
    l1_parameter->maximum_size =
    (entity_parameter->memo_duration)*1000 + 1;
    l1_parameter->dtx_used = (BOOL)(entity_parameter->compression_mode);
    l1_parameter->record_coeff_ul = (UINT16)(entity_parameter->microphone_gain);
    l1_parameter->record_coeff_dl = (UINT16)(entity_parameter->network_gain);

    /* Calculation of the frequency index */
    /* note: we add +0.5 to compensate the truncation */
    frequency_beep = (double)(entity_parameter->tones_parameter.tones[0].frequency_tone);
    frequency_index = (256 * cos(6.283185*(frequency_beep/8000)));
    /* Calculation of the amplitude index */
    /* note: we add +0.5 to compensate the truncation */
    amplitude_beep = (double)(entity_parameter->tones_parameter.tones[0].amplitude_tone);
    amplitude      = exp((amplitude_beep*0.115129)+(5.544625));
    amplitude_index = amplitude * sin(6.283185*(frequency_beep/8000));

    l1_parameter->d_k_x1_t0 = (UINT16)((((UINT16)(frequency_index))<<8) | ((UINT16)(amplitude_index)));

    /* Calculation of the frequency index */
    /* note: we add +0.5 to compensate the truncation */
    frequency_beep = (double)(entity_parameter->tones_parameter.tones[1].frequency_tone);
    frequency_index = (256 * cos(6.283185*(frequency_beep/8000)));
    /* Calculation of the amplitude index */
    /* note: we add +0.5 to compensate the truncation */
    amplitude_beep = (double)(entity_parameter->tones_parameter.tones[1].amplitude_tone);
    amplitude      = exp((amplitude_beep*0.115129)+(5.544625));
    amplitude_index = amplitude * sin(6.283185*(frequency_beep/8000));

    l1_parameter->d_k_x1_t1 = (UINT16)((((UINT16)(frequency_index))<<8) | ((UINT16)(amplitude_index)));

    /* Calculation of the frequency index */
    /* note: we add +0.5 to compensate the truncation */
    frequency_beep = (double)(entity_parameter->tones_parameter.tones[2].frequency_tone);
    frequency_index = (256 * cos(6.283185*(frequency_beep/8000)));
    /* Calculation of the amplitude index */
    /* note: we add +0.5 to compensate the truncation */
    amplitude_beep = (double)(entity_parameter->tones_parameter.tones[2].amplitude_tone);
    amplitude      = exp((amplitude_beep*0.115129)+(5.544625));
    amplitude_index = amplitude * sin(6.283185*(frequency_beep/8000));

    l1_parameter->d_k_x1_t2 = (UINT16)((((UINT16)(frequency_index))<<8) | ((UINT16)(amplitude_index)));

    l1_parameter->d_t0_on = (UINT16)((entity_parameter->tones_parameter.tones[0].start_tone)/20);
    l1_parameter->d_t1_on = (UINT16)((entity_parameter->tones_parameter.tones[1].start_tone)/20);
    l1_parameter->d_t2_on = (UINT16)((entity_parameter->tones_parameter.tones[2].start_tone)/20);

    l1_parameter->d_t0_off = (UINT16)((entity_parameter->tones_parameter.tones[0].stop_tone)/20);
    l1_parameter->d_t1_off = (UINT16)((entity_parameter->tones_parameter.tones[1].stop_tone)/20);
    l1_parameter->d_t2_off = (UINT16)((entity_parameter->tones_parameter.tones[2].stop_tone)/20);

    l1_parameter->d_bu_off = (UINT16)((entity_parameter->tones_parameter.frame_duration)/20);

    l1_parameter->d_se_off = (UINT16)((entity_parameter->tones_parameter.sequence_duration)/20);

    l1_parameter->d_pe_off = (UINT16)((entity_parameter->tones_parameter.period_duration)/20);

    l1_parameter->d_pe_rep = entity_parameter->tones_parameter.repetition;
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   mmi_vm_record_stop_con                                   */
  /*                                                                              */
  /*    Purpose:  this function handles case  MMI_VM_RECORD_STOP_CON              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Message to the audio entity                                           */
  /*        mb_status                                                             */
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
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x//line 109
 //new function for clone code removal
static void audio_mmi_vm_record_stop_con(T_RV_HDR *p_message, T_RVF_MB_STATUS *mb_status)
{
	T_AUDIO_FFS_STOP_REQ *p_send_message;

                *mb_status = rvf_alloc_buf_audio_class1 (sizeof (T_AUDIO_FFS_STOP_REQ),
					 AUDIO_FFS_STOP_REQ,
                                         (T_RVF_BUFFER **) (&p_send_message));

	/* fill the parameter */
	((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_RECORD;

	/* fill the recorded size */
	p_audio_gbl_var->vm_record.recorded_size = ((T_MMI_VM_RECORD_CON *)p_message)->recorded_size;

	/* send the message to the entity */
	rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId, p_send_message);

	/* change to the state AUDIO_VM_RECORD_WAIT_STOP_FFS */
	p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_STOP_FFS;
} // case MMI_VM_RECORD_STOP_CON:
#endif//TI_BSP_TO_BE_BASELINED_FOR_N12N5x

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_record_manager                                  */
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
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x//line 129
 //new function for clone code removal
	void audio_ffs_stop_send (T_RVF_MB_STATUS *mb_status, UINT8 session_id)
          {
            T_AUDIO_FFS_STOP_REQ *p_send_message;

                *mb_status = rvf_alloc_buf_audio_class1 (sizeof (T_AUDIO_FFS_STOP_REQ),
					 AUDIO_FFS_STOP_REQ,
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* fill the parameter */
            ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = session_id;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VM_RECORD_WAIT_STOP_FFS */
				if(session_id == AUDIO_FFS_SESSION_VM_RECORD)
					p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_STOP_FFS;
				else
					p_audio_gbl_var->vm_play.state = AUDIO_VM_PLAY_WAIT_STOP_FFS;
          } // case AUDIO_FFS_INIT_DONE:
#endif//TI_BSP_TO_BE_BASELINED_FOR_N12N5x

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_record_manager                                  */
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
  void audio_vm_record_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    T_RVF_MB_STATUS mb_status;

    /**************** audio_vm_record_manager function begins ***********************/

    switch(p_audio_gbl_var->vm_record.state)
    {
      case AUDIO_VM_RECORD_IDLE:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_VM_RECORD_START_REQ:
              {
            T_AUDIO_FFS_RAM_2_FLASH_START *p_send_message;

                /* save the task id of the entity */
                p_audio_gbl_var->vm_record.task_id = p_message->src_addr_id;

                /* save the return path */
                p_audio_gbl_var->vm_record.return_path.callback_func =
                  ((T_AUDIO_VM_RECORD_START*)(p_message))->return_path.callback_func;
                p_audio_gbl_var->vm_record.return_path.addr_id   =
                  ((T_AUDIO_VM_RECORD_START*)(p_message))->return_path.addr_id;

#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x
                mb_status = rvf_alloc_buf_audio_class1 (sizeof (T_AUDIO_FFS_RAM_2_FLASH_START),
					 AUDIO_FFS_RAM_2_FLASH_START_REQ,
                                         (T_RVF_BUFFER **) (&p_send_message));
#else
                /* Send the Start message to the FFS */
                /* allocate the message buffer */
                mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                                         sizeof (T_AUDIO_FFS_RAM_2_FLASH_START),
                                         (T_RVF_BUFFER **) (&p_send_message));

                /* If insufficient resources, then report a memory error and abort.               */
                if (mb_status == RVF_RED)
                {
                  audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
                }

                /* fill the header of the message */
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_send_message)->os_hdr.msg_id = AUDIO_FFS_RAM_2_FLASH_START_REQ;
#endif

                /* fill the parameters */
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_send_message)->audio_ffs_fd =
                  ((T_AUDIO_VM_RECORD_START*)(p_message))->audio_ffs_fd;
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_send_message)->initial_size =
                  AUDIO_VM_RECORD_INITIAL_SIZE;
            ((T_AUDIO_FFS_RAM_2_FLASH_START *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_RECORD;

                /* allocate the buffer for the message to the L1 */
                p_audio_gbl_var->vm_record.p_l1_send_message =
                  audio_allocate_l1_message(sizeof(T_MMI_VM_RECORD_REQ));

                if ( p_audio_gbl_var->vm_record.p_l1_send_message != NULL )
                {
                  /* Convert the entity parameters to the audio L1 parameters */
                  audio_vm_record_convert_parameter( ((T_AUDIO_FFS_RAM_2_FLASH_START *)(p_send_message))->session_id,
                          ((T_AUDIO_VM_RECORD_START*)(p_message)),
                          ((T_MMI_VM_RECORD_REQ *)(p_audio_gbl_var->vm_record.p_l1_send_message)));

                }

                /* send the message to the entity */
                rvf_send_msg ( p_audio_gbl_var->audio_ffs_addrId,
                               p_send_message);

                /* change to the state AUDIO_VM_RECORD_WAIT_INIT_FFS */
                p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_INIT_FFS;
                break;
          } /* case AUDIO_VM_RECORD_START_REQ: */

              /* A stop request is sent during a start connection */
              /* event error - send an error message, the recorded size is null  */
              case AUDIO_VM_RECORD_STOP_REQ:
              {
                 audio_vm_record_send_status ( AUDIO_ERROR, 0,
                              ((T_AUDIO_VM_RECORD_STOP *)(p_message))->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                 break;
              } /* case AUDIO_VM_RECORD_STOP_REQ */
        } /* switch(p_message->msg_id) */
        break;
        } /* case AUDIO_VM_RECORD_IDLE */

      case AUDIO_VM_RECORD_WAIT_INIT_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
              {
                /* set the channel_id */
                ((T_MMI_VM_RECORD_REQ *)p_audio_gbl_var->vm_record.p_l1_send_message)->session_id =
                  ((T_AUDIO_FFS_INIT *)p_message)->channel_id;

                /* send the start voice memo record message to the L1 */
                audio_send_l1_message( MMI_VM_RECORD_START_REQ, p_audio_gbl_var->vm_record.p_l1_send_message);

                /* change to the state AUDIO_VM_RECORD_WAIT_START_CONFIRMATION */
                p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_START_CON;
                break;
          } // case AUDIO_FFS_INIT_DONE:

             case AUDIO_VM_RECORD_STOP_REQ:
              {
                /*.Before stopping this task, control that            */
                /* stop task id caller = Voice Memorization task id   */
                   if ( p_audio_gbl_var->vm_record.task_id == p_message->src_addr_id)
                   {
                     /* Deallocate the L1 message previously allocated */
                     audio_deallocate_l1_message(p_audio_gbl_var->vm_record.p_l1_send_message);

                     /* change to the state AUDIO_VM_RECORD_WAIT_INIT_DONE */
                     p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_INIT_DONE;
                   }
                   else
                   {
                     /* A stop request from an other task is sent during a start connection */
                     /* event error - send an error message, the recorded size is null  */
                     audio_vm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VM_RECORD_STOP *)(p_message))->return_path);
                     audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                   }
                break;
              } /* case AUDIO_VM_RECORD_STOP_REQ */

             case AUDIO_VM_RECORD_START_REQ:
              {
                  /* event error - send an error message, the recorded size is null*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VM_RECORD_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VM_RECORD_START_REQ:

          } /* switch */
        break;
        } /* case AUDIO_VM_RECORD_E1_WAIT_INIT_FFS */

      case AUDIO_VM_RECORD_WAIT_INIT_DONE:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_INIT_DONE:
          {
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x
	  audio_ffs_stop_send (&mb_status, AUDIO_FFS_SESSION_VM_RECORD);
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
            ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_RECORD;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VM_RECORD_WAIT_STOP_FFS */
                p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_STOP_FFS;
#endif//TI_BSP_TO_BE_BASELINED_FOR_N12N5x
              break;
          } // case AUDIO_FFS_INIT_DONE:

             case AUDIO_VM_RECORD_START_REQ:
              {
                  /* event error - send an error message, the recorded size is null*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
			if ( rfs_close(((T_AUDIO_VM_RECORD_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
			  if ( ffs_close(((T_AUDIO_VM_RECORD_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_record_send_status ( AUDIO_ERROR, 0,
                    ((T_AUDIO_VM_RECORD_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VM_RECORD_START_REQ:

             case AUDIO_VM_RECORD_STOP_REQ:
              {
                  /* event error - send an error message, the recorded size is null*/
                  audio_vm_record_send_status ( AUDIO_ERROR, 0,
                        ((T_AUDIO_VM_RECORD_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
          } // case AUDIO_VM_RECORD_STOP_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VM_RECORD_WAIT_INIT_DONE*/

      case AUDIO_VM_RECORD_WAIT_START_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_VM_RECORD_START_CON:
              {
                /* change to the state AUDIO_VM_RECORD_WAIT_STOP_COMMAND */
                p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_STOP_COMMAND;
                break;
          } // case MMI_VM_RECORD_START_CON:

             case AUDIO_VM_RECORD_STOP_REQ:
          {
            /*.Before stopping this task, control that   */
               /* stop task id caller = voice memo task id  */
                if ( p_audio_gbl_var->vm_record.task_id == p_message->src_addr_id)
                {
                   /* change to the state AUDIO_VM_RECORD_WAIT_STOP_CONFIRMATION */
                  p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_START_CON_TO_STOP;
                }
                else
                {
                   /* A stop request from an other task is sent during a start connection */
                   /* event error - send an error message, the recorded size is null  */
                   audio_vm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VM_RECORD_STOP *)(p_message))->return_path);
                   audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                }
               break;
          } // case AUDIO_VM_RECORD_STOP_REQ:

             case AUDIO_VM_RECORD_START_REQ:
             {
               /* event error - send an error message, the recorded size is null*/
               /* Close the FFS file previously open by the audio start API*/
               #ifndef _WINDOWS
#if(AS_RFS_API == 1)
			if ( rfs_close(((T_AUDIO_VM_RECORD_START *)p_message)->audio_ffs_fd) != RFS_EOK )
#else
			  if ( ffs_close(((T_AUDIO_VM_RECORD_START *)p_message)->audio_ffs_fd) != EFFS_OK )
#endif
                 {
                   audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                 }
               #endif
               audio_vm_record_send_status ( AUDIO_ERROR, 0,
                       ((T_AUDIO_VM_RECORD_START *)p_message)->return_path);
               audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
               break;
          } // case AUDIO_VM_RECORD_START_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VM_RECORD_WAIT_START_CON */

       case AUDIO_VM_RECORD_WAIT_START_CON_TO_STOP:
        {
          switch(p_message->msg_id)
          {
             case MMI_VM_RECORD_START_CON:
              {
            DummyStruct *p_send_message;

                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(0);
                if ( p_send_message != NULL)
                {
                  /* send the stop command to the audio L1 */
                  audio_send_l1_message(MMI_VM_RECORD_STOP_REQ, p_send_message);
                }

                /* change to the state AUDIO_VM_RECORD_WAIT_STOP_CON */
                p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_STOP_CON;
               break;
          } // case MMI_VM_RECORD_START_CON:

             case AUDIO_VM_RECORD_START_REQ:
              {
                  /* event error - send an error message the recorded size is null*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_record_send_status ( AUDIO_ERROR, 0,
                          ((T_AUDIO_VM_RECORD_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VM_RECORD_START_REQ:

             case AUDIO_VM_RECORD_STOP_REQ:
              {
                  /* event error - send an error message, the recorded size is null*/
                  audio_vm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VM_RECORD_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
          } // case AUDIO_VM_RECORD_STOP_REQ:

        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VM_RECORD_WAIT_START_CON_TO_STOP */

       case AUDIO_VM_RECORD_WAIT_STOP_COMMAND:
        {
          switch(p_message->msg_id)
          {
             case MMI_VM_RECORD_STOP_CON:
               {
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x
			audio_mmi_vm_record_stop_con(p_message, &mb_status);
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
            ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->session_id = AUDIO_FFS_SESSION_VM_RECORD;

                /* fill the recorded size */
                p_audio_gbl_var->vm_record.recorded_size = ((T_MMI_VM_RECORD_CON *)p_message)->recorded_size;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                                p_send_message);

                /* change to the state AUDIO_VM_RECORD_WAIT_STOP_FFS */
                p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_STOP_FFS;
#endif
               break;
          } // case MMI_VM_RECORD_STOP_CON:

             case AUDIO_VM_RECORD_STOP_REQ:
          {
            /*.Before stopping this task, control that stop task id caller = Voice task id                    */
               if ( p_audio_gbl_var->vm_record.task_id == p_message->src_addr_id)
               {
              DummyStruct *p_send_message;

                   /* send the stop command to the audio L1 */
                   /* allocate the buffer for the message to the L1 */
                   p_send_message = audio_allocate_l1_message(0);
                  if (p_send_message != NULL)
                  {
                     /* send the stop command to the audio L1 */
                     audio_send_l1_message(MMI_VM_RECORD_STOP_REQ, p_send_message);
                   }

                  /* change to the state AUDIO_VM_RECORD_WAIT_STOP_CONFIRMATION */
                   p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_STOP_CON;
               }
               else
               {
                 /* A stop request from an other task is sent during a start connection */
                 /* event error - send an error message, the recorded size is null  */
                 audio_vm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VM_RECORD_STOP *)(p_message))->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
               }
             break;
          } // case AUDIO_VM_RECORD_STOP_REQ:

             case AUDIO_VM_RECORD_START_REQ:
               {
                 /* A start request from an other task is sent during a start connection */
                 /* event error - send an error message, the recorded size is null */
                  /* Close the FFS file previously open by the audio start API*/
                 #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				   if ( ffs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                   {
                     audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                   }
                 #endif
                 audio_vm_record_send_status ( AUDIO_ERROR, 0,
                        ((T_AUDIO_VM_RECORD_START *)(p_message))->return_path);
                 audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
          } // case AUDIO_VM_RECORD_START_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VM_RECORD_WAIT_STOP_COMMAND */

        case AUDIO_VM_RECORD_WAIT_STOP_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_VM_RECORD_STOP_CON:
              {
#ifdef TI_BSP_TO_BE_BASELINED_FOR_N12N5x
			audio_mmi_vm_record_stop_con(p_message, &mb_status);
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
                ((T_AUDIO_FFS_STOP_REQ *)(p_send_message))->os_hdr.msg_id = AUDIO_FFS_STOP_REQ;

                /* fill the parameter */
                ((T_AUDIO_FFS_STOP_REQ *)(p_send_message))->session_id = AUDIO_FFS_SESSION_VM_RECORD;

                /* fill the recorded size */
                p_audio_gbl_var->vm_record.recorded_size = ((T_MMI_VM_RECORD_CON *)p_message)->recorded_size;

                /* send the message to the entity */
                rvf_send_msg (p_audio_gbl_var->audio_ffs_addrId,
                              p_send_message);

                /* change to the state AUDIO_VM_RECORD_WAIT_STOP_FFS */
                 p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_WAIT_STOP_FFS;
#endif//TI_BSP_TO_BE_BASELINED_FOR_N12N5x
               break;
          } // case MMI_VM_RECORD_STOP_CON:

              case AUDIO_VM_RECORD_STOP_REQ:
                {
                  /* event error - send an error message, the recorded size is null*/
                  audio_vm_record_send_status (AUDIO_ERROR, 0,
                        ((T_AUDIO_VM_RECORD_STOP *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
          } // case AUDIO_VM_RECORD_STOP_REQ:

              case AUDIO_VM_RECORD_START_REQ:
                {
                  /* event error - send an error message, the recorded size is null*/
                  /* Close the FFS file previously open by the audio start API*/
                  #ifndef _WINDOWS
#if(AS_RFS_API == 1)
					if ( rfs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
					if ( ffs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                    {
                      audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                    }
                  #endif
                  audio_vm_record_send_status ( AUDIO_ERROR, 0,
                      ((T_AUDIO_VM_RECORD_START *)(p_message))->return_path);
                  audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VM_RECORD_START_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VM_RECORD_WAIT_STOP_CON */

        case AUDIO_VM_RECORD_WAIT_STOP_FFS:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_FFS_STOP_CON:
              {
                audio_vm_record_send_status ( AUDIO_OK,
                      p_audio_gbl_var->vm_record.recorded_size,
                      p_audio_gbl_var->vm_record.return_path);

                /* change to the state AUDIO_VM_RECORD_IDLE */
                p_audio_gbl_var->vm_record.state = AUDIO_VM_RECORD_IDLE;

                break;
          } // case AUDIO_FFS_STOP_CON:

             case AUDIO_VM_RECORD_STOP_REQ:
              {
                /* event error - send an error message, the recorded size is null*/
                audio_vm_record_send_status ( AUDIO_ERROR, 0,
                                ((T_AUDIO_VM_RECORD_STOP *)(p_message))->return_path);
                audio_voice_memo_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
          } // case AUDIO_VM_RECORD_STOP_REQ:

             case AUDIO_VM_RECORD_START_REQ:
              {
                /* event error - send an error message, the recorded size is null*/
                /* Close the FFS file previously open by the audio start API*/
                #ifndef _WINDOWS
#if(AS_RFS_API == 1)
				  if ( rfs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != RFS_EOK )
#else
				  if ( ffs_close(((T_AUDIO_VM_RECORD_START *)(p_message))->audio_ffs_fd) != EFFS_OK )
#endif
                  {
                    audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_NO_CLOSE);
                  }
                #endif
                audio_vm_record_send_status ( AUDIO_ERROR, 0,
                          ((T_AUDIO_VM_RECORD_START *)(p_message))->return_path);
                audio_voice_memo_error_trace(AUDIO_ERROR_START_EVENT);
                break;
          } // case AUDIO_VM_RECORD_START_REQ:
        } // switch(p_message->msg_id)
        break;
        } /* case AUDIO_VM_RECORD_WAIT_STOP_CON */
    } /* switch(p_audio_gbl_var->vm_record.state) */
  } /*********************** End of audio_vm_record_manager function **********************/

  #endif /* VM_RECORD */
#endif /* RVM_AUDIO_MAIN_SWE */



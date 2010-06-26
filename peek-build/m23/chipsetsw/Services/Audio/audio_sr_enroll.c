/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_sr_enroll.c                                           */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            enrollment of a word for the speech recognition module.       */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  09 Oct. 2001 Create                                                     */
/*                                                                          */
/*  Author                                                                  */
/*     Francois Mazard - Stephanie Gerthoux                                 */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/


#ifdef RVM_AUDIO_MAIN_SWE
#include "rv/rv_defined_swe.h"
  #ifndef _WINDOWS
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

  /* include the usefull L1 header */
  #include "l1_confg.h"

  #if (SPEECH_RECO)
    #include "rv/rv_general.h"
    #include "rvm/rvm_gen.h"
    #include "audio/audio_features_i.h"
    #include "audio/audio_ffs_i.h"
    #include "audio/audio_api.h"
    #include "audio/audio_structs_i.h"
    #include "audio/audio_var_i.h"
    #include "audio/audio_messages_i.h"
    #include "rvf/rvf_target.h"
    #include "audio/audio_const_i.h"
    #include "audio/audio_error_hdlr_i.h"
    #include "audio/audio_macro_i.h"

    /* include the usefull L1 header */
    #define BOOL_FLAG
    #define CHAR_FLAG
    #include "l1_types.h"
    #include "l1audio_cust.h"
    #include "l1audio_msgty.h"
    #include "l1audio_signa.h"
    #include "l1audio_cust.h"

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_enroll_convert_parameter                        */
  /*                                                                              */
  /*    Purpose:  Convert the speech reco enroll parameters from the entity to    */
  /*              the l1 parameters                                               */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        entity speech reco enroll message                                     */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        layer 1 speech reco enroll message                                    */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_sr_enroll_convert_parameter( T_AUDIO_SR_ENROLL_START *entity_parameter,
                                          T_MMI_SR_ENROLL_REQ *l1_parameter)
  {
    T_RVF_MB_STATUS mb_status;

    l1_parameter->database_id = 0;
    l1_parameter->word_index = 0;

    /* Check if the speech sample must be saved */
    if (entity_parameter->voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
    {
      l1_parameter->speech = TRUE;
      /* Allocate the RAM buffer for the speech samples */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_audio_ffs,
                               (SC_SR_MMI_2_L1_SPEECH_SIZE * sizeof(UINT16)),
                               (T_RVF_BUFFER **) (&(p_audio_gbl_var->speech_reco.sr_enroll.p_speech_address)));

      l1_parameter->speech_address = (UWORD16 *)p_audio_gbl_var->speech_reco.sr_enroll.p_speech_address;

      AUDIO_SEND_TRACE_PARAM(" AUDIO SR ENROLL: speech buffer allocate ",
        l1_parameter->speech_address,
        RV_TRACE_LEVEL_DEBUG_LOW);

      /* If insufficient resource, then report a memory error and abort.               */
      if (mb_status == RVF_RED)
      {
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
      }
    }
    else
    {
      l1_parameter->speech = FALSE;
    }
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_enroll_send_status                              */
  /*                                                                              */
  /*    Purpose:  This function sends the speech reco enroll status to the        */
  /*              entity.                                                         */
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
  void audio_sr_enroll_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_SR_ENROLL_STATUS *p_send_message = NULL;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_SR_ENROLL_STATUS),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }

    /*fill the header of the message */
    ((T_AUDIO_SR_ENROLL_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_SR_ENROLL_STATUS_MSG;

    if (status > 0)
    {
      status = (INT8)((-2) - status);
    }

    /* fill the status parameters */
    ((T_AUDIO_SR_ENROLL_STATUS *)p_send_message)->status = status;

    if (return_path.callback_func == NULL)
    {
      /* send the message to the entity */
      rvf_send_msg (return_path.addr_id,
                    p_send_message);
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
  /*    Function Name:   audio_sr_enroll_manager                                  */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a speech reco enrollment      */
  /*              process.                                                        */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Speech reco enroll Parameters,                                  */
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
  void audio_sr_enroll_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    DummyStruct *p_send_message;
    T_RV_RET  status;

    /**************** audio_sr_enroll_manager function begins *********************/
    switch(p_audio_gbl_var->speech_reco.sr_enroll.state)
    {
      case AUDIO_SR_ENROLL_IDLE:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_SR_ENROLL_START_REQ:
              {
                /* save the task id of the entity */
                p_audio_gbl_var->speech_reco.sr_enroll.task_id =
                  ((T_AUDIO_SR_ENROLL_START*)(p_message))->os_hdr.src_addr_id;

                /* save the return path */
                p_audio_gbl_var->speech_reco.sr_enroll.return_path.callback_func =
                  ((T_AUDIO_SR_ENROLL_START*)(p_message))->return_path.callback_func;
                p_audio_gbl_var->speech_reco.sr_enroll.return_path.addr_id =
                  ((T_AUDIO_SR_ENROLL_START*)(p_message))->return_path.addr_id;

                /* save the message parameters */
                p_audio_gbl_var->speech_reco.sr_enroll.sr_ffs_fd =
                  ((T_AUDIO_SR_ENROLL_START*)(p_message))->sr_ffs_fd;
                p_audio_gbl_var->speech_reco.sr_enroll.voice_ffs_fd =
                  ((T_AUDIO_SR_ENROLL_START*)(p_message))->voice_ffs_fd;

                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(sizeof(T_MMI_SR_ENROLL_REQ));
                if ( p_send_message != NULL)
                {
                  /* Convert the entity parameters to the audio L1 parameters */
                  audio_sr_enroll_convert_parameter((T_AUDIO_SR_ENROLL_START *)p_message,
                                                    (T_MMI_SR_ENROLL_REQ *)p_send_message);

                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_SR_ENROLL_START_REQ, p_send_message);
                }

                /* change to the state AUDIO_SR_ENROLL_WAIT_START_CONFIRMATION */
                p_audio_gbl_var->speech_reco.sr_enroll.state = AUDIO_SR_ENROLL_WAIT_START_CON;
               break;
               }

             case AUDIO_SR_ENROLL_STOP_REQ:
              {
                /* event error - send an error message*/
                audio_sr_enroll_send_status (AUDIO_ERROR, ((T_AUDIO_SR_ENROLL_STOP *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_SR_ENROLL_IDLE */

      case AUDIO_SR_ENROLL_WAIT_START_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_SR_ENROLL_START_CON:
              {
                /* change to the state AUDIO_SR_ENROLL_WAIT_STOP_COMMAND */
              p_audio_gbl_var->speech_reco.sr_enroll.state = AUDIO_SR_ENROLL_WAIT_STOP_COMMAND;
              break;
              }
             case AUDIO_SR_ENROLL_STOP_REQ:
               /*.Before stopping this task, control that                           */
               /*          stop task id caller = SR enroll task id                  */
               {
                 if ( p_audio_gbl_var->speech_reco.sr_enroll.task_id ==
                        ((T_AUDIO_SR_ENROLL_STOP*)(p_message))->os_hdr.src_addr_id)
                 {
                    /* save the return path */
                    p_audio_gbl_var->speech_reco.sr_enroll.return_path.callback_func =
                      ((T_AUDIO_SR_ENROLL_STOP*)(p_message))->return_path.callback_func;
                    p_audio_gbl_var->speech_reco.sr_enroll.return_path.addr_id =
                      ((T_AUDIO_SR_ENROLL_STOP*)(p_message))->return_path.addr_id;

                    /* change to the state AUDIO_SR_ENROLL_WAIT_STOP_CONFIRMATION */
                    p_audio_gbl_var->speech_reco.sr_enroll.state = AUDIO_SR_ENROLL_WAIT_START_CON_TO_STOP;
                 }
                 else
                 {
                   /* A stop request from an other task is sent during a start connection */
                   /* event error - send an error message  */
                   audio_sr_enroll_send_status (AUDIO_ERROR, ((T_AUDIO_SR_ENROLL_STOP *)(p_message))->return_path);

                   audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
                 }
              break;
              }
            case AUDIO_SR_ENROLL_START_REQ:
              {
                /* close the model file */
                /* close the voice sample file if it exists*/
                #ifndef _WINDOWS
                  ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->sr_ffs_fd);
                  if ( ((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
                  {
                    ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd);
                  }
                #endif
                /* event error - send an error message*/
                audio_sr_enroll_send_status (AUDIO_ERROR, ((T_AUDIO_SR_ENROLL_START *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_START_EVENT);
              break;
              }
          }
        break;
        } /* case AUDIO_SR_ENROLL_WAIT_START_CON */

       case AUDIO_SR_ENROLL_WAIT_START_CON_TO_STOP:
        {
          switch(p_message->msg_id)
          {
             case MMI_SR_ENROLL_START_CON:
              {
                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(0);
                if ( p_send_message != NULL)
                {
                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_SR_ENROLL_STOP_REQ, p_send_message);
                }

                /* change to the state AUDIO_SR_ENROLL_WAIT_STOP_CON */
                p_audio_gbl_var->speech_reco.sr_enroll.state = AUDIO_SR_ENROLL_WAIT_STOP_CON;
               break;
               }
              case AUDIO_SR_ENROLL_START_REQ:
              {
                /* close the model file */
                /* close the voice sample file if it exists*/
                #ifndef _WINDOWS
                  ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->sr_ffs_fd);
                  if ( ((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
                  {
                    ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd);
                  }
                #endif
                /* event error - send an error message*/
                audio_sr_enroll_send_status (AUDIO_ERROR, ((T_AUDIO_SR_ENROLL_START *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_START_EVENT);
              break;
              }
              case AUDIO_SR_ENROLL_STOP_REQ:
              {
                /* event error - send an error message*/
                audio_sr_enroll_send_status (AUDIO_ERROR, ((T_AUDIO_SR_ENROLL_STOP *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
              break;
              }
          }
        break;
        }

       case AUDIO_SR_ENROLL_WAIT_STOP_COMMAND:
        {
          switch(p_message->msg_id)
          {
             case MMI_SR_ENROLL_STOP_CON:
              {
                /* close the model file */
                /* close the voice sample file if it exists*/
                #ifndef _WINDOWS
                  ffs_close(p_audio_gbl_var->speech_reco.sr_enroll.sr_ffs_fd);
                  if (p_audio_gbl_var->speech_reco.sr_enroll.voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
                  {
                    ffs_close(p_audio_gbl_var->speech_reco.sr_enroll.voice_ffs_fd);
                  }
                #endif

                /* Deallocate the speech buffer if it was used */
                if (p_audio_gbl_var->speech_reco.sr_enroll.voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
                {
                  status = rvf_free_buf(p_audio_gbl_var->speech_reco.sr_enroll.p_speech_address);
                  if (status != RVF_GREEN)
                  {
                    AUDIO_SEND_TRACE("AUDIO SR ENROLL: A wrong speech buffer is deallocated",
                                   RV_TRACE_LEVEL_ERROR);
                  }
                }

                audio_sr_enroll_send_status (((T_MMI_SR_ENROLL_STOP_CON *)p_message)->error_id,
                  p_audio_gbl_var->speech_reco.sr_enroll.return_path);

                /* change to the state AUDIO_SR_ENROLL_IDLE */
                p_audio_gbl_var->speech_reco.sr_enroll.state = AUDIO_SR_ENROLL_IDLE;
               break;
              }

             case AUDIO_SR_ENROLL_STOP_REQ:
             {
              /*.Before stopping this task, control that                           */
              /*          stop task id caller = SR enroll task id                  */
              if ( p_audio_gbl_var->speech_reco.sr_enroll.task_id ==
                     ((T_AUDIO_SR_ENROLL_STOP*)(p_message))->os_hdr.src_addr_id)
              {
                /* save the return path */
                p_audio_gbl_var->speech_reco.sr_enroll.return_path.callback_func =
                  ((T_AUDIO_SR_ENROLL_STOP*)(p_message))->return_path.callback_func;
                p_audio_gbl_var->speech_reco.sr_enroll.return_path.addr_id =
                  ((T_AUDIO_SR_ENROLL_STOP*)(p_message))->return_path.addr_id;

                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(0);
                if ( p_send_message != NULL)
                {
                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_SR_ENROLL_STOP_REQ, p_send_message);
                }

                /* change to the state AUDIO_SR_ENROLL_WAIT_STOP_CONFIRMATION */
                p_audio_gbl_var->speech_reco.sr_enroll.state = AUDIO_SR_ENROLL_WAIT_STOP_CON;
              }
              else
              {
                /* A stop request from an other task is sent during a start connection */
                /* event error - send an error message  */
                audio_sr_enroll_send_status (AUDIO_ERROR, ((T_AUDIO_SR_ENROLL_STOP *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
              }
              break;
            }

            case AUDIO_SR_ENROLL_START_REQ:
              {
                /* close the model file */
                /* close the voice sample file if it exists*/
                #ifndef _WINDOWS
                  ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->sr_ffs_fd);
                  if ( ((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
                  {
                    ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd);
                  }
                #endif
                /* event error - send an error message*/
                audio_sr_enroll_send_status (AUDIO_ERROR, ((T_AUDIO_SR_ENROLL_START *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_START_EVENT);
              break;
              }
          }
        break;
        } /* case AUDIO_SR_ENROLL_WAIT_STOP_COMMAND */

        case AUDIO_SR_ENROLL_WAIT_STOP_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_SR_ENROLL_STOP_CON:
              {
                /* close the model file */
                /* close the voice sample file if it exists*/
                #ifndef _WINDOWS
                  ffs_close(p_audio_gbl_var->speech_reco.sr_enroll.sr_ffs_fd);
                  if (p_audio_gbl_var->speech_reco.sr_enroll.voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
                  {
                    ffs_close(p_audio_gbl_var->speech_reco.sr_enroll.voice_ffs_fd);
                  }
                #endif

                  /* Deallocate the speech buffer if it was used */
                if (p_audio_gbl_var->speech_reco.sr_enroll.voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
                {
                  status = rvf_free_buf(p_audio_gbl_var->speech_reco.sr_enroll.p_speech_address);
                  if (status != RVF_GREEN)
                  {
                    AUDIO_SEND_TRACE("AUDIO SR ENROLL: A wrong speech buffer is deallocated ",
                                   RV_TRACE_LEVEL_ERROR);
                  }
                }

                audio_sr_enroll_send_status (((T_MMI_SR_ENROLL_STOP_CON *)p_message)->error_id,
                  p_audio_gbl_var->speech_reco.sr_enroll.return_path);

                /* change to the state AUDIO_SR_ENROLL_IDLE */
                 p_audio_gbl_var->speech_reco.sr_enroll.state = AUDIO_SR_ENROLL_IDLE;
               break;
               }
             case AUDIO_SR_ENROLL_STOP_REQ:
              {
                /* event error - send an error message*/
                audio_sr_enroll_send_status (AUDIO_ERROR, ((T_AUDIO_SR_ENROLL_STOP *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
              break;
              }
            case AUDIO_SR_ENROLL_START_REQ:
              {
                /* close the model file */
                /* close the voice sample file if it exists*/
                #ifndef _WINDOWS
                  ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->sr_ffs_fd);
                  if ( ((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd != AUDIO_SR_NO_RECORD_SPEECH)
                  {
                    ffs_close(((T_AUDIO_SR_ENROLL_START *)(p_message))->voice_ffs_fd);
                  }
                #endif
                /* event error - send an error message*/
                audio_sr_enroll_send_status (AUDIO_ERROR, ((T_AUDIO_SR_ENROLL_START *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_START_EVENT);
              break;
              }
          }
        break;
        } /* case AUDIO_SR_ENROLL_WAIT_STOP_CON */
    } /* switch(p_audio_gbl_var->speech_reco.state) */
  } /*********************** End of audio_sr_enroll_manager function **********************/
  #endif /*  #if (SPEECH_RECO) */
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */
void dummy_function4(void )
{
;
}


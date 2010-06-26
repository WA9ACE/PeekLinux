/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_sr_reco.c                                             */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            recognition of a word for the speech recognition module.      */
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

    #include <string.h>

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_reco_convert_parameter                          */
  /*                                                                              */
  /*    Purpose:  Convert the speech reco parameters from the entity to           */
  /*              the l1 parameters                                               */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        entity speech reco message                                            */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        layer 1 speech reco message                                           */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_sr_reco_convert_parameter( T_AUDIO_SR_RECO_START *entity_parameter,
                                          T_MMI_SR_RECO_REQ *l1_parameter)
  {
    l1_parameter->database_id = 0;
    l1_parameter->vocabulary_size = entity_parameter->vocabulary_size;
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_reco_send_status                                */
  /*                                                                              */
  /*    Purpose:  This function sends the speech reco status to the               */
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
  void audio_sr_reco_send_status (T_AUDIO_RET status, UINT8 word_index, T_RV_RETURN return_path)
  {
    T_AUDIO_SR_RECO_STATUS  *p_send_message = NULL;
    T_RVF_MB_STATUS mb_status = RVF_RED;
    #ifndef _WINDOWS
      UINT8   i;
      char    *p_database;
      char    *p_word_name;
      UINT8   model_name_lengh;
    #endif

    while (mb_status == RVF_RED)
    {
     /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_SR_RECO_STATUS),
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
    ((T_AUDIO_SR_RECO_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_SR_RECO_STATUS_MSG;

    if (status > 0)
    {
      status = (INT8)((-2) - status);
    }

    if (status == SC_NO_ERROR)
    /* no error occured dring the recognition */
    {
      #ifndef _WINDOWS
        /* search the name of the recognized word */
        p_database = p_audio_gbl_var->speech_reco.sr_reco.p_database;
        for(i=0; i< word_index; i++)
        {
          p_database += AUDIO_PATH_NAME_MAX_SIZE;
        }

        /* the _sr need to be suppress */
        model_name_lengh = (strlen(p_database) - 3);

        /* copy this path in the message */
        strncpy(((T_AUDIO_SR_RECO_STATUS *)(p_send_message))->word_recognized,
          p_database,
          model_name_lengh);
        p_word_name = (char *)(((T_AUDIO_SR_RECO_STATUS *)(p_send_message))->word_recognized);
        p_word_name += model_name_lengh;
        *p_word_name = 0;
      #endif
    }

    /* fill the status parameters */
    ((T_AUDIO_SR_RECO_STATUS *)p_send_message)->status = status;

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
  /*    Function Name:   audio_sr_reco_manager                                    */
  /*                                                                              */
  /*    Purpose:  This function is called to manage a speech reco                 */
  /*              process.                                                        */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Speech reco reco Parameters,                                    */
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
  void audio_sr_reco_manager (T_RV_HDR *p_message)
  {
    /* Declare local variables.                                                   */
    DummyStruct *p_send_message;

    /**************** audio_sr_reco_manager function begins *********************/
    switch(p_audio_gbl_var->speech_reco.sr_reco.state)
    {
      case AUDIO_SR_RECO_IDLE:
        {
          switch(p_message->msg_id)
          {
             case AUDIO_SR_RECO_START_REQ:
              {
                /* save the task id of the entity */
                p_audio_gbl_var->speech_reco.sr_reco.task_id =
                  ((T_AUDIO_SR_RECO_START*)(p_message))->os_hdr.src_addr_id;

                /* save the return path */
                p_audio_gbl_var->speech_reco.sr_reco.return_path.callback_func =
                  ((T_AUDIO_SR_RECO_START*)(p_message))->return_path.callback_func;
                p_audio_gbl_var->speech_reco.sr_reco.return_path.addr_id =
                  ((T_AUDIO_SR_RECO_START*)(p_message))->return_path.addr_id;

                /* save the message parameters */
                p_audio_gbl_var->speech_reco.sr_reco.vocabulary_size =
                  ((T_AUDIO_SR_RECO_START*)(p_message))->vocabulary_size;
                p_audio_gbl_var->speech_reco.sr_reco.p_database =
                  ((T_AUDIO_SR_RECO_START*)(p_message))->p_database;

                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(sizeof(T_MMI_SR_RECO_REQ));
                if ( p_send_message != NULL)
                {
                  /* Convert the entity parameters to the audio L1 parameters */
                  audio_sr_reco_convert_parameter((T_AUDIO_SR_RECO_START *)p_message,
                                                    (T_MMI_SR_RECO_REQ *)p_send_message);

                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_SR_RECO_START_REQ, p_send_message);
                }

                /* change to the state AUDIO_SR_RECO_WAIT_START_CONFIRMATION */
                p_audio_gbl_var->speech_reco.sr_reco.state = AUDIO_SR_RECO_WAIT_START_CON;
               break;
               }

             case AUDIO_SR_RECO_STOP_REQ:
              {
                /* event error - send an error message*/
                audio_sr_reco_send_status (AUDIO_ERROR, 0,
                  ((T_AUDIO_SR_RECO_STOP *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
              }
          }
        break;
        } /* case AUDIO_SR_RECO_IDLE */

      case AUDIO_SR_RECO_WAIT_START_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_SR_RECO_START_CON:
              {
                /* change to the state AUDIO_SR_RECO_WAIT_STOP_COMMAND */
              p_audio_gbl_var->speech_reco.sr_reco.state = AUDIO_SR_RECO_WAIT_STOP_COMMAND;
              break;
              }
             case AUDIO_SR_RECO_STOP_REQ:
               /*.Before stopping this task, control that                           */
               /*          stop task id caller = SR reco task id                  */
               {
                 if ( p_audio_gbl_var->speech_reco.sr_reco.task_id ==
                        ((T_AUDIO_SR_RECO_STOP*)(p_message))->os_hdr.src_addr_id)
                 {
                    /* save the return path */
                    p_audio_gbl_var->speech_reco.sr_reco.return_path.callback_func =
                      ((T_AUDIO_SR_RECO_STOP*)(p_message))->return_path.callback_func;
                    p_audio_gbl_var->speech_reco.sr_reco.return_path.addr_id =
                      ((T_AUDIO_SR_RECO_STOP*)(p_message))->return_path.addr_id;

                    /* change to the state AUDIO_SR_RECO_WAIT_STOP_CONFIRMATION */
                    p_audio_gbl_var->speech_reco.sr_reco.state = AUDIO_SR_RECO_WAIT_START_CON_TO_STOP;
                 }
                 else
                 {
                   /* A stop request from an other task is sent during a start connection */
                   /* event error - send an error message  */
                   audio_sr_reco_send_status (AUDIO_ERROR, 0,
                     ((T_AUDIO_SR_RECO_STOP *)(p_message))->return_path);

                   audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
                 }
              break;
              }
            case AUDIO_SR_RECO_START_REQ:
              {
                /* deallocate the vocabulary data buffer */
                rvf_free_buf(
                  (T_RVF_BUFFER *)(((T_AUDIO_SR_RECO_START *)(p_message))->p_database));

                /* event error - send an error message*/
                audio_sr_reco_send_status (AUDIO_ERROR, 0,
                  ((T_AUDIO_SR_RECO_START *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_START_EVENT);
              break;
              }
          }
        break;
        } /* case AUDIO_SR_RECO_WAIT_START_CON */

       case AUDIO_SR_RECO_WAIT_START_CON_TO_STOP:
        {
          switch(p_message->msg_id)
          {
             case MMI_SR_RECO_START_CON:
              {
                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(0);
                if ( p_send_message != NULL)
                {
                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_SR_RECO_STOP_REQ, p_send_message);
                }

                /* change to the state AUDIO_SR_RECO_WAIT_STOP_CON */
                p_audio_gbl_var->speech_reco.sr_reco.state = AUDIO_SR_RECO_WAIT_STOP_CON;
               break;
               }
              case AUDIO_SR_RECO_START_REQ:
              {
                /* deallocate the vocabulary data buffer */
                rvf_free_buf(
                  (T_RVF_BUFFER *)(((T_AUDIO_SR_RECO_START *)(p_message))->p_database));

                /* event error - send an error message*/
                audio_sr_reco_send_status (AUDIO_ERROR, 0,
                  ((T_AUDIO_SR_RECO_START *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_START_EVENT);
              break;
              }
              case AUDIO_SR_RECO_STOP_REQ:
              {
                /* event error - send an error message*/
                audio_sr_reco_send_status (AUDIO_ERROR, 0,
                  ((T_AUDIO_SR_RECO_STOP *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
              break;
              }
          }
        break;
        }

       case AUDIO_SR_RECO_WAIT_STOP_COMMAND:
        {
          switch(p_message->msg_id)
          {
             case MMI_SR_RECO_STOP_CON:
              {
                /* deallocate the vocabulary data buffer */
                rvf_free_buf(
                  (T_RVF_BUFFER *)(p_audio_gbl_var->speech_reco.sr_reco.p_database));

                audio_sr_reco_send_status (((T_MMI_SR_RECO_STOP_CON *)p_message)->error_id,
                  (UINT8)((T_MMI_SR_RECO_STOP_CON *)p_message)->best_word_index,
                  p_audio_gbl_var->speech_reco.sr_reco.return_path);

                /* change to the state AUDIO_SR_RECO_IDLE */
                p_audio_gbl_var->speech_reco.sr_reco.state = AUDIO_SR_RECO_IDLE;
               break;
              }

             case AUDIO_SR_RECO_STOP_REQ:
             {
              /*.Before stopping this task, control that                           */
              /*          stop task id caller = SR reco task id                  */
              if ( p_audio_gbl_var->speech_reco.sr_reco.task_id ==
                     ((T_AUDIO_SR_RECO_STOP*)(p_message))->os_hdr.src_addr_id)
              {
                /* save the return path */
                p_audio_gbl_var->speech_reco.sr_reco.return_path.callback_func =
                  ((T_AUDIO_SR_RECO_STOP*)(p_message))->return_path.callback_func;
                p_audio_gbl_var->speech_reco.sr_reco.return_path.addr_id =
                  ((T_AUDIO_SR_RECO_STOP*)(p_message))->return_path.addr_id;

                /* send the stop command to the audio L1 */
                /* allocate the buffer for the message to the L1 */
                p_send_message = audio_allocate_l1_message(0);
                if ( p_send_message != NULL)
                {
                  /* send the start command to the audio L1 */
                  audio_send_l1_message(MMI_SR_RECO_STOP_REQ, p_send_message);
                }

                /* change to the state AUDIO_SR_RECO_WAIT_STOP_CONFIRMATION */
                p_audio_gbl_var->speech_reco.sr_reco.state = AUDIO_SR_RECO_WAIT_STOP_CON;
              }
              else
              {
                /* A stop request from an other task is sent during a start connection */
                /* event error - send an error message  */
                audio_sr_reco_send_status (AUDIO_ERROR, 0,
                  ((T_AUDIO_SR_RECO_STOP *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
              }
              break;
            }

            case AUDIO_SR_RECO_START_REQ:
              {
                /* deallocate the vocabulary data buffer */
                rvf_free_buf(
                  (T_RVF_BUFFER *)(((T_AUDIO_SR_RECO_START *)(p_message))->p_database));

                /* event error - send an error message*/
                audio_sr_reco_send_status (AUDIO_ERROR, 0,
                  ((T_AUDIO_SR_RECO_START *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_START_EVENT);
              break;
              }
          }
        break;
        } /* case AUDIO_SR_RECO_WAIT_STOP_COMMAND */

        case AUDIO_SR_RECO_WAIT_STOP_CON:
        {
          switch(p_message->msg_id)
          {
             case MMI_SR_RECO_STOP_CON:
              {
                /* deallocate the vocabulary data buffer */
                rvf_free_buf(
                  (T_RVF_BUFFER *)(p_audio_gbl_var->speech_reco.sr_reco.p_database));

                audio_sr_reco_send_status (((T_MMI_SR_RECO_STOP_CON *)p_message)->error_id,
                  (UINT8)((T_MMI_SR_RECO_STOP_CON *)p_message)->best_word_index,
                  p_audio_gbl_var->speech_reco.sr_reco.return_path);

                /* change to the state AUDIO_SR_RECO_IDLE */
                 p_audio_gbl_var->speech_reco.sr_reco.state = AUDIO_SR_RECO_IDLE;
               break;
               }
             case AUDIO_SR_RECO_STOP_REQ:
              {
                /* event error - send an error message*/
                audio_sr_reco_send_status (AUDIO_ERROR, 0,
                  ((T_AUDIO_SR_RECO_STOP *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_STOP_EVENT);
              break;
              }
            case AUDIO_SR_RECO_START_REQ:
              {
                /* deallocate the vocabulary data buffer */
                rvf_free_buf(
                  (T_RVF_BUFFER *)(((T_AUDIO_SR_RECO_START *)(p_message))->p_database));

                /* event error - send an error message*/
                audio_sr_reco_send_status (AUDIO_ERROR, 0,
                  ((T_AUDIO_SR_RECO_START *)(p_message))->return_path);

                audio_sr_error_trace(AUDIO_ERROR_START_EVENT);
              break;
              }
          }
        break;
        } /* case AUDIO_SR_RECO_WAIT_STOP_CON */
    } /* switch(p_audio_gbl_var->speech_reco.state) */
  } /*********************** End of audio_sr_reco_manager function **********************/
  #endif /*  #if (SPEECH_RECO) */
  
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */
void dummy_function3(void )
{
;
}


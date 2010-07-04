/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_tones.c                                               */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to manage the       */
/*            tones  .                                                      */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  28 May 2001 Create                                                      */
/*                                                                          */
/*  Author                                                                  */
/*     Francois Mazard - Stephanie Gerthoux                                 */
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

  #include "l1_confg.h"
  #if (TONE)
    #include <math.h>
    #include "rv/rv_general.h"
    #include "rvm/rvm_gen.h"
    #include "Audio/audio_features_i.h"
    #include "Audio/audio_ffs_i.h"
    #include "Audio/audio_api.h"
    #include "Audio/audio_structs_i.h"
    #include "Audio/audio_var_i.h"
    #include "Audio/audio_messages_i.h"
    #include "rvf/rvf_target.h"
    #include "Audio/audio_const_i.h"
    #include "Audio/audio_error_hdlr_i.h"

    /* include the usefull L1 header */
    #define BOOL_FLAG
    #define CHAR_FLAG
    #include "l1_types.h"
    #include "l1audio_cust.h"
    #include "l1audio_msgty.h"
    #include "l1audio_signa.h"

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_tones_convert_parameter                            */
  /*                                                                              */
  /*    Purpose:  Convert the tones paramters from the entity to the l1           */
  /*              parameters                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        entity tones message                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        layer 1  tones message                                                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  void audio_tones_convert_parameter( T_AUDIO_TONES_START *entity_parameter,
                                      T_MMI_TONE_REQ *l1_parameter)
  {
    double frequency_index, frequency_beep, amplitude_beep, amplitude, amplitude_index;

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
    /*    Function Name:   audio_tones_send_status                                  */
    /*                                                                              */
    /*    Purpose:  This function sends the tones status to the entity.             */
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
    void audio_tones_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
    {
      T_AUDIO_TONES_STATUS *p_send_message = NULL;
      T_RVF_MB_STATUS mb_status = RVF_RED;

      while (mb_status == RVF_RED)
      {
        /* allocate the message buffer */
        mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                                 sizeof (T_AUDIO_TONES_STATUS),
                                 (T_RVF_BUFFER **) (&p_send_message));

        /* If insufficient resources, then report a memory error and abort.               */
        /* and wait until more ressource is given */
        if (mb_status == RVF_RED)
        {
          audio_tones_error_trace(AUDIO_ENTITY_NO_MEMORY);
          rvf_delay(RVF_MS_TO_TICKS(1000));
        }
      }

      /*fill the header of the message */
      ((T_AUDIO_TONES_STATUS *)p_send_message)->os_hdr.msg_id = AUDIO_TONES_STATUS_MSG;

      /* fill the status parameters */
      ((T_AUDIO_TONES_STATUS *)p_send_message)->status = status;

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
    /*    Function Name:   audio_tones_manager                                      */
    /*                                                                              */
    /*    Purpose:  This function is called to manage a tones generation            */
    /*                                                                              */
    /*    Input Parameters:                                                         */
    /*        Audio Key Beep Parameters,                                            */
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
    void audio_tones_manager (T_RV_HDR *p_message)
    {

      /* Declare local variables.                                                   */
      DummyStruct *p_send_message;

      /**************** audio_tones_manager function begins *********************/
      switch(p_audio_gbl_var->tones.state)
      {
        case AUDIO_TONES_IDLE:
          {
            switch(p_message->msg_id)
            {
               case AUDIO_TONES_START_REQ:
                {
                  /* save the addr id of the entity */
                  p_audio_gbl_var->tones.task_id = p_message->src_addr_id;

                  /* save the return path */
                  p_audio_gbl_var->tones.return_path.callback_func = ((T_AUDIO_TONES_START*)p_message)->return_path.callback_func;
                  p_audio_gbl_var->tones.return_path.addr_id   = ((T_AUDIO_TONES_START*)p_message)->return_path.addr_id;

                  /* allocate the buffer for the message to the L1 */
                  p_send_message = audio_allocate_l1_message(sizeof(T_MMI_TONE_REQ));
                  if (p_send_message != NULL)
                  {
                    /* Convert the entity parameters to the audio L1 parameters */
                    audio_tones_convert_parameter((T_AUDIO_TONES_START *)p_message,
                                                  (T_MMI_TONE_REQ *)p_send_message);

                    /* send the start command to the audio L1 */
                    audio_send_l1_message(MMI_TONE_START_REQ, p_send_message);
                  }

                  /* change to the state AUDIO_TONES_WAIT_START_CONFIRMATION */
                  p_audio_gbl_var->tones.state = AUDIO_TONES_WAIT_START_CON;
                 break;
                 }

               case AUDIO_TONES_STOP_REQ:
                {
                  /* event error - send an error message*/
                  audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_STOP *)(p_message))->return_path);

                  audio_tones_error_trace(AUDIO_ERROR_STOP_EVENT);
                  break;
                }
            }
          break;
          } /* case AUDIO_TONES_IDLE */

        case AUDIO_TONES_WAIT_START_CON:
          {
            switch(p_message->msg_id)
            {
               case MMI_TONE_START_CON:
                {
                  /* change to the state AUDIO_TONES_WAIT_STOP_COMMAND */
                  p_audio_gbl_var->tones.state = AUDIO_TONES_WAIT_STOP_COMMAND;
                break;
                }
               case AUDIO_TONES_STOP_REQ:
                 /*.Before stopping this task, control that                           */
                 /*          stop task id caller = Tones task id                    */
                 {
                   if ( p_audio_gbl_var->tones.task_id == p_message->src_addr_id)
                   {
                      /* change to the state AUDIO_TONES_WAIT_STOP_CONFIRMATION */
                      p_audio_gbl_var->tones.state = AUDIO_TONES_WAIT_START_CON_TO_STOP;
                   }
                   else
                   {
                     /* A stop request from an other task is sent during a start connection */
                     /* event error - send an error message  */
                     audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_STOP *)(p_message))->return_path);

                     audio_tones_error_trace(AUDIO_ERROR_STOP_EVENT);
                   }
                break;
                }
              case AUDIO_TONES_START_REQ:
                {
                  /* event error - send an error message*/
                  audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_START *)(p_message))->return_path);

                  audio_tones_error_trace(AUDIO_ERROR_START_EVENT);
                break;
                }
            }
          break;
          } /* case AUDIO_TONES_WAIT_START_CON */

         case AUDIO_TONES_WAIT_START_CON_TO_STOP:
          {
            switch(p_message->msg_id)
            {
               case MMI_TONE_START_CON:
                {
                  /* allocate the buffer for the message to the L1 */
                  p_send_message = audio_allocate_l1_message(0);
                  if (p_send_message != NULL)
                  {
                    /* send the start command to the audio L1 */
                    audio_send_l1_message(MMI_TONE_STOP_REQ, p_send_message);
                  }

                  /* change to the state AUDIO_TONES_WAIT_STOP_CON */
                  p_audio_gbl_var->tones.state = AUDIO_TONES_WAIT_STOP_CON;
                 break;
                 }
                case AUDIO_TONES_START_REQ:
                {
                  /* event error - send an error message*/
                  audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_START *)(p_message))->return_path);

                  audio_tones_error_trace(AUDIO_ERROR_START_EVENT);
                break;
                }
                case AUDIO_TONES_STOP_REQ:
                {
                  /* event error - send an error message*/
                  audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_STOP *)(p_message))->return_path);

                  audio_tones_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
                }
            }
          break;
          }

         case AUDIO_TONES_WAIT_STOP_COMMAND:
          {
            switch(p_message->msg_id)
            {
               case MMI_TONE_STOP_CON:
                {
                  audio_tones_send_status (AUDIO_OK, p_audio_gbl_var->tones.return_path);

                  /* change to the state AUDIO_TONES_IDLE */
                  p_audio_gbl_var->tones.state = AUDIO_TONES_IDLE;
                 break;
                }

               case AUDIO_TONES_STOP_REQ:
               {
                 /* A stop request is sent during a stop connection                        */
                 /* Save the addr id of the entity */
                 p_audio_gbl_var->tones.task_id = p_message->src_addr_id;

                 /*.Control that (stop task id caller = tones task id) and stop the task */
                 if ( p_audio_gbl_var->tones.task_id == p_message->src_addr_id)
                 {
                   /* allocate the buffer for the message to the L1 */
                   p_send_message = audio_allocate_l1_message(0);
                   if (p_send_message != NULL)
                   {
                     /* send the start command to the audio L1 */
                     audio_send_l1_message(MMI_TONE_STOP_REQ, p_send_message);
                   }

                   /* change to the state AUDIO_TONES_WAIT_STOP_CONFIRMATION */
                   p_audio_gbl_var->tones.state = AUDIO_TONES_WAIT_STOP_CON;
                 }
                 else
                 {
                   /* A stop request from an other task is sent during a stop connection */
                   /* event error - send an error message  */
                   audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_STOP *)(p_message))->return_path);

                   audio_tones_error_trace(AUDIO_ERROR_STOP_EVENT);
                 }
                break;
                }

              case AUDIO_TONES_START_REQ:
                {
                  /* event error - send an error message*/
                  audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_START *)(p_message))->return_path);

                  audio_tones_error_trace(AUDIO_ERROR_START_EVENT);
                break;
                }
            }
          break;
          } /* case AUDIO_TONES_WAIT_STOP_COMMAND */

          case AUDIO_TONES_WAIT_STOP_CON:
          {
            switch(p_message->msg_id)
            {
               case MMI_TONE_STOP_CON:
                {
                  audio_tones_send_status (AUDIO_OK, p_audio_gbl_var->tones.return_path);

                  /* change to the state AUDIO_TONES_IDLE */
                   p_audio_gbl_var->tones.state = AUDIO_TONES_IDLE;
                 break;
                 }
               case AUDIO_TONES_STOP_REQ:
                {
                  /* event error - send an error message*/
                  audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_STOP *)(p_message))->return_path);

                  audio_tones_error_trace(AUDIO_ERROR_STOP_EVENT);
                break;
                }
              case AUDIO_TONES_START_REQ:
                {
                  /* event error - send an error message*/
                  audio_tones_send_status (AUDIO_ERROR, ((T_AUDIO_TONES_START *)(p_message))->return_path);

                  audio_tones_error_trace(AUDIO_ERROR_START_EVENT);
                break;
                }
            }
          break;
          } /* case AUDIO_TONES_WAIT_STOP_CON */

      } /* switch(p_audio_gbl_var->tones.state) */
    } /*********************** End of audio_tones_manager function **********************/

  #endif /* #if (TONE) */
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */

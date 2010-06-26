/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_mode_full_write.c                                     */
/*                                                                          */
/*  Purpose:  This file contains all the functions used for audio mode full */
/*            write services.                                               */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------------------------------------------*/
/*  14 Jan 2002  Create                                                     */
/*                                                                          */
/*  Author      Francois Mazard                                             */
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
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
  #include "audio/audio_ffs_i.h"
  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "audio/audio_macro_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"

  /* include the usefull L1 header */
  #ifdef _WINDOWS
    #define BOOL_FLAG
    //#define CHAR_FLAG
  #endif
  #include "l1_types.h"
  #include "l1audio_const.h"
  #include "l1audio_cust.h"
  #include "l1audio_defty.h"
  #include "l1audio_msgty.h"
  #include "l1audio_signa.h"
  #if TESTMODE
   #include "l1tm_defty.h"
  #endif
  #if (L1_GTT == 1)
    #include "l1gtt_const.h"
    #include "l1gtt_defty.h"
  #endif
  #include "l1_const.h"
  #if (L1_MP3 == 1)
    #include "l1mp3_defty.h"
  #endif
  #if (L1_AAC == 1)
    #include "l1aac_defty.h"
  #endif
  #include "l1_defty.h"
  #include "l1_msgty.h"
  #include "l1_signa.h"
  #ifdef _WINDOWS
    #define L1_ASYNC_C
  #endif
  #include "l1_varex.h"

  #include "ffs/ffs_api.h"

  #ifndef _WINDOWS
    #include "l1audio_cust.h"
  #endif

  /* external dependency */
 #if (AUDIO_DSP_FEATURES == 1)
  extern T_AUDIO_RET audio_mode_volume_speed_write                      (INT16  *data);
  extern T_AUDIO_RET audio_mode_microphone_gain_write                   (INT16  *data);
  extern T_AUDIO_RET audio_mode_speaker_gain_write                      (INT16  *data);
  extern T_AUDIO_RET audio_mode_sidetone_gain_write                     (INT16  *data);
 #else
  extern T_AUDIO_RET audio_mode_microphone_mode_write          (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_gain_write                   (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_extra_gain_write    (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_output_bias_write   (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_mode_write             (INT8  *data);
  #if(AS_FM_RADIO==1)
  extern T_AUDIO_RET audio_mode_fm_mode_write 			   (UINT8  *data);
  #endif
  extern T_AUDIO_RET audio_mode_speaker_gain_write                      (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_extra_gain_write       (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_filter_write           (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_highpass_filter_write  (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_buzzer_write           (INT8  *data);
  extern T_AUDIO_RET audio_mode_sidetone_gain_write                     (INT8  *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_mode_write          (INT8  *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_stereo_mono_write   (INT8  *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_sampling_freq_write (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_volume_write           (T_AUDIO_SPEAKER_LEVEL *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_volume_write    (T_AUDIO_STEREO_SPEAKER_LEVEL *data);
 #endif

  extern T_AUDIO_RET audio_mode_voice_path_write               (T_AUDIO_VOICE_PATH_SETTING *data,
                                                                UINT8 *message_to_confirm);
  #if (L1_AEC == 1)
      T_AUDIO_RET audio_mode_aec_write                      (T_AUDIO_AEC_CFG *data);
  #endif
  #if (L1_AEC== 2)
      T_AUDIO_RET audio_mode_aec_write                      (T_AUDIO_AQI_AEC_CFG *data);
  #endif
  extern T_AUDIO_RET audio_mode_speaker_microphone_fir_write   (T_AUDIO_FIR_COEF *data_speaker,
                                                                T_AUDIO_FIR_COEF *data_microphone);
  #if (L1_ANR == 1)
    extern T_AUDIO_RET audio_mode_microphone_anr_write         (T_AUDIO_ANR_CFG *data);
  #endif
  #if (L1_ANR == 2)
    extern T_AUDIO_RET audio_mode_microphone_anr_write         (T_AUDIO_AQI_ANR_CFG *data);
  #endif
  #if (L1_ES == 1)
    extern T_AUDIO_RET audio_mode_microphone_es_write          (T_AUDIO_ES_CFG *data);
  #endif
  #if (L1_IIR == 1)
    extern T_AUDIO_RET audio_mode_speaker_iir_write            (T_AUDIO_IIR_CFG *data);
  #elif (L1_IIR == 2)
    extern T_AUDIO_RET audio_mode_speaker_iir_write            (T_AUDIO_IIR_DL_CFG *data);
  #endif
  #if (L1_AGC_UL == 1)
    extern audio_mode_agc_ul_write                             (T_AUDIO_AQI_AGC_UL_REQ *data);
  #endif
  #if (L1_AGC_DL == 1)
    extern audio_mode_agc_dl_write                             (T_AUDIO_AQI_AGC_DL_REQ *data);  
  #endif
  #if (L1_DRC == 1)
    extern T_AUDIO_RET audio_mode_speaker_drc_write            (T_AUDIO_DRC_CFG *data);
  #endif
  #if (L1_LIMITER == 1)
    extern T_AUDIO_RET audio_mode_speaker_limiter_write        (T_AUDIO_LIMITER_CFG *data);
  #endif



 #if (AUDIO_DSP_ONOFF == 1)
  extern T_AUDIO_RET audio_mode_onoff_write                             (INT8  *data);
 #endif

 #if (L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
  extern T_AUDIO_RET audio_mode_onoff_write                             (T_AUDIO_MODE_ONOFF  *data);
 #endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_full_access_write_send_status                 */
  /*                                                                              */
  /*    Purpose:  This function sends the full acess write status to the entity.  */
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
  void audio_mode_full_access_write_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_FULL_ACCESS_WRITE_DONE *p_send_message = NULL;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_FULL_ACCESS_WRITE_DONE),
                               (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.               */
      /* and wait until more ressource is given */
      if (mb_status == RVF_RED)
      {
        audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
        rvf_delay(RVF_MS_TO_TICKS(1000));
      }
    }
    /*fill the header of the message */
    ((T_AUDIO_FULL_ACCESS_WRITE_DONE *)p_send_message)->os_hdr.msg_id =
      AUDIO_FULL_ACCESS_WRITE_DONE;

    /* fill the status parameters */
    ((T_AUDIO_FULL_ACCESS_WRITE_DONE *)p_send_message)->status = status;

    if (return_path.callback_func == NULL)
    {
     if (return_path.addr_id != 0 )
      /* send the message to the entity */
      rvf_send_msg (return_path.addr_id, p_send_message);
      else
         rvf_free_buf((T_RVF_BUFFER *)p_send_message);

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
  /*    Function Name:   audio_mode_full_access_write_manager                     */
  /*                                                                              */
  /*    Purpose:  This function manage the audio mode full write services.        */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio message.                                                        */
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
  void audio_mode_full_access_write_manager (T_RV_HDR *p_message)
  {
    T_AUDIO_MODE_FULL_ACCESS_WRITE_REQ *p_parameter;
    T_RVF_MB_STATUS     status_free_buf;
    T_AUDIO_RET   status = AUDIO_ERROR;
    UINT8         message_to_confirm;

    p_parameter = ((T_AUDIO_MODE_FULL_ACCESS_WRITE_REQ *)p_message);

    switch (p_audio_gbl_var->audio_mode_var.full_access_write_var.state)
    {
      case AUDIO_MODE_FULL_WRITE_IDLE:
      {
        // All variable_identifiers are valid because there was a check in audio_api.c
        switch (p_parameter->audio_parameter.variable_indentifier)
        {
        #if (AUDIO_DSP_FEATURES == 1)
          case AUDIO_MICROPHONE_GAIN:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: microphone gain", RV_TRACE_LEVEL_DEBUG_LOW);
            status = audio_mode_microphone_gain_write((INT16  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_SPEAKER_GAIN:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: speaker gain", RV_TRACE_LEVEL_DEBUG_LOW);
            status = audio_mode_speaker_gain_write((INT16  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_MICROPHONE_SPEAKER_LOOP_SIDETONE:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: sidetone", RV_TRACE_LEVEL_DEBUG_LOW);
            status = audio_mode_sidetone_gain_write((INT16  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_VOLUME_SPEED:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: volume speed", RV_TRACE_LEVEL_DEBUG_LOW);
            status = audio_mode_volume_speed_write((INT16 *)p_parameter->audio_parameter.data);
            break;
          }
        #else // AUDIO_DSP_FEATURES == 0  
          case AUDIO_MICROPHONE_MODE:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: microphone mode", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_microphone_mode_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_MICROPHONE_GAIN:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: microphone gain", RV_TRACE_LEVEL_DEBUG_LOW);
            status = audio_mode_microphone_gain_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_MICROPHONE_EXTRA_GAIN:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: microphone extra gain", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_microphone_extra_gain_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_MICROPHONE_OUTPUT_BIAS:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: microphone output bias", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_microphone_output_bias_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
        #if(AS_FM_RADIO==1)
        case AUDIO_FM_SPEAKER_MODE:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: speaker mode", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_fm_mode_write((UINT8  *)p_parameter->audio_parameter.data);
            break;
          }
        #endif
          case AUDIO_SPEAKER_MODE:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: speaker mode", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_speaker_mode_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_SPEAKER_GAIN:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: speaker gain", RV_TRACE_LEVEL_DEBUG_LOW);
            status = audio_mode_speaker_gain_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_SPEAKER_EXTRA_GAIN:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: speaker extra gain", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_speaker_extra_gain_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_SPEAKER_FILTER:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: speaker filter", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_speaker_filter_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_SPEAKER_HIGHPASS_FILTER:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: speaker highpass filter", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_speaker_highpass_filter_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_SPEAKER_BUZZER_STATE:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: buzzer state", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_speaker_buzzer_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_MICROPHONE_SPEAKER_LOOP_SIDETONE:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: sidetone", RV_TRACE_LEVEL_DEBUG_LOW);
            status = audio_mode_sidetone_gain_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_STEREO_SPEAKER_MODE:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: stereo speaker mode", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_stereo_speaker_mode_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_STEREO_SPEAKER_STEREO_MONO:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: stereo speaker stereo/mono conversion", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_stereo_speaker_stereo_mono_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_STEREO_SPEAKER_SAMPLING_FREQUENCY:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: stereo speaker sampling frequency", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_stereo_speaker_sampling_freq_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_SPEAKER_VOLUME_LEVEL:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: speaker volume level", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_speaker_volume_write((T_AUDIO_SPEAKER_LEVEL *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_STEREO_SPEAKER_VOLUME_LEVEL:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: stereo speaker volume level", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_stereo_speaker_volume_write((T_AUDIO_STEREO_SPEAKER_LEVEL *)p_parameter->audio_parameter.data);
            break;
          }
          case AUDIO_STEREO_SPEAKER_EXTRA_GAIN:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: stereo speaker extra gain", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_stereo_speaker_extra_gain_write((INT8  *)p_parameter->audio_parameter.data);
            break;
          }
        #endif // AUDIO_DSP_FEATURES == 1

          case AUDIO_PATH_USED:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: voice path", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_voice_path_write((T_AUDIO_VOICE_PATH_SETTING *)p_parameter->audio_parameter.data,
                                                 &message_to_confirm);

            if (status == AUDIO_OK)
            {
              /* Save the return path */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                p_parameter->return_path.callback_func;
              p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                p_parameter->return_path.addr_id;

              /* save the buffer pointer in order to deallocate it after */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                p_parameter->audio_parameter.data;

              /* Calculate the number of confirmation message to receive */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = message_to_confirm;

              p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
              return;
            }
            break;
          }
          case AUDIO_MICROPHONE_FIR:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: microphone FIR", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_speaker_microphone_fir_write(NULL,
                      (T_AUDIO_FIR_COEF *)p_parameter->audio_parameter.data);

            if (status == AUDIO_OK)
            {
              /* save the buffer pointer in order to deallocate it after */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                p_parameter->audio_parameter.data;

              /* Save the return path */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                p_parameter->return_path.callback_func;
              p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                p_parameter->return_path.addr_id;

              /* wait 1 message: FIR */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

              p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
              return;
            }
            break;
          }
          case AUDIO_SPEAKER_FIR:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: speaker FIR", RV_TRACE_LEVEL_DEBUG_LOW);

            status = audio_mode_speaker_microphone_fir_write ((T_AUDIO_FIR_COEF *)p_parameter->audio_parameter.data,
                      NULL);

            if (status == AUDIO_OK)
          {
              /* save the buffer pointer in order to deallocate it after */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                p_parameter->audio_parameter.data;

              /* Save the return path */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                p_parameter->return_path.callback_func;
              p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                p_parameter->return_path.addr_id;

              /* wait 1 message: FIR */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

              p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
              return;
            }
            break;
          }
          case AUDIO_MICROPHONE_SPEAKER_LOOP_AEC:
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: AEC", RV_TRACE_LEVEL_DEBUG_LOW);

			#if (L1_AEC == 1)
            status = audio_mode_aec_write((T_AUDIO_AEC_CFG *)p_parameter->audio_parameter.data);
            #endif
            #if (L1_AEC== 2)
              status = audio_mode_aec_write((T_AUDIO_AQI_AEC_CFG *)p_parameter->audio_parameter.data);
            #endif

            if (status == AUDIO_OK)
            {
              /* save the buffer pointer in order to deallocate it after */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                p_parameter->audio_parameter.data;

              /* Save the return path */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                p_parameter->return_path.callback_func;
              p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                p_parameter->return_path.addr_id;

              /* wait 1 message: AEC */
              p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

              p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
              return;
            }
            break;
          }

          #if (L1_ANR == 1)
            case AUDIO_MICROPHONE_ANR:
            {
              AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: ANR", RV_TRACE_LEVEL_DEBUG_LOW);

              status = audio_mode_microphone_anr_write((T_AUDIO_ANR_CFG *)p_parameter->audio_parameter.data);

              if (status == AUDIO_OK)
              {
                /* save the buffer pointer in order to deallocate it after */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                  p_parameter->audio_parameter.data;

                /* Save the return path */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                  p_parameter->return_path.callback_func;
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                  p_parameter->return_path.addr_id;

                /* wait 1 message: ANR */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

                p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
                return;
              }
              break;
            }
        #endif
          #if (L1_ANR == 2)
            case AUDIO_MICROPHONE_ANR:
            {
              AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: ANR", RV_TRACE_LEVEL_DEBUG_LOW);

              status = audio_mode_microphone_anr_write((T_AUDIO_AQI_ANR_CFG *)p_parameter->audio_parameter.data);

              if (status == AUDIO_OK)
              {
                /* save the buffer pointer in order to deallocate it after */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                  p_parameter->audio_parameter.data;

                /* Save the return path */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                  p_parameter->return_path.callback_func;
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                  p_parameter->return_path.addr_id;

                /* wait 1 message: ANR */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

                p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
                return;
              }
              break;
            }
        #endif		

        #if (L1_ES == 1)
            case AUDIO_MICROPHONE_ES:
            {
              AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: ES", RV_TRACE_LEVEL_DEBUG_LOW);

              status = audio_mode_microphone_es_write((T_AUDIO_ES_CFG *)p_parameter->audio_parameter.data);

              if (status == AUDIO_OK)
              {
                /* save the buffer pointer in order to deallocate it after */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                  p_parameter->audio_parameter.data;

                /* Save the return path */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                  p_parameter->return_path.callback_func;
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                  p_parameter->return_path.addr_id;

                /* wait 1 message: ES */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

                p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
                return;
              }
              break;
            }
        #endif

        #if (L1_IIR == 1 || L1_IIR == 2)
            case AUDIO_SPEAKER_IIR:
            {
              AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: IIR", RV_TRACE_LEVEL_DEBUG_LOW);
#if(L1_IIR == 1)
              status = audio_mode_speaker_iir_write((T_AUDIO_IIR_CFG *)p_parameter->audio_parameter.data);
#elif(L1_IIR == 2)
              status = audio_mode_speaker_iir_write((T_AUDIO_IIR_DL_CFG *)p_parameter->audio_parameter.data);
#endif
              if (status == AUDIO_OK)
              {
                /* save the buffer pointer in order to deallocate it after */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                  p_parameter->audio_parameter.data;

                /* Save the return path */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                  p_parameter->return_path.callback_func;
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                  p_parameter->return_path.addr_id;

                /* wait 1 message: IIR */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

                p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
                return;
              }
              break;
            }
        #endif

        #if (L1_DRC == 1)
            case AUDIO_SPEAKER_DRC:
            {
              AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: DRC", RV_TRACE_LEVEL_DEBUG_LOW);

              status = audio_mode_speaker_drc_write((T_AUDIO_DRC_CFG *)p_parameter->audio_parameter.data);

              if (status == AUDIO_OK)
              {
                /* save the buffer pointer in order to deallocate it after */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                  p_parameter->audio_parameter.data;

                /* Save the return path */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                  p_parameter->return_path.callback_func;
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                  p_parameter->return_path.addr_id;

                /* wait 1 message: DRC */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

                p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
                return;
              }
              break;
            }
        #endif

		#if (L1_AGC_UL == 1)
		    case AUDIO_MICROPHONE_AGC:
            {
              AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: AGC_UL", RV_TRACE_LEVEL_DEBUG_LOW);

              status = audio_mode_agc_ul_write((T_AUDIO_AQI_AGC_UL_REQ *)p_parameter->audio_parameter.data);
			  if (status == AUDIO_OK)
              {
                /* save the buffer pointer in order to deallocate it after */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                  p_parameter->audio_parameter.data;

                /* Save the return path */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                  p_parameter->return_path.callback_func;
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                  p_parameter->return_path.addr_id;

                /* wait 1 message: IIR */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

                p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
                return;
              }
			  break;
		    }
		#endif

		#if (L1_AGC_DL == 1)
            case AUDIO_SPEAKER_AGC:
            {
              AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: AGC_DL", RV_TRACE_LEVEL_DEBUG_LOW);

              status = audio_mode_agc_dl_write((T_AUDIO_AQI_AGC_DL_REQ *)p_parameter->audio_parameter.data);
			  if (status == AUDIO_OK)
              {
                /* save the buffer pointer in order to deallocate it after */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                  p_parameter->audio_parameter.data;

                /* Save the return path */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                  p_parameter->return_path.callback_func;
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                  p_parameter->return_path.addr_id;

                /* wait 1 message: IIR */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

                p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
                return;
              }
			  break;
		    }		
		#endif

        #if (L1_LIMITER == 1)
            case AUDIO_SPEAKER_LIMITER:
            {
              AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: LIMITER", RV_TRACE_LEVEL_DEBUG_LOW);

              status = audio_mode_speaker_limiter_write((T_AUDIO_LIMITER_CFG *)p_parameter->audio_parameter.data);

              if (status == AUDIO_OK)
              {
                /* save the buffer pointer in order to deallocate it after */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                  p_parameter->audio_parameter.data;

                /* Save the return path */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                  p_parameter->return_path.callback_func;
                p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                  p_parameter->return_path.addr_id;

                /* wait 1 message: LIMITER */
                p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;

                p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
                return;
              }
              break;
            }
        #endif


        #if (AUDIO_DSP_ONOFF == 1 || L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1 )
           case AUDIO_ONOFF:
           {
              AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: Audio on/off", RV_TRACE_LEVEL_DEBUG_LOW);
	#if (AUDIO_DSP_ONOFF == 1)
             status = audio_mode_onoff_write((INT8 *)p_parameter->audio_parameter.data);
	#endif
	#if ( L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
             status = audio_mode_onoff_write((T_AUDIO_MODE_ONOFF *)p_parameter->audio_parameter.data);
	#endif
             if (status == AUDIO_OK)
             {
               /* save the buffer pointer in order to deallocate it after */
               p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer =
                 p_parameter->audio_parameter.data;

               /* Save the return path */
               p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.callback_func =
                 p_parameter->return_path.callback_func;
               p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path.addr_id =
                 p_parameter->return_path.addr_id;

               /* wait 1 message: AUDIO ONOFF CON */
               p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message = 1;
     
               p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF;
               return;
             }
             break;
           }
        #endif // AUDIO_DSP_ONOFF
          default:
          {
            AUDIO_SEND_TRACE_PARAM("AUDIO MODE FULL ACCESS WRITE: identifier not supported",
              p_parameter->audio_parameter.variable_indentifier, RV_TRACE_LEVEL_DEBUG_LOW);
            status = AUDIO_ERROR;
          }
        } // switch (p_parameter->audio_parameter.variable_indentifier)

        status_free_buf = (T_RVF_MB_STATUS) rvf_free_buf((T_RVF_BUFFER *)p_parameter->audio_parameter.data);
        if (status_free_buf != RVF_GREEN)
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: A wrong data buffer is deallocated ",
                         RV_TRACE_LEVEL_ERROR);
        }

        /* Send the confirmation message */
        audio_mode_full_access_write_send_status (status, p_parameter->return_path);
        break;
      } // case AUDIO_MODE_FULL_WRITE_IDLE:
      case AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF:
      {
        p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message--;

        if (p_audio_gbl_var->audio_mode_var.full_access_write_var.number_of_message == 0)
        {
          status_free_buf = (T_RVF_MB_STATUS)rvf_free_buf(p_audio_gbl_var->audio_mode_var.full_access_write_var.buffer);

          if (status_free_buf != RVF_GREEN)
          {
            AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS WRITE: A wrong data buffer is deallocated ",
                            RV_TRACE_LEVEL_ERROR);
            /* Send the confirmation message */
            audio_mode_full_access_write_send_status (AUDIO_ERROR,
              p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path);

            p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_IDLE;
            return;
          }

          /* Send the confirmation message */
          audio_mode_full_access_write_send_status (AUDIO_OK,
            p_audio_gbl_var->audio_mode_var.full_access_write_var.return_path);

          p_audio_gbl_var->audio_mode_var.full_access_write_var.state = AUDIO_MODE_FULL_WRITE_IDLE;
        }

        break;
      } // case AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF:
    } // switch (p_audio_gbl_var->audio_mode_var.full_access_write_var.state)
  }
#endif /* RVM_AUDIO_MAIN_SWE */

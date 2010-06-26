/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_api.c                                                 */
/*                                                                          */
/*  Purpose:  This file contains all the functions used to service          */
/*            primitives.                                                   */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------------------------------------------*/
/*  14 May 2001  Create                                                     */
/*                                                                          */
/*  Author      Francois Mazard - Stephanie Gerthoux                        */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"

#ifdef RVM_AUDIO_MAIN_SWE
  #ifndef _WINDOWS
    #include "l1sw.cfg"
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

  #include "nucleus.h"

  #include "l1_types.h"
  #include "l1_confg.h"
  #include "l1audio_cust.h"

  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"

  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_macro_i.h"

  #include "audio/audio_ffs_i.h"

#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#else
  #include "ffs/ffs_api.h"
#endif

#include <string.h>

#if (ANLG_FAM == 11)
    #include "types.h"
    #include "bspTwl3029_I2c.h"
    #include "bspTwl3029_Audio.h"
	#include "bspTwl3029_Intc.h"
	#include "Sys_types.h"
#endif

  /* external dependency */
  #if (SPEECH_RECO)
    extern INT8 audio_sr_create_vocabulary_database(char* directory, void** pp_database);
  #endif
  /* read */

 #if (AUDIO_DSP_FEATURES == 1)
  extern T_AUDIO_RET audio_mode_volume_speed_read                      (INT16  *data);
  extern T_AUDIO_RET audio_mode_microphone_gain_read                   (INT16  *data);
  extern T_AUDIO_RET audio_mode_speaker_gain_read                      (INT16  *data);
  extern T_AUDIO_RET audio_mode_sidetone_gain_read                     (INT16  *data);
 #else
  extern T_AUDIO_RET audio_mode_microphone_mode_read                   (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_gain_read                   (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_extra_gain_read             (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_output_bias_read            (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_mode_read                      (INT8  *data);
  #if(AS_FM_RADIO==1)
  extern T_AUDIO_RET audio_mode_fm_mode_read                      		(UINT8  *data);
  #endif
  extern T_AUDIO_RET audio_mode_speaker_gain_read                      (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_extra_gain_read                (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_filter_read                    (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_highpass_filter_read           (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_buzzer_read                    (INT8  *data);
  extern T_AUDIO_RET audio_mode_sidetone_gain_read                     (INT8  *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_mode_read               (INT8  *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_stereo_mono_read        (INT8  *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_sampling_freq_read      (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_volume_read                    (T_AUDIO_SPEAKER_LEVEL *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_volume_read             (T_AUDIO_STEREO_SPEAKER_LEVEL *data);
 #endif

  extern T_AUDIO_RET audio_mode_voice_path_read                        (T_AUDIO_VOICE_PATH_SETTING *data);
  extern T_AUDIO_RET audio_mode_microphone_fir_read                    (T_AUDIO_FIR_COEF *data);
  extern T_AUDIO_RET audio_mode_speaker_fir_read                       (T_AUDIO_FIR_COEF *data);
  #if (L1_AEC == 1)
  extern T_AUDIO_RET audio_mode_aec_read                               (T_AUDIO_AEC_CFG *data);
  #endif
  #if (L1_AEC == 2)
  extern T_AUDIO_RET audio_mode_aec_read                               (T_AUDIO_AQI_AEC_CFG *data);
  #endif
  #if (L1_ANR == 1)
  extern T_AUDIO_RET audio_mode_microphone_anr_read                  (T_AUDIO_ANR_CFG *data);
#endif
#if (L1_IIR == 1)
  extern T_AUDIO_RET audio_mode_speaker_iir_read                     (T_AUDIO_IIR_CFG *data);
#elif(L1_IIR == 2)
  extern T_AUDIO_RET audio_mode_speaker_iir_read                     (T_AUDIO_IIR_DL_CFG *data);
  #endif
  #if (L1_ANR == 2)
  extern T_AUDIO_RET audio_mode_microphone_anr_read                  (T_AUDIO_AQI_ANR_CFG *data);
  #endif
//  extern T_AUDIO_RET audio_mode_speaker_iir_read                     (T_AUDIO_IIR_CFG *data);
  extern T_AUDIO_RET audio_mode_speaker_limiter_read                 (T_AUDIO_LIMITER_CFG *data);
  extern T_AUDIO_RET audio_mode_microphone_es_read                   (T_AUDIO_ES_CFG *data);

 #if (AUDIO_DSP_ONOFF == 1)
  extern T_AUDIO_RET audio_mode_onoff_read                             (INT8  *data);
 #endif
 #if (L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
  extern T_AUDIO_RET audio_mode_onoff_read                             (T_AUDIO_MODE_ONOFF  *data);
 #endif



// #endif

  T_AUDIO_RET audio_driver_handle_session(UINT32 msg_id, UINT8 channel_id, T_RV_RETURN return_path);

UINT8 ptt_send_size_to_l1_flag = FALSE;
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_keybeep_start                                      */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate a key beep generation       */
  /*              and DTMF generation. The key beep is the generation of two      */
  /*              sine waves                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Key Beep Parameters,                                            */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the keybeep parameters.                                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_keybeep_start (T_AUDIO_KEYBEEP_PARAMETER parameter,
                                    T_RV_RETURN return_path)
  {
    #if (KEYBEEP)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS   mb_status = RVF_GREEN;
      T_AUDIO_KEYBEEP_START *p_msg_start = NULL;

      /************************ audio_keybeep_start function begins ******************/

      if (p_audio_gbl_var == NULL )
      {
         audio_keybeep_error_trace(AUDIO_ENTITY_NOT_START);
         return(AUDIO_ERROR);
      }

      /* If bad parameters, then report an error and abort.*/
      if (
          #if(FREQUENCY_BEEP_MIN > 0)
		  (parameter.frequency_beep[0] < FREQUENCY_BEEP_MIN) ||
		  #endif
          (parameter.frequency_beep[0] > FREQUENCY_BEEP_MAX) ||
          #if(FREQUENCY_BEEP_MIN > 0)
          (parameter.frequency_beep[1] < FREQUENCY_BEEP_MIN) ||
          #endif
          (parameter.frequency_beep[1] > FREQUENCY_BEEP_MAX) ||
          (parameter.amplitude_beep[0] < AMPLITUDE_BEEP_MIN) ||
          (parameter.amplitude_beep[0] > AMPLITUDE_BEEP_MAX) ||
          (parameter.amplitude_beep[1] < AMPLITUDE_BEEP_MIN) ||
          (parameter.amplitude_beep[1] > AMPLITUDE_BEEP_MAX) ||
          (parameter.duration < DURATION_BEEP_MIN ))
      {
        audio_keybeep_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_KEYBEEP_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.     */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_keybeep_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_keybeep_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_KEYBEEP_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->keybeep_parameter.frequency_beep[0]     = parameter.frequency_beep[0];
      p_msg_start->keybeep_parameter.frequency_beep[1]     = parameter.frequency_beep[1];
      p_msg_start->keybeep_parameter.amplitude_beep[0]     = parameter.amplitude_beep[0];
      p_msg_start->keybeep_parameter.amplitude_beep[1]     = parameter.amplitude_beep[1];
      p_msg_start->keybeep_parameter.duration              = parameter.duration;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
       else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Keybeep not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_Keybeep_Start function ******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_keybeep_stop                                       */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a key beep generation           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_keybeep_stop (T_RV_RETURN return_path)
  {
    #if (KEYBEEP)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS   mb_status = RVF_GREEN;
      T_AUDIO_KEYBEEP_STOP *p_msg  = NULL;

      /************************ audio_keybeep_stop function begins ****************/

      if (p_audio_gbl_var == NULL )
      {
        audio_keybeep_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_KEYBEEP_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort.         */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_keybeep_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_keybeep_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }


      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_KEYBEEP_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
       else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Keybeep not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_Keybeep_Stop function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_tones_start                                        */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate tones generation.           */
  /*              The tones are the generation of up to three scheduled           */
  /*              sine waves..........................................            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio tones Parameters,                                               */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the tones parameters.                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_tones_start (T_AUDIO_TONES_PARAMETER* parameter,
                                 T_RV_RETURN return_path)
  {
    #if (TONE)
      /* Declare local variables. */
      T_RVF_MB_STATUS   mb_status = RVF_GREEN;
      T_AUDIO_TONES_START *p_msg_start = NULL;

    /************************ audio_tones_start function begins ********************/

      if (p_audio_gbl_var == NULL )
      {
        audio_tones_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* If bad tones parameters, then report an error and abort.*/
      if (
           #if(FREQUENCY_BEEP_MIN > 0)
		   (parameter->tones[0].frequency_tone < FREQUENCY_BEEP_MIN) ||
           (parameter->tones[1].frequency_tone < FREQUENCY_BEEP_MIN) ||
           (parameter->tones[2].frequency_tone < FREQUENCY_BEEP_MIN) ||
           #endif
           (parameter->tones[0].frequency_tone > FREQUENCY_BEEP_MAX) ||
           (parameter->tones[1].frequency_tone > FREQUENCY_BEEP_MAX) ||
           (parameter->tones[2].frequency_tone > FREQUENCY_BEEP_MAX) ||
           (parameter->tones[0].amplitude_tone < AMPLITUDE_BEEP_MIN) ||
           (parameter->tones[1].amplitude_tone < AMPLITUDE_BEEP_MIN) ||
           (parameter->tones[2].amplitude_tone < AMPLITUDE_BEEP_MIN) ||
           (parameter->tones[0].amplitude_tone > AMPLITUDE_BEEP_MAX) ||
           (parameter->tones[1].amplitude_tone > AMPLITUDE_BEEP_MAX) ||
           (parameter->tones[2].amplitude_tone > AMPLITUDE_BEEP_MAX) ||
           (parameter->frame_duration == 0 )                         ||
           (parameter->period_duration < parameter->sequence_duration) ||
           (parameter->sequence_duration < parameter->frame_duration)  ||
           (parameter->period_duration < parameter->frame_duration)    ||
           ((parameter->tones[0].stop_tone - parameter->tones[0].start_tone) < DURATION_BEEP_MIN) ||
           ((parameter->tones[1].stop_tone - parameter->tones[1].start_tone) < DURATION_BEEP_MIN) ||
           ((parameter->tones[2].stop_tone - parameter->tones[2].start_tone) < DURATION_BEEP_MIN) )
      {
        audio_tones_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_TONES_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.     */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_tones_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_tones_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_TONES_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->tones_parameter.tones[0].frequency_tone = parameter->tones[0].frequency_tone;
      p_msg_start->tones_parameter.tones[1].frequency_tone = parameter->tones[1].frequency_tone;
      p_msg_start->tones_parameter.tones[2].frequency_tone = parameter->tones[2].frequency_tone;

      p_msg_start->tones_parameter.tones[0].amplitude_tone = parameter->tones[0].amplitude_tone;
      p_msg_start->tones_parameter.tones[1].amplitude_tone = parameter->tones[1].amplitude_tone;
      p_msg_start->tones_parameter.tones[2].amplitude_tone = parameter->tones[2].amplitude_tone;

      p_msg_start->tones_parameter.tones[0].start_tone = parameter->tones[0].start_tone;
      p_msg_start->tones_parameter.tones[1].start_tone = parameter->tones[1].start_tone;
      p_msg_start->tones_parameter.tones[2].start_tone = parameter->tones[2].start_tone;
      p_msg_start->tones_parameter.tones[0].stop_tone = parameter->tones[0].stop_tone;
      p_msg_start->tones_parameter.tones[1].stop_tone = parameter->tones[1].stop_tone;
      p_msg_start->tones_parameter.tones[2].stop_tone = parameter->tones[2].stop_tone;

      p_msg_start->tones_parameter.frame_duration= parameter->frame_duration;
      p_msg_start->tones_parameter.sequence_duration = parameter->sequence_duration;
      p_msg_start->tones_parameter.period_duration = parameter->period_duration;
      p_msg_start->tones_parameter.repetition = parameter->repetition;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
       else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Tones not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_tones_Start function ********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_tones_stop                                         */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a tones generation              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Validation of the tones parameters.                                   */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_tones_stop (T_RV_RETURN return_path)
  {
    #if (TONE)
      /* Declare local variables.                                                  */
      T_RVF_MB_STATUS   mb_status = RVF_GREEN;
      T_AUDIO_TONES_STOP *p_msg  = NULL;

    /************************ audio_tones_stop function begins *********************/

      if (p_audio_gbl_var == NULL )
      {
        audio_tones_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_TONES_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort.           */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_tones_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_tones_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_TONES_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
       else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Tones not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_Tones_Stop function *********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E1_start                                    */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate the melody E1 generation    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Melody E1 Parameters,                                           */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the melody E1 parameters.                              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_melody_E1_start (T_AUDIO_MELODY_E1_PARAMETER *p_parameter,
                                     T_RV_RETURN return_path)
  {
    #if (MELODY_E1)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS   mb_status = RVF_GREEN;
      T_AUDIO_MELODY_E1_START *p_msg_start = NULL;
#if(AS_RFS_API == 1)
	  T_RFS_FD            ffs_fd;
#else
      T_FFS_FD            ffs_fd;
#endif

    /************************ audio_melody_E1_start function begins ***************/

      if (p_audio_gbl_var == NULL )
      {
        audio_melody_E1_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the melody E1 file exist */
      #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_parameter->melody_name, RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_parameter->melody_name, FFS_O_RDONLY);
#endif
        if (ffs_fd <= 0)
        {
          audio_melody_E1_error_trace(AUDIO_ENTITY_FILE_ERROR);
          return (AUDIO_ERROR);
        }
      #else
        ffs_fd = 0x00000001;
      #endif

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_MELODY_E1_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.               */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
			rfs_close(ffs_fd);
#else
			ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_MELODY_E1_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd = ffs_fd;

#if(AS_RFS_API == 1)
	  wstrcpy(p_msg_start->melody_E1_parameter.melody_name,
        p_parameter->melody_name);
#else
	  strcpy(p_msg_start->melody_E1_parameter.melody_name,
        p_parameter->melody_name);
#endif

      if ( (p_parameter->loopback == AUDIO_MELODY_NO_LOOPBACK) ||
           (p_parameter->loopback == AUDIO_MELODY_LOOPBACK) )
      {
        p_msg_start->melody_E1_parameter.loopback = p_parameter->loopback;
      }
      else
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        /* Wrong parameter */
        audio_melody_E1_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		  rfs_close(ffs_fd);
#else
		  ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      if ( (p_parameter->melody_mode == AUDIO_MELODY_GAME_MODE) ||
           (p_parameter->melody_mode == AUDIO_MELODY_NORMAL_MODE) )
      {
        p_msg_start->melody_E1_parameter.melody_mode     = p_parameter->melody_mode;
      }
      else
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        /* Wrong parameter */
        audio_melody_E1_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }

      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Melody E1 not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_melody_E1_Start function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E1_stop                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a melody_E1 generation          */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Melody E1 Stop Parameters,                                      */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the melody E1 parameters.                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_melody_E1_stop (T_AUDIO_MELODY_E1_STOP_PARAMETER *p_parameter,
                                    T_RV_RETURN return_path)
  {
    #if (MELODY_E1)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS   mb_status = RVF_GREEN;
      T_AUDIO_MELODY_E1_STOP *p_msg  = NULL;

    /************************ audio_melody_E1_stop function begins ****************/

      if (p_audio_gbl_var == NULL )
      {
        audio_melody_E1_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_MELODY_E1_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort.        */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_melody_E1_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_MELODY_E1_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

#if(AS_RFS_API == 1)
	  wstrcpy (p_msg->melody_name, p_parameter->melody_name);
#else
	  strcpy (p_msg->melody_name, p_parameter->melody_name);
#endif

      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
       else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Melody E1 not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_melody_E1_Stop function *****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_play_start                                      */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate the voice Memorization play */
  /*              generation                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Voice Memorization Play Parameters,                             */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the Voice Memorization Play parameters.                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_vm_play_start (T_AUDIO_VM_PLAY_PARAMETER *p_parameter,
                                   T_RV_RETURN return_path)
  {
    #if (VOICE_MEMO)
      /* Declare local variables. */
      T_RVF_MB_STATUS       mb_status = RVF_GREEN;
      T_AUDIO_VM_PLAY_START *p_msg_start = NULL;
#if(AS_RFS_API == 1)
	  T_RFS_FD            ffs_fd;
#else
      T_FFS_FD            ffs_fd;
#endif

    /************************ audio_vm_play_start function begins ******************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the voice memo play file exist        */
      #ifndef _WINDOWS

#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_parameter->memo_name, RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_parameter->memo_name, FFS_O_RDONLY);
#endif
        if ( ffs_fd <= 0)
        {
          audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_ERROR);
          return (AUDIO_ERROR);
        }
      #else
        ffs_fd = 0x00000010;
      #endif

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PLAY_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.        */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_PLAY_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd = ffs_fd;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }

      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM Play not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_play_start function ******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_play_stop                                       */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a voice memorization Play       */
  /*              generation                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_vm_play_stop (T_RV_RETURN return_path)
  {
    #if (VOICE_MEMO)
      /* Declare local variables.                                                  */
      T_RVF_MB_STATUS        mb_status = RVF_GREEN;
      T_AUDIO_VM_PLAY_STOP   *p_msg  = NULL;

    /************************ audio_vm_play_stop function begins *******************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PLAY_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort.            */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_VM_PLAY_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
       else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM Play not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_play_Stop function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_record_start                                    */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate the voice Memorization      */
  /*              record generation                                               */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Voice Memorization Record Parameters,                           */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the Voice Memorization Record parameters.              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_vm_record_start ( T_AUDIO_VM_RECORD_PARAMETER *p_record_parameter,
                                      T_AUDIO_TONES_PARAMETER *p_tones_parameter,
                                      T_RV_RETURN return_path)
  {
    #if (VOICE_MEMO)
      /* Declare local variables. */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_VM_RECORD_START   *p_msg_start = NULL;
#if(AS_RFS_API == 1)
	  T_RFS_FD            ffs_fd;
#else
	  T_FFS_FD            ffs_fd;
#endif

      /************************ audio_vm_record_start function begins **************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the voice memo record file already exist        */
      #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_record_parameter->memo_name,
                  RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_record_parameter->memo_name,
                  FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
        if ( ffs_fd <= 0)
        {
          audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_ERROR);
          return (AUDIO_ERROR);
        }
      #else
        ffs_fd = 0x00000011;
      #endif

      /* If bad tones parameters, then report an error and abort.*/
      if (
           #if(FREQUENCY_BEEP_MIN > 0)
		   (p_tones_parameter->tones[0].frequency_tone < FREQUENCY_BEEP_MIN) ||
           (p_tones_parameter->tones[1].frequency_tone < FREQUENCY_BEEP_MIN) ||
           (p_tones_parameter->tones[2].frequency_tone < FREQUENCY_BEEP_MIN) ||
           #endif
           (p_tones_parameter->tones[0].frequency_tone > FREQUENCY_BEEP_MAX) ||
           (p_tones_parameter->tones[1].frequency_tone > FREQUENCY_BEEP_MAX) ||
           (p_tones_parameter->tones[2].frequency_tone > FREQUENCY_BEEP_MAX) ||
           (p_tones_parameter->tones[0].amplitude_tone < AMPLITUDE_BEEP_MIN) ||
           (p_tones_parameter->tones[1].amplitude_tone < AMPLITUDE_BEEP_MIN) ||
           (p_tones_parameter->tones[2].amplitude_tone < AMPLITUDE_BEEP_MIN) ||
           (p_tones_parameter->tones[0].amplitude_tone > AMPLITUDE_BEEP_MAX) ||
           (p_tones_parameter->tones[1].amplitude_tone > AMPLITUDE_BEEP_MAX) ||
           (p_tones_parameter->tones[2].amplitude_tone > AMPLITUDE_BEEP_MAX) ||
           (p_tones_parameter->frame_duration == 0 )                         ||
           (p_tones_parameter->period_duration < p_tones_parameter->sequence_duration) ||
           (p_tones_parameter->sequence_duration < p_tones_parameter->frame_duration)  ||
           (p_tones_parameter->period_duration < p_tones_parameter->frame_duration)    ||
           ((p_tones_parameter->tones[0].stop_tone - p_tones_parameter->tones[0].start_tone) < DURATION_BEEP_MIN) ||
           ((p_tones_parameter->tones[1].stop_tone - p_tones_parameter->tones[1].start_tone) < DURATION_BEEP_MIN) ||
           ((p_tones_parameter->tones[2].stop_tone - p_tones_parameter->tones[2].start_tone) < DURATION_BEEP_MIN) )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* If bad voice memo record parameters, then report an error and abort.*/
      if ( ( p_record_parameter->memo_duration == 0) ||
           (( p_record_parameter->compression_mode != AUDIO_VM_NO_COMPRESSION_MODE ) &&
            ( p_record_parameter->compression_mode != AUDIO_VM_COMPRESSION_MODE )))
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_RECORD_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_RECORD_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd                           = ffs_fd;
      p_msg_start->compression_mode   = p_record_parameter->compression_mode;
      p_msg_start->memo_duration      = p_record_parameter->memo_duration;
      p_msg_start->microphone_gain    = p_record_parameter->microphone_gain;
      p_msg_start->network_gain       = p_record_parameter->network_gain;

      /* fill the message tones parameters */
      p_msg_start->tones_parameter.tones[0].frequency_tone = p_tones_parameter->tones[0].frequency_tone;
      p_msg_start->tones_parameter.tones[1].frequency_tone = p_tones_parameter->tones[1].frequency_tone;
      p_msg_start->tones_parameter.tones[2].frequency_tone = p_tones_parameter->tones[2].frequency_tone;

      p_msg_start->tones_parameter.tones[0].amplitude_tone = p_tones_parameter->tones[0].amplitude_tone;
      p_msg_start->tones_parameter.tones[1].amplitude_tone = p_tones_parameter->tones[1].amplitude_tone;
      p_msg_start->tones_parameter.tones[2].amplitude_tone = p_tones_parameter->tones[2].amplitude_tone;

      p_msg_start->tones_parameter.tones[0].start_tone = p_tones_parameter->tones[0].start_tone;
      p_msg_start->tones_parameter.tones[1].start_tone = p_tones_parameter->tones[1].start_tone;
      p_msg_start->tones_parameter.tones[2].start_tone = p_tones_parameter->tones[2].start_tone;
      p_msg_start->tones_parameter.tones[0].stop_tone = p_tones_parameter->tones[0].stop_tone;
      p_msg_start->tones_parameter.tones[1].stop_tone = p_tones_parameter->tones[1].stop_tone;
      p_msg_start->tones_parameter.tones[2].stop_tone = p_tones_parameter->tones[2].stop_tone;

      p_msg_start->tones_parameter.frame_duration= p_tones_parameter->frame_duration;
      p_msg_start->tones_parameter.sequence_duration = p_tones_parameter->sequence_duration;
      p_msg_start->tones_parameter.period_duration = p_tones_parameter->period_duration;
      p_msg_start->tones_parameter.repetition = p_tones_parameter->repetition;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }

      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM Record not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_record_start function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_record_stop                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a voice memorization Record     */
  /*              generation                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_vm_record_stop (T_RV_RETURN return_path)
  {
    #if (VOICE_MEMO)
      /* Declare local variables.                                                  */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_VM_RECORD_STOP    *p_msg  = NULL;

    /************************ audio_vm_record_stop function begins *****************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_RECORD_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort.          */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_VM_RECORD_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
       else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM Record not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_record_Stop function *****************/




/*VENKAT*/
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_pcm_vm_play_start                                      */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate the voice Memorization play */
  /*              generation                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Voice Memorization Play Parameters,                             */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the Voice Memorization Play parameters.                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_vm_pcm_play_start (T_AUDIO_VM_PCM_PLAY_PARAMETER *p_parameter,
                                   T_RV_RETURN return_path)
  {

    #if (L1_PCM_EXTRACTION) //VENKAT
      /* Declare local variables. */
      T_RVF_MB_STATUS       mb_status = RVF_GREEN;
      T_AUDIO_VM_PCM_PLAY_START *p_msg_start = NULL;
#if(AS_RFS_API == 1)
		T_RFS_FD            ffs_fd;
#else
	  T_FFS_FD            ffs_fd;
#endif


    /************************ audio_vm_play_start function begins ******************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }
     if (*(p_parameter->memo_name) == NULL)
     {
        audio_voice_memo_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        return (AUDIO_ERROR);
      }

	    if (((p_parameter->memo_name) == NULL) || ( p_parameter->memo_duration == 0) ||
	     ((p_parameter->speaker_gain != 0) && (p_parameter->speaker_gain) != 0x20) ||
	     ((p_parameter->memo_name) == NULL) ||
	     ((p_parameter->network_gain) != 0 && (p_parameter->network_gain) != 0x20))
	     {
                  audio_voice_memo_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
	           return (AUDIO_ERROR);
	      }

      /* check if the voice memo play file exist        */
      #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_parameter->memo_name, RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_parameter->memo_name, FFS_O_RDONLY);
#endif
        if ( ffs_fd <= 0)
        {
          audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_ERROR);
          return (AUDIO_ERROR);
        }
      #else
        ffs_fd = 0x00000010;
      #endif

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PCM_PLAY_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.        */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_PCM_PLAY_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd = ffs_fd;
      p_msg_start->memo_duration = p_parameter->memo_duration;
      p_msg_start->download_ul_gain = p_parameter->network_gain;
      p_msg_start->download_dl_gain = p_parameter->speaker_gain;

       AUDIO_SEND_TRACE_PARAM("API: play: memo_duration", p_parameter->memo_duration, RV_TRACE_LEVEL_DEBUG_LOW);
	AUDIO_SEND_TRACE_PARAM("API: play: microphone_gain", p_parameter->speaker_gain, RV_TRACE_LEVEL_DEBUG_LOW);
	AUDIO_SEND_TRACE_PARAM("API: play: network_gain", p_parameter->network_gain, RV_TRACE_LEVEL_DEBUG_LOW);

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }

      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }
  AUDIO_SEND_TRACE("API: audio_vm_pcm_play_start() Sending Msg: AUDIO_VM_PCM_PLAY_START_REQ", RV_TRACE_LEVEL_ERROR);
      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM PCM Play not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_play_start function ******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_pcm_play_stop                                       */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a voice memorization Play       */
  /*              generation                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_vm_pcm_play_stop (T_RV_RETURN return_path)
  {
    #if (L1_PCM_EXTRACTION) //VENKAT
      /* Declare local variables.                                                  */
      T_RVF_MB_STATUS        mb_status = RVF_GREEN;
      T_AUDIO_VM_PCM_PLAY_STOP   *p_msg  = NULL;

    /************************ audio_vm_play_stop function begins *******************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PCM_PLAY_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort.            */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_VM_PCM_PLAY_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
       else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }
  AUDIO_SEND_TRACE("API: audio_vm_pcm_play_stop() Sending Msg: AUDIO_VM_PCM_PLAY_STOP_REQ", RV_TRACE_LEVEL_ERROR);
      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM PCM Play not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_play_Stop function *******************/



  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_pcm_record_start                                    */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate the voice Memorization      */
  /*              record generation on PCM.                                              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Voice Memorization Record Parameters,                           */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the Voice Memorization Record parameters.              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_vm_pcm_record_start ( T_AUDIO_VM_PCM_RECORD_PARAMETER *p_record_parameter,
                                      T_RV_RETURN return_path)
  {
    #if (L1_PCM_EXTRACTION) //VENKAT
      /* Declare local variables. */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_VM_PCM_RECORD_START   *p_msg_start = NULL;
#if(AS_RFS_API == 1)
	  T_RFS_FD            ffs_fd;
#else
	  T_FFS_FD            ffs_fd;
#endif

      /************************ audio_vm_record_start function begins **************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the voice memo record file already exist        */
      #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_record_parameter->memo_name,
                  RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_record_parameter->memo_name,
                  FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
        if ( ffs_fd <= 0)
        {
          audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_ERROR);
          return (AUDIO_ERROR);
        }
      #else
        ffs_fd = 0x00000011;
      #endif


      /* If bad voice memo record parameters, then report an error and abort.*/
      if ( ( p_record_parameter->memo_duration == 0) ||
	     ( p_record_parameter->microphone_gain != 0 && p_record_parameter->microphone_gain != 0x20) ||
	     (*(p_record_parameter->memo_name) == NULL) ||
	     ( p_record_parameter->network_gain != 0 && p_record_parameter->network_gain != 0x20))
     {
        audio_voice_memo_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PCM_RECORD_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_PCM_RECORD_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd                           = ffs_fd;
      p_msg_start->memo_duration      = p_record_parameter->memo_duration;
      p_msg_start->upload_ul_gain    = p_record_parameter->microphone_gain;
      p_msg_start->upload_dl_gain       = p_record_parameter->network_gain;

      p_audio_gbl_var->vm_pcm_record.size_copied_to_flash = 0;
      p_audio_gbl_var->vm_pcm_record.recorded_size = 0;

       AUDIO_SEND_TRACE_PARAM("API:record:memo_duration", p_record_parameter->memo_duration, RV_TRACE_LEVEL_DEBUG_LOW);
	AUDIO_SEND_TRACE_PARAM("API:record:microphone_gain", p_record_parameter->microphone_gain, RV_TRACE_LEVEL_DEBUG_LOW);
	AUDIO_SEND_TRACE_PARAM("API:record:network_gain", p_record_parameter->network_gain, RV_TRACE_LEVEL_DEBUG_LOW);

      /* fill the message tones parameters */
      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }

      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }
  AUDIO_SEND_TRACE("API: audio_vm_pcm_record_start () Sending Msg: AUDIO_VM_PCM_RECORD_START_REQ", RV_TRACE_LEVEL_ERROR);
      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM PCM Record not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_record_start function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_pcm_record_stop                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a voice memorization Record     */
  /*              generation                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_vm_pcm_record_stop (T_RV_RETURN return_path)
  {
    #if (L1_PCM_EXTRACTION) //VENKAT
      /* Declare local variables.                                                  */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_VM_PCM_RECORD_STOP    *p_msg  = NULL;

    /************************ audio_vm_record_stop function begins *****************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PCM_RECORD_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort.          */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_VM_PCM_RECORD_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
       else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }
  AUDIO_SEND_TRACE("API: audio_vm_pcm_record_stop() Sending Msg: AUDIO_VM_PCM_RECORD_STOP_REQ", RV_TRACE_LEVEL_ERROR);
      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM PCM Record not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_record_Stop function *****************/

/*!VENKAT*/


 /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_voice_buffering_pcm_record_start                                    */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate the voice Memorization      */
  /*              record generation on PCM.                                              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Voice Memorization Record Parameters,                           */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the Voice Memorization Record parameters.              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_voice_buffering_pcm_record_start ( T_AUDIO_VBUF_PCM_RECORD_PARAMETER *p_record_parameter,
                                      T_RV_RETURN return_path)
  {
    #if (L1_PCM_EXTRACTION) //VENKAT
      /* Declare local variables. */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_VBUF_PCM_RECORD_START   *p_msg_start = NULL;
#if(AS_RFS_API == 1)
		T_RFS_FD            ffs_fd;
#else
	  T_FFS_FD            ffs_fd;
#endif

      /************************ audio_vm_record_start function begins **************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the voice memo record file already exist        */
      #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_record_parameter->memo_name,
                  RFS_O_CREAT | RFS_O_RDWR | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_record_parameter->memo_name,
                  FFS_O_CREATE | FFS_O_RDWR | FFS_O_TRUNC | FFS_O_APPEND);
#endif
        if ( ffs_fd < 0)
        {
          audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_ERROR);
          return (AUDIO_ERROR);
        }
      #else
        ffs_fd = 0x00000011;
      #endif


      /* If bad voice memo record parameters, then report an error and abort.*/
      if ( ( p_record_parameter->memo_duration == 0) ||
	     ( p_record_parameter->microphone_gain != 0 && p_record_parameter->microphone_gain != 0x20) ||
	     (*(p_record_parameter->memo_name) == NULL) ||
	     ( p_record_parameter->network_gain != 0 && p_record_parameter->network_gain != 0x20))
     {
        audio_voice_memo_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }


      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VBUF_PCM_RECORD_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VBUF_PCM_RECORD_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;


      /* fill the message parameters */
#if(AS_RFS_API == 1)
	  wstrcpy(p_msg_start->memo_name, p_record_parameter->memo_name);
#else
	  strcpy(p_msg_start->memo_name, p_record_parameter->memo_name);
#endif
      p_msg_start->audio_ffs_fd                           = ffs_fd;
      p_msg_start->memo_duration      = p_record_parameter->memo_duration;
      p_msg_start->upload_ul_gain    = p_record_parameter->microphone_gain;
      p_msg_start->upload_dl_gain       = p_record_parameter->network_gain;

     p_audio_gbl_var->vbuf_pcm_play.size_read_from_flash =   0;
     p_audio_gbl_var->vbuf_pcm_record.size_copied_to_flash = 0;
     ptt_send_size_to_l1_flag = FALSE;
	AUDIO_SEND_TRACE_PARAM("API:record:memo_duration", p_record_parameter->memo_duration, RV_TRACE_LEVEL_DEBUG_LOW);
	AUDIO_SEND_TRACE_PARAM("API:record:microphone_gain", p_record_parameter->microphone_gain, RV_TRACE_LEVEL_DEBUG_LOW);
	AUDIO_SEND_TRACE_PARAM("API:record:network_gain", p_record_parameter->network_gain, RV_TRACE_LEVEL_DEBUG_LOW);



      /* fill the message tones parameters */
      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }

      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }
      AUDIO_SEND_TRACE("API: audio_voice_buffering_pcm_record_start Sending Msg: AUDIO_VBUF_PCM_RECORD_START_REQ", RV_TRACE_LEVEL_ERROR);
      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM PCM Record not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_voice_buffering_record_start function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_voice_buffering_pcm_record_stop                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a voice buffering Record     */
  /*              generation                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_voice_buffering_pcm_record_stop (/*T_AUDIO_VBUF_PCM_RECORD_STOP_PARAMETER* p_parameter,*/ T_RV_RETURN return_path)
  {
    #if (L1_PCM_EXTRACTION) //VENKAT
      /* Declare local variables.                                                  */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_VBUF_PCM_RECORD_STOP    *p_msg  = NULL;

    /************************ audio_vm_record_stop function begins *****************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PCM_RECORD_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort.          */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

//	p_audio_gbl_var->vbuf_pcm_record.delete_after_play = p_parameter->delete_file_after_play;
      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_VBUF_PCM_RECORD_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;


      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
       else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }
      AUDIO_SEND_TRACE("API: audio_voice_buffering_pcm_record_stop() Msg: AUDIO_VBUF_PCM_RECORD_STOP_REQ", RV_TRACE_LEVEL_ERROR);
      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM PCM Record not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_voice_buffering_record_Stop function *****************/

/*!VENKAT*/



/*VENKAT*/
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_pcm_voice_buffering_play_start                                      */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate the voice buffering play */
  /*              generation                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Voice Memorization Play Parameters,                             */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the Voice Memorization Play parameters.                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_voice_buffering_pcm_play_start (T_AUDIO_VBUF_PCM_PLAY_PARAMETER *p_parameter,
                                   T_RV_RETURN return_path)
  {

    #if (L1_PCM_EXTRACTION) //VENKAT
      /* Declare local variables. */
      T_RVF_MB_STATUS       mb_status = RVF_GREEN;
      T_AUDIO_VBUF_PCM_PLAY_START *p_msg_start = NULL;
#if(AS_RFS_API == 1)
		T_RFS_FD            ffs_fd;
#else
	  T_FFS_FD            ffs_fd;
#endif

    /************************ audio_vm_play_start function begins ******************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }
     if (*(p_parameter->memo_name) == NULL)
     {
        audio_voice_memo_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        return (AUDIO_ERROR);
      }

	    if (((p_parameter->memo_name) == NULL) || ( p_parameter->memo_duration == 0) ||
	     ((p_parameter->speaker_gain != 0) && (p_parameter->speaker_gain) != 0x20) ||
	     ((p_parameter->memo_name) == NULL) ||
	     ((p_parameter->network_gain) != 0 && (p_parameter->network_gain) != 0x20))
	     {
                  audio_voice_memo_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
	           return (AUDIO_ERROR);
	      }

      /* check if the voice memo play file exist        */
      #ifndef _WINDOWS
        ffs_fd = p_audio_gbl_var->vbuf_pcm_record.vbuf_ffs_fd;
        if ( ffs_fd <= 0)
        {
          audio_voice_memo_error_trace(AUDIO_ENTITY_FILE_ERROR);
          return (AUDIO_ERROR);
        }
      #else
        ffs_fd = 0x00000010;
      #endif

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PCM_PLAY_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.        */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VBUF_PCM_PLAY_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;


      /* fill the message parameters */
      p_msg_start->audio_ffs_fd = ffs_fd;
      p_msg_start->memo_duration = p_parameter->memo_duration;
      p_msg_start->download_ul_gain = p_parameter->network_gain;
      p_msg_start->download_dl_gain = p_parameter->speaker_gain;

	AUDIO_SEND_TRACE_PARAM("API: play: memo_duration", p_parameter->memo_duration, RV_TRACE_LEVEL_DEBUG_LOW);
	AUDIO_SEND_TRACE_PARAM("API: play: microphone_gain", p_parameter->speaker_gain, RV_TRACE_LEVEL_DEBUG_LOW);
	AUDIO_SEND_TRACE_PARAM("API: play: network_gain", p_parameter->network_gain, RV_TRACE_LEVEL_DEBUG_LOW);

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }

      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }
      AUDIO_SEND_TRACE("API: audio_voice_buffering_pcm_play_start() Sending Msg: AUDIO_VBUF_PCM_PLAY_START_REQ", RV_TRACE_LEVEL_ERROR);
      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM PCM Play not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_play_start function ******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_pcm_play_stop                                       */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a voice memorization Play       */
  /*              generation                                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_voice_buffering_pcm_play_stop (T_RV_RETURN return_path)
  {
    #if (L1_PCM_EXTRACTION) //VENKAT
      /* Declare local variables.                                                  */
      T_RVF_MB_STATUS        mb_status = RVF_GREEN;
      T_AUDIO_VM_PCM_PLAY_STOP   *p_msg  = NULL;

    /************************ audio_vm_play_stop function begins *******************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_PCM_PLAY_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort.            */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_VBUF_PCM_PLAY_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;


      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
       else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }
      AUDIO_SEND_TRACE("API: audio_voice_buffering_pcm_play_stop() Sending Msg: AUDIO_VBUF_PCM_PLAY_STOP_REQ", RV_TRACE_LEVEL_ERROR);
      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("VM PCM Play not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_vm_play_Stop function *******************/


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_set_file_delete_property                                                            */
  /*                                                                                                                                     */
  /*    Purpose:  This function is called to CLEANUP the RECORD state machine in case of abrupt */
  /*              PTT stop before PLAY commences                                                                       */
  /*                                                                                                                                     */
  /*    Input Parameters:                                                                                                       */
  /*        file_stat(FALSE)                                                                                                      */
  /*                                                                                                                                     */
  /*    Output Parameters:                                                                                                     */
  /*        None.                                                                                                                     */
  /*                                                                                                                                     */
  /*    Note:                                                                                                                         */
  /*        None.                                                                                                                     */
  /*                                                                                                                                     */
  /*    Revision History:                                                                                                         */
  /*        None.                                                                                                                     */
  /*                                                                                                                                     */
  /********************************************************************************/
T_AUDIO_RET audio_set_file_delete_property(UINT8 file_stat )
{
 #if (L1_PCM_EXTRACTION)

       T_AUDIO_VBUF_PCM_PLAY_STOP_CON *p_send_message=NULL; //nekkareb_June_16
       T_RVF_MB_STATUS mb_status = RVF_GREEN;

//Added by nekkareb_June_16 : start
       AUDIO_SEND_TRACE("audio_set_file_delete_property: MSG - AUDIO_VBUF_PCM_PLAY_STOP_CON", RV_TRACE_LEVEL_ERROR);

      if (p_audio_gbl_var == NULL )
      {
         audio_voice_memo_error_trace(AUDIO_ENTITY_NOT_START);
         return(AUDIO_ERROR);
      }

	// Make delete_after_play as FALSE to delete the file and clean the Record state machine
	p_audio_gbl_var->vbuf_pcm_record.delete_after_play = file_stat;

       /* Send FFS stop command */
       /* allocate the message buffer */
       mb_status = rvf_get_buf (p_audio_gbl_var->mb_external, //p_audio_gbl_var->mb_internal,
                         sizeof (T_AUDIO_VBUF_PCM_PLAY_STOP_CON),
                         (T_RVF_BUFFER **) (&p_send_message));

      /* If insufficient resources, then report a memory error and abort.            */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_send_message);
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_voice_memo_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }


      /* fill the header of the message */
      // T_AUDIO_FFS_STOP_REQ  nekkareb why?
      ((T_AUDIO_FFS_STOP_REQ *)p_send_message)->os_hdr.msg_id = AUDIO_VBUF_PCM_PLAY_STOP_CON;

      /* fill the address source id */
      p_send_message->os_hdr.src_addr_id = rvf_get_taskid();
      p_send_message->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the message to the Audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,//p_audio_gbl_var->audio_ffs_addrId,
                p_send_message);

      //p_audio_gbl_var->vmemo_vbuf_session = 0; already done in record state machine

       return (AUDIO_OK);

    #else

       AUDIO_SEND_TRACE("VBUF PCM Play not compiled", RV_TRACE_LEVEL_ERROR);
       return (AUDIO_ERROR);

    #endif
//Added by nekkareb_June_16 : end
}


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_enroll_start                                    */
  /*                                                                              */
  /*    Purpose:  This function is called to start the SR enrollment              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        SR enrollment parameters                                              */
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
  T_AUDIO_RET audio_sr_enroll_start (T_AUDIO_SR_ENROLL_PARAMETER *p_parameter,
                                     T_RV_RETURN return_path)
  {
    #if (SPEECH_RECO)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_SR_ENROLL_START   *p_msg_start  = NULL;
      char                      sr_name[AUDIO_SR_PATH_NAME_MAX_SIZE];
#if(AS_RFS_API == 1)
	  T_RFS_FD            ffs_fd, sr_ffs_fd;
	  T_WCHAR             sr_name[AUDIO_SR_PATH_NAME_MAX_SIZE];
#else
	  T_FFS_FD            ffs_fd, sr_ffs_fd;
	  char                      sr_name[AUDIO_SR_PATH_NAME_MAX_SIZE];
#endif

    /************************ audio_sr_enroll_start function begins ***************/

      if (p_audio_gbl_var == NULL )
      {
        audio_sr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* Check the speech sample parameter */
      if ( (p_parameter->record_speech != AUDIO_SR_RECORD_SPEECH)     &&
           (p_parameter->record_speech != AUDIO_SR_NO_RECORD_SPEECH) )
      {
        audio_sr_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        return(AUDIO_ERROR);
      }

      /* Create the speech recognition sample file name */
#if(AS_RFS_API == 1)
      wstrcpy(sr_name, p_parameter->enroll_name);
      wstrcat(sr_name,"_sr");
#else
	  strcpy(sr_name, p_parameter->enroll_name);
      strcat(sr_name,"_sr");
#endif

      #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	  sr_ffs_fd = rfs_open(sr_name,
          RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	  sr_ffs_fd = ffs_open(sr_name,
          FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
        if (sr_ffs_fd <= 0)
        {
          audio_sr_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return(AUDIO_ERROR);
      }
      #else
        sr_ffs_fd = 0x00000100;
      #endif

      /* Create the speech sample file if it's requested.*/
      if (p_parameter->record_speech == AUDIO_SR_RECORD_SPEECH)
      {
      #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_parameter->enroll_name,
                      RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_parameter->enroll_name,
                      FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif

		if (ffs_fd <= 0)
        {
#if(AS_RFS_API == 1)
			rfs_close(sr_ffs_fd);
#else
			ffs_close(sr_ffs_fd);
#endif
            audio_sr_error_trace(AUDIO_ENTITY_FILE_ERROR);
          return (AUDIO_ERROR);
        }
      #else
          ffs_fd = 0x00000101;
      #endif
      }
      else
      {
        ffs_fd = AUDIO_SR_NO_RECORD_SPEECH;
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_SR_ENROLL_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.               */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(sr_ffs_fd);
#else
		ffs_close(sr_ffs_fd);
#endif
          if (p_parameter->record_speech == AUDIO_SR_RECORD_SPEECH)
          {
#if(AS_RFS_API == 1)
			rfs_close(ffs_fd);
#else
			ffs_close(ffs_fd);
#endif
          }
        #endif
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        #ifndef _WINDOWS
          /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(sr_ffs_fd);
#else
		ffs_close(sr_ffs_fd);
#endif
          if (p_parameter->record_speech == AUDIO_SR_RECORD_SPEECH)
          {
#if(AS_RFS_API == 1)
			rfs_close(ffs_fd);
#else
			ffs_close(ffs_fd);
#endif
          }
        #endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_SR_ENROLL_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->sr_ffs_fd = sr_ffs_fd;
      p_msg_start->voice_ffs_fd = ffs_fd;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Speech Reco not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_enroll_Start function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_enroll_stop                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to stop the SR enrollment               */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        SR enrollment parameters                                              */
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
  T_AUDIO_RET audio_sr_enroll_stop (T_RV_RETURN return_path)
  {
    #if (SPEECH_RECO)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_SR_ENROLL_STOP   *p_msg_start  = NULL;

    /************************ audio_sr_enroll_start function begins ***************/

      if (p_audio_gbl_var == NULL )
      {
        audio_sr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_SR_ENROLL_STOP),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.          */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_SR_ENROLL_STOP_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Speech Reco not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_sr_enroll_stop function *****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_update_start                                    */
  /*                                                                              */
  /*    Purpose:  This function is called to start the SR update                  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        SR update parameters                                                  */
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
  T_AUDIO_RET audio_sr_update_start (T_AUDIO_SR_UPDATE_PARAMETER *p_parameter,
                                     T_RV_RETURN return_path)
  {
    #if (SPEECH_RECO)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_SR_UPDATE_START  *p_msg_start  = NULL;
      UINT8                     i, folder_size=0, path_size;
      char                      sr_name[AUDIO_SR_PATH_NAME_MAX_SIZE],
                                sr_dir[AUDIO_SR_PATH_NAME_MAX_SIZE],
                                *p_database;
      INT8                      model_index, model_number;
      #ifndef _WINDOWS
        char                    *p_model_path;
      #endif

    /************************ audio_sr_update_start function begins ***************/

      if (p_audio_gbl_var == NULL )
      {
        audio_sr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* Check the speech sample parameter */
      if ( (p_parameter->record_speech != AUDIO_SR_RECORD_SPEECH)     &&
           (p_parameter->record_speech != AUDIO_SR_NO_RECORD_SPEECH) )
      {
        audio_sr_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        return(AUDIO_ERROR);
      }

      /* Create the speech recognition sample file name */

	  strcpy(sr_name, p_parameter->update_name);
      strcat(sr_name,"_sr");


      /* Check the number of speech reco model are included in the current folder */
      /* in order to know if the update or the update check needs to be run */
      path_size = (UINT8)strlen(p_parameter->update_name);
      i = 0;
      while (i < path_size)
      {
        if (strncmp(&(p_parameter->update_name[i]), "/", 1) == 0)
        {
          folder_size = i;
        }
        i++;
      }
      if ((folder_size + 1) == path_size)
      {
      /* this isn't a name but a folder */
      audio_sr_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return(AUDIO_ERROR);
      }
      /* Create the speech recognition folder name */
       strncpy(sr_dir, p_parameter->update_name, folder_size);
      sr_dir[folder_size] = 0;

      /* check the number of model in the database */
      model_number = audio_sr_create_vocabulary_database(sr_dir, (void **)(&p_database));
      if (model_number == AUDIO_ERROR)
      {
        return(AUDIO_ERROR);
      }

      #ifndef _WINDOWS
        /* find the index of the model to updae */
        p_model_path = p_database;
        model_index == -1;
        for (i=0; i<model_number; i++)
        {
           if ( strcmp(p_model_path, sr_name) == 0 )
          {
            model_index = i;
          }
          p_model_path += AUDIO_PATH_NAME_MAX_SIZE;
        }
        if (model_index == -1)
        /* no file correspoding to the model to update */
        {
          audio_sr_error_trace(AUDIO_ENTITY_FILE_ERROR);
          rvf_free_buf((T_RVF_BUFFER *)p_database);
          return (AUDIO_ERROR);
        }
      #else
        model_index = 0;
      #endif

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_SR_UPDATE_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.               */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_database);
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        rvf_free_buf((T_RVF_BUFFER *)p_database);
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_SR_UPDATE_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->vocabulary_size = model_number;
      p_msg_start->model_index = model_index;
      p_msg_start->p_database = p_database;
      p_msg_start->record_speech = p_parameter->record_speech;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Speech Reco not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_sr_update_start function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_update_stop                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to stop the SR update   t               */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        SR update parameters                                                  */
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
  T_AUDIO_RET audio_sr_update_stop (T_RV_RETURN return_path)
  {
    #if (SPEECH_RECO)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_SR_UPDATE_STOP   *p_msg_start  = NULL;

    /************************ audio_sr_update_start function begins ***************/

      if (p_audio_gbl_var == NULL )
      {
        audio_sr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_SR_UPDATE_STOP),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.         */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_SR_UPDATE_STOP_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Speech Reco not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_sr_update_stop function *****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_reco_start                                      */
  /*                                                                              */
  /*    Purpose:  This function is called to start the SR recognition             */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        SR update parameters                                                  */
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
  T_AUDIO_RET audio_sr_reco_start (T_AUDIO_SR_RECO_PARAMETER *p_parameter,
                                     T_RV_RETURN return_path)
  {
    #if (SPEECH_RECO)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_SR_RECO_START  *p_msg_start  = NULL;
      INT8                      model_number;
      void                      *p_database;

      /************************ audio_sr_reco_start function begins ****************/

      if (p_audio_gbl_var == NULL )
      {
        audio_sr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the / is in the end of the directory */
      if(strlen(p_parameter->database_directory) == 0)
      {
        audio_sr_error_trace(AUDIO_ENTITY_BAD_DATABASE);
        return(AUDIO_ERROR);
      }
      else if ( strncmp(&(p_parameter->database_directory[strlen(p_parameter->database_directory) - 1]),
           "/", 1) == 0 )
      {
        audio_sr_error_trace(AUDIO_ENTITY_BAD_DATABASE);
        return(AUDIO_ERROR);
      }

      /* check the number of model in the database */
      model_number = audio_sr_create_vocabulary_database(p_parameter->database_directory,
        (void **)(&p_database));
      if (model_number == AUDIO_ERROR)
      {
        audio_sr_error_trace(AUDIO_ENTITY_BAD_DATABASE);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_SR_RECO_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.               */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_database);
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        rvf_free_buf((T_RVF_BUFFER *)p_database);
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_SR_RECO_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->vocabulary_size = model_number;
      p_msg_start->p_database = p_database;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Speech Reco not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_sr_reco_Start function ******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_sr_reco_stop                                       */
  /*                                                                              */
  /*    Purpose:  This function is called to stop the SR recognition              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        SR reco parameters                                                    */
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
  T_AUDIO_RET audio_sr_reco_stop (T_RV_RETURN return_path)
  {
    #if (SPEECH_RECO)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS           mb_status = RVF_GREEN;
      T_AUDIO_SR_RECO_STOP   *p_msg_start  = NULL;

    /************************ audio_sr_reco_stop function begins ******************/

      if (p_audio_gbl_var == NULL )
      {
        audio_sr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_SR_RECO_STOP),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.         */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_sr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_SR_RECO_STOP_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Speech Reco not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_sr_reco_stop function *******************/

    /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_full_access_write                                  */
  /*                                                                              */
  /*    Purpose:  This function is called to configure the audio mode             */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        audio configuration                                                   */
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
  T_AUDIO_RET audio_full_access_write (T_AUDIO_FULL_ACCESS_WRITE  *p_parameter,
                                       T_RV_RETURN                return_path)
  {
    UINT8                               size;
    UINT8                                i;
    T_RVF_MB_STATUS                     mb_status = RVF_GREEN;
    T_AUDIO_MODE_FULL_ACCESS_WRITE_REQ  *p_msg_start  = NULL;
    void                                *p_data_buffer = NULL;
    INT8                                *p_read, *p_write;

    /************************ audio_full_access_write function begins ***********************/

    if (p_audio_gbl_var == NULL )
    {
      audio_mode_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* check if the data exist */
    if (p_parameter->data == NULL)
    {
      audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return(AUDIO_ERROR);
    }

    switch (p_parameter->variable_indentifier)
    {
    #if (AUDIO_DSP_FEATURES == 1)
      case AUDIO_MICROPHONE_GAIN:
      {
        if ( (*((INT16  *)p_parameter->data) < 0x0000) ||
             (*((INT16  *)p_parameter->data) > 0x7fff) )
          {
            audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
            return (AUDIO_ERROR);
          }
        size = sizeof(INT16 );
        break;
      }
      case AUDIO_SPEAKER_GAIN:
      {
        if ( (*((INT16  *)p_parameter->data) < 0x0000) ||
             (*((INT16  *)p_parameter->data) > 0x7fff) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT16 );
        break;
      }
      case AUDIO_MICROPHONE_SPEAKER_LOOP_SIDETONE:
      {
        if ( (*((INT16  *)p_parameter->data) < 0x0000) ||
             (*((INT16  *)p_parameter->data) > 0xffff) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT16 );
        break;
      }
      case AUDIO_VOLUME_SPEED:
      {
        if ( (*((INT16  *)p_parameter->data) < 0x0001) ||
             (*((INT16  *)p_parameter->data) > 0x7fff) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT16 );
        break;
      }
    #else // AUDIO_DSP_FEATURES == 0
      case AUDIO_MICROPHONE_MODE:
      {
#if (ANLG_FAM != 11)
        if ( (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_HANDHELD) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_HANDFREE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_HEADSET) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_HEADSET_DIFFERENTIAL)
             )
#else
        if ( (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_NONE) &&
	     (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_HANDSET_25_6DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_HEADSET_4_9_DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_HEADSET_25_6DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_FM) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_HEADSET_18DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_AUX_4_9DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_AUX_28_2DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_FM_MONO) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_CARKIT)
             )
#endif
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
      case AUDIO_MICROPHONE_GAIN:
      {
        if (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_MUTE)
        {
          if ( (*((INT8  *)p_parameter->data) < -12) ||
               (*((INT8  *)p_parameter->data) > 12) )
          {
            audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
            return (AUDIO_ERROR);
          }
        }

        size = sizeof(INT8 );
        break;
      }
      case AUDIO_MICROPHONE_EXTRA_GAIN:
      {
#if (ANLG_FAM != 11)
        if ( (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_AUX_GAIN_28_2dB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_AUX_GAIN_4_6dB) )
#else
		i = (*((INT8  *)p_parameter->data))%2;

        if ( (i != 0) || (*((INT8  *)p_parameter->data) < -2) ||
             (*((INT8  *)p_parameter->data) > 14) )
#endif
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
      case AUDIO_MICROPHONE_OUTPUT_BIAS:
      {
        if ( (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_OUTPUT_BIAS_2_0V) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MICROPHONE_OUTPUT_BIAS_2_5V) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
      case AUDIO_SPEAKER_MODE:
      {
#if (ANLG_FAM != 11)
        if ( (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HANDHELD) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HANDFREE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HEADSET) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_BUZZER) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HANDHELD_HANDFREE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HANDHELD_8OHM)
           )
#else
        if ( (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_NONE) &&
			 (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HANDHELD) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HANDFREE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HEADSET) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_AUX) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_CARKIT)&&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_INVERTED_VOICE)&&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HANDFREE_CLASS_D)
           )
#endif
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }

#if(AS_FM_RADIO==1)
 case AUDIO_FM_SPEAKER_MODE:
      {
#if (ANLG_FAM != 11)
        if ( (*((INT8  *)p_parameter->data) != AUDIO_FM_SPEAKER_NONE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_FM_STEREO_SPEAKER_HEADSET) &&
             (*((INT8  *)p_parameter->data) != AUDIO_FM_MONO_SPEAKER_HANDFREE) &&
             (*((INT8  *)p_parameter->data) !=AUDIO_FM_MONO_SPEAKER_HANDFREE_CLASSD)

           )
#else
       if ( (*((INT8  *)p_parameter->data) != AUDIO_FM_SPEAKER_NONE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_FM_STEREO_SPEAKER_HEADSET) &&
             (*((INT8  *)p_parameter->data) != AUDIO_FM_MONO_SPEAKER_HANDFREE) &&
             (*((INT8  *)p_parameter->data) !=AUDIO_FM_MONO_SPEAKER_HANDFREE_CLASSD)

           )
#endif
        {
          AUDIO_SEND_TRACE("AUDIO MODE WRITE:in function caseFM mode:",RV_TRACE_LEVEL_DEBUG_LOW);
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
#endif
      case AUDIO_SPEAKER_GAIN:
      {
        if ( (*((INT8  *)p_parameter->data) < -6) ||
             (*((INT8  *)p_parameter->data) > 6) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
      case AUDIO_SPEAKER_EXTRA_GAIN:
      {
        if ( ((*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_SPK_GAIN_2_5DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_SPK_GAIN_8_5DB)) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_SPK_GAIN_MINUS_3_5DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_SPK_GAIN_MINUS_22_5DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_EAR_GAIN_MINUS_11DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_EAR_GAIN_1DB))
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
      case AUDIO_STEREO_SPEAKER_EXTRA_GAIN:
      {
        if ( ((*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_SPK_GAIN_2_5DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_SPK_GAIN_8_5DB)) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_SPK_GAIN_MINUS_3_5DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_SPK_GAIN_MINUS_22_5DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_EAR_GAIN_MINUS_11DB) &&
             (*((INT8  *)p_parameter->data) != AUDIO_EAR_GAIN_1DB))
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
      case AUDIO_SPEAKER_FILTER:
      {
        if ( (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_FILTER_ON) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_FILTER_OFF) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8);
        break;
      }
      case AUDIO_SPEAKER_HIGHPASS_FILTER:
      {
        if ( (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HIGHPASS_FILTER_ON) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_HIGHPASS_FILTER_OFF) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8);
        break;
      }
#if (ANLG_FAM != 11)
      case AUDIO_SPEAKER_BUZZER_STATE:
      {
        if ( (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_BUZZER_ON) &&
             (*((INT8  *)p_parameter->data) != AUDIO_SPEAKER_BUZZER_OFF) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
#endif
      case AUDIO_MICROPHONE_SPEAKER_LOOP_SIDETONE:
      {
        i = ((*((INT8  *)p_parameter->data))-1)%3;

        if ( (*((INT8  *)p_parameter->data) != AUDIO_SIDETONE_OPEN) &&
             ( (*((INT8  *)p_parameter->data) < -23) ||
               (*((INT8  *)p_parameter->data) > 1)   ||
               (i != 0) ) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
      case AUDIO_STEREO_SPEAKER_MODE:
      {
#if (ANLG_FAM != 11)
        if ( (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_HANDHELD) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_HANDFREE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_HEADPHONE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_HANDHELD_8OHM)
           )
#else
        if ( (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_NONE) &&
			(*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_HANDHELD) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_HANDFREE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_HEADPHONE) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_AUX) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_CARKIT)&&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SPEAKER_HANDFREE_CLASS_D)
           )
#endif
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8);
        break;
      }
      case AUDIO_STEREO_SPEAKER_STEREO_MONO:
      {
#if (ANLG_FAM != 11)
        if ( (*((INT8  *)p_parameter->data) != AUDIO_STEREO) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MONO_LEFT) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MONO_RIGHT) &&
             (*((INT8  *)p_parameter->data) != (AUDIO_MONO_LEFT|AUDIO_MONO_RIGHT))
           )
#else
        if ( (*((INT8  *)p_parameter->data) != AUDIO_STEREO) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MONO_RIGHT) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MONO_LEFT) &&
             (*((INT8  *)p_parameter->data) != AUDIO_MONO_LEFT_RIGHT)
           )
#endif
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8);
        break;
      }
      case AUDIO_STEREO_SPEAKER_SAMPLING_FREQUENCY:
      {
#if (ANLG_FAM != 11)
        if ( (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_48KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_44_1KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_32KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_22_05KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_16KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_11_025KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_8KHZ)
           )
#else
        if ( (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_48KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_44_1KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_32KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_24KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_22_05KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_16KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_12KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_11_025KHZ) &&
             (*((INT8  *)p_parameter->data) != AUDIO_STEREO_SAMPLING_FREQUENCY_8KHZ)
           )
#endif
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8);
        break;
      }
      case AUDIO_SPEAKER_VOLUME_LEVEL:
      {
        if ( (((T_AUDIO_SPEAKER_LEVEL *)p_parameter->data)->audio_speaker_level != AUDIO_SPEAKER_VOLUME_MUTE) &&
             (((T_AUDIO_SPEAKER_LEVEL *)p_parameter->data)->audio_speaker_level != AUDIO_SPEAKER_VOLUME_0dB) &&
             (((T_AUDIO_SPEAKER_LEVEL *)p_parameter->data)->audio_speaker_level != AUDIO_SPEAKER_VOLUME_6dB) &&
             (((T_AUDIO_SPEAKER_LEVEL *)p_parameter->data)->audio_speaker_level != AUDIO_SPEAKER_VOLUME_12dB) &&
             (((T_AUDIO_SPEAKER_LEVEL *)p_parameter->data)->audio_speaker_level != AUDIO_SPEAKER_VOLUME_18dB) &&
             (((T_AUDIO_SPEAKER_LEVEL *)p_parameter->data)->audio_speaker_level != AUDIO_SPEAKER_VOLUME_24dB) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(T_AUDIO_SPEAKER_LEVEL);
        break;
      }
      case AUDIO_STEREO_SPEAKER_VOLUME_LEVEL:
      {
        if ( (((T_AUDIO_STEREO_SPEAKER_LEVEL *)p_parameter->data)->audio_stereo_speaker_level_left != AUDIO_STEREO_SPEAKER_VOLUME_MUTE) &&
             (((T_AUDIO_STEREO_SPEAKER_LEVEL *)p_parameter->data)->audio_stereo_speaker_level_left > 30)
           )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        if ( (((T_AUDIO_STEREO_SPEAKER_LEVEL *)p_parameter->data)->audio_stereo_speaker_level_right != AUDIO_STEREO_SPEAKER_VOLUME_MUTE) &&
             (((T_AUDIO_STEREO_SPEAKER_LEVEL *)p_parameter->data)->audio_stereo_speaker_level_right > 30)
           )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(T_AUDIO_STEREO_SPEAKER_LEVEL);
        break;
      }
    #endif // AUDIO_DSP_FEATURES

      case AUDIO_PATH_USED:
      {
        if ( (*((T_AUDIO_VOICE_PATH_SETTING *)p_parameter->data) != AUDIO_GSM_VOICE_PATH) &&
             (*((T_AUDIO_VOICE_PATH_SETTING *)p_parameter->data) != AUDIO_BLUETOOTH_CORDLESS_VOICE_PATH) &&
             (*((T_AUDIO_VOICE_PATH_SETTING *)p_parameter->data) != AUDIO_BLUETOOTH_HEADSET) &&
             (*((T_AUDIO_VOICE_PATH_SETTING *)p_parameter->data) != AUDIO_DAI_ENCODER) &&
             (*((T_AUDIO_VOICE_PATH_SETTING *)p_parameter->data) != AUDIO_DAI_DECODER) &&
             (*((T_AUDIO_VOICE_PATH_SETTING *)p_parameter->data) != AUDIO_DAI_ACOUSTIC) )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(T_AUDIO_VOICE_PATH_SETTING);
        break;
      }
      case AUDIO_MICROPHONE_FIR:
      {
        size = sizeof(T_AUDIO_FIR_COEF);
        break;
      }
      case AUDIO_SPEAKER_FIR:
      {
        size = sizeof(T_AUDIO_FIR_COEF);
        break;
      }
      case AUDIO_MICROPHONE_SPEAKER_LOOP_AEC:
      {
      #if (L1_AEC == 1)
        T_AUDIO_AEC_CFG *aec_param;
        aec_param = (T_AUDIO_AEC_CFG *)p_parameter->data;

      #if (L1_NEW_AEC)
        if ( ((aec_param->aec_enable != AUDIO_AEC_ENABLE) &&
              (aec_param->aec_enable != AUDIO_AEC_DISABLE)) ||
             ((aec_param->aec_visibility != AUDIO_AEC_VISIBILITY_ENABLE) &&
              (aec_param->aec_visibility != AUDIO_AEC_VISIBILITY_DISABLE)) ||
             ((aec_param->continuous_filtering != TRUE) &&
              (aec_param->continuous_filtering != FALSE))
             #if (L1_ANR ==0)
              ||
              ((aec_param->noise_suppression_enable != AUDIO_NOISE_SUPPRESSION_ENABLE) &&
                (aec_param->noise_suppression_enable != AUDIO_NOISE_SUPPRESSION_DISABLE)) ||
               ((aec_param->noise_suppression_level != AUDIO_NOISE_NO_LIMITATION) &&
                (aec_param->noise_suppression_level != AUDIO_NOISE_6dB) &&
                (aec_param->noise_suppression_level != AUDIO_NOISE_12dB) &&
                (aec_param->noise_suppression_level != AUDIO_NOISE_18dB))
             #endif
                )
      #else
        if ( ((aec_param->aec_enable != AUDIO_AEC_ENABLE) &&
              (aec_param->aec_enable != AUDIO_AEC_DISABLE)) ||
             ((aec_param->aec_mode != AUDIO_SHORT_ECHO) &&
              (aec_param->aec_mode != AUDIO_LONG_ECHO)) ||
             ((aec_param->echo_suppression_level != AUDIO_ECHO_0dB) &&
             (aec_param->echo_suppression_level != AUDIO_ECHO_6dB) &&
             (aec_param->echo_suppression_level != AUDIO_ECHO_12dB) &&
              (aec_param->echo_suppression_level != AUDIO_ECHO_18dB))
             #if (L1_ANR == 0)
               ||
               ((aec_param->noise_suppression_enable != AUDIO_NOISE_SUPPRESSION_ENABLE) &&
                (aec_param->noise_suppression_enable != AUDIO_NOISE_SUPPRESSION_DISABLE)) ||
              ((aec_param->noise_suppression_level != AUDIO_NOISE_NO_LIMITATION) &&
              (aec_param->noise_suppression_level != AUDIO_NOISE_6dB) &&
             (aec_param->noise_suppression_level != AUDIO_NOISE_12dB) &&
              (aec_param->noise_suppression_level != AUDIO_NOISE_18dB))
            #endif
              )
      #endif
	  #endif
	  #if (L1_AEC == 2)
	    T_AUDIO_AQI_AEC_CFG *aec_param;
        aec_param = (T_AUDIO_AQI_AEC_CFG *)p_parameter->data;
        if ( ((aec_param->aec_control != AUDIO_AQI_AEC_STOP) &&
              (aec_param->aec_control != AUDIO_AQI_AEC_START) &&
              (aec_param->aec_control != AUDIO_AQI_AEC_UPDATE)) ||
             ((aec_param->aec_mode != AUDIO_AQI_AEC_1_8_WITH_VAD_ES) &&
              (aec_param->aec_mode != AUDIO_AQI_AEC_1_8_WITHOUT_VAD_ES) &&
              (aec_param->aec_mode != AUDIO_AQI_AEC_2_0_WITH_VAD_ES) &&
              (aec_param->aec_mode != AUDIO_AQI_AEC_2_0_WITHOUT_VAD_ES)) ||
             ((aec_param->mu != AUDIO_AQI_MU)) ||
             ((aec_param->cont_filter != AUDIO_AQI_CONT_FILT_ENABLED) &&
              (aec_param->cont_filter != AUDIO_AQI_CONT_FILT_DISABLED)) ||
             ((aec_param->scale_input_ul != AUDIO_AQI_NO_SCALING_UL)&&
              (aec_param->scale_input_ul != AUDIO_AQI_SCALING_UL))  ||
             ((aec_param->scale_input_dl != AUDIO_AQI_NO_SCALING_DL)&&
              (aec_param->scale_input_dl != AUDIO_AQI_SCALING_DL))  ||
             ((aec_param->div_dmax != AUDIO_AQI_DIV_DMAX))  ||
             ((aec_param->div_swap_good != AUDIO_AQI_DIV_SWAPE_GOOD))  ||
             ((aec_param->div_swap_bad != AUDIO_AQI_DIV_SWAPE_BAD))  ||
             ((aec_param->block_init != AUDIO_AQI_BLOCK_INIT))  ||
             ((aec_param->fact_vad != AUDIO_AQI_FACT_VAD))  ||
             ((aec_param->fact_asd_fil != AUDIO_AQI_FACT_ASD_FIL))  ||
             ((aec_param->fact_asd_mut != AUDIO_AQI_FACT_ASD_MUT))  ||
             ((aec_param->thrs_abs != AUDIO_AQI_THR_ABS))  ||
             ((aec_param->es_level_max < AUDIO_AQI_ES_LEVEL_MIN)&&
              (aec_param->es_level_max > AUDIO_AQI_ES_LEVEL_MAX))  ||
             ((aec_param->granularity_att != AUDIO_AQI_GRANUL_ATT))  ||
             ((aec_param->coef_smooth < AUDIO_AQI_COEF_SMOOTH_MIN)&&
              (aec_param->coef_smooth > AUDIO_AQI_COEF_SMOOTH_MAX))
              )
	  #endif
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }
	    #if (L1_AEC == 1)
        size = sizeof(T_AUDIO_AEC_CFG);
		#endif
		#if (L1_AEC == 2)
        size = sizeof(T_AUDIO_AQI_AEC_CFG);
		#endif
        break;
      }


    #if (AUDIO_DSP_ONOFF == 1)
      case AUDIO_ONOFF:
      {
        if ( (*((INT8  *)p_parameter->data) != AUDIO_ON) &&
             (*((INT8  *)p_parameter->data) != AUDIO_OFF)
           )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(INT8 );
        break;
      }
    #endif // AUDIO_DSP_ONOFF

    #if (L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
      case AUDIO_ONOFF:
      {
        if ( ((((T_AUDIO_MODE_ONOFF *)p_parameter->data)->vul_onoff != AUDIO_VUL_OFF) &&
             (((T_AUDIO_MODE_ONOFF *)p_parameter->data)->vul_onoff != AUDIO_VUL_ON) &&
             (((T_AUDIO_MODE_ONOFF *)p_parameter->data)->vul_onoff != AUDIO_VUL_NO_ACTION)) ||
	     ((((T_AUDIO_MODE_ONOFF *)p_parameter->data)->vdl_onoff != AUDIO_VDL_OFF) &&
             (((T_AUDIO_MODE_ONOFF *)p_parameter->data)->vdl_onoff != AUDIO_VDL_ON) &&
             (((T_AUDIO_MODE_ONOFF *)p_parameter->data)->vdl_onoff != AUDIO_VDL_NO_ACTION))
	   )
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(T_AUDIO_MODE_ONOFF);
        break;
      }
    #endif // AUDIO_DSP_ONOFF

    #if (L1_ANR == 1)
      case AUDIO_MICROPHONE_ANR:
      {
        if ( (((T_AUDIO_ANR_CFG *)p_parameter->data)->anr_enable!= AUDIO_ANR_ENABLE) &&
             (((T_AUDIO_ANR_CFG *)p_parameter->data)->anr_enable!= AUDIO_ANR_DISABLE))
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(T_AUDIO_ANR_CFG);
        break;
      }
    #endif
    #if (L1_ANR == 2)
      case AUDIO_MICROPHONE_ANR:
      {
        if ( (((T_AUDIO_AQI_ANR_CFG *)p_parameter->data)->parameters.control != AUDIO_ANR_DISABLE) &&
             (((T_AUDIO_AQI_ANR_CFG *)p_parameter->data)->parameters.control != AUDIO_ANR_TONEDET_ENABLE) &&
             (((T_AUDIO_AQI_ANR_CFG *)p_parameter->data)->parameters.control != AUDIO_ANR_ENABLE_TONEDET_DISABLE))
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(T_AUDIO_AQI_ANR_CFG);
        break;
      }
    #endif

    #if (L1_AGC_UL== 1)
      case AUDIO_MICROPHONE_AGC:
      {
        if ( (((T_AUDIO_AQI_AGC_UL_REQ *)p_parameter->data)->parameters.control != AUDIO_AGC_ENABLE) &&
             (((T_AUDIO_AQI_AGC_UL_REQ *)p_parameter->data)->parameters.control!= AUDIO_AGC_DISABLE))
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(T_AUDIO_AQI_AGC_UL_REQ);
        break;
      }
    #endif
    #if (L1_AGC_DL== 1)
      case AUDIO_SPEAKER_AGC:
      {
        if ( (((T_AUDIO_AQI_AGC_DL_REQ *)p_parameter->data)->parameters.control != AUDIO_AGC_ENABLE) &&
             (((T_AUDIO_AQI_AGC_DL_REQ *)p_parameter->data)->parameters.control!= AUDIO_AGC_DISABLE))
        {
          audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
          return (AUDIO_ERROR);
        }

        size = sizeof(T_AUDIO_AQI_AGC_DL_REQ);
        break;
      }
    #endif

    #if (L1_LIMITER == 1)
      case AUDIO_SPEAKER_LIMITER:
      {
        if ( ((T_AUDIO_LIMITER_CFG *)p_parameter->data)->limiter_enable == AUDIO_LIMITER_DISABLE )
        {
          size = sizeof(T_AUDIO_LIMITER_CFG);
          break;
        }
        else
        {
          if ( (((T_AUDIO_LIMITER_CFG *)p_parameter->data)->limiter_enable!= AUDIO_LIMITER_ENABLE) ||
               (((T_AUDIO_LIMITER_CFG *)p_parameter->data)->block_size!= AUDIO_LIMITER_BLOCK_SIZE)  ||
               (((T_AUDIO_LIMITER_CFG *)p_parameter->data)->nb_fir_coefs > AUDIO_LIMITER_MAX_FILTER_COEFS) ||
               ((((T_AUDIO_LIMITER_CFG *)p_parameter->data)->nb_fir_coefs) & 0x00)) /* odd number */
          {
            audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
            return (AUDIO_ERROR);
          }
        }
        size = sizeof(T_AUDIO_LIMITER_CFG);
        break;
      }
    #endif

    #if (L1_IIR == 1)
      case AUDIO_SPEAKER_IIR:
      {
        T_AUDIO_IIR_CFG *iir_param;
        iir_param = (T_AUDIO_IIR_CFG *)p_parameter->data;

        if (iir_param->iir_enable == AUDIO_IIR_DISABLE)
        {
           size = sizeof(T_AUDIO_IIR_CFG);
           break;
        }
        else
        {
          if ( (iir_param->iir_enable!= AUDIO_IIR_ENABLE)||
               (iir_param->nb_iir_blocks !=0) &&
               ((iir_param->nb_iir_blocks < AUDIO_IIR_MIN_IIR_BLOCKS) ||
               (iir_param->nb_iir_blocks > AUDIO_IIR_MAX_IIR_BLOCKS))||
               (iir_param->nb_fir_coefs  < AUDIO_IIR_MIN_FIR_COEFS) ||
               (iir_param->nb_fir_coefs  > AUDIO_IIR_MAX_FIR_COEFS)||
               (iir_param->input_scaling < AUDIO_IIR_MIN_RANGE) ||
               (iir_param->input_scaling > AUDIO_IIR_MAX_RANGE)||
               (iir_param->fir_scaling < AUDIO_IIR_MIN_RANGE) ||
               (iir_param->fir_scaling > AUDIO_IIR_MAX_RANGE)||
               (iir_param->input_gain_scaling  < AUDIO_IIR_MIN_RANGE) ||
               (iir_param->input_gain_scaling  > AUDIO_IIR_MAX_RANGE)    ||
               (iir_param->output_gain_scaling < AUDIO_IIR_MIN_RANGE) ||
               (iir_param->output_gain_scaling > AUDIO_IIR_MAX_RANGE))
          {
            audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
            return (AUDIO_ERROR);
          }
        }

        size = sizeof(T_AUDIO_IIR_CFG);
        break;
      }
    #elif (L1_IIR == 2)
	case AUDIO_SPEAKER_IIR:
      {
	 T_AUDIO_IIR_DL_CFG * iir_param;
	 iir_param = (T_AUDIO_IIR_DL_CFG *)p_parameter->data;

	 if(iir_param->iir_dl_control == AUDIO_IIR_STOP ||
	 	iir_param->parameters.control == AUDIO_IIR_DISABLE)
	 {
	    size = sizeof(T_AUDIO_IIR_DL_CFG);
	 }
	 else
	 {
	    if((iir_param->parameters.control != AUDIO_IIR_ENABLE) ||
		((iir_param->iir_dl_control != AUDIO_IIR_START) &&
		(iir_param->iir_dl_control != AUDIO_IIR_UPDATE)) ||
		(iir_param->parameters.frame_size != AUDIO_IIR_FRAME_SIZE) ||
		((iir_param->parameters.fir_filter.fir_enable == AUDIO_IIR_ENABLE_FIR) &&
		((iir_param->parameters.fir_filter.fir_length < AUDIO_IIR_MIN_FIR_TAPS) ||
		(iir_param->parameters.fir_filter.fir_length > AUDIO_IIR_MAX_FIR_TAPS))) ||
		((iir_param->parameters.sos_filter.sos_enable == AUDIO_IIR_ENABLE_IIR) &&
		((iir_param->parameters.sos_filter.sos_number < AUDIO_IIR_MIN_IIR_BLOCKS) ||
		(iir_param->parameters.sos_filter.sos_number > AUDIO_IIR_MAX_IIR_BLOCKS))))
	    {
	       audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
              return (AUDIO_ERROR);
	    }
	 }

	 size = sizeof(T_AUDIO_IIR_DL_CFG);

        break;
      }
    #endif

   #if (L1_DRC == 1)
   case AUDIO_SPEAKER_DRC:
   {
        T_AUDIO_DRC_CFG *drc_param;
        drc_param = (T_AUDIO_DRC_CFG *)p_parameter->data;

        if (drc_param->drc_dl_control != AUDIO_DRC_STOP &&
                       drc_param->drc_dl_control != AUDIO_DRC_START &&
                       drc_param->drc_dl_control != AUDIO_DRC_UPDATE)
        {
            audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
            return (AUDIO_ERROR);
        }

        size = sizeof(T_AUDIO_DRC_CFG);
        break;
    }
    #endif

    #if (L1_ES == 1)
      case AUDIO_MICROPHONE_ES:
      {
        T_AUDIO_ES_CFG *es_param;
        es_param = (T_AUDIO_ES_CFG *)p_parameter->data;

        if (es_param->es_enable == AUDIO_ES_DISABLE)
        {
           size = sizeof(T_AUDIO_ES_CFG);
           break;
        }
        else
        {
          if ( (es_param->es_enable != AUDIO_ES_ENABLE)||
               ( (es_param->es_behavior > AUDIO_ES_BEHAVIOUR_3) && (es_param->es_behavior != AUDIO_ES_CUSTOM) ) ||
               ((es_param->es_behavior == AUDIO_ES_CUSTOM) &&
                ((!(es_param->es_mode & AUDIO_ES_B_ES_UL)) ||
                  ((es_param->es_mode & AUDIO_ES_B_CNG) &&
                   (es_param->es_mode & AUDIO_ES_B_NSF)))))
          {
            audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
            return (AUDIO_ERROR);
          }
        }

        size = sizeof(T_AUDIO_ES_CFG);
        break;
      }
    #endif

      default:
      {
        audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        return (AUDIO_ERROR);
      }
    } // switch (p_parameter->variable_indentifier)

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                             sizeof (T_AUDIO_MODE_FULL_ACCESS_WRITE_REQ),
                             (T_RVF_BUFFER **) (&p_msg_start));

    /* If insufficient resources, then report a memory error and abort.               */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* allocate the memory for the data to write */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                             size,
                             (T_RVF_BUFFER **) (&p_data_buffer));

    /* If insufficient resources, then report a memory error and abort.               */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
      rvf_free_buf((T_RVF_BUFFER *)p_data_buffer);
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill the message parameters */
    p_msg_start->audio_parameter.data = p_data_buffer;

    p_read  = (INT8 *)(p_parameter->data);
    p_write = (INT8 *)p_data_buffer;
    for (i=0; i<size; i++)
    {
      *p_write++ = *p_read++;
    }

    p_msg_start->audio_parameter.variable_indentifier = p_parameter->variable_indentifier;

    /* fill the message id */
    p_msg_start->os_hdr.msg_id    = AUDIO_MODE_WRITE_REQ;

    /* fill the task source id */
    p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
    p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    if (return_path.callback_func == NULL)
    {
      p_msg_start->return_path.addr_id = return_path.addr_id;
      p_msg_start->return_path.callback_func = NULL;
    }
    else
    {
      p_msg_start->return_path.callback_func = return_path.callback_func;
    }

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId,
                  p_msg_start);

    return (AUDIO_OK);
    /************************ Enf of audio_full_access_write function ***********************/
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_full_access_read                                   */
  /*                                                                              */
  /*    Purpose:  This function is called to read the current audio mode.         */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status                                                                */
  /*        Audio settings                                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_full_access_read (T_AUDIO_FULL_ACCESS_READ *p_parameter)
  {
    T_AUDIO_RET status = AUDIO_ERROR;

    /************************ audio_full_access_write function begins ***********************/

    if (p_audio_gbl_var == NULL )
    {
      audio_mode_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    switch (p_parameter->variable_indentifier)
    {
    #if (AUDIO_DSP_FEATURES == 1)
      case AUDIO_MICROPHONE_GAIN:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: microphone gain", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_microphone_gain_read((INT16 *)p_parameter->data);
        break;
      }
      case AUDIO_SPEAKER_GAIN:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker gain", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_speaker_gain_read((INT16 *)p_parameter->data);
        break;
      }
      case AUDIO_MICROPHONE_SPEAKER_LOOP_SIDETONE:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: sidetone gain", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_sidetone_gain_read((INT16 *)p_parameter->data);
        break;
      }
      case AUDIO_VOLUME_SPEED:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: audio volume speed", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_volume_speed_read((INT16 *)p_parameter->data);
        break;
      }
    #else // AUDIO_DSP_FEATURES == 0
      case AUDIO_MICROPHONE_MODE:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: microphone mode", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_microphone_mode_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_MICROPHONE_GAIN:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: microphone gain", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_microphone_gain_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_MICROPHONE_EXTRA_GAIN:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: microphone extra gain", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_microphone_extra_gain_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_MICROPHONE_OUTPUT_BIAS:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: output bias", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_microphone_output_bias_read((INT8 *)p_parameter->data);
        break;
      }
      #if(AS_FM_RADIO==1)
      case AUDIO_FM_SPEAKER_MODE:
		{
		 AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker FM  mode", RV_TRACE_LEVEL_DEBUG_LOW);
		status =audio_mode_fm_mode_read((UINT8 *)p_parameter->data);
		break;
      		}
      #endif
      case AUDIO_SPEAKER_MODE:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker mode", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_speaker_mode_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_SPEAKER_GAIN:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker gain", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_speaker_gain_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_SPEAKER_EXTRA_GAIN:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker extra gain", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_speaker_extra_gain_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_STEREO_SPEAKER_EXTRA_GAIN:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker extra gain", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_stereo_speaker_extra_gain_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_SPEAKER_FILTER:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker filter", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_speaker_filter_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_SPEAKER_HIGHPASS_FILTER:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker highpass filter", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_speaker_highpass_filter_read((INT8 *)p_parameter->data);
        break;
      }
         case AUDIO_SPEAKER_BUZZER_STATE:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker buzzer", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_speaker_buzzer_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_MICROPHONE_SPEAKER_LOOP_SIDETONE:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: sidetone gain", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_sidetone_gain_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_STEREO_SPEAKER_MODE:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: stereo speaker mode", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_stereo_speaker_mode_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_STEREO_SPEAKER_STEREO_MONO:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: stereo speaker stereo/mono", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_stereo_speaker_stereo_mono_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_STEREO_SPEAKER_SAMPLING_FREQUENCY:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: stereo speaker sampling frequency", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_stereo_speaker_sampling_freq_read((INT8 *)p_parameter->data);
        break;
      }
      case AUDIO_SPEAKER_VOLUME_LEVEL:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker volume", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_speaker_volume_read((T_AUDIO_SPEAKER_LEVEL *)p_parameter->data);
        break;
      }
      case AUDIO_STEREO_SPEAKER_VOLUME_LEVEL:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: stereo speaker volume", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_stereo_speaker_volume_read((T_AUDIO_STEREO_SPEAKER_LEVEL *)p_parameter->data);
        break;
      }
    #endif // AUDIO_DSP_FEATURES == 1

      case AUDIO_PATH_USED:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: voice path", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_voice_path_read((T_AUDIO_VOICE_PATH_SETTING *)p_parameter->data);
        break;
      }
      case AUDIO_MICROPHONE_FIR:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: microphone FIR", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_microphone_fir_read((T_AUDIO_FIR_COEF *)p_parameter->data);
        break;
      }
      case AUDIO_SPEAKER_FIR:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: speaker_FIR", RV_TRACE_LEVEL_DEBUG_LOW);
        status = audio_mode_speaker_fir_read((T_AUDIO_FIR_COEF *)p_parameter->data);
        break;
      }
      case AUDIO_MICROPHONE_SPEAKER_LOOP_AEC:
      {
        AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: AEC", RV_TRACE_LEVEL_DEBUG_LOW);
        #if (L1_AEC == 1)
        status = audio_mode_aec_read((T_AUDIO_AEC_CFG *)p_parameter->data);
		#endif
        #if (L1_AEC == 2)
        status = audio_mode_aec_read((T_AUDIO_AQI_AEC_CFG *)p_parameter->data);
        #endif
        break;
      }
      #if (L1_ANR ==1)
        case AUDIO_MICROPHONE_ANR:
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: ANR", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_microphone_anr_read((T_AUDIO_ANR_CFG *)p_parameter->data);
          break;
        }
      #endif
      #if (L1_ANR == 2)
        case AUDIO_MICROPHONE_ANR:
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: ANR", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_microphone_anr_read((T_AUDIO_AQI_ANR_CFG *)p_parameter->data);
          break;
        }
      #endif
      #if (L1_ES ==1)
        case AUDIO_MICROPHONE_ES:
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: ES", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_microphone_es_read((T_AUDIO_ES_CFG *)p_parameter->data);
          break;
        }
      #endif

      #if (L1_IIR ==1)
        case AUDIO_SPEAKER_IIR:
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: IIR", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_speaker_iir_read((T_AUDIO_IIR_CFG *)p_parameter->data);
          break;
        }
      #elif(L1_IIR == 2)
	 case AUDIO_SPEAKER_IIR:
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: IIR 4.x", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_speaker_iir_read((T_AUDIO_IIR_DL_CFG *)p_parameter->data);
          break;
        }
      #endif
     #if (L1_DRC ==1)
        case AUDIO_SPEAKER_DRC:
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: DRC", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_speaker_drc_read((T_AUDIO_DRC_CFG *)p_parameter->data);
          break;
        }
      #endif
     #if (L1_AGC_UL==1)
        case AUDIO_MICROPHONE_AGC:
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: AGC_UL", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_agc_ul_read((T_AUDIO_AQI_AGC_UL_REQ *)p_parameter->data);
          break;
        }
      #endif
      #if (L1_AGC_DL==1)
        case AUDIO_SPEAKER_AGC:
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: AGC_DL", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_agc_dl_read((T_AUDIO_AQI_AGC_DL_REQ *)p_parameter->data);
          break;
        }
      #endif
      #if (L1_LIMITER ==1)
        case AUDIO_SPEAKER_LIMITER:
        {
          AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: LIMITER", RV_TRACE_LEVEL_DEBUG_LOW);
          status = audio_mode_speaker_limiter_read((T_AUDIO_LIMITER_CFG *)p_parameter->data);
          break;
        }
      #endif

    #if (AUDIO_DSP_ONOFF == 1)
       case AUDIO_ONOFF:
       {
         AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: audio on/off", RV_TRACE_LEVEL_DEBUG_LOW);
         status = audio_mode_onoff_read((INT8 *)p_parameter->data);
         break;
       }
    #endif
    #if (L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
       case AUDIO_ONOFF:
       {
         AUDIO_SEND_TRACE("AUDIO MODE FULL ACCESS READ: audio on/off", RV_TRACE_LEVEL_DEBUG_LOW);
         status = audio_mode_onoff_read((T_AUDIO_MODE_ONOFF *)p_parameter->data);
         break;
       }
    #endif
      default:
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE FULL ACCESS READ: identifier not supported",
          p_parameter->variable_indentifier, RV_TRACE_LEVEL_DEBUG_LOW);
        return (AUDIO_ERROR);
      }
    }

    return(status);

    /************************ Enf of audio_full_access_write function ***********************/
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_save                                          */
  /*                                                                              */
  /*    Purpose:  This function is called to save the current audio mode.         */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        audio mode file name.                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status                                                                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_save (T_AUDIO_MODE_SAVE *p_parameter, T_RV_RETURN return_path)
  {
    /************************ audio_full_access_write function begins ***********************/

#if(AS_RFS_API == 1)
	T_RFS_FD              audio_mode_ffs_fd, audio_volume_ffs_fd;
	T_WCHAR               audio_mode_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#else
	T_FFS_FD              audio_mode_ffs_fd, audio_volume_ffs_fd;
	char                  audio_mode_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#endif
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
#if(AS_RFS_API == 1)
	T_RFS_FD audio_stereo_volume_ffs_fd;
#else
  T_FFS_FD audio_stereo_volume_ffs_fd;
#endif
  #endif
	T_AUDIO_MODE_SAVE_REQ *p_msg_start;
  #if(AS_RFS_API == 1)
		T_WCHAR mp_uc1[15];
  		T_WCHAR mp_uc2[15];
		T_WCHAR mp_uc3[15];
		T_WCHAR mp_uc4[15];
  		char *str1 = "/FFS/aud/";
		char *str2 = ".cfg";
		char *str3 = ".vol";
		char *str4 = ".volst";
  #endif

    T_RVF_MB_STATUS       mb_status = RVF_GREEN;
#if(AS_RFS_API == 1)
	convert_u8_to_unicode(str1, mp_uc1);
	convert_u8_to_unicode(str2, mp_uc2);
	convert_u8_to_unicode(str3, mp_uc3);
	convert_u8_to_unicode(str4, mp_uc4);
#endif
    if (p_audio_gbl_var == NULL )
    {
      audio_mode_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* Create the audio mode path name */
    audio_mode_path_name[0] = 0;
#if(AS_RFS_API == 1)
 	wstrcpy(audio_mode_path_name,mp_uc1);
    wstrcpy(&(audio_mode_path_name[9]), p_parameter->audio_mode_filename);
     wstrcat(audio_mode_path_name,mp_uc2);
#else
	strcpy(audio_mode_path_name,"/aud/");
    strcpy(&(audio_mode_path_name[5]), p_parameter->audio_mode_filename);
    strcat(audio_mode_path_name,".cfg");
#endif

    /* Open the FFS file */
    #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	audio_mode_ffs_fd = rfs_open(audio_mode_path_name,
        RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	audio_mode_ffs_fd = ffs_open(audio_mode_path_name,
        FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
      if (audio_mode_ffs_fd < EFFS_OK)
      {
        audio_mode_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return(AUDIO_ERROR);
    }
    #else
      audio_mode_ffs_fd = 0x00000110;
    #endif

    /* Create the volume path name */
    audio_mode_path_name[9] = 0;
#if(AS_RFS_API == 1)
	wstrcpy(&(audio_mode_path_name[9]), p_parameter->audio_mode_filename);
     wstrcat(audio_mode_path_name,mp_uc3);
#else
	strcpy(&(audio_mode_path_name[5]), p_parameter->audio_mode_filename);
    strcat(audio_mode_path_name,".vol");
#endif

    /* Open the FFS file */
    #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	audio_volume_ffs_fd = rfs_open(audio_mode_path_name,
        RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	audio_volume_ffs_fd = ffs_open(audio_mode_path_name,
        FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
#if(AS_RFS_API == 1)
		if (audio_volume_ffs_fd < RFS_EOK)
#else
	  if (audio_volume_ffs_fd < EFFS_OK)
#endif
      {
#if(AS_RFS_API == 1)
		rfs_close(audio_mode_ffs_fd);
#else
		ffs_close(audio_mode_ffs_fd);
#endif
        audio_mode_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return(AUDIO_ERROR);
    }
    #else
      audio_volume_ffs_fd = 0x00000111;
    #endif

  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
    /* Create the volume path name */
    audio_mode_path_name[9] = 0;
#if(AS_RFS_API == 1)
	wstrcpy(&(audio_mode_path_name[9]), p_parameter->audio_mode_filename);
     wstrcat(audio_mode_path_name,mp_uc4);
#else
	strcpy(&(audio_mode_path_name[5]), p_parameter->audio_mode_filename);
    strcat(audio_mode_path_name,".volst");
#endif

    /* Open the FFS file */
#if(AS_RFS_API == 1)
	audio_stereo_volume_ffs_fd = rfs_open(audio_mode_path_name,
      RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	audio_stereo_volume_ffs_fd = ffs_open(audio_mode_path_name,
      FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
#if(AS_RFS_API == 1)
	if (audio_stereo_volume_ffs_fd < RFS_EOK)
#else
	if (audio_stereo_volume_ffs_fd < EFFS_OK)
#endif
    {
#if(AS_RFS_API == 1)
	  rfs_close(audio_mode_ffs_fd);
      rfs_close(audio_volume_ffs_fd);
#else
	  ffs_close(audio_mode_ffs_fd);
      ffs_close(audio_volume_ffs_fd);
#endif
      audio_mode_error_trace(AUDIO_ENTITY_FILE_ERROR);
      return(AUDIO_ERROR);
    }
  #endif

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                             sizeof (T_AUDIO_MODE_SAVE_REQ),
                             (T_RVF_BUFFER **) (&p_msg_start));

    /* If insufficient resources, then report a memory error and abort.               */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill the message parameters */
    p_msg_start->audio_ffs_fd = audio_mode_ffs_fd;
    p_msg_start->audio_volume_ffs_fd = audio_volume_ffs_fd;
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
    p_msg_start->audio_stereo_volume_ffs_fd = audio_stereo_volume_ffs_fd;
  #endif

    /* fill the message id */
    p_msg_start->os_hdr.msg_id    = AUDIO_MODE_SAVE_REQ;

    /* fill the task source id */
    p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
    p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    if (return_path.callback_func == NULL)
    {
      p_msg_start->return_path.addr_id = return_path.addr_id;
      p_msg_start->return_path.callback_func = NULL;
    }
    else
    {
      p_msg_start->return_path.callback_func = return_path.callback_func;
    }

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId,
                  p_msg_start);

    return (AUDIO_OK);
    /************************ Enf of audio_full_access_write function ***********************/
  }
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_save                                          */
  /*                                                                              */
  /*    Purpose:  This function is called to load an audio mode.                  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        audio mode file name.                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status                                                                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_load (T_AUDIO_MODE_LOAD *p_parameter, T_RV_RETURN return_path)
  {
    /************************ audio_full_access_write function begins ***********************/

#if(AS_RFS_API == 1)
	T_RFS_FD              audio_mode_ffs_fd, audio_volume_ffs_fd;
	T_WCHAR                  audio_path_name[AUDIO_PATH_NAME_MAX_SIZE];
    T_WCHAR                  audio_mode_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#else
	T_FFS_FD              audio_mode_ffs_fd, audio_volume_ffs_fd;
	char                  audio_path_name[AUDIO_PATH_NAME_MAX_SIZE];
    char                  audio_mode_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#endif
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
#if(AS_RFS_API == 1)
	T_RFS_FD              audio_stereo_volume_ffs_fd;
#else
  T_FFS_FD              audio_stereo_volume_ffs_fd;
#endif
  #endif
	T_AUDIO_MODE_LOAD_REQ *p_msg_start;
    #if(AS_RFS_API == 1)
		T_WCHAR mp_uc1[15];
  		T_WCHAR mp_uc2[15];
		T_WCHAR mp_uc3[15];
		T_WCHAR mp_uc4[15];
  		char *str1 = "/FFS/aud/";
		char *str2 = ".cfg";
		char *str3 = ".vol";
		char *str4 = ".volst";
  #endif


    T_RVF_MB_STATUS       mb_status = RVF_GREEN;

#if(AS_RFS_API == 1)
	convert_u8_to_unicode(str1, mp_uc1);
	convert_u8_to_unicode(str2, mp_uc2);
	convert_u8_to_unicode(str3, mp_uc3);
	convert_u8_to_unicode(str4, mp_uc4);
#endif


    if (p_audio_gbl_var == NULL )
    {
      audio_mode_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* Create the audio mode path name */
    audio_path_name[0] = 0;

#if(AS_RFS_API == 1)
 	wstrcpy(audio_path_name,mp_uc1);
    wstrcpy(&(audio_path_name[9]), p_parameter->audio_mode_filename);
    wstrcpy(audio_mode_path_name, audio_path_name);
     wstrcat(audio_path_name,mp_uc2);

  AUDIO_SEND_TRACE("audio path Create the audio mode path name ",RV_TRACE_LEVEL_ERROR);

#else
	strcpy(audio_path_name,"/aud/");
    strcpy(&(audio_path_name[5]), p_parameter->audio_mode_filename);
    strcpy(audio_mode_path_name, audio_path_name);
    strcat(audio_path_name,".cfg");
#endif

    /* Open the FFS file */
    #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	audio_mode_ffs_fd = rfs_open(audio_path_name,
        RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	audio_mode_ffs_fd = ffs_open(audio_path_name,
        FFS_O_RDONLY);
#endif

      AUDIO_SEND_TRACE_PARAM("Ist FD is  ",  audio_mode_ffs_fd, RV_TRACE_LEVEL_ERROR);

      if (audio_mode_ffs_fd < 0)
      {
        audio_mode_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return(AUDIO_ERROR);
      }
    #else
      audio_mode_ffs_fd = 0x00001000;
    #endif

    /* Create the volume path name */
    audio_path_name[9] = 0;
#if(AS_RFS_API == 1)
    wstrcpy(&(audio_path_name[9]), p_parameter->audio_mode_filename);
     wstrcat(audio_path_name,mp_uc3);
    AUDIO_SEND_TRACE("audio path Create the volume path name ",RV_TRACE_LEVEL_ERROR);
#else
	strcpy(&(audio_path_name[5]), p_parameter->audio_mode_filename);
    strcat(audio_path_name,".vol");
#endif

    /* Open the FFS file */
    #ifndef _WINDOWS
#if(AS_RFS_API == 1)
	audio_volume_ffs_fd = rfs_open(audio_path_name,
        RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	audio_volume_ffs_fd = ffs_open(audio_path_name,
        FFS_O_RDONLY);
#endif
#if(AS_RFS_API == 1)
	  if (audio_volume_ffs_fd < RFS_EOK)
#else
	  if (audio_volume_ffs_fd < EFFS_OK)
#endif
      {
#if(AS_RFS_API == 1)
		rfs_close(audio_mode_ffs_fd);
#else
		ffs_close(audio_mode_ffs_fd);
#endif
        audio_mode_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return(AUDIO_ERROR);
      }
    #else
      audio_volume_ffs_fd = 0x00001001;
    #endif

  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
    /* Create the stereo volume path name */
    audio_path_name[9] = 0;
#if(AS_RFS_API == 1)
	wstrcpy(&(audio_path_name[9]), p_parameter->audio_mode_filename);
     wstrcat(audio_path_name,mp_uc4);
	 AUDIO_SEND_TRACE("audio path Create the stereo volume path name ",RV_TRACE_LEVEL_ERROR);
#else
	strcpy(&(audio_path_name[5]), p_parameter->audio_mode_filename);
    strcat(audio_path_name,".volst");
#endif

    /* Open the FFS file */
#if(AS_RFS_API == 1)
	audio_stereo_volume_ffs_fd = rfs_open(audio_path_name,
      RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	audio_stereo_volume_ffs_fd = ffs_open(audio_path_name,
      FFS_O_RDONLY);
#endif
#if(AS_RFS_API == 1)
	if (audio_stereo_volume_ffs_fd < RFS_EOK)
#else
	if (audio_stereo_volume_ffs_fd < EFFS_OK)
#endif
    {
#if(AS_RFS_API == 1)
	  rfs_close(audio_mode_ffs_fd);
      rfs_close(audio_volume_ffs_fd);
#else
	  ffs_close(audio_mode_ffs_fd);
      ffs_close(audio_volume_ffs_fd);
#endif
      audio_mode_error_trace(AUDIO_ENTITY_FILE_ERROR);
      return(AUDIO_ERROR);
    }
  #endif

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                             sizeof (T_AUDIO_MODE_LOAD_REQ),
                             (T_RVF_BUFFER **) (&p_msg_start));

    /* If insufficient resources, then report a memory error and abort. */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill the message parameters */
    p_msg_start->audio_ffs_fd = audio_mode_ffs_fd;
    p_msg_start->audio_volume_ffs_fd = audio_volume_ffs_fd;
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
    p_msg_start->audio_stereo_volume_ffs_fd = audio_stereo_volume_ffs_fd;
  #endif
#if(AS_RFS_API == 1)
	wstrcpy(p_msg_start->audio_mode_path_name , audio_mode_path_name);
#else
  strcpy(p_msg_start->audio_mode_path_name , audio_mode_path_name);
#endif

    /* fill the message id */
    p_msg_start->os_hdr.msg_id    = AUDIO_MODE_LOAD_REQ;

    /* fill the task source id */
    p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
    p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    if (return_path.callback_func == NULL)
    {
      p_msg_start->return_path.addr_id = return_path.addr_id;
      p_msg_start->return_path.callback_func = NULL;
    }
    else
    {
      p_msg_start->return_path.callback_func = return_path.callback_func;
    }

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId,
                  p_msg_start);

    return (AUDIO_OK);
    /************************ Enf of audio_full_access_write function ***********************/
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_speaker_volume                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to change the current speaker volume.   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        speaker volume command.                                               */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status                                                                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_speaker_volume(T_AUDIO_SPEAKER_VOLUME volume, T_RV_RETURN return_path)
  {
    /************************ audio_speaker_volume function begins ***********************/
    T_AUDIO_SPEAKER_VOLUME_REQ *p_msg_start;
    T_RVF_MB_STATUS       mb_status = RVF_GREEN;

    if (p_audio_gbl_var == NULL )
    {
      audio_mode_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* Check if an audio mode is already loaded */
    if (p_audio_gbl_var->audio_mode_var.audio_mode_path_name[0] == 0)
    {
      audio_mode_error_trace(AUDIO_ENTITY_AUDIO_MODE_NO_LOADED);
      return (AUDIO_ERROR);
    }

    /* Check parameters */
    if ( (volume.volume_action != AUDIO_SPEAKER_VOLUME_INCREASE) &&
         (volume.volume_action != AUDIO_SPEAKER_VOLUME_DECREASE) &&
         (volume.volume_action != AUDIO_SPEAKER_VOLUME_SET) )
    {
      audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    if ( (volume.value != AUDIO_SPEAKER_VOLUME_MUTE) &&
         (volume.value != AUDIO_SPEAKER_VOLUME_24dB) &&
         (volume.value != AUDIO_SPEAKER_VOLUME_18dB) &&
         (volume.value != AUDIO_SPEAKER_VOLUME_12dB) &&
         (volume.value != AUDIO_SPEAKER_VOLUME_6dB)  &&
         (volume.value != AUDIO_SPEAKER_VOLUME_0dB) )
    {
      audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                             sizeof (T_AUDIO_SPEAKER_VOLUME_REQ),
                             (T_RVF_BUFFER **) (&p_msg_start));

    /* If insufficient resources, then report a memory error and abort. */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill the message parameters */
    p_msg_start->volume.value         = volume.value;
    p_msg_start->volume.volume_action = volume.volume_action;

    /* fill the message id */
    p_msg_start->os_hdr.msg_id    = AUDIO_SPEAKER_VOLUME_REQ;

    /* fill the task source id */
    p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
    p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    if (return_path.callback_func == NULL)
    {
      p_msg_start->return_path.addr_id = return_path.addr_id;
      p_msg_start->return_path.callback_func = NULL;
    }
    else
    {
      p_msg_start->return_path.callback_func = return_path.callback_func;
    }

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId,
                  p_msg_start);

    return (AUDIO_OK);
    /************************ Enf of audio_full_access_write function ***********************/
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_stereo_speaker_volume                              */
  /*                                                                              */
  /*    Purpose:  This function is called to change the current speaker volume.   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        stereo speaker volume command.                                        */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status                                                                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_stereo_speaker_volume(T_AUDIO_STEREO_SPEAKER_VOLUME volume, T_RV_RETURN return_path)
  {
    /************************ audio_speaker_volume function begins ***********************/
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
    T_AUDIO_STEREO_SPEAKER_VOLUME_REQ *p_msg_start;
    T_RVF_MB_STATUS       mb_status = RVF_GREEN;

    if (p_audio_gbl_var == NULL )
    {
      audio_mode_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* Check if an audio mode is already loaded */
    if (p_audio_gbl_var->audio_mode_var.audio_mode_path_name[0] == 0)
    {
      audio_mode_error_trace(AUDIO_ENTITY_AUDIO_MODE_NO_LOADED);
      return (AUDIO_ERROR);
    }

    /* Check parameters */
    if ( (volume.volume_action_left != AUDIO_STEREO_SPEAKER_VOLUME_INCREASE) &&
         (volume.volume_action_left != AUDIO_STEREO_SPEAKER_VOLUME_DECREASE) &&
         (volume.volume_action_left != AUDIO_STEREO_SPEAKER_VOLUME_SET)
       )
    {
      audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    if ( (volume.value_left != AUDIO_STEREO_SPEAKER_VOLUME_MUTE) &&
         (volume.value_left > 30)
       )
    {
      audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    if ( (volume.volume_action_right != AUDIO_STEREO_SPEAKER_VOLUME_INCREASE) &&
         (volume.volume_action_right != AUDIO_STEREO_SPEAKER_VOLUME_DECREASE) &&
         (volume.volume_action_right != AUDIO_STEREO_SPEAKER_VOLUME_SET)
       )
    {
      audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    if ( (volume.value_right != AUDIO_STEREO_SPEAKER_VOLUME_MUTE) &&
         (volume.value_right > 30)
       )
    {
      audio_mode_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                             sizeof (T_AUDIO_STEREO_SPEAKER_VOLUME_REQ),
                             (T_RVF_BUFFER **) (&p_msg_start));

    /* If insufficient resources, then report a memory error and abort. */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill the message parameters */
    p_msg_start->volume.value_left          = volume.value_left;
    p_msg_start->volume.volume_action_left  = volume.volume_action_left;
    p_msg_start->volume.value_right         = volume.value_right;
    p_msg_start->volume.volume_action_right = volume.volume_action_right;

    /* fill the message id */
    p_msg_start->os_hdr.msg_id    = AUDIO_STEREO_SPEAKER_VOLUME_REQ;

    /* fill the task source id */
    p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
    p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    if (return_path.callback_func == NULL)
    {
      p_msg_start->return_path.addr_id = return_path.addr_id;
      p_msg_start->return_path.callback_func = NULL;
    }
    else
    {
      p_msg_start->return_path.callback_func = return_path.callback_func;
    }

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId,
                  p_msg_start);

    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("STEREO SPEAKER available only for analog base band SYREN", RV_TRACE_LEVEL_ERROR);
    return (AUDIO_ERROR);
  #endif
    /************************ Enf of audio_full_access_write function ***********************/
  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E2_start                                    */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate the melody E2 generation    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Melody E2 Parameters,                                           */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the melody E2 parameters.                              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_melody_E2_start (T_AUDIO_MELODY_E2_PARAMETER *p_parameter,
                                     T_RV_RETURN return_path)
  {
    #if (MELODY_E2)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS         mb_status    = RVF_GREEN;
      T_AUDIO_MELODY_E2_START *p_msg_start = NULL;

#if(AS_RFS_API == 1)
		T_RFS_FD               ffs_fd;
#else
	  T_FFS_FD               ffs_fd;
#endif

    /************************ audio_melody_E2_start function begins ****************/

      if (p_audio_gbl_var == NULL )
      {
        audio_melody_E2_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the melody E2 file exist */
      #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		ffs_fd = rfs_open(p_parameter->melody_E2_name, RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_parameter->melody_E2_name, FFS_O_RDONLY);
#endif
        if (ffs_fd <= 0)
        {
          audio_melody_E2_error_trace(AUDIO_ENTITY_FILE_ERROR);
          return (AUDIO_ERROR);
        }
      #else
        ffs_fd = 0x00001010;
      #endif

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_MELODY_E2_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.*/
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);

        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_MELODY_E2_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      /* the ffs_fd */
      p_msg_start->audio_E2_ffs_fd = ffs_fd;

      /* the name */
#if(AS_RFS_API == 1)
	  wstrcpy(p_msg_start->melody_E2_parameter.melody_E2_name,
             p_parameter->melody_E2_name);
#else
	  strcpy(p_msg_start->melody_E2_parameter.melody_E2_name,
             p_parameter->melody_E2_name);
#endif

      /* the loopback */
      if ( (p_parameter->E2_loopback == AUDIO_MELODY_NO_LOOPBACK) ||
           (p_parameter->E2_loopback == AUDIO_MELODY_LOOPBACK) )
      {
        p_msg_start->melody_E2_parameter.E2_loopback = p_parameter->E2_loopback;
      }
      else
      {
        /* Wrong parameter */
        audio_melody_E2_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      /* the melody mode */
      if ( (p_parameter->melody_E2_mode == AUDIO_MELODY_GAME_MODE) ||
           (p_parameter->melody_E2_mode == AUDIO_MELODY_NORMAL_MODE) )
      {
        p_msg_start->melody_E2_parameter.melody_E2_mode = p_parameter->melody_E2_mode;
      }
      else
      {
        /* Wrong parameter */
        audio_melody_E2_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        /* close the file previously open */
        #ifndef _WINDOWS
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        #endif
        return (AUDIO_ERROR);
      }

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }

      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg_start);

      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("Melody E2 not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_melody_E2_Start function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E2_stop                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to stop a melody_E2 generation          */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio Melody E2 Parameters,                                           */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the melody E2 parameters.                              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_melody_E2_stop (T_AUDIO_MELODY_E2_STOP_PARAMETER *p_parameter,
                                    T_RV_RETURN return_path)
  {
    #if (MELODY_E2)
      /* Declare local variables.                                                 */
      T_RVF_MB_STATUS        mb_status = RVF_GREEN;
      T_AUDIO_MELODY_E2_STOP *p_msg    = NULL;

      /********************** audio_melody_E2_stop function begins ****************/

      if (p_audio_gbl_var == NULL )
      {
        audio_melody_E2_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_MELODY_E2_STOP),
                               (T_RVF_BUFFER **) (&p_msg));

      /* If insufficient resources, then report a memory error and abort */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg);
        audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else
      if (mb_status == RVF_RED)
      {
        audio_melody_E2_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg->os_hdr.msg_id = AUDIO_MELODY_E2_STOP_REQ;

      /* fill the address source id */
      p_msg->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;


      /* fill the message parameters */
      /* the name */
#if(AS_RFS_API == 1)
	  wstrcpy(p_msg->melody_E2_name,
             p_parameter->melody_E2_name);
#else
	  strcpy(p_msg->melody_E2_name,
             p_parameter->melody_E2_name);
#endif

      /* the return path */
      if (return_path.callback_func == NULL)
      {
        p_msg->return_path.addr_id = return_path.addr_id;
        p_msg->return_path.callback_func = NULL;
      }
      else
      {
        p_msg->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId,
                    p_msg);

      return (AUDIO_OK);

    #else
      AUDIO_SEND_TRACE("Melody E2 not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*********************** End of audio_melody_E2_Stop function *****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_melody_E2_load_file_instruments                    */
  /*                                                                              */
  /*    Purpose:  This function is called in order to load the instruments file   */
  /*              used in the melody E2 format (.lsi file)                        */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        File name of the melody.                                              */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the melody E2 load file instruments parameters.        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_melody_E2_load_file_instruments (
                 T_AUDIO_MELODY_E2_LOAD_FILE_INSTR_PARAMETER *p_parameter)
  {
    #if (MELODY_E2)
      /**** Declare local variables  ****/
      #ifndef _WINDOWS
         /* FFS File descriptor type */
#if(AS_RFS_API == 1)
		T_RFS_FD     ffs_fd_1, ffs_fd_2;
		T_RFS_STAT   stat;
		T_RFS_FILE_DIR_STAT size = 0;
#else
	  T_FFS_FD     ffs_fd_1, ffs_fd_2;
	  T_FFS_STAT   stat;
	  T_FFS_SIZE   size = 0;
#endif



         INT8 i;

         /* basic structure of the .lsi file */
         T_AUDIO_MELODY_E2_ID_NAME file_E2;
      #endif
       /* Nb of instruments in the .lsi file */
       INT8 nb_of_instruments = 0;


      /*************** audio_melody_E2_load_file_instruments function begins ********/

      if (p_audio_gbl_var == NULL )
      {
        audio_melody_E2_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* Before to start the file download, check if the melody is OFF */
      if ((p_audio_gbl_var->melody_E2_0.state != AUDIO_MELODY_E2_IDLE ) &&
          (p_audio_gbl_var->melody_E2_1.state != AUDIO_MELODY_E2_IDLE ))
      {
         /* The instrument file can't be downloaded */
         audio_melody_E2_error_trace (AUDIO_ERROR_DOWNLOAD);
         return (AUDIO_ERROR);
      }

      /**** Find the size of the .lsi file ****/
      /* the .lsi file is stores into the flash */
      /* check if the .lsi file exists */
      #ifndef _WINDOWS

#if(AS_RFS_API == 1)
	  ffs_fd_1 = rfs_open(p_parameter->melody_E2_file_name, RFS_O_RDONLY, RFS_RDWREX_ALL );
#else
	  ffs_fd_1 = ffs_open(p_parameter->melody_E2_file_name, FFS_O_RDONLY );
#endif

         if (ffs_fd_1 < EFFS_OK)
         {
            audio_melody_E2_error_trace(AUDIO_ENTITY_LOAD_FILE_INSTR_ERROR);
            return (AUDIO_ERROR);
         }

#if(AS_RFS_API == 1)
		 rfs_stat(p_parameter->melody_E2_file_name,&stat);
#else
		 ffs_stat(p_parameter->melody_E2_file_name,&stat);
#endif


         /* check if the file contains some data */
#if(AS_RFS_API == 1)
		 if (stat.file_dir.size ==0)
#else
		 if (stat.size ==0)
#endif
         {
           /* the file doesn't contains data */
           /* an error is generated */
           audio_melody_E2_error_trace(AUDIO_ENTITY_FILE_ERROR);
           return(AUDIO_ERROR);
         }

         /* the file exists and contains data */
         /* Nb of instruments in the file */
#if(AS_RFS_API == 1)
		nb_of_instruments = stat.file_dir.size / (sizeof(T_AUDIO_MELODY_E2_ID_NAME));
#else
		 nb_of_instruments = stat.size / (sizeof(T_AUDIO_MELODY_E2_ID_NAME));
#endif


         /**** check if the melody E2 load instruments files (.mwa file) exist ****/
         /* open the .mwa file */
          for (i=0;i< nb_of_instruments;i++)
          {
             /* Load the instruments file from the FFS */
#if(AS_RFS_API == 1)

			if (rfs_read ( ffs_fd_1,
                              (&file_E2),
                              (sizeof(INT8) + AUDIO_PATH_NAME_MAX_SIZE)) < RFS_EOK )

#else
			 if (ffs_read ( ffs_fd_1,
                              (&file_E2),
                              (sizeof(INT8) + AUDIO_PATH_NAME_MAX_SIZE)) < EFFS_OK )
#endif

             {
                AUDIO_SEND_TRACE("AUDIO MELODY E2: impossible to load the .lsi file", RV_TRACE_LEVEL_ERROR);

                /* Close the file */
#if(AS_RFS_API == 1)
				rfs_close(ffs_fd_1);
#else
				ffs_close(ffs_fd_1);
#endif

                return (AUDIO_ERROR);
             }

#if(AS_RFS_API == 1)
			ffs_fd_2 = rfs_open( file_E2.melody_name,
                                  RFS_O_RDONLY, RFS_RDWREX_ALL );
#else
			 ffs_fd_2 = ffs_open( file_E2.melody_name,
                                  FFS_O_RDONLY );
#endif

#if(AS_RFS_API == 1)
			if (ffs_fd_2 < RFS_EOK)
#else
			 if (ffs_fd_2 < EFFS_OK)
#endif
             {
                AUDIO_SEND_TRACE("AUDIO MELODY E2: impossible to open the .mwa file instruments", RV_TRACE_LEVEL_ERROR);

                /* Close the .mwa file */
#if(AS_RFS_API == 1)
				rfs_close(ffs_fd_1);
                rfs_close(ffs_fd_2);
#else
				ffs_close(ffs_fd_1);
                ffs_close(ffs_fd_2);
#endif

                return (AUDIO_ERROR);
             }

#if(AS_RFS_API == 1)
			rfs_close(ffs_fd_2);
#else
			 /* Close the file */
             ffs_close(ffs_fd_2);
#endif
         }
#if(AS_RFS_API == 1)
		 rfs_close(ffs_fd_1);
#else
		 ffs_close(ffs_fd_1);
#endif
      #endif

      /* Copy the new .lsi name */
#if(AS_RFS_API == 1)
	  wstrcpy (p_audio_gbl_var->melody_E2_load_file_instruments.instrument_file_name,
              p_parameter->melody_E2_file_name);
#else
	  strcpy (p_audio_gbl_var->melody_E2_load_file_instruments.instrument_file_name,
              p_parameter->melody_E2_file_name);
#endif

      /* Copy the number of instrument listed by this .lsi file */
      p_audio_gbl_var->melody_E2_load_file_instruments.nb_of_instruments
        = nb_of_instruments;

      p_audio_gbl_var->melody_E2_load_file_instruments.file_downloaded = TRUE;

      return (AUDIO_OK);

    #else
      AUDIO_SEND_TRACE("Melody E2 not compiled", RV_TRACE_LEVEL_ERROR);
      return (AUDIO_ERROR);
    #endif
  } /*************** End of audio_melody_E2_load_file_instrument function **************/


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_amr_record_to_ffs_start/stop                       */
  /*                                                                              */
  /*    Purpose:  This function is called in order to start/stop the recording of */
  /*              a AMR-MMS in Flash                                                  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Record parameters                                                     */
  /*        Return_path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters.                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_amr_record_to_ffs_start (T_AUDIO_AMR_RECORD_TO_FFS_PARAMETER *p_record_parameter,
                                             T_RV_RETURN return_path)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_RECORD_TO_FFS_START *p_msg_start = NULL;
      T_RVF_MB_STATUS                     mb_status = RVF_GREEN;
#if(AS_RFS_API == 1)
		T_RFS_FD                            ffs_fd;
#else
	  T_FFS_FD                            ffs_fd;
#endif

      /************************ audio_amr_record_to_ffs_start function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the voice memo record file already exists */
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_record_parameter->memo_name,
                 RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_record_parameter->memo_name,
                 FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
      if ( ffs_fd <= 0)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return (AUDIO_ERROR);
      }

      /* check parameters */
      if ( ((p_record_parameter->compression_mode != AUDIO_AMR_NO_COMPRESSION_MODE ) &&
            (p_record_parameter->compression_mode != AUDIO_AMR_COMPRESSION_MODE ))||
            ((p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_4_75) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_5_15) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_5_90) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_6_70) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_7_40) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_7_95) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_10_2) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_12_2))
         )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_RECORD_TO_FFS_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_AMR_RECORD_TO_FFS_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd       = ffs_fd;
      p_msg_start->compression_mode   = p_record_parameter->compression_mode;
      p_msg_start->memo_duration      = p_record_parameter->memo_duration;
      p_msg_start->microphone_gain    = p_record_parameter->microphone_gain;
      p_msg_start->amr_vocoder        = p_record_parameter->amr_vocoder;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_start);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_record_to_ffs_start function ****************/

  T_AUDIO_RET audio_amr_record_to_ffs_stop (void)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_RECORD_STOP *p_msg_stop = NULL;
      T_RVF_MB_STATUS            mb_status   = RVF_GREEN;

      /************************ audio_amr_record_to_ffs_stop function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_RECORD_STOP),
                               (T_RVF_BUFFER **) (&p_msg_stop));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_stop);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_stop->os_hdr.msg_id    = AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ;

      /* fill the address source id */
      p_msg_stop->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_stop->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_stop);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_record_to_ffs_stop function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mms_record_to_ffs_start/stop                       */
  /*                                                                              */
  /*    Purpose:  This function is called in order to start/stop the recording of */
  /*              a AMR-MMS in Flash (MMS header added)                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Record parameters                                                     */
  /*        Return_path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters.                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mms_record_to_ffs_start (T_AUDIO_MMS_RECORD_TO_FFS_PARAMETER *p_record_parameter,
                                             T_RV_RETURN return_path)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_RECORD_TO_FFS_START *p_msg_start = NULL;
      T_RVF_MB_STATUS                     mb_status = RVF_GREEN;
#if(AS_RFS_API == 1)
	  T_RFS_FD                            ffs_fd;
#else
	  T_FFS_FD                            ffs_fd;
#endif
      UINT8                               temp[6] = "#!AMR\n";

      /************************ audio_amr_record_to_ffs_start function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the voice memo record file already exists */
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_record_parameter->memo_name,
                 RFS_O_CREAT | RFS_O_WRONLY | RFS_O_TRUNC | RFS_O_APPEND, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_record_parameter->memo_name,
                 FFS_O_CREATE | FFS_O_WRONLY | FFS_O_TRUNC | FFS_O_APPEND);
#endif
      if ( ffs_fd <= 0)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return (AUDIO_ERROR);
      }

      /* check parameters */
      if ( ((p_record_parameter->compression_mode != AUDIO_AMR_NO_COMPRESSION_MODE ) &&
            (p_record_parameter->compression_mode != AUDIO_AMR_COMPRESSION_MODE ))||
            ((p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_4_75) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_5_15) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_5_90) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_6_70) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_7_40) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_7_95) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_10_2) &&
             (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_12_2))
         )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_RECORD_TO_FFS_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_AMR_RECORD_TO_FFS_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd       = ffs_fd;
      p_msg_start->compression_mode   = p_record_parameter->compression_mode;
      p_msg_start->memo_duration      = p_record_parameter->memo_duration;
      p_msg_start->microphone_gain    = p_record_parameter->microphone_gain;
      p_msg_start->amr_vocoder        = p_record_parameter->amr_vocoder;

      // write magix string #!AMR\n
#if(AS_RFS_API == 1)
	  rfs_write(ffs_fd, temp, 6);
#else
	  ffs_write(ffs_fd, temp, 6);
#endif

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_start);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_mms_record_to_ffs_start function ****************/

  T_AUDIO_RET audio_mms_record_to_ffs_stop (void)
  {
    return audio_amr_record_to_ffs_stop();
  } /*********************** End of audio_mms_record_to_ffs_stop function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_amr_play_from_ffs_start/stop                       */
  /*                                                                              */
  /*    Purpose:  This function is called in order to start/stop the playing of   */
  /*              a AMR-MMS from Flash                                            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Play parameters                                                       */
  /*        Return_path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters.                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_amr_play_from_ffs_start (T_AUDIO_AMR_PLAY_FROM_FFS_PARAMETER *p_play_parameter,
                                             T_RV_RETURN return_path)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_FROM_FFS_START *p_msg_start = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;
#if(AS_RFS_API == 1)
	  T_RFS_FD                           ffs_fd;
#else
	  T_FFS_FD                           ffs_fd;
#endif

      /************************ audio_amr_play_from_ffs_start function begins **************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the voice memo play file already exists        */
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_play_parameter->memo_name, RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_play_parameter->memo_name, FFS_O_RDONLY);
#endif
      if ( ffs_fd <= 0)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_FROM_FFS_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_FFS_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd        = ffs_fd;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_start);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_play_from_ffs_start function ****************/

  T_AUDIO_RET audio_amr_play_from_ffs_stop (void)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_STOP *p_msg_stop = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;

      /************************ audio_amr_play_from_ffs_stop function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_STOP),
                               (T_RVF_BUFFER **) (&p_msg_stop));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_stop);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_stop->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ;

      /* fill the address source id */
      p_msg_stop->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_stop->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_stop);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_play_from_ffs_stop function ****************/



 T_AUDIO_RET audio_amr_play_from_ffs_pause (void)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_PAUSE *p_msg_pause = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;

      /************************ audio_amr_play_from_ffs_pause function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_PAUSE),
                               (T_RVF_BUFFER **) (&p_msg_pause));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_pause);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_pause->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ;

      /* fill the address source id */
      p_msg_pause->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_pause->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_pause);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_play_from_ffs_pause function ****************/


T_AUDIO_RET audio_mms_play_from_ffs_pause (void)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_PAUSE *p_msg_pause = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;

      /************************ audio_mms_play_from_ffs_pause function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_PAUSE),
                               (T_RVF_BUFFER **) (&p_msg_pause));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_pause);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_pause->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ;

      /* fill the address source id */
      p_msg_pause->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_pause->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_pause);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_mms_play_from_ffs_pause function ****************/

 
T_AUDIO_RET audio_amr_play_from_ram_pause (void)
  {
    #if (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_PAUSE *p_msg_pause = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;

      /************************ audio_amr_play_from_ram_pause function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_PAUSE),
                               (T_RVF_BUFFER **) (&p_msg_pause));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_pause);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_pause->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_RAM_PAUSE_REQ;

      /* fill the address source id */
      p_msg_pause->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_pause->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_pause);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_play_from_ram_pause function ****************/




 T_AUDIO_RET audio_amr_play_from_ffs_resume (void)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_RESUME *p_msg_resume = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;

      /************************ audio_amr_play_from_ffs_stop function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_RESUME),
                               (T_RVF_BUFFER **) (&p_msg_resume));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_resume);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_resume->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ;

      /* fill the address source id */
      p_msg_resume->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_resume->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_resume);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_play_from_ffs_resume function ****************/

 
T_AUDIO_RET audio_mms_play_from_ffs_resume (void)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_RESUME *p_msg_resume = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;

      /************************ audio_mms_play_from_ffs_resume function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_RESUME),
                               (T_RVF_BUFFER **) (&p_msg_resume));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_resume);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_resume->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ;

      /* fill the address source id */
      p_msg_resume->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_resume->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_resume);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_mms_play_from_ffs_resume function ****************/


 T_AUDIO_RET audio_amr_play_from_ram_resume (void)
  {
    #if (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_RESUME *p_msg_resume = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;

      /************************ audio_amr_play_from_ram_resume function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_RESUME),
                               (T_RVF_BUFFER **) (&p_msg_resume));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_resume);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_resume->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_RAM_RESUME_REQ;

      /* fill the address source id */
      p_msg_resume->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_resume->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_resume);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_play_from_ram_resume function ****************/




  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mms_play_from_ffs_start/stop                       */
  /*                                                                              */
  /*    Purpose:  This function is called in order to start/stop the playing of   */
  /*              a AMR-MMS from Flash (MMS header handled)                       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Play parameters                                                       */
  /*        Return_path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters.                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mms_play_from_ffs_start (T_AUDIO_MMS_PLAY_FROM_FFS_PARAMETER *p_play_parameter,
                                             T_RV_RETURN return_path)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_FROM_FFS_START *p_msg_start = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;
#if(AS_RFS_API == 1)
		T_RFS_FD                           ffs_fd;
#else
      T_FFS_FD                           ffs_fd;
#endif
      UINT8                              temp[6];

      /************************ audio_amr_play_from_ffs_start function begins **************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check if the voice memo play file already exists        */
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_play_parameter->memo_name, RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_play_parameter->memo_name, FFS_O_RDONLY);
#endif
      if ( ffs_fd <= 0)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return (AUDIO_ERROR);
      }
      // read magic string #!AMR\n
#if(AS_RFS_API == 1)
		rfs_read(ffs_fd, temp, 6);
#else
	  ffs_read(ffs_fd, temp, 6);
#endif

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_FROM_FFS_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_FFS_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd        = ffs_fd;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_start);

      return (AUDIO_OK);
    #else // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_NEW_FFS_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_mms_play_from_ffs_start function ****************/

  T_AUDIO_RET audio_mms_play_from_ffs_stop (void)
  {
    return audio_amr_play_from_ffs_stop();
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_amr_record_to_ram_start/stop                       */
  /*                                                                              */
  /*    Purpose:  This function is called in order to record a MMS                */
  /*              in RAM                                                          */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Record Parameters                                                     */
  /*        Return_path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters.                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_amr_record_to_ram_start (T_AUDIO_AMR_RECORD_TO_RAM_PARAMETER *p_record_parameter,
                                             T_RV_RETURN return_path)
  {
    #if (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_RECORD_TO_RAM_START *p_msg_start = NULL;
      T_RVF_MB_STATUS                     mb_status = RVF_GREEN;

      /************************ audio_amr_record_to_ram_start function begins **************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      if ( (p_record_parameter->p_buffer == NULL) ||
           ((p_record_parameter->compression_mode != AUDIO_AMR_NO_COMPRESSION_MODE ) &&
            (p_record_parameter->compression_mode != AUDIO_AMR_COMPRESSION_MODE ))||
           ((p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_4_75) &&
            (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_5_15) &&
            (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_5_90) &&
            (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_6_70) &&
            (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_7_40) &&
            (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_7_95) &&
            (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_10_2) &&
            (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_12_2))
         )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_RECORD_TO_RAM_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_AMR_RECORD_TO_RAM_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->p_buffer           = p_record_parameter->p_buffer;
      p_msg_start->compression_mode   = p_record_parameter->compression_mode;
      p_msg_start->memo_duration      = p_record_parameter->memo_duration;
      p_msg_start->microphone_gain    = p_record_parameter->microphone_gain;
      p_msg_start->amr_vocoder        = p_record_parameter->amr_vocoder;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_start);

      return (AUDIO_OK);
    #else // (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_record_to_ram_start function ****************/

  T_AUDIO_RET audio_amr_record_to_ram_stop (void)
  {
    #if (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_RECORD_STOP *p_msg_stop = NULL;
      T_RVF_MB_STATUS                     mb_status = RVF_GREEN;

      /************************ audio_amr_record_to_ram_stop function begins **************/

      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_RECORD_STOP),
                               (T_RVF_BUFFER **) (&p_msg_stop));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_stop);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_stop->os_hdr.msg_id    = AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ;

      /* fill the address source id */
      p_msg_stop->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_stop->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_stop);

      return (AUDIO_OK);
    #else // (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_record_to_ram_stop function ****************/

  T_AUDIO_RET audio_amr_play_from_ram_start (T_AUDIO_AMR_PLAY_FROM_RAM_PARAMETER *p_play_parameter,
                                             T_RV_RETURN return_path)
  {
    #if (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_FROM_RAM_START *p_msg_start = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;

      /************************ audio_amr_play_from_ram_start function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* check parameters */
      if (p_play_parameter->p_buffer == NULL)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_FROM_RAM_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_RAM_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->p_buffer        = p_play_parameter->p_buffer;
      p_msg_start->buffer_size     = p_play_parameter->buffer_size;

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_start);

      return (AUDIO_OK);
    #else // (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_play_from_ram_start function ****************/

  T_AUDIO_RET audio_amr_play_from_ram_stop (void)
  {
    #if (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      /* Declare local variables. */
      T_AUDIO_VM_AMR_PLAY_STOP *p_msg_stop = NULL;
      T_RVF_MB_STATUS                    mb_status = RVF_GREEN;

      /************************ audio_amr_play_from_ram_stop function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_VM_AMR_PLAY_STOP),
                               (T_RVF_BUFFER **) (&p_msg_stop));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_stop);
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_stop->os_hdr.msg_id    = AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ;

      /* fill the address source id */
      p_msg_stop->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_stop->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_stop);

      return (AUDIO_OK);
    #else // (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
      AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif // (AUDIO_RAM_MANAGER)&&(L1_VOICE_MEMO_AMR)
  } /*********************** End of audio_amr_play_from_ram_stop function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_init_vm_amr_record_session                  */
  /*                                                                              */
  /*    Purpose:  This function is called in order to initialize VM AMR record    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Specific VM AMR record parameters                                     */
  /*        Driver parameters                                                     */
  /*        Return path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters                                         */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_driver_init_vm_amr_record_session(T_AUDIO_DRIVER_VM_AMR_RECORD_PARAMETER *p_record_parameter,
                                                      T_AUDIO_DRIVER_PARAMETER *p_driver_parameter,
                                                      T_RV_RETURN return_path)
  {
  #if (L1_VOICE_MEMO_AMR)
    /* Declare local variables.                                                 */
    T_RVF_MB_STATUS   mb_status = RVF_GREEN;
    T_AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION *p_msg  = NULL;

    /************************ function begins ****************/

    /* check entity started */
    if (p_audio_gbl_var == NULL )
    {
      audio_driver_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* If bad voice memo record parameters, then report an error and abort.*/
    if ( ((p_record_parameter->compression_mode != AUDIO_AMR_NO_COMPRESSION_MODE ) &&
          (p_record_parameter->compression_mode != AUDIO_AMR_COMPRESSION_MODE ))||
         ((p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_4_75) &&
          (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_5_15) &&
          (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_5_90) &&
          (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_6_70) &&
          (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_7_40) &&
          (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_7_95) &&
          (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_10_2) &&
          (p_record_parameter->amr_vocoder != AUDIO_AMR_VOCODER_12_2))||
         (p_driver_parameter->buffer_size < AUDIO_VM_AMR_MAX_SAMPLE_SIZE_16BIT)||
         ((p_driver_parameter->nb_buffer < AUDIO_DRIVER_MIN_BUFFER_PER_SESSION)||
          (p_driver_parameter->nb_buffer > AUDIO_DRIVER_MAX_BUFFER_PER_SESSION))
         )
    {
      audio_driver_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                             sizeof (T_AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION),
                             (T_RVF_BUFFER **) (&p_msg));

    /* If insufficient resources, then report a memory error and abort.         */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg);
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill message id */
    p_msg->os_hdr.msg_id = AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION;
    p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    /* fill the message parameters */
    p_msg->compression_mode   = p_record_parameter->compression_mode;
    p_msg->memo_duration      = p_record_parameter->memo_duration;
    p_msg->microphone_gain    = p_record_parameter->microphone_gain;
    p_msg->amr_vocoder        = p_record_parameter->amr_vocoder;

    /* fill parameters */
    p_msg->driver_parameter.buffer_size = p_driver_parameter->buffer_size;
    p_msg->driver_parameter.nb_buffer   = p_driver_parameter->nb_buffer;

    if (return_path.callback_func == NULL)
    {
      p_msg->return_path.addr_id = return_path.addr_id;
      p_msg->return_path.callback_func = NULL;
    }
    else
      p_msg->return_path.callback_func = return_path.callback_func;

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId, p_msg);

    return (AUDIO_OK);
  #else  // L1_VOICE_MEMO_AMR
    AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
    return (AUDIO_ERROR);
  #endif // L1_VOICE_MEMO_AMR
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_init_vm_amr_play_session                    */
  /*                                                                              */
  /*    Purpose:  This function is called in order to initialize VM AMR play      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Specific VM AMR play parameters                                       */
  /*        Driver parameters                                                     */
  /*        Return path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters                                         */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_driver_init_vm_amr_play_session(T_AUDIO_DRIVER_PARAMETER *p_driver_parameter, T_RV_RETURN return_path)
  {
  #if (L1_VOICE_MEMO_AMR)
    /* Declare local variables.                                                 */
    T_RVF_MB_STATUS   mb_status = RVF_GREEN;
    T_AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION *p_msg  = NULL;

    /************************ audio_keybeep_stop function begins ****************/

    if (p_audio_gbl_var == NULL )
    {
      audio_driver_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* If bad voice memo record parameters, then report an error and abort.*/
    if ( (p_driver_parameter->buffer_size < AUDIO_VM_AMR_MAX_SAMPLE_SIZE_16BIT)||
         ((p_driver_parameter->nb_buffer < AUDIO_DRIVER_MIN_BUFFER_PER_SESSION)||
          (p_driver_parameter->nb_buffer > AUDIO_DRIVER_MAX_BUFFER_PER_SESSION))
       )
    {
      audio_driver_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                             sizeof (T_AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION),
                             (T_RVF_BUFFER **) (&p_msg));

    /* If insufficient resources, then report a memory error and abort.         */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg);
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill the message id */
    p_msg->os_hdr.msg_id = AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION;
    p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    /* fill parameters */
    p_msg->driver_parameter.buffer_size = p_driver_parameter->buffer_size;
    p_msg->driver_parameter.nb_buffer   = p_driver_parameter->nb_buffer;

    if (return_path.callback_func == NULL)
    {
      p_msg->return_path.addr_id = return_path.addr_id;
      p_msg->return_path.callback_func = NULL;
    }
    else
      p_msg->return_path.callback_func = return_path.callback_func;

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId, p_msg);

    return (AUDIO_OK);
  #else  // L1_VOICE_MEMO_AMR
    AUDIO_SEND_TRACE("Voice Memo AMR not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
    return (AUDIO_ERROR);
  #endif // L1_VOICE_MEMO_AMR
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_ext_midi_start                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to initiate a MIDI file playback        */
  /*              (used when an external module generates midi samples to play)   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Midi file parameters,                                                 */
  /*        Audio Driver parameters,                                              */
  /*        Return path.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the MIDI parameters.                                   */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_ext_midi_start( T_AUDIO_EXT_MIDI_PARAMETER *p_midi_parameter,
							                  		T_RV_RETURN                *p_return_path)
  {
    #if (L1_EXT_AUDIO_MGT==1)
      /* Declare local variables */
      T_RVF_MB_STATUS    mb_status=RVF_GREEN;
      T_AUDIO_MIDI_START *p_msg_start=NULL;

      /******************** audio_ext_midi_start function begins ******************/

      if(p_audio_gbl_var==NULL)
      {
         audio_midi_error_trace(AUDIO_ENTITY_NOT_START);
         return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status=rvf_get_buf(p_audio_gbl_var->mb_external,
                            sizeof(T_AUDIO_MIDI_START),
                            (T_RVF_BUFFER **)(&p_msg_start));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status==RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_midi_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status==RVF_RED)
        {
          audio_midi_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return (AUDIO_ERROR);
        }
      }

      /* initialization of the 2 counters */
      p_audio_gbl_var->midi.get_play_counter = 0;
      p_audio_gbl_var->midi.layer1_counter = 0;

      /* fill the message id */
      p_msg_start->os_hdr.msg_id=AUDIO_MIDI_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id =rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id=p_audio_gbl_var->addrId;


      /* Save the values in the global variable*/
      p_audio_gbl_var->midi.sampling_rate = p_midi_parameter->sampling_rate;
      p_audio_gbl_var->midi.data_type     = p_midi_parameter->data_type;
      p_audio_gbl_var->midi.bit_type      = p_midi_parameter->bit_type;
      p_audio_gbl_var->midi.channel       = p_midi_parameter->channel;
      p_audio_gbl_var->midi.frame_number  = p_midi_parameter->frame_number;


      if(p_return_path->callback_func==NULL)
      {
        p_msg_start->return_path.addr_id=p_return_path->addr_id;
        p_msg_start->return_path.callback_func=NULL;
      }
      else
        p_msg_start->return_path.callback_func=p_return_path->callback_func;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_start);

      return(AUDIO_OK);
    #else /* L1_EXT_AUDIO_MGT!=1 */
      AUDIO_SEND_TRACE("MIDI not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_EXT_AUDIO_MGT==1 */
  } /*********************** End of audio_ext_midi_start function ****************/


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_ext_midi_stop                                      */
  /*                                                                              */
  /*    Purpose:  This function is called to stop MIDI file playback              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_ext_midi_stop(void)
  {
    #if (L1_EXT_AUDIO_MGT==1)
      /* Declare local variables */
      T_RVF_MB_STATUS   mb_status  =RVF_GREEN;
      T_AUDIO_MIDI_STOP *p_msg_stop=NULL;

      /************************ audio_midi_stop function begins ****************/

      if(p_audio_gbl_var==NULL)
      {
        audio_midi_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status=rvf_get_buf(p_audio_gbl_var->mb_external,
                            sizeof(T_AUDIO_MIDI_STOP),
                            (T_RVF_BUFFER **)(&p_msg_stop));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status==RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_stop);
        audio_midi_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status==RVF_RED)
        {
          audio_midi_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return(AUDIO_ERROR);
        }
      }

      /* fill the message id */
      p_msg_stop->os_hdr.msg_id=AUDIO_MIDI_STOP_REQ;

      /* fill the address source id */
      p_msg_stop->os_hdr.src_addr_id =rvf_get_taskid();
      p_msg_stop->os_hdr.dest_addr_id=p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_stop);

      return(AUDIO_OK);
    #else /* L1_EXT_AUDIO_MGT!=1 */
      AUDIO_SEND_TRACE("MIDI not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_EXT_AUDIO_MGT==1 */
  } /*********************** End of audio_midi_stop function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_init_midi_session                           */
  /*                                                                              */
  /*    Purpose:  This function is called in order to initialize MIDI             */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Specific MIDI parameters                                              */
  /*        Driver parameters                                                     */
  /*        Return path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters                                         */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_driver_init_midi_session(T_AUDIO_DRIVER_PARAMETER *p_driver_parameter, T_RV_RETURN *p_return_path)
  {
  #if (L1_EXT_AUDIO_MGT==1)
    /* Declare local variables.                                                 */
    T_RVF_MB_STATUS   mb_status = RVF_GREEN;
    T_AUDIO_DRIVER_INIT_MIDI_SESSION *p_msg  = NULL;

    /******************** audio_driver_init_midi_session function begins **********/

    if (p_audio_gbl_var == NULL )
    {
      audio_driver_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* If bad parameters report an error and abort.*/
    if(p_driver_parameter->nb_buffer<AUDIO_DRIVER_MIN_BUFFER_PER_SESSION)
    {
      audio_driver_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                             sizeof (T_AUDIO_DRIVER_INIT_MIDI_SESSION),
                             (T_RVF_BUFFER **) (&p_msg));

    /* If insufficient resources, then report a memory error and abort.         */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg);
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill the message id */
    p_msg->os_hdr.msg_id = AUDIO_DRIVER_INIT_MIDI_SESSION;
    p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    /* fill parameters */
    p_msg->driver_parameter.buffer_size = p_driver_parameter->buffer_size;
    p_msg->driver_parameter.nb_buffer   = p_driver_parameter->nb_buffer;

    if (p_return_path->callback_func == NULL)
    {
      p_msg->return_path.addr_id = p_return_path->addr_id;
      p_msg->return_path.callback_func = NULL;
    }
    else
      p_msg->return_path.callback_func = p_return_path->callback_func;

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId, p_msg);

    return (AUDIO_OK);
    #else  /* L1_EXT_AUDIO_MGT!=1 */
      AUDIO_SEND_TRACE("MIDI not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif /* L1_EXT_AUDIO_MGT==1 */
  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_start                                          */
  /*                                                                              */
  /*    Purpose:  This function is called in order to start the playing of        */
  /*              MP3 melody from Flash                                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Play parameters                                                       */
  /*        Return_path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters.                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mp3_start (T_AUDIO_MP3_PARAMETER *p_parameter,
                               T_RV_RETURN return_path)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_MP3)
      /* Declare local variables. */
      T_AUDIO_MP3_START   *p_msg_start = NULL;
      T_RVF_MB_STATUS     mb_status = RVF_GREEN;
#if(AS_RFS_API == 1)
		T_RFS_FD            ffs_fd;
		T_RFS_STAT          stat;
#else
	  T_FFS_FD            ffs_fd;
	  T_FFS_STAT          stat;
#endif

      //UINT8 name[80];

      /************************ audio_mp3_start function begins **************/

      if (p_audio_gbl_var == NULL )
      {
        audio_mp3_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      //First check whether the mp3 file size is at least 1600bytes. If its not do not
      //give it to L1 for playback.
#if(AS_RFS_API == 1)
	  rfs_stat (p_parameter->mp3_name, &stat);
#else
	  ffs_stat (p_parameter->mp3_name, &stat);
#endif

#if(AS_RFS_API == 1)
	  if(stat.file_dir.size < 1600)
#else
	  if(stat.size < 1600)
#endif
      {
        AUDIO_SEND_TRACE("MP3 File size < 1600bytes!", RV_TRACE_LEVEL_DEBUG_LOW);
        return (AUDIO_ERROR);
      }

      /* check if the MP3 file already exists */
#if(AS_RFS_API == 1)
	  ffs_fd = rfs_open(p_parameter->mp3_name, RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_parameter->mp3_name, FFS_O_RDONLY);
#endif


     AUDIO_SEND_TRACE_PARAM("FD value", ~ffs_fd+1, RV_TRACE_LEVEL_ERROR);

	  if ( ffs_fd <= 0)
      {
        audio_mp3_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_MP3_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_MP3_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd        = ffs_fd;
#if(AS_RFS_API == 1)
	p_msg_start->audio_filesize       = stat.file_dir.size;
#else
      p_msg_start->audio_filesize       = stat.size;
#endif
      p_msg_start->play_bar_on   = p_parameter->play_bar_on;


      /* the name */
	  // wstrcpy(  T_WCHAR *dest, const T_WCHAR *src)
#if(AS_RFS_API == 1)
	  wstrcpy(p_msg_start->mp3_parameter.mp3_name, p_parameter->mp3_name);
#else
	  strcpy(p_msg_start->mp3_parameter.mp3_name,p_parameter->mp3_name);
#endif

      /* the melody mode */
      if ( (p_parameter->mono_stereo == AUDIO_MP3_MONO) ||
           (p_parameter->mono_stereo == AUDIO_MP3_STEREO) )
      {
        p_msg_start->mp3_parameter.mono_stereo = p_parameter->mono_stereo;
      }
      else
      {
        /* Wrong parameter */
        audio_mp3_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      p_msg_start->mp3_parameter.size_file_start = p_parameter->size_file_start;

      /* Removed as part of the effort to reduce the number of warnings*/
#if 0
      if (p_msg_start->mp3_parameter.size_file_start <0)
      {
        /* Wrong parameter */
        audio_mp3_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        /* close the file previously open */
        ffs_close(ffs_fd);
        return (AUDIO_ERROR);
      }
#endif

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_start);

      return (AUDIO_OK);
    #else /* (AUDIO_NEW_FFS_MANAGER)&&(L1_MP3) */
      AUDIO_SEND_TRACE("Audio MP3 not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif /* (AUDIO_NEW_FFS_MANAGER)&&(L1_MP3) */
  } /*********************** End of audio_mp3_start function ****************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_stop                                           */
  /*                                                                              */
  /*    Purpose:  This function is called to stop MP3 file playback               */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_mp3_stop(UINT32 *size_played)
  {
    #if (L1_MP3 == 1)
      /* Declare local variables */
      T_RVF_MB_STATUS   mb_status   = RVF_GREEN;
      T_AUDIO_MP3_STOP *p_msg_stop  = NULL;

      /************************ audio_mp3_stop function begins ****************/

      if(p_audio_gbl_var==NULL)
      {
        audio_mp3_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf( p_audio_gbl_var->mb_external,
                               sizeof(T_AUDIO_MP3_STOP),
                               (T_RVF_BUFFER **)(&p_msg_stop));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_stop);
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status == RVF_RED)
        {
          audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return(AUDIO_ERROR);
        }
      }

      /* fill the message id */
      p_msg_stop->os_hdr.msg_id = AUDIO_MP3_STOP_REQ;

      /* fill the address source id */
      p_msg_stop->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_stop->os_hdr.dest_addr_id= p_audio_gbl_var->addrId;

      *size_played = p_audio_gbl_var->audio_mp3.size_played;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_stop);

      return(AUDIO_OK);

    #else /* L1_MP3!=1 */
      AUDIO_SEND_TRACE("Audio MP3 not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_MP3==1 */

  } /*********************** End of audio_mp3_stop function *******************/
/********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_forward                                        */
  /*                                                                              */
  /*    Purpose:  This function is called to forward MP3 file playback            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        none.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        Mp3 forward rewind- A-50                                              */
  /*                                                                              */
  /********************************************************************************/
INT32 mp3_skip_time=0;//global variable used by audio_mp3.c for forward-rewind.......
  T_AUDIO_RET audio_mp3_forward(UINT32 forward_skip_time)//MP3-FR
  {
#if (L1_MP3 == 1)
 T_AUDIO_RET status;
    if(mp3_skip_time ==0)
    {
	   mp3_skip_time = forward_skip_time;//set the global var for forward request
	   //message content OK
	    //send OKAY message to MMI
	    status = AUDIO_OK;
    }
    else
    {
	    //message content not okay
	     //error message- request can not be processed.
	    status = AUDIO_ERROR;
    }

    return(status);
    #else /* L1_MP3!=1 */
      AUDIO_SEND_TRACE("Audio MP3 not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /*else  L1_MP3==1 */
  }
/********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_rewind                                         */
  /*                                                                              */
  /*    Purpose:  This function is called to rewind MP3 file playback             */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_mp3_rewind(UINT32 rewind_skip_time)//MP3-FR
  {
    #if (L1_MP3 == 1)
    T_AUDIO_RET status;
    if(mp3_skip_time ==0)
    {
	    mp3_skip_time = (-(INT32) rewind_skip_time);//set the global var to start rewind
	    //send OKAY message to MMI
	    status = AUDIO_OK;
    }
    else
    {
	    //error message- request can not be processed.
	    status = AUDIO_ERROR;
    }
    return(status);
    #else /* L1_MP3!=1 */
      AUDIO_SEND_TRACE("Audio MP3 not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /*else  L1_MP3==1 */
  }
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_pause                                          */
  /*                                                                              */
  /*    Purpose:  This function is called to pause MP3 file playback              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_mp3_pause(void)
  {
    #if (L1_MP3 == 1)
      /* Declare local variables */
      T_RVF_MB_STATUS   mb_status    = RVF_GREEN;
      T_AUDIO_MP3_PAUSE *p_msg_pause = NULL;

      /************************ audio_mp3_pause function begins ****************/

      if(p_audio_gbl_var==NULL)
      {
        audio_mp3_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf( p_audio_gbl_var->mb_external,
                               sizeof(T_AUDIO_MP3_PAUSE),
                               (T_RVF_BUFFER **)(&p_msg_pause));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_pause);
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status == RVF_RED)
        {
          audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return(AUDIO_ERROR);
        }
      }

      /* fill the message id */
      p_msg_pause->os_hdr.msg_id = AUDIO_MP3_PAUSE_REQ;

      /* fill the address source id */
      p_msg_pause->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_pause->os_hdr.dest_addr_id= p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_pause);

      return(AUDIO_OK);

    #else /* L1_MP3!=1 */
      AUDIO_SEND_TRACE("Audio MP3 not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_MP3==1 */

  } /*********************** End of audio_mp3_pause function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_resume                                         */
  /*                                                                              */
  /*    Purpose:  This function is called to resume a MP3 file playback           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_mp3_resume(void)
  {
    #if (L1_MP3 == 1)
      /* Declare local variables */
      T_RVF_MB_STATUS   mb_status   = RVF_GREEN;
      T_AUDIO_MP3_RESUME *p_msg_resume  = NULL;

      /************************ audio_mp3_resume function begins ****************/

      if(p_audio_gbl_var==NULL)
      {
        audio_mp3_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf( p_audio_gbl_var->mb_external,
                               sizeof(T_AUDIO_MP3_RESUME),
                               (T_RVF_BUFFER **)(&p_msg_resume));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_resume);
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status == RVF_RED)
        {
          audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return(AUDIO_ERROR);
        }
      }

      /* fill the message id */
      p_msg_resume->os_hdr.msg_id = AUDIO_MP3_RESUME_REQ;

      /* fill the address source id */
      p_msg_resume->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_resume->os_hdr.dest_addr_id= p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_resume);

      return(AUDIO_OK);

    #else /* L1_MP3!=1 */
      AUDIO_SEND_TRACE("Audio MP3 not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_MP3==1 */

  } /*********************** End of audio_mp3_resume function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mp3_info                                           */
  /*                                                                              */
  /*    Purpose:  This function is called to request information about the        */
  /*              currently decoded MP3 frame                                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_mp3_info(void)
  {
    #if (L1_MP3 == 1)
      /* Declare local variables */
      T_RVF_MB_STATUS   mb_status    = RVF_GREEN;
      T_AUDIO_MP3_INFO  *p_msg_info  = NULL;

      /************************ audio_mp3_info function begins ****************/

      if(p_audio_gbl_var==NULL)
      {
        audio_mp3_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf( p_audio_gbl_var->mb_external,
                               sizeof(T_AUDIO_MP3_INFO),
                               (T_RVF_BUFFER **)(&p_msg_info));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_info);
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status == RVF_RED)
        {
          audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return(AUDIO_ERROR);
        }
      }

      /* fill the message id */
      p_msg_info->os_hdr.msg_id = AUDIO_MP3_INFO_REQ;

      /* fill the address source id */
      p_msg_info->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_info->os_hdr.dest_addr_id= p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_info);

      return(AUDIO_OK);

    #else /* L1_MP3!=1 */
      AUDIO_SEND_TRACE("Audio MP3 not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_MP3==1 */

  } /*********************** End of audio_mp3_info function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_init_mp3_session                            */
  /*                                                                              */
  /*    Purpose:  This function is called in order to initialize MP3 play         */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Specific MP3 play parameters                                          */
  /*        Driver parameters                                                     */
  /*        Return path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters                                         */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_driver_init_mp3_session( T_AUDIO_DRIVER_PARAMETER *p_driver_parameter,
                                             T_RV_RETURN return_path)
  {

  #if (L1_MP3 == 1)

    /* Declare local variables.                                                 */
    T_RVF_MB_STATUS   mb_status = RVF_GREEN;
    T_AUDIO_DRIVER_INIT_MP3_SESSION *p_msg  = NULL;

    /************************ audio_driver_init_mp3_session function begins ****************/

    if (p_audio_gbl_var == NULL )
    {
      audio_driver_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* If bad mp3 parameters, then report an error and abort.*/
    if ( /*(p_driver_parameter->buffer_size < AUDIO_MP3_MAX_SAMPLE_SIZE_16BIT)||*/
         ((p_driver_parameter->nb_buffer < AUDIO_DRIVER_MIN_BUFFER_PER_SESSION)||
          (p_driver_parameter->nb_buffer > AUDIO_DRIVER_MAX_BUFFER_PER_SESSION))
       )
    {
      audio_driver_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                             sizeof (T_AUDIO_DRIVER_INIT_MP3_SESSION),
                             (T_RVF_BUFFER **) (&p_msg));

    /* If insufficient resources, then report a memory error and abort. */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg);
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill the message id */
    p_msg->os_hdr.msg_id = AUDIO_DRIVER_INIT_MP3_SESSION;
    p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    /* fill parameters */
    p_msg->driver_parameter.buffer_size = p_driver_parameter->buffer_size;
    p_msg->driver_parameter.nb_buffer   = p_driver_parameter->nb_buffer;

    if (return_path.callback_func == NULL)
    {
      p_msg->return_path.addr_id = return_path.addr_id;
      p_msg->return_path.callback_func = NULL;
    }
    else
      p_msg->return_path.callback_func = return_path.callback_func;

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId, p_msg);

    return (AUDIO_OK);
  #else  // L1_MP3
    AUDIO_SEND_TRACE("Audio MP3 not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
    return (AUDIO_ERROR);
  #endif // L1_MP3

  }



/********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_amr_forward                                     */
  /*                                                                              */
  /*    Purpose:  This function is called to update forward skip time             */
  /*                                                                              */
  /*                                                                              */
  /********************************************************************************/
INT32 vm_amr_skip_time=0;//global variable used by audio_vm_amr.c for forward-rewind.......
  T_AUDIO_RET audio_vm_amr_forward(UINT32 forward_skip_time)
  {
#if (L1_VOICE_MEMO_AMR == 1)
 	T_AUDIO_RET status;

    if(vm_amr_skip_time ==0)
    {
       AUDIO_SEND_TRACE_PARAM ("FWD Update skip time :", forward_skip_time, RV_TRACE_LEVEL_ERROR);
	   vm_amr_skip_time = forward_skip_time;//set the global var for forward request
	   //message content OK
	    //send OKAY message to MMI
	    status = AUDIO_OK;
    }
    else
    {
        AUDIO_SEND_TRACE("FWD skip time Error ", RV_TRACE_LEVEL_ERROR);
	    //message content not okay
	     //error message- request can not be processed.
	    status = AUDIO_ERROR;
    }

    return(status);

    #else 
      AUDIO_SEND_TRACE("Audio VM AMR not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif 
  }
/********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_vm_amr_rewind                                      */
  /*                                                                              */
  /*    Purpose:  This function is called to update rewind skip time              */
  /*                                                                              */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_vm_amr_rewind(UINT32 rewind_skip_time)
  {
    #if (L1_VOICE_MEMO_AMR == 1)
    T_AUDIO_RET status;

    if(vm_amr_skip_time ==0)
    {
        AUDIO_SEND_TRACE_PARAM ("RND Update skip time :", rewind_skip_time, RV_TRACE_LEVEL_ERROR);
	    vm_amr_skip_time = (-(INT32) rewind_skip_time);//set the global var to start rewind
	    //send OKAY message to MMI
	    status = AUDIO_OK;
    }
    else
    {
        AUDIO_SEND_TRACE("RND skip time error",RV_TRACE_LEVEL_DEBUG_LOW);
	    //error message- request can not be processed.
	    status = AUDIO_ERROR;
    }
    return(status);

    #else 
      AUDIO_SEND_TRACE("Audio VM AMR not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif 
  }
  
  



  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_aac_start                                          */
  /*                                                                              */
  /*    Purpose:  This function is called in order to start the playing of        */
  /*              AAC melody from Flash                                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Play parameters                                                       */
  /*        Return_path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters.                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_aac_start (T_AUDIO_AAC_PARAMETER *p_parameter,
                               T_RV_RETURN return_path)
  {
    #if (AUDIO_NEW_FFS_MANAGER)&&(L1_AAC)
      /* Declare local variables. */
      T_AUDIO_AAC_START   *p_msg_start = NULL;
      T_RVF_MB_STATUS     mb_status = RVF_GREEN;
#if(AS_RFS_API == 1)
	  T_RFS_FD            ffs_fd;
	  T_RFS_STAT          stat;
#else
	  T_FFS_FD            ffs_fd;
	  T_FFS_STAT          stat;
#endif


      /************************ audio_aac_start function begins **************/

      if (p_audio_gbl_var == NULL )
      {
        audio_aac_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      //First check whether the AAC file is atleast having 1600bytes. If not do not give to L1
      //for playback.
#if(AS_RFS_API == 1)
	  rfs_stat (p_parameter->aac_name, &stat);
#else
	  ffs_stat (p_parameter->aac_name, &stat);
#endif

#if(AS_RFS_API == 1)
	  if(stat.file_dir.size < 1600)
#else
	  if(stat.size < 1600)
#endif
      {
        AUDIO_SEND_TRACE("AAC File size < 1600bytes!", RV_TRACE_LEVEL_DEBUG_LOW);
        return (AUDIO_ERROR);
      }
      /* check if the AAC file already exists */

#if(AS_RFS_API == 1)
	   ffs_fd = rfs_open(p_parameter->aac_name, RFS_O_RDONLY, RFS_RDWREX_ALL);
#else
	  ffs_fd = ffs_open(p_parameter->aac_name, FFS_O_RDONLY);
#endif

	  if ( ffs_fd <= 0)
      {
        audio_aac_error_trace(AUDIO_ENTITY_FILE_ERROR);
        return (AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_AAC_START),
                               (T_RVF_BUFFER **) (&p_msg_start));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_start->os_hdr.msg_id    = AUDIO_AAC_START_REQ;

      /* fill the address source id */
      p_msg_start->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      /* fill the message parameters */
      p_msg_start->audio_ffs_fd        = ffs_fd;
#if(AS_RFS_API == 1)
	p_msg_start->audio_filesize       = stat.file_dir.size;
#else
      p_msg_start->audio_filesize       = stat.size;
#endif
      p_msg_start->play_bar_on   = p_parameter->play_bar_on;


      /* the name */
#if(AS_RFS_API == 1)
	  wstrcpy(p_msg_start->aac_parameter.aac_name,
             p_parameter->aac_name);
#else
	  strcpy(p_msg_start->aac_parameter.aac_name,
             p_parameter->aac_name);
#endif

      /* the melody mode */
      if ( (p_parameter->mono_stereo == AUDIO_AAC_MONO) ||
           (p_parameter->mono_stereo == AUDIO_AAC_STEREO) )
      {
        p_msg_start->aac_parameter.mono_stereo = p_parameter->mono_stereo;
      }
      else
      {
        /* Wrong parameter */
        audio_aac_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
        /* close the file previously open */
#if(AS_RFS_API == 1)
		rfs_close(ffs_fd);
#else
		ffs_close(ffs_fd);
#endif
        return (AUDIO_ERROR);
      }

      p_msg_start->aac_parameter.size_file_start = p_parameter->size_file_start;

//      if (p_msg_start->aac_parameter.size_file_start <0)
//      {
//        /* Wrong parameter */
//        audio_aac_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
//        /* deallocate the memory */
//        rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
//        /* close the file previously open */
//        ffs_close(ffs_fd);
//        return (AUDIO_ERROR);
//      }

      if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_start);

      return (AUDIO_OK);
    #else /* (AUDIO_NEW_FFS_MANAGER)&&(L1_AAC) */
      AUDIO_SEND_TRACE("Audio AAC not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif /* (AUDIO_NEW_FFS_MANAGER)&&(L1_AAC) */
  } /*********************** End of audio_aac_start function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_aac_stop                                           */
  /*                                                                              */
  /*    Purpose:  This function is called to stop AAC file playback               */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_aac_stop(UINT32 *size_played)
  {
    #if (L1_AAC == 1)
      /* Declare local variables */
      T_RVF_MB_STATUS   mb_status   = RVF_GREEN;
      T_AUDIO_AAC_STOP *p_msg_stop  = NULL;

      /************************ audio_aac_stop function begins ****************/

      if(p_audio_gbl_var==NULL)
      {
        audio_aac_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

    if(p_audio_gbl_var->audio_aac.stop_aac_true == TRUE)
        { 
           return(AUDIO_AAC_NOT_STOP);
      	}
	  

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf( p_audio_gbl_var->mb_external,
                               sizeof(T_AUDIO_AAC_STOP),
                               (T_RVF_BUFFER **)(&p_msg_stop));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_stop);
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status == RVF_RED)
        {
          audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return(AUDIO_ERROR);
        }
      }

      /* fill the message id */
      p_msg_stop->os_hdr.msg_id = AUDIO_AAC_STOP_REQ;

      /* fill the address source id */
      p_msg_stop->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_stop->os_hdr.dest_addr_id= p_audio_gbl_var->addrId;

      *size_played = p_audio_gbl_var->audio_aac.size_played;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_stop);

      return(AUDIO_OK);

    #else /* L1_AAC!=1 */
      AUDIO_SEND_TRACE("Audio AAC not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_AAC==1 */

  } /*********************** End of audio_aac_stop function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_aac_pause                                          */
  /*                                                                              */
  /*    Purpose:  This function is called to pause AAC file playback              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_aac_pause(void)
  {
    #if (L1_AAC == 1)
      /* Declare local variables */
      T_RVF_MB_STATUS   mb_status    = RVF_GREEN;
      T_AUDIO_AAC_PAUSE *p_msg_pause = NULL;

      /************************ audio_aac_pause function begins ****************/

      if(p_audio_gbl_var==NULL)
      {
        audio_aac_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf( p_audio_gbl_var->mb_external,
                               sizeof(T_AUDIO_AAC_PAUSE),
                               (T_RVF_BUFFER **)(&p_msg_pause));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_pause);
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status == RVF_RED)
        {
          audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return(AUDIO_ERROR);
        }
      }

      /* fill the message id */
      p_msg_pause->os_hdr.msg_id = AUDIO_AAC_PAUSE_REQ;

      /* fill the address source id */
      p_msg_pause->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_pause->os_hdr.dest_addr_id= p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_pause);

      return(AUDIO_OK);

    #else /* L1_AAC!=1 */
      AUDIO_SEND_TRACE("Audio AAC not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_AAC==1 */

  } /*********************** End of audio_aac_pause function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_aac_resume                                         */
  /*                                                                              */
  /*    Purpose:  This function is called to resume a AAC file playback           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_aac_resume(void)
  {
    #if (L1_AAC == 1)
      /* Declare local variables */
      T_RVF_MB_STATUS   mb_status   = RVF_GREEN;
      T_AUDIO_AAC_RESUME *p_msg_resume  = NULL;

      /************************ audio_aac_resume function begins ****************/

      if(p_audio_gbl_var==NULL)
      {
        audio_aac_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf( p_audio_gbl_var->mb_external,
                               sizeof(T_AUDIO_AAC_RESUME),
                               (T_RVF_BUFFER **)(&p_msg_resume));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_resume);
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status == RVF_RED)
        {
          audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return(AUDIO_ERROR);
        }
      }

      /* fill the message id */
      p_msg_resume->os_hdr.msg_id = AUDIO_AAC_RESUME_REQ;

      /* fill the address source id */
      p_msg_resume->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_resume->os_hdr.dest_addr_id= p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_resume);

      return(AUDIO_OK);

    #else /* L1_AAC!=1 */
      AUDIO_SEND_TRACE("Audio AAC not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_AAC==1 */

  } /*********************** End of audio_aac_resume function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_aac_info                                           */
  /*                                                                              */
  /*    Purpose:  This function is called to request information about the        */
  /*              currently decoded AAC frame                                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Return path.                                                          */
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
  T_AUDIO_RET audio_aac_info(void)
  {
    #if (L1_AAC == 1)
      /* Declare local variables */
      T_RVF_MB_STATUS   mb_status    = RVF_GREEN;
      T_AUDIO_AAC_INFO  *p_msg_info  = NULL;

      /************************ audio_aac_info function begins ****************/

      if(p_audio_gbl_var==NULL)
      {
        audio_aac_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf( p_audio_gbl_var->mb_external,
                               sizeof(T_AUDIO_AAC_INFO),
                               (T_RVF_BUFFER **)(&p_msg_info));

      /* If insufficient resources, then report a memory error and abort */
      if(mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_info);
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return(AUDIO_ERROR);
      }
      else
      {
        if(mb_status == RVF_RED)
        {
          audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
          return(AUDIO_ERROR);
        }
      }

      /* fill the message id */
      p_msg_info->os_hdr.msg_id = AUDIO_AAC_INFO_REQ;

      /* fill the address source id */
      p_msg_info->os_hdr.src_addr_id = rvf_get_taskid();
      p_msg_info->os_hdr.dest_addr_id= p_audio_gbl_var->addrId;

      /* send the messsage to the audio entity */
      rvf_send_msg(p_audio_gbl_var->addrId,p_msg_info);

      return(AUDIO_OK);

    #else /* L1_AAC!=1 */
      AUDIO_SEND_TRACE("Audio AAC not compiled",RV_TRACE_LEVEL_DEBUG_LOW);
      return(AUDIO_ERROR);
    #endif /* L1_AAC==1 */

  } /*********************** End of audio_aac_info function *******************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_aac_restart                                        */
  /*                                                                              */
  /*    Purpose:  This function is called in order to restart the playing of      */
  /*              AAC melody from Flash                                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Play parameters                                                       */
  /*        Return_path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters.                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_aac_restart (UINT32 size_file_restart)
  {
    #if (L1_AAC)
      /* Declare local variables. */
      T_AUDIO_AAC_RESTART   *p_msg_restart = NULL;
      T_RVF_MB_STATUS       mb_status = RVF_GREEN;

      /************************ audio_aac_restart function begins **************/
      if (p_audio_gbl_var == NULL )
      {
        audio_aac_error_trace(AUDIO_ENTITY_NOT_START);
        return(AUDIO_ERROR);
      }

      /* allocate the memory for the message to send */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_AAC_RESTART),
                               (T_RVF_BUFFER **) (&p_msg_restart));

      /* If insufficient resources, then report a memory error and abort.      */
      if (mb_status == RVF_YELLOW)
      {
        /* deallocate the memory */
        rvf_free_buf((T_RVF_BUFFER *)p_msg_restart);
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }
      else if (mb_status == RVF_RED)
      {
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
        return (AUDIO_ERROR);
      }

      /* fill the message id */
      p_msg_restart->os_hdr.msg_id    = AUDIO_AAC_RESTART_REQ;

      /* fill the address source id */
      p_msg_restart->os_hdr.src_addr_id  = rvf_get_taskid();
      p_msg_restart->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

      p_msg_restart->aac_restart_parameter.size_file_start = size_file_restart;
      AUDIO_SEND_TRACE_PARAM("size_file_restart",size_file_restart, RV_TRACE_LEVEL_DEBUG_LOW);

//      if (p_msg_restart->aac_restart_parameter.size_file_start <0)
//      {
//        /* Wrong parameter */
//        audio_aac_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
//        /* deallocate the memory */
//        rvf_free_buf((T_RVF_BUFFER *)p_msg_restart);
//        return (AUDIO_ERROR);
//      }

      /* send the messsage to the audio entity */
      rvf_send_msg (p_audio_gbl_var->addrId, p_msg_restart);

      return (AUDIO_OK);
    #else /* L1_AAC!=1 */
      AUDIO_SEND_TRACE("Audio AAC not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    #endif /* L1_AAC == 1 */
  } /*********************** End of audio_aac_restart function **********************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_init_aac_session                            */
  /*                                                                              */
  /*    Purpose:  This function is called in order to initialize AAC play         */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Specific AAC play parameters                                          */
  /*        Driver parameters                                                     */
  /*        Return path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters                                         */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_driver_init_aac_session( T_AUDIO_DRIVER_PARAMETER *p_driver_parameter,
                                             T_RV_RETURN return_path)
  {

  #if (L1_AAC == 1)

    /* Declare local variables.                                                 */
    T_RVF_MB_STATUS   mb_status = RVF_GREEN;
    T_AUDIO_DRIVER_INIT_AAC_SESSION *p_msg  = NULL;

    /**************** audio_driver_init_aac_session function begins ****************/

    if (p_audio_gbl_var == NULL )
    {
      audio_driver_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* If bad aac parameters, then report an error and abort.*/
    if ( ((p_driver_parameter->nb_buffer < AUDIO_DRIVER_MIN_BUFFER_PER_SESSION)||
          (p_driver_parameter->nb_buffer > AUDIO_DRIVER_MAX_BUFFER_PER_SESSION))
       )
    {
      audio_driver_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                             sizeof (T_AUDIO_DRIVER_INIT_AAC_SESSION),
                             (T_RVF_BUFFER **) (&p_msg));

    /* If insufficient resources, then report a memory error and abort. */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg);
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_driver_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }

    /* fill the message id */
    p_msg->os_hdr.msg_id = AUDIO_DRIVER_INIT_AAC_SESSION;
    p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    /* fill parameters */
    p_msg->driver_parameter.buffer_size = p_driver_parameter->buffer_size;
    p_msg->driver_parameter.nb_buffer   = p_driver_parameter->nb_buffer;

    if (return_path.callback_func == NULL)
    {
      p_msg->return_path.addr_id = return_path.addr_id;
      p_msg->return_path.callback_func = NULL;
    }
    else
      p_msg->return_path.callback_func = return_path.callback_func;

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId, p_msg);

    return (AUDIO_OK);
  #else  // L1_AAC
    AUDIO_SEND_TRACE("Audio AAC not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
    return (AUDIO_ERROR);
  #endif // L1_AAC

  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_driver_..._session                                 */
  /*                                                                              */
  /*    Purpose:  This function is called in order to start/stop/free session     */
  /*                                                  pause/resume/restart session*/
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        channel identifier                                                    */
  /*        Return path                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*         Validation of the parameters                                         */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_driver_start_session(UINT8 channel_id, T_RV_RETURN notification_return_path)
  {
    return audio_driver_handle_session(AUDIO_DRIVER_START_SESSION, channel_id, notification_return_path);
  }

  T_AUDIO_RET audio_driver_stop_session(UINT8 channel_id)
  {
    T_RV_RETURN return_path;

    return_path.callback_func = NULL;
    return_path.addr_id       = 0;

    return audio_driver_handle_session(AUDIO_DRIVER_STOP_SESSION, channel_id, return_path);
  }

  T_AUDIO_RET audio_driver_free_session(UINT8 channel_id, T_RV_RETURN return_path)
  {
    return audio_driver_handle_session(AUDIO_DRIVER_FREE_SESSION, channel_id, return_path);
  }

  T_AUDIO_RET audio_driver_pause_session(UINT8 channel_id, T_RV_RETURN return_path)
  {
    return audio_driver_handle_session(AUDIO_DRIVER_PAUSE_SESSION, channel_id, return_path);
  }

  T_AUDIO_RET audio_driver_resume_session(UINT8 channel_id, T_RV_RETURN return_path)
  {
    return audio_driver_handle_session(AUDIO_DRIVER_RESUME_SESSION, channel_id, return_path);
  }

  T_AUDIO_RET audio_driver_info_session(UINT8 channel_id, T_RV_RETURN return_path)
  {
    return audio_driver_handle_session(AUDIO_DRIVER_INFO_SESSION, channel_id, return_path);
  }

#if (L1_AAC == 1)

  T_AUDIO_RET audio_driver_info_aac_session(UINT8 channel_id, T_RV_RETURN return_path)
  {
    return audio_driver_handle_session(AUDIO_DRIVER_INFO_AAC_SESSION, channel_id, return_path);
  }
#endif

  T_AUDIO_RET audio_driver_restart_session(UINT8 channel_id, T_RV_RETURN return_path)
  {
    return audio_driver_handle_session(AUDIO_DRIVER_RESTART_SESSION, channel_id, return_path);
  }


  T_AUDIO_RET audio_driver_handle_session(UINT32 msg_id, UINT8 channel_id, T_RV_RETURN return_path)
  {
  #if (L1_AUDIO_DRIVER)
    /* Declare local variables.                                                 */
    T_RVF_MB_STATUS   mb_status = RVF_GREEN;
    T_AUDIO_DRIVER_HANDLE_SESSION *p_msg  = NULL;

    /************************ audio_driver_handle_session function begins ****************/

    if (p_audio_gbl_var == NULL )
    {
      audio_driver_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }

    /* If bad voice memo record parameters, then report an error and abort.*/
    if (channel_id >= AUDIO_DRIVER_MAX_CHANNEL)
    {
      audio_driver_error_trace(AUDIO_ENTITY_BAD_PARAMETER);
      return (AUDIO_ERROR);
    }

    /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                             sizeof (T_AUDIO_DRIVER_HANDLE_SESSION),
                             (T_RVF_BUFFER **) (&p_msg));

    /* If insufficient resources, then report a memory error and abort.         */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg);
      #if (L1_MP3 == 1)
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
      #endif
      #if (L1_AAC == 1)
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
      #endif
      #if (L1_VOICE_MEMO_AMR == 1)
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
      #endif
      #if (L1_EXT_AUDIO_MGT == 1)
        audio_midi_error_trace(AUDIO_ENTITY_NO_MEMORY);
      #endif
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      #if (L1_MP3 == 1)
        audio_mp3_error_trace(AUDIO_ENTITY_NO_MEMORY);
      #endif
      #if (L1_AAC == 1)
        audio_aac_error_trace(AUDIO_ENTITY_NO_MEMORY);
      #endif
      #if (L1_VOICE_MEMO_AMR == 1)
        audio_voice_memo_amr_error_trace(AUDIO_ENTITY_NO_MEMORY);
      #endif
      #if (L1_EXT_AUDIO_MGT == 1)
        audio_midi_error_trace(AUDIO_ENTITY_NO_MEMORY);
      #endif

      return (AUDIO_ERROR);
    }

    /* fill the message id */
    p_msg->os_hdr.msg_id = msg_id;
    p_msg->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

    /* fill the message parameters */
    p_msg->channel_id = channel_id;

    if (return_path.callback_func == NULL)
    {
      p_msg->return_path.addr_id = return_path.addr_id;
      p_msg->return_path.callback_func = NULL;
    }
    else
      p_msg->return_path.callback_func = return_path.callback_func;

    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId, p_msg);

    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("Audio Driver not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
    return(AUDIO_ERROR);
  #endif
  }

  T_AUDIO_RET audio_driver_get_play_buffer(UINT8 channel_id, UINT8 **pp_buffer)
  {
  #if (L1_AUDIO_DRIVER)
    T_AUDIO_DRIVER_SESSION *p_session;
    UINT8 index_appli;




    /* Test CHANNEL_ID */
    if (channel_id >= AUDIO_DRIVER_MAX_CHANNEL)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER GET BUFFER: channel_id not valid", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    /* get driver session */
    p_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);
    index_appli = p_session->session_info.index_appli;

    /* channel must be initialized */
    if (p_session->session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_INIT)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER GET BUFFER: channel not initialized", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    /* play buffer must have been used after a previous call of this API */
    if (p_session->session_info.play_api_state != AUDIO_PLAY_API_STATE_GET_BUF)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER GET BUFFER: PLAY not called after GET", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    /* test if next buffer is available */
    if (index_appli != p_session->session_info.index_l1)
    {
      *pp_buffer = (UINT8 *)(p_session->session_info.buffer[index_appli].p_start_pointer);
      AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER GET BUFFER: buffer", *pp_buffer, RV_TRACE_LEVEL_DEBUG_LOW);
      p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_PLAY_BUF;
    }
    else
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER GET BUFFER: no buffer available", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    /* 1st time, index_l1 = 0xFF. At 1st play, layer1 becomes valid so we put 0
       This way, appli can't circle round buffers when in WAIT_START state */
    if (p_session->session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_START)
      p_session->session_info.index_l1 = 0;

    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("Audio Driver not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
    return(AUDIO_ERROR);
  #endif
  }

  T_AUDIO_RET audio_driver_play_buffer(UINT8 channel_id, UINT8 *p_buffer)
  {
  #if (L1_AUDIO_DRIVER)
    T_AUDIO_DRIVER_SESSION *p_session;

    /* Test CHANNEL_ID */
    if (channel_id >= AUDIO_DRIVER_MAX_CHANNEL)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER PLAY BUFFER: channel_id not valid", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    /* get driver session */
    p_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);

    /* channel must be initialized */
    if (p_session->session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_INIT)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER PLAY BUFFER: channel not initialized", RV_TRACE_LEVEL_DEBUG_LOW);
      //p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;
      return (AUDIO_ERROR);
    }

    /* get buffer must have been called before */
    if (p_session->session_info.play_api_state != AUDIO_PLAY_API_STATE_PLAY_BUF)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER GET BUFFER: GET not called before play", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    #if (L1_EXT_AUDIO_MGT!=1)
      /* check validity of buffer */
      if ( p_buffer !=
         ((UINT8 *)(p_session->session_info.buffer[p_session->session_info.index_appli].p_start_pointer)))
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER PLAY BUFFER: buffer is not valid", p_buffer, RV_TRACE_LEVEL_DEBUG_LOW);
        return (AUDIO_ERROR);
      }
    #endif

    /* increment index_appli */
    AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER PLAY BUFFER:", p_session->session_info.index_appli, RV_TRACE_LEVEL_DEBUG_LOW);
    p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;
    p_session->session_info.index_appli++;

    if (p_session->session_info.index_appli == p_session->session_req.nb_buffer)
      p_session->session_info.index_appli = 0;

    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("Audio Driver not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
    return(AUDIO_ERROR);
  #endif

  }

T_AUDIO_RET audio_driver_play_midi_buffer(UINT8 channel_id, UINT8 *p_buffer)
  {
  #if (L1_AUDIO_DRIVER)
    T_AUDIO_DRIVER_SESSION *p_session;

    /* Test CHANNEL_ID */
    if (channel_id >= AUDIO_DRIVER_MAX_CHANNEL)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER PLAY BUFFER: channel_id not valid", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    /* get driver session */
    p_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);

    /* channel must be initialized */
    if (p_session->session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_INIT)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER PLAY BUFFER: channel not initialized", RV_TRACE_LEVEL_DEBUG_LOW);
      //p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;
      return (AUDIO_ERROR);
    }

    /* get buffer must have been called before */
    if (p_session->session_info.play_api_state != AUDIO_PLAY_API_STATE_PLAY_BUF)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER GET BUFFER: GET not called before play", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    /* increment index_appli */
    AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER PLAY BUFFER:", p_session->session_info.index_appli, RV_TRACE_LEVEL_DEBUG_LOW);
    p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;
    p_session->session_info.index_appli++;

    if (p_session->session_info.index_appli == p_session->session_req.nb_buffer)
      p_session->session_info.index_appli = 0;

    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("Audio Driver not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
    return(AUDIO_ERROR);
  #endif

  }


  T_AUDIO_RET audio_driver_flush_buffer(UINT8 channel_id, UINT8 *p_buffer)
  {
  #if (L1_AUDIO_DRIVER)
    T_AUDIO_DRIVER_SESSION *p_session;

    /* Test CHANNEL_ID */
    if (channel_id >= AUDIO_DRIVER_MAX_CHANNEL)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER FLUS BUFFER: channel_id not valid", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    /* get driver session */
    p_session = &(p_audio_gbl_var->audio_driver_session[channel_id]);

    /* channel must be initialized */
    if (p_session->session_info.state == AUDIO_DRIVER_CHANNEL_WAIT_INIT)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER FLUSH BUFFER: channel not initialized", RV_TRACE_LEVEL_DEBUG_LOW);
      //p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;
      return (AUDIO_ERROR);
    }

    /* get buffer must have been called before */
    if (p_session->session_info.play_api_state != AUDIO_PLAY_API_STATE_PLAY_BUF)
    {
      AUDIO_SEND_TRACE("AUDIO DRIVER GET BUFFER: GET not called before play", RV_TRACE_LEVEL_DEBUG_LOW);
      return (AUDIO_ERROR);
    }

    #if (L1_EXT_AUDIO_MGT!=1)
      /* check validity of buffer */
      if ( p_buffer !=
         ((UINT8 *)(p_session->session_info.buffer[p_session->session_info.index_appli].p_start_pointer)))
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER FLUSH BUFFER: buffer is not valid", p_buffer, RV_TRACE_LEVEL_DEBUG_LOW);
        return (AUDIO_ERROR);
      }
    #endif

    /* increment index_appli */
    AUDIO_SEND_TRACE_PARAM("AUDIO DRIVER FLUSH BUFFER:", p_session->session_info.index_appli, RV_TRACE_LEVEL_DEBUG_LOW);
    p_session->session_info.play_api_state = AUDIO_PLAY_API_STATE_GET_BUF;
    p_session->session_info.index_appli++;

    if (p_session->session_info.index_appli == p_session->session_req.nb_buffer)
      p_session->session_info.index_appli = 0;

    #if (L1_MP3)
      p_audio_gbl_var->audio_mp3.index_last_buffer = p_session->session_info.index_appli;
    #endif
    #if (L1_AAC)
      p_audio_gbl_var->audio_aac.index_last_buffer = p_session->session_info.index_appli;
    #endif
    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("Audio Driver not compiled", RV_TRACE_LEVEL_DEBUG_LOW);
    return(AUDIO_ERROR);
  #endif

  }


#if(L1_BT_AUDIO==1)
  T_AUDIO_RET  audio_bt_cfg(T_AUDIO_BT_PARAMETER *p_parameter,T_RV_RETURN return_path )
{
    T_AUDIO_BT_CFG_REQ *p_msg_start;
    T_RVF_MB_STATUS       mb_status = RVF_GREEN;

    if (p_audio_gbl_var == NULL )
    {
      audio_mode_error_trace(AUDIO_ENTITY_NOT_START);
      return(AUDIO_ERROR);
    }
     /* allocate the memory for the message to send */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                             sizeof (T_AUDIO_BT_CFG_REQ),
                             (T_RVF_BUFFER **) (&p_msg_start));

    /* If insufficient resources, then report a memory error and abort. */
    if (mb_status == RVF_YELLOW)
    {
      /* deallocate the memory */
      rvf_free_buf((T_RVF_BUFFER *)p_msg_start);
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    else
    if (mb_status == RVF_RED)
    {
      audio_mode_error_trace(AUDIO_ENTITY_NO_MEMORY);
      return (AUDIO_ERROR);
    }
    /* fill the message parameters */
    p_msg_start->bt_parameter.connected_status   = p_parameter->connected_status;

    /* fill the message id */
    p_msg_start->os_hdr.msg_id    = AUDIO_BT_CFG_REQ;

    /* fill the task source id */
    p_msg_start->os_hdr.src_addr_id = rvf_get_taskid();
    p_msg_start->os_hdr.dest_addr_id = p_audio_gbl_var->addrId;

	if (return_path.callback_func == NULL)
      {
        p_msg_start->return_path.addr_id = return_path.addr_id;
        p_msg_start->return_path.callback_func = NULL;
      }
      else
      {
        p_msg_start->return_path.callback_func = return_path.callback_func;
      }


    /* send the messsage to the audio entity */
    rvf_send_msg (p_audio_gbl_var->addrId,
                  p_msg_start);
    return (AUDIO_OK);
  }

#endif	
#endif // #ifdef RVM_AUDIO_MAIN_SWE

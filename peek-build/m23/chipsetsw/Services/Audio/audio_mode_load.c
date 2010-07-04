/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_mode_load.c                                           */
/*                                                                          */
/*  Purpose:  This file contains all the functions used for audio mode      */
/*            load services.                                                */
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
    #include "l1sw.cfg"
    #include "swconfig.cfg"
    #include "sys.cfg"
    #include "chipset.cfg"
  #endif

  #include "l1_confg.h"
  
  #include "rv/rv_general.h"
  #include "rvm/rvm_gen.h"
#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#else
  #include "Audio/audio_ffs_i.h"
#endif
  #include "Audio/audio_api.h"
  #include "Audio/audio_structs_i.h"
  #include "Audio/audio_error_hdlr_i.h"
  #include "Audio/audio_var_i.h"
  #include "Audio/audio_messages_i.h"
  #include "Audio/audio_macro_i.h"
  #include "rvf/rvf_target.h"
  #include "Audio/audio_const_i.h"

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

#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#else
  #include "ffs/ffs_api.h"
#endif

#if (ANLG_FAM == 11)
    #include "types.h"
    #include "bspTwl3029_I2c.h"
    #include "bspTwl3029_Audio.h"
#endif
  #ifdef _WINDOWS
    #include "audio/tests/audio_test.h"
  #endif

#include <string.h>

  /* external functions */
  /* write */
 #if (AUDIO_DSP_FEATURES == 1)
  extern T_AUDIO_RET audio_mode_volume_speed_write                      (INT16  *data);
  extern T_AUDIO_RET audio_mode_microphone_gain_write                   (INT16  *data);
  extern T_AUDIO_RET audio_mode_speaker_gain_write                      (INT16  *data);
  extern T_AUDIO_RET audio_mode_sidetone_gain_write                     (INT16  *data);
 #else // AUDIO_DSP_FEATURES == 0
  extern T_AUDIO_RET audio_mode_microphone_mode_write          (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_gain_write          (INT8  *data);
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
 #endif // AUDIO_DSP_FEATURES == 1

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
  #if (L1_DRC == 1)
    extern T_AUDIO_RET audio_mode_speaker_drc_write            (T_AUDIO_DRC_CFG *data);
  #endif
  #if (L1_LIMITER == 1)
    extern T_AUDIO_RET audio_mode_speaker_limiter_write        (T_AUDIO_LIMITER_CFG *data);
  #endif



 #if (AUDIO_DSP_ONOFF == 1)
  extern T_AUDIO_RET audio_mode_onoff_write                             (INT8  *data);
 #endif

  /* Define a macro to simplify the code */
  #define CHECK_STATUS(function, param)     if ( (function(&(param))) == AUDIO_ERROR ) \
                                            { \
                                              return(AUDIO_ERROR); \
                                            } \

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_load_send_status                              */
  /*                                                                              */
  /*    Purpose:  This function sends the audio mdoe load status to the entity.   */
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
  void audio_mode_load_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_LOAD_DONE *p_send_message = NULL;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof (T_AUDIO_LOAD_DONE),
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
    ((T_AUDIO_LOAD_DONE *)p_send_message)->os_hdr.msg_id =
      AUDIO_MODE_LOAD_DONE;

    /* fill the status parameters */
    ((T_AUDIO_LOAD_DONE *)p_send_message)->status = status;

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
  /*    Function Name:   audio_mode_set                                           */
  /*                                                                              */
  /*    Purpose:  This function set the audio mode structure to the mobile.       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio mode structure.                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_set(T_AUDIO_MODE *p_audio_mode, UINT8 *message_to_confirm)
  {
    T_AUDIO_FIR_COEF *data_speaker = NULL, *data_microphone = NULL;

    *message_to_confirm = 0;

#if (PSP_STANDALONE != 1)
    /* Audio voice path */
    if (audio_mode_voice_path_write (&p_audio_mode->audio_path_setting, message_to_confirm) == AUDIO_ERROR)
    {
      return(AUDIO_ERROR);
    }
#endif
	/*Audio FM path*/
	#if(AS_FM_RADIO==1)
  CHECK_STATUS(audio_mode_fm_mode_write,p_audio_mode->audio_fm_setting)
  #endif
  #if (AUDIO_DSP_FEATURES == 1)

    CHECK_STATUS(audio_mode_volume_speed_write,    p_audio_mode->audio_microphone_speaker_setting.volume_speed)
    CHECK_STATUS(audio_mode_speaker_gain_write,    p_audio_mode->audio_speaker_setting.speaker_gain)
    CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.micro_gain)
    
    data_speaker = &(p_audio_mode->audio_speaker_setting.fir);
    data_microphone = &(p_audio_mode->audio_microphone_setting.fir);

    /* Audio microphone ANR & ES */
    #if (L1_ANR == 1 || L1_ANR == 2)
      CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.anr)
      *message_to_confirm += 1;
    #endif
    #if (L1_ES == 1)
      CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.es)
      *message_to_confirm += 1;
    #endif

    /* AGC_UL of the speaker */
    #if (L1_AGC_UL == 1)
      CHECK_STATUS(audio_mode_agc_ul_write, p_audio_mode->audio_microphone_setting.agc_ul);
      *message_to_confirm += 1;	
    #endif

    /* AGC_DL of the speaker */
    #if (L1_AGC_DL == 1)
      CHECK_STATUS(audio_mode_agc_dl_write, p_audio_mode->audio_speaker_setting.agc_dl);
      *message_to_confirm += 1;	
    #endif
  /* Voice speaker DRC*/
    #if (L1_DRC == 1)
      CHECK_STATUS(audio_mode_speaker_drc_write, p_audio_mode->audio_speaker_setting.iir)
      *message_to_confirm += 1;
    #endif

    /* Voice speaker IIR & Limiter */
    #if (L1_IIR == 1 || L1_IIR == 2)
      CHECK_STATUS(audio_mode_speaker_iir_write, p_audio_mode->audio_speaker_setting.iir)
      *message_to_confirm += 1;
    #endif
    #if (L1_LIMITER == 1)
      CHECK_STATUS(audio_mode_speaker_limiter_write, p_audio_mode->audio_speaker_setting.limiter)
      *message_to_confirm += 1;
    #endif

  #else // AUDIO_DSP_FEATURES == 0

    /* Audio microphone mode */
    CHECK_STATUS(audio_mode_microphone_mode_write, p_audio_mode->audio_microphone_setting.mode)

#if (ANLG_FAM != 11)
    /* Audio microphone setting */
    switch (p_audio_mode->audio_microphone_setting.mode)
    {
      case AUDIO_MICROPHONE_HANDHELD:
      {
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.handheld.gain)
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.handheld.fir);
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.handheld.output_bias)
        #if (L1_ANR == 1)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.handheld.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.handheld.es)
          *message_to_confirm += 1;
        #endif
        break;
      }
      case AUDIO_MICROPHONE_HANDFREE:
      {
        CHECK_STATUS(audio_mode_microphone_extra_gain_write, p_audio_mode->audio_microphone_setting.setting.handfree.extra_gain)
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.handfree.fir);
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.handfree.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.handfree.output_bias)
        #if (L1_ANR == 1)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.handfree.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.handfree.es)
          *message_to_confirm += 1;
        #endif
        break;
      }
      case AUDIO_MICROPHONE_HEADSET:
      {
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.headset.gain)
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.headset.fir);
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.headset.output_bias)
        #if (L1_ANR == 1)    
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.headset.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)    
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.headset.es)
          *message_to_confirm += 1;
        #endif
        break;
      }
      case AUDIO_MICROPHONE_HEADSET_DIFFERENTIAL:
      {
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.headset_diff.gain)
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.headset_diff.fir);
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.headset_diff.output_bias)
        #if (L1_ANR == 1)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.headset_diff.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.headset_diff.es)
          *message_to_confirm += 1;
        #endif
        break;
      }
    }
#else
    /* Audio microphone setting */
    switch (p_audio_mode->audio_microphone_setting.mode)
    {
      case AUDIO_MICROPHONE_HANDSET_25_6DB:
      {
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.handset_25_6db.fir);
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.output_bias)
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.gain)
        #if (PSP_STANDALONE != 1)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.es)
          *message_to_confirm += 1;
        #endif
        /* AGC_UL of the speaker */
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_write, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.agc_ul);
          *message_to_confirm += 1;		
        #endif		
		#endif
        break;
      }
      case AUDIO_MICROPHONE_HEADSET_4_9_DB:
      {
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.headset_4_9db.fir);
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.output_bias)
        #if (PSP_STANDALONE != 1)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.es)
          *message_to_confirm += 1;
        #endif
        /* AGC_UL of the speaker */
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_write, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.agc_ul);
          *message_to_confirm += 1;		
        #endif		
		#endif
        break;
      }
	  case AUDIO_MICROPHONE_HEADSET_25_6DB:
      {
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.headset_25_6db.fir);
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.output_bias)
        #if (PSP_STANDALONE != 1)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.es)
          *message_to_confirm += 1;
        #endif
        /* AGC_UL of the speaker */
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_write, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.agc_ul);
          *message_to_confirm += 1;		
        #endif		
		#endif
        break;
      }
	  case AUDIO_MICROPHONE_HEADSET_18DB:
      {
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.headset_18db.fir);
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.headset_18db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.headset_18db.output_bias)
        #if (PSP_STANDALONE != 1)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.headset_18db.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.headset_18db.es)
          *message_to_confirm += 1;
        #endif
        /* AGC_UL of the speaker */
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_write, p_audio_mode->audio_microphone_setting.setting.headset_18db.agc_ul);
          *message_to_confirm += 1;		
        #endif		
		#endif
        break;
      }
	  case AUDIO_MICROPHONE_AUX_4_9DB:
      {
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.aux_4_9db.fir);
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.output_bias)
        #if (PSP_STANDALONE != 1)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.es)
          *message_to_confirm += 1;
        #endif
        /* AGC_UL of the speaker */
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_write, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.agc_ul);
          *message_to_confirm += 1;		
        #endif		
		#endif
        break;
      }
	  case AUDIO_MICROPHONE_AUX_28_2DB:
      {
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.aux_28_2db.fir);
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.output_bias)
        #if (PSP_STANDALONE != 1)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.es)
          *message_to_confirm += 1;
        #endif
        /* AGC_UL of the speaker */
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_write, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.agc_ul);
          *message_to_confirm += 1;		
        #endif		
		#endif
        break;
      }
	  case AUDIO_MICROPHONE_CARKIT:
      {
        data_microphone = &(p_audio_mode->audio_microphone_setting.setting.carkit.fir);
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.carkit.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.carkit.output_bias)
        #if (PSP_STANDALONE != 1)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_write, p_audio_mode->audio_microphone_setting.setting.carkit.anr)
          *message_to_confirm += 1;
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_write, p_audio_mode->audio_microphone_setting.setting.carkit.es)
          *message_to_confirm += 1;
        #endif
        /* AGC_UL of the speaker */
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_write, p_audio_mode->audio_microphone_setting.setting.carkit.agc_ul);
          *message_to_confirm += 1;		
        #endif		
		#endif
        break;
      }
	  case AUDIO_MICROPHONE_FM:
      {
        CHECK_STATUS(audio_mode_microphone_extra_gain_write, p_audio_mode->audio_microphone_setting.setting.fm.extra_gain)
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.fm.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.fm.output_bias)
        break;
      }
	  case AUDIO_MICROPHONE_FM_MONO:
      {
        CHECK_STATUS(audio_mode_microphone_extra_gain_write, p_audio_mode->audio_microphone_setting.setting.fm_mono.extra_gain)
        CHECK_STATUS(audio_mode_microphone_gain_write, p_audio_mode->audio_microphone_setting.setting.fm_mono.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_write, p_audio_mode->audio_microphone_setting.setting.fm_mono.output_bias)
        break;
      }
    }
#endif
    /* Voice speaker mode */
    CHECK_STATUS(audio_mode_speaker_mode_write, p_audio_mode->audio_speaker_setting.mode)
    CHECK_STATUS(audio_mode_stereo_speaker_mode_write, p_audio_mode->audio_speaker_setting.mode)

    /* Voice speaker setting */
    switch(p_audio_mode->audio_speaker_setting.mode)
    {
      case AUDIO_SPEAKER_HANDHELD:
      {
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
        CHECK_STATUS(audio_mode_speaker_highpass_filter_write, p_audio_mode->audio_speaker_setting.setting.handheld.audio_highpass_filter)
      #endif
      #if (ANLG_FAM == 11)
	  CHECK_STATUS(audio_mode_speaker_extra_gain_write, p_audio_mode->audio_speaker_setting.setting.handheld.extra_gain)
        /* AGC_DL of the speaker */
        #if (L1_AGC_DL == 1)
          CHECK_STATUS(audio_mode_agc_dl_write, p_audio_mode->audio_speaker_setting.setting.handheld.agc_dl);
          *message_to_confirm += 1;		
        #endif	  
	  #endif
        CHECK_STATUS(audio_mode_speaker_filter_write, p_audio_mode->audio_speaker_setting.setting.handheld.audio_filter)
		CHECK_STATUS(audio_mode_speaker_gain_write, p_audio_mode->audio_speaker_setting.setting.handheld.gain)
		#if (PSP_STANDALONE != 1)
        #if (L1_IIR == 0)
          data_speaker = &(p_audio_mode->audio_speaker_setting.setting.handheld.fir);
        #endif
        #if (L1_IIR == 1 || L1_IIR == 2)
          CHECK_STATUS(audio_mode_speaker_iir_write, p_audio_mode->audio_speaker_setting.setting.handheld.iir)
          *message_to_confirm += 1;
        #endif
        #if (L1_DRC == 1)
          CHECK_STATUS(audio_mode_speaker_drc_write, p_audio_mode->audio_speaker_setting.setting.handheld.drc)
          *message_to_confirm += 1;
        #endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_write, p_audio_mode->audio_speaker_setting.setting.handheld.limiter)
          *message_to_confirm += 1;
        #endif
		#endif
        break;
      }
      case AUDIO_SPEAKER_HANDFREE:
#if (ANLG_FAM == 11)
      case AUDIO_SPEAKER_HANDFREE_CLASS_D:
#endif
      {
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
        CHECK_STATUS(audio_mode_speaker_highpass_filter_write, p_audio_mode->audio_speaker_setting.setting.handfree.audio_highpass_filter)
      #endif
      #if (ANLG_FAM == 11)
	  CHECK_STATUS(audio_mode_speaker_extra_gain_write, p_audio_mode->audio_speaker_setting.setting.handheld.extra_gain)
        /* AGC_DL of the speaker */
        #if (L1_AGC_DL == 1)
          CHECK_STATUS(audio_mode_agc_dl_write, p_audio_mode->audio_speaker_setting.setting.handfree.agc_dl);
          *message_to_confirm += 1;		
        #endif	  
	  #endif	  
        CHECK_STATUS(audio_mode_speaker_filter_write, p_audio_mode->audio_speaker_setting.setting.handfree.audio_filter)
	  CHECK_STATUS(audio_mode_speaker_gain_write, p_audio_mode->audio_speaker_setting.setting.handfree.gain)
	#if (PSP_STANDALONE != 1)
        #if (L1_IIR == 0)
          data_speaker = &(p_audio_mode->audio_speaker_setting.setting.handfree.fir);
        #endif
        #if (L1_IIR == 1 || L1_IIR == 2)
          CHECK_STATUS(audio_mode_speaker_iir_write, p_audio_mode->audio_speaker_setting.setting.handfree.iir)
          *message_to_confirm += 1;
        #endif
	 #if (L1_DRC == 1)
          CHECK_STATUS(audio_mode_speaker_drc_write, p_audio_mode->audio_speaker_setting.setting.handfree.drc)
          *message_to_confirm += 1;
        #endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_write, p_audio_mode->audio_speaker_setting.setting.handfree.limiter)
          *message_to_confirm += 1;
        #endif
		#endif
        break;
      }
      case AUDIO_SPEAKER_HEADSET:
      {
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
        CHECK_STATUS(audio_mode_speaker_highpass_filter_write, p_audio_mode->audio_speaker_setting.setting.headset.audio_highpass_filter)
      #endif
        CHECK_STATUS(audio_mode_speaker_filter_write, p_audio_mode->audio_speaker_setting.setting.headset.audio_filter)
		CHECK_STATUS(audio_mode_speaker_gain_write, p_audio_mode->audio_speaker_setting.setting.headset.gain)
		#if (PSP_STANDALONE != 1)
        #if (L1_IIR == 0)
          data_speaker = &(p_audio_mode->audio_speaker_setting.setting.headset.fir);
        #endif
        #if (L1_IIR == 1 || L1_IIR == 2)
          CHECK_STATUS(audio_mode_speaker_iir_write, p_audio_mode->audio_speaker_setting.setting.headset.iir)
          *message_to_confirm += 1;
        #endif
	 #if (L1_DRC == 1)
          CHECK_STATUS(audio_mode_speaker_drc_write, p_audio_mode->audio_speaker_setting.setting.headset.drc)
          *message_to_confirm += 1;
        #endif
		#if ( ANLG_FAM == 11 )	
          /* AGC_DL of the speaker */
          #if (L1_AGC_DL == 1)
            CHECK_STATUS(audio_mode_agc_dl_write, p_audio_mode->audio_speaker_setting.setting.headset.agc_dl);
            *message_to_confirm += 1;		  
          #endif		
		#endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_write, p_audio_mode->audio_speaker_setting.setting.headset.limiter)
          *message_to_confirm += 1;
        #endif
		#endif
        break;
      }

#if (ANLG_FAM != 11)
	  case AUDIO_SPEAKER_BUZZER:
      {
        CHECK_STATUS(audio_mode_speaker_buzzer_write, p_audio_mode->audio_speaker_setting.setting.buzzer.activate)
        break;
      }
      case AUDIO_SPEAKER_HANDHELD_HANDFREE:
      {
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
        CHECK_STATUS(audio_mode_speaker_highpass_filter_write, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.audio_highpass_filter)
      #endif
        CHECK_STATUS(audio_mode_speaker_filter_write, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.audio_filter)
        #if (L1_IIR == 0)
          data_speaker = &(p_audio_mode->audio_speaker_setting.setting.handheld_handfree.fir);
        #endif
        CHECK_STATUS(audio_mode_speaker_gain_write, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.gain)
        #if (L1_IIR == 1 || L1_IIR == 2)
          CHECK_STATUS(audio_mode_speaker_iir_write, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.iir)
          *message_to_confirm += 1;
        #endif
	 #if (L1_DRC == 1)
          CHECK_STATUS(audio_mode_speaker_drc_write, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.drc)
          *message_to_confirm += 1;
        #endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_write, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.limiter)
          *message_to_confirm += 1;
        #endif
        break;
      }
      case AUDIO_SPEAKER_HANDHELD_8OHM:
      {
        CHECK_STATUS(audio_mode_speaker_highpass_filter_write, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.audio_highpass_filter)
        CHECK_STATUS(audio_mode_speaker_filter_write, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.audio_filter)
        #if (L1_IIR == 0)
          data_speaker = &(p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.fir);
        #endif
        CHECK_STATUS(audio_mode_speaker_extra_gain_write, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.extra_gain)
        CHECK_STATUS(audio_mode_speaker_gain_write, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.gain)
        #if (L1_IIR == 1 || L1_IIR == 2)
          CHECK_STATUS(audio_mode_speaker_iir_write, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.iir)
          *message_to_confirm += 1;
        #endif
	 #if (L1_DRC == 1)
          CHECK_STATUS(audio_mode_speaker_drc_write, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.drc)
          *message_to_confirm += 1;
        #endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_write, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.limiter)
          *message_to_confirm += 1;
        #endif
        break;
      }
#else
      case AUDIO_SPEAKER_AUX:
      {
        CHECK_STATUS(audio_mode_speaker_highpass_filter_write, p_audio_mode->audio_speaker_setting.setting.aux.audio_highpass_filter)
        CHECK_STATUS(audio_mode_speaker_filter_write, p_audio_mode->audio_speaker_setting.setting.aux.audio_filter)
        CHECK_STATUS(audio_mode_speaker_gain_write, p_audio_mode->audio_speaker_setting.setting.aux.gain)
		#if (PSP_STANDALONE != 1)
		#if (L1_IIR == 0)
          data_speaker = &(p_audio_mode->audio_speaker_setting.setting.aux.fir);
        #endif
        #if (L1_IIR == 1 || L1_IIR == 2)
          CHECK_STATUS(audio_mode_speaker_iir_write, p_audio_mode->audio_speaker_setting.setting.aux.iir)
          *message_to_confirm += 1;
        #endif
	 #if (L1_DRC == 1)
          CHECK_STATUS(audio_mode_speaker_drc_write, p_audio_mode->audio_speaker_setting.setting.aux.drc)
          *message_to_confirm += 1;
        #endif
        /* AGC_DL of the speaker */
        #if (L1_AGC_DL == 1)
          CHECK_STATUS(audio_mode_agc_dl_write, p_audio_mode->audio_speaker_setting.setting.aux.agc_dl);
          *message_to_confirm += 1;		
        #endif		
		
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_write, p_audio_mode->audio_speaker_setting.setting.aux.limiter)
          *message_to_confirm += 1;
        #endif
		#endif
        break;
      }

      case AUDIO_SPEAKER_CARKIT:
      {
        CHECK_STATUS(audio_mode_speaker_highpass_filter_write, p_audio_mode->audio_speaker_setting.setting.carkit.audio_highpass_filter)
        CHECK_STATUS(audio_mode_speaker_filter_write, p_audio_mode->audio_speaker_setting.setting.carkit.audio_filter)
        CHECK_STATUS(audio_mode_speaker_gain_write, p_audio_mode->audio_speaker_setting.setting.carkit.gain)
		#if (PSP_STANDALONE != 1)
		#if (L1_IIR == 0)
          data_speaker = &(p_audio_mode->audio_speaker_setting.setting.carkit.fir);
        #endif
        #if (L1_IIR == 1 || L1_IIR == 2)
          CHECK_STATUS(audio_mode_speaker_iir_write, p_audio_mode->audio_speaker_setting.setting.carkit.iir)
          *message_to_confirm += 1;
        #endif
	 #if (L1_DRC == 1)
          CHECK_STATUS(audio_mode_speaker_drc_write, p_audio_mode->audio_speaker_setting.setting.carkit.drc)
          *message_to_confirm += 1;
        #endif
        /* AGC_DL of the speaker */
        #if (L1_AGC_DL == 1)
          CHECK_STATUS(audio_mode_agc_dl_write, p_audio_mode->audio_speaker_setting.setting.carkit.agc_dl);
          *message_to_confirm += 1;		
        #endif		
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_write, p_audio_mode->audio_speaker_setting.setting.carkit.limiter)
          *message_to_confirm += 1;
        #endif
		#endif
        break;
      }
#endif
    }


  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
    /* Audio speaker setting */
    switch(p_audio_mode->audio_stereo_speaker_setting.mode)
    {
      case AUDIO_STEREO_SPEAKER_HEADPHONE:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_stereo_mono_write,
          p_audio_mode->audio_stereo_speaker_setting.setting.headphone.stereo_mono)
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_write,
          p_audio_mode->audio_stereo_speaker_setting.setting.headphone.sampling_frequency)
      }
      break;
      case AUDIO_STEREO_SPEAKER_HANDHELD:
//TISH patch for OMAPS00102027
  	case AUDIO_SPEAKER_INVERTED_VOICE:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_write,
          p_audio_mode->audio_stereo_speaker_setting.setting.handheld.sampling_frequency)
      }
      break;

      case AUDIO_STEREO_SPEAKER_HANDFREE:
//TISH patch for OMAPS00102027
      case AUDIO_SPEAKER_HANDFREE_CLASS_D:	  	
      {
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_write,
          p_audio_mode->audio_stereo_speaker_setting.setting.handfree.sampling_frequency)
        break;
      }
#if (ANLG_FAM != 11)
      case AUDIO_STEREO_SPEAKER_HANDHELD_8OHM:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_write,
          p_audio_mode->audio_stereo_speaker_setting.setting.handheld_8ohm.sampling_frequency)
      }
      break;
#else
      case AUDIO_STEREO_SPEAKER_AUX:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_write,
          p_audio_mode->audio_stereo_speaker_setting.setting.aux.sampling_frequency)
      }
      break;
	  case AUDIO_STEREO_SPEAKER_CARKIT:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_stereo_mono_write,
          p_audio_mode->audio_stereo_speaker_setting.setting.carkit.stereo_mono)
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_write,
          p_audio_mode->audio_stereo_speaker_setting.setting.carkit.sampling_frequency)
      }
#endif
  #endif
    } // switch(p_audio_mode->audio_stereo_speaker_setting.mode)
 #endif // AUDIO_DSP_FEATURES == 1

#if (PSP_STANDALONE != 1)
  /* Audio speaker microphone loop setting */
  CHECK_STATUS(audio_mode_aec_write, p_audio_mode->audio_microphone_speaker_loop_setting.aec)
  /* confirm the AEC message */
  *message_to_confirm += 1;
#endif

  CHECK_STATUS(audio_mode_sidetone_gain_write, p_audio_mode->audio_microphone_speaker_loop_setting.sidetone_gain)

#if (PSP_STANDALONE != 1)
 if ((p_audio_mode->audio_microphone_setting.mode != AUDIO_MICROPHONE_FM) && 
 	(p_audio_mode->audio_microphone_setting.mode !=  AUDIO_MICROPHONE_FM_MONO))

 {
 	 /* Set the speaker and microphone FIR */
	 if ( (audio_mode_speaker_microphone_fir_write(data_speaker, data_microphone))
	 	== AUDIO_ERROR )
	 {
	    return(AUDIO_ERROR);
         }
          /* confirm the FIR message */
         *message_to_confirm += 1;
 }
#endif

#if (PSP_STANDALONE != 1)
 #if (AUDIO_DSP_ONOFF)
   CHECK_STATUS(audio_mode_onoff_write,
         p_audio_mode->audio_microphone_speaker_setting.audio_onoff)
   /* confirm the AUDIO on off message */
   *message_to_confirm += 1;
 #endif
#endif
    return(AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_volume_set                                         */
  /*                                                                              */
  /*    Purpose:  This function sets the audio volume.                            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio volume structure.                                               */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_volume_set(T_AUDIO_SPEAKER_LEVEL *p_audio_volume)
  {
    CHECK_STATUS(audio_mode_speaker_volume_write, *p_audio_volume)

    return(AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_stereo_volume_set                                  */
  /*                                                                              */
  /*    Purpose:  This function sets the stereo audio volume.                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Audio volume structure.                                               */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
  T_AUDIO_RET audio_stereo_volume_set(T_AUDIO_STEREO_SPEAKER_LEVEL *p_audio_volume)
  {
    CHECK_STATUS(audio_mode_stereo_speaker_volume_write, *p_audio_volume)

    return(AUDIO_OK);
  }
#endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_load_manager                                  */
  /*                                                                              */
  /*    Purpose:  This function manages the audio mode load services.             */
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

  void audio_mode_load_manager (T_RV_HDR *p_message)
  {
    T_AUDIO_SPEAKER_LEVEL audio_volume;
  #if ((ANLG_FAM ==3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
    T_AUDIO_STEREO_SPEAKER_LEVEL audio_stereo_volume;
  #endif
    T_RVF_MB_STATUS       mb_status;
    UINT8                 message_to_confirm;
    #ifdef _WINDOWS
      INT8  *p_read, *p_write;
      UINT8 i;
    #endif

    switch (p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state)
    {
      case AUDIO_MODE_LOAD_IDLE:
      {
        /* allocate the buffer for the current Audio mode */
        mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                                 sizeof (T_AUDIO_MODE),
                                 (T_RVF_BUFFER **) (&(p_audio_gbl_var->audio_mode_var.audio_mode_load_var.p_audio_mode)));
        /* If insufficient resources, then report a memory error and abort.               */
        if (mb_status == RVF_RED)
        {
          AUDIO_SEND_TRACE("AUDIO MODE LOAD: not enough memory to allocate the audio mode buffer", RV_TRACE_LEVEL_ERROR);
          audio_mode_load_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path);
         return;
        }

        #ifdef _WINDOWS
          #if ((AUDIO_REGR == SW_COMPILED) || (AUDIO_MISC == SW_COMPILED))
            p_read = (INT8 *)(&(p_audio_test->audio_mode_1));
            p_write = (INT8 *)(p_audio_gbl_var->audio_mode_var.audio_mode_load_var.p_audio_mode);

            for (i=0; i<sizeof(T_AUDIO_MODE); i++)
            {
              *p_write++ = *p_read++;
            }
          #endif
        #else
          /* Load the audio mode structure from the FFS */

#if(AS_RFS_API == 1)
			if ( rfs_read (((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_ffs_fd,
                         (void *)(p_audio_gbl_var->audio_mode_var.audio_mode_load_var.p_audio_mode),
                         sizeof(T_AUDIO_MODE)) < RFS_EOK )
#else
          if ( ffs_read (((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_ffs_fd,
                         (void *)(p_audio_gbl_var->audio_mode_var.audio_mode_load_var.p_audio_mode),
                         sizeof(T_AUDIO_MODE)) < EFFS_OK )
#endif
          {
            AUDIO_SEND_TRACE("AUDIO MODE LOAD: impossible to load the current audio mode", RV_TRACE_LEVEL_ERROR);

            /* Close the files */
#if(AS_RFS_API == 1)
			rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_ffs_fd);
            rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#else
			ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_ffs_fd);
            ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#endif
          #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
#if(AS_RFS_API == 1)
			rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
		  ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif
          #endif

            /* free the audio mode buffer */
            rvf_free_buf((T_RVF_BUFFER *)(p_audio_gbl_var->audio_mode_var.audio_mode_load_var.p_audio_mode));

            /* send the status message */
            audio_mode_load_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path);
            return;
          }
        #endif

        /* Set the audio mode structure */
        if ( (audio_mode_set((p_audio_gbl_var->audio_mode_var.audio_mode_load_var.p_audio_mode),
              &message_to_confirm)) == AUDIO_ERROR)
        {
          AUDIO_SEND_TRACE("AUDIO MODE LOAD: error in the the audio mode set function", RV_TRACE_LEVEL_ERROR);
          /* free the audio mode buffer */
          rvf_free_buf((T_RVF_BUFFER *)(p_audio_gbl_var->audio_mode_var.audio_mode_load_var.p_audio_mode));

          #ifndef _WINDOWS
            /* Close the files */
#if(AS_RFS_API == 1)
			rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_ffs_fd);
            rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#else
		    ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_ffs_fd);
            ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#endif
          #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
#if(AS_RFS_API == 1)
			rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
		  ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif
          #endif
          #endif

          /* send the status message */
          audio_mode_load_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path);
          return;
        }

        /* Calculate the number of confirmation message to receive */
        p_audio_gbl_var->audio_mode_var.audio_mode_load_var.number_of_message = message_to_confirm;

        #ifndef _WINDOWS
          /* Close the file */
#if(AS_RFS_API == 1)
		rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_ffs_fd);
#else
		ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_ffs_fd);
#endif
        #endif

       #if (AUDIO_DSP_FEATURES == 0)
        #ifndef _WINDOWS
          /* Load the audio speaker volume structure from the FFS */
#if(AS_RFS_API == 1)
			if ( (rfs_read (((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd,
                          (void *)(&audio_volume),
                          sizeof(T_AUDIO_SPEAKER_LEVEL))) < RFS_EOK )
#else
          if ( (ffs_read (((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd,
                          (void *)(&audio_volume),
                          sizeof(T_AUDIO_SPEAKER_LEVEL))) < EFFS_OK )
#endif
          {
            AUDIO_SEND_TRACE("AUDIO MODE LOAD: impossible to load the current speaker volume", RV_TRACE_LEVEL_ERROR);

            /* Close the file */
#if(AS_RFS_API == 1)
			rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#else
			ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#endif

            /* send the status message */
            audio_mode_load_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path);
            return;
          }
        #else
          #if ((AUDIO_REGR == SW_COMPILED) || (AUDIO_MISC == SW_COMPILED))
            audio_volume.audio_speaker_level = p_audio_test->speaker_volume_1.audio_speaker_level;
          #endif
        #endif

        /* Fill the audio volume structure */
        if ( (audio_volume_set(&audio_volume)) == AUDIO_ERROR)
        {
          AUDIO_SEND_TRACE("AUDIO MODE LOAD: error in the the audio speaker volume set function", RV_TRACE_LEVEL_ERROR);
          #ifndef _WINDOWS
            /* Close the file */
#if(AS_RFS_API == 1)
			rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#else
		  ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#endif
          #endif

          /* send the status message */
          audio_mode_load_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path);
          return;
        }
        #ifndef _WINDOWS
          /* Close the file */
#if(AS_RFS_API == 1)
		rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#else
		ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#endif
        #endif

      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
        /* Load the audio speaker volume structure from the FFS */
#if(AS_RFS_API == 1)
		if ( (rfs_read (((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd,
                        (void *)(&audio_stereo_volume),
                        sizeof(T_AUDIO_STEREO_SPEAKER_LEVEL))) < RFS_EOK )
#else
        if ( (ffs_read (((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd,
                        (void *)(&audio_stereo_volume),
                        sizeof(T_AUDIO_STEREO_SPEAKER_LEVEL))) < EFFS_OK )
#endif
        {
          AUDIO_SEND_TRACE("AUDIO MODE LOAD: impossible to load the current stereo speaker volume", RV_TRACE_LEVEL_ERROR);

          /* Close the file */
#if(AS_RFS_API == 1)
			rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
		  ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif

          /* send the status message */
          audio_mode_load_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path);
          return;
        }
        /* Fill the audio stereo volume structure */
        if ( (audio_stereo_volume_set(&audio_stereo_volume)) == AUDIO_ERROR)
        {
          AUDIO_SEND_TRACE("AUDIO MODE LOAD: error in the the audio stereo speaker volume set function", RV_TRACE_LEVEL_ERROR);
#if(AS_RFS_API == 1)
			rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#else
		  ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
#endif

          /* send the status message */
          audio_mode_load_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path);
          return;
        }
#if(AS_RFS_API == 1)
		rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
		ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif
      #endif // #if (ANLG_FAM == 3)

     #else // AUDIO_DSP_FEATURES == 1
#if(AS_RFS_API == 1)
		rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
        rfs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
	    ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_volume_ffs_fd);
        ffs_close(((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif
     #endif // AUDIO_DSP_FEATURES == 0

        /* Save the return path */
        p_audio_gbl_var->audio_mode_var.audio_mode_load_var.return_path.callback_func =
          ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path.callback_func;
        p_audio_gbl_var->audio_mode_var.audio_mode_load_var.return_path.addr_id =
          ((T_AUDIO_MODE_LOAD_REQ *)p_message)->return_path.addr_id;

        /* Save the name of the volume path name */
#if(AS_RFS_API == 1)
		wstrcpy(p_audio_gbl_var->audio_mode_var.audio_mode_path_name,
              ((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_mode_path_name);
#else
        strcpy(p_audio_gbl_var->audio_mode_var.audio_mode_path_name,
              ((T_AUDIO_MODE_LOAD_REQ *)p_message)->audio_mode_path_name);
#endif

        /* change the state */
        p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state =
          AUDIO_MODE_LOAD_WAIT_MESSAGE_CONF;
        break;
      }
      case AUDIO_MODE_LOAD_WAIT_MESSAGE_CONF:
      {
        p_audio_gbl_var->audio_mode_var.audio_mode_load_var.number_of_message--;

        if (p_audio_gbl_var->audio_mode_var.audio_mode_load_var.number_of_message == 0)
        {
          /* free the audio mode buffer */
          rvf_free_buf((T_RVF_BUFFER *)(p_audio_gbl_var->audio_mode_var.audio_mode_load_var.p_audio_mode));

          /* send the status message */
          audio_mode_load_send_status (AUDIO_OK,
            p_audio_gbl_var->audio_mode_var.audio_mode_load_var.return_path);

          /* change the state */
          p_audio_gbl_var->audio_mode_var.audio_mode_load_var.state =
            AUDIO_MODE_LOAD_IDLE;
        }
        break;
      }
    }
  }
#endif /* RVM_AUDIO_MAIN_SWE */

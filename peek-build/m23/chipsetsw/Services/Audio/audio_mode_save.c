/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_mode_save.c                                           */
/*                                                                          */
/*  Purpose:  This file contains all the functions used for audio mode      */
/*            save services.                                                */
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
  #include "audio/audio_ffs_i.h"
#endif
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

#if (ANLG_FAM == 11)
    #include "types.h"
    #include "bspTwl3029_I2c.h"
    #include "bspTwl3029_Audio.h"
#endif

  #ifdef _WINDOWS
    #include "audio/tests/audio_test.h"
  #endif

  /* external functions */
  /* read */
 #if (AUDIO_DSP_FEATURES == 1)
  extern T_AUDIO_RET audio_mode_volume_speed_read                      (INT16  *data);
  extern T_AUDIO_RET audio_mode_microphone_gain_read                   (INT16  *data);
  extern T_AUDIO_RET audio_mode_speaker_gain_read                      (INT16  *data);
  extern T_AUDIO_RET audio_mode_sidetone_gain_read                     (INT16  *data);
 #else // AUDIO_DSP_FEATURES == 0
  extern T_AUDIO_RET audio_mode_microphone_mode_read          (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_gain_read                   (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_extra_gain_read    (INT8  *data);
  extern T_AUDIO_RET audio_mode_microphone_output_bias_read   (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_mode_read             (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_gain_read                      (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_extra_gain_read       (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_highpass_filter_read  (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_filter_read           (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_buzzer_read           (INT8  *data);
  extern T_AUDIO_RET audio_mode_sidetone_gain_read                     (INT8  *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_mode_read          (INT8  *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_stereo_mono_read   (INT8  *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_sampling_freq_read (INT8  *data);
  extern T_AUDIO_RET audio_mode_speaker_volume_read           (T_AUDIO_SPEAKER_LEVEL *data);
  extern T_AUDIO_RET audio_mode_stereo_speaker_volume_read    (T_AUDIO_STEREO_SPEAKER_LEVEL *data);
 #endif
 #if(AS_FM_RADIO==1)
 extern T_AUDIO_RET audio_mode_fm_mode_read(UINT8 *data);
 #endif
  extern T_AUDIO_RET audio_mode_voice_path_read               (T_AUDIO_VOICE_PATH_SETTING *data);
  extern T_AUDIO_RET audio_mode_microphone_fir_read           (T_AUDIO_FIR_COEF *data);
  extern T_AUDIO_RET audio_mode_speaker_fir_read              (T_AUDIO_FIR_COEF *data);
  #if (L1_AEC == 1) 
  extern T_AUDIO_RET audio_mode_aec_read                      (T_AUDIO_AEC_CFG *data);
  #endif
  #if (L1_AEC == 2) 
  extern T_AUDIO_RET audio_mode_aec_read                               (T_AUDIO_AQI_AEC_CFG *data);
  #endif
  #if (L1_ANR == 1)
    extern T_AUDIO_RET audio_mode_microphone_anr_read         (T_AUDIO_ANR_CFG *data);
  #endif
  #if (L1_ANR == 2)
    extern T_AUDIO_RET audio_mode_microphone_anr_read         (T_AUDIO_AQI_ANR_CFG *data);
  #endif  
  #if (L1_ES == 1)
    extern T_AUDIO_RET audio_mode_microphone_es_read          (T_AUDIO_ES_CFG *data);
  #endif
  #if (L1_IIR == 1)
    extern T_AUDIO_RET audio_mode_speaker_iir_read            (T_AUDIO_IIR_CFG *data);
  #elif (L1_IIR == 2)
    extern T_AUDIO_RET audio_mode_speaker_iir_read            (T_AUDIO_IIR_DL_CFG *data);
  #endif
  #if (L1_DRC == 1)
    extern T_AUDIO_RET audio_mode_speaker_drc_read            (T_AUDIO_DRC_CFG *data);
  #endif
  #if (L1_LIMITER == 1)
    extern T_AUDIO_RET audio_mode_speaker_limiter_read        (T_AUDIO_LIMITER_CFG *data);
  #endif
 #if (AUDIO_DSP_ONOFF == 1)
  extern T_AUDIO_RET audio_mode_onoff_read                             (INT8  *data);
 #endif

  /* Define a macro to simplify the code */
  #define CHECK_STATUS(function, param)     if ( (function(&(param))) == AUDIO_ERROR ) \
                                            { \
                                              return(AUDIO_ERROR); \
                                            } \

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_save_send_status                              */
  /*                                                                              */
  /*    Purpose:  This function sends the audio mdoe save status to the entity.   */
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
  void audio_mode_save_send_status (T_AUDIO_RET status, T_RV_RETURN return_path)
  {
    T_AUDIO_SAVE_DONE *p_send_message = NULL;
    T_RVF_MB_STATUS mb_status = RVF_RED;

    while (mb_status == RVF_RED)
    {
      /* allocate the message buffer */
      mb_status = rvf_get_buf (p_audio_gbl_var->mb_external,
                               sizeof(T_AUDIO_SAVE_DONE),
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
    ((T_AUDIO_SAVE_DONE *)p_send_message)->os_hdr.msg_id =
      AUDIO_MODE_SAVE_DONE;

    /* fill the status parameters */
    ((T_AUDIO_SAVE_DONE *)p_send_message)->status = status;

    if (return_path.callback_func == NULL)
    {
      /* send the message to the entity */
      rvf_send_msg (return_path.addr_id,
                    p_send_message);
    }
    else
    {
      /* call the callback function */
      (*return_path.callback_func)((void *)(p_send_message));
	  rvf_free_buf((T_RVF_BUFFER *)p_send_message);
    }
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_get                                           */
  /*                                                                              */
  /*    Purpose:  This function fill the audio mode structure.                    */
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
  T_AUDIO_RET audio_mode_get(T_AUDIO_MODE *p_audio_mode)
  {

#if (PSP_STANDALONE != 1)
    /* Audio voice path */
    CHECK_STATUS(audio_mode_voice_path_read, p_audio_mode->audio_path_setting)
#endif
	 #if(AS_FM_RADIO==1)
	/* audio FM setting*/
    CHECK_STATUS(audio_mode_fm_mode_read,p_audio_mode->audio_fm_setting)
	#endif
  #if (AUDIO_DSP_FEATURES == 1)
    CHECK_STATUS(audio_mode_volume_speed_read,    p_audio_mode->audio_microphone_speaker_setting.volume_speed)
    CHECK_STATUS(audio_mode_speaker_gain_read,    p_audio_mode->audio_speaker_setting.speaker_gain)
    CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.micro_gain)

    CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.fir)
    CHECK_STATUS(audio_mode_speaker_fir_read, p_audio_mode->audio_speaker_setting.fir)

    /* Audio microphone ANR & ES */
    #if (L1_ANR == 1 || L1_ANR == 2)
        CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.anr)
    #endif
    #if (L1_ES == 1)
        CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.es)
    #endif

    /* Audio speaker IIR & Limiter */
    #if (L1_IIR == 1 || L1_IIR == 2)
       CHECK_STATUS(audio_mode_speaker_iir_read, p_audio_mode->audio_speaker_setting.iir)
    #endif
   /* Audio speaker DRC */
    #if (L1_DRC == 1)
       CHECK_STATUS(audio_mode_speaker_drc_read, p_audio_mode->audio_speaker_setting.iir)
    #endif
    /* Audio microphone AGC_UL & AGC_DL */
    #if (L1_AGC_UL == 1)
        CHECK_STATUS(audio_mode_agc_ul_read, p_audio_mode->audio_microphone_setting.agc_ul)
    #endif
    #if (L1_AGC_DL == 1)
        CHECK_STATUS(audio_mode_agc_dl_read, p_audio_mode->audio_microphone_setting.agc_dl)
    #endif	
    #if (L1_LIMITER == 1)
       CHECK_STATUS(audio_mode_speaker_limiter_read, p_audio_mode->audio_speaker_setting.limiter)
    #endif

  #else // AUDIO_DSP_FEATURES == 0

    /* Audio microphone mode */
    CHECK_STATUS(audio_mode_microphone_mode_read, p_audio_mode->audio_microphone_setting.mode)

#if (ANLG_FAM != 11)
    /* Audio microphone setting */
    switch (p_audio_mode->audio_microphone_setting.mode)
    {
      case AUDIO_MICROPHONE_HANDHELD:
      {
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.handheld.gain)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.handheld.fir)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.handheld.output_bias)
        #if (L1_ANR == 1)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.handheld.anr)
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.handheld.es)
        #endif
        break;
      }
      case AUDIO_MICROPHONE_HANDFREE:
      {
        CHECK_STATUS(audio_mode_microphone_extra_gain_read, p_audio_mode->audio_microphone_setting.setting.handfree.extra_gain)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.handfree.fir)
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.handfree.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.handfree.output_bias)
        #if (L1_ANR == 1)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.handfree.anr)
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.handfree.es)
        #endif

        break;
      }
      case AUDIO_MICROPHONE_HEADSET:
      {
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.headset.gain)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.headset.fir)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.headset.output_bias)
        #if (L1_ANR == 1)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.headset.anr)
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.headset.es)
        #endif

        break;
      }
      case AUDIO_MICROPHONE_HEADSET_DIFFERENTIAL:
      {
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.headset_diff.gain)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.headset_diff.fir)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.headset_diff.output_bias)
        #if (L1_ANR == 1)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.headset_diff.anr)
        #endif
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.headset_diff.es)
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
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.output_bias)
        #if (PSP_STANDALONE != 1)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.fir)
		#if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.anr)
        #endif
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_read, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.agc_ul)
        #endif		
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.handset_25_6db.es)
        #endif
		#endif
        break;
      }
      case AUDIO_MICROPHONE_HEADSET_4_9_DB:
      {
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.output_bias)
        #if (PSP_STANDALONE != 1)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.fir)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.anr)
        #endif
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_read, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.agc_ul)
        #endif		
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.headset_4_9db.es)
        #endif
		#endif
		break;
      }
	  case AUDIO_MICROPHONE_HEADSET_25_6DB:
      {
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.output_bias)
        #if (PSP_STANDALONE != 1)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.fir)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.anr)
        #endif
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_read, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.agc_ul)
        #endif		
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.headset_25_6db.es)
        #endif
    	#endif
        break;
      }
	  case AUDIO_MICROPHONE_HEADSET_18DB:
      {
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.headset_18db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.headset_18db.output_bias)
        #if (PSP_STANDALONE != 1)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.headset_18db.fir)
        
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.headset_18db.anr)
        #endif
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_read, p_audio_mode->audio_microphone_setting.setting.headset_18db.agc_ul)
        #endif		
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.headset_18db.es)
        #endif
		#endif
		break;
      }
	  case AUDIO_MICROPHONE_AUX_4_9DB:
      {
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.output_bias)
		#if (PSP_STANDALONE != 1)
		CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.fir)
		#if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.anr)
        #endif
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_read, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.agc_ul)
        #endif		
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.aux_4_9db.es)
        #endif
		#endif
		break;
      }
	  case AUDIO_MICROPHONE_AUX_28_2DB:
      {
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.output_bias)
        #if (PSP_STANDALONE != 1)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.fir)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.anr)
        #endif
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_read, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.agc_ul)
        #endif		
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.aux_28_2db.es)
        #endif
		#endif
		break;
      }
	  case AUDIO_MICROPHONE_CARKIT:
      {
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.carkit.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.carkit.output_bias)
        #if (PSP_STANDALONE != 1)
        CHECK_STATUS(audio_mode_microphone_fir_read, p_audio_mode->audio_microphone_setting.setting.carkit.fir)
        #if (L1_ANR == 1 || L1_ANR == 2)
          CHECK_STATUS(audio_mode_microphone_anr_read, p_audio_mode->audio_microphone_setting.setting.carkit.anr)
        #endif
        #if (L1_AGC_UL == 1)
          CHECK_STATUS(audio_mode_agc_ul_read, p_audio_mode->audio_microphone_setting.setting.carkit.agc_ul)
        #endif		
        #if (L1_ES == 1)
          CHECK_STATUS(audio_mode_microphone_es_read, p_audio_mode->audio_microphone_setting.setting.carkit.es)
        #endif
		#endif
		break;
      }
	  case AUDIO_MICROPHONE_FM:
      {
        CHECK_STATUS(audio_mode_microphone_extra_gain_read, p_audio_mode->audio_microphone_setting.setting.fm.extra_gain)
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.fm.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.fm.output_bias)
        break;
      }
	  case AUDIO_MICROPHONE_FM_MONO:
      {
        CHECK_STATUS(audio_mode_microphone_extra_gain_read, p_audio_mode->audio_microphone_setting.setting.fm_mono.extra_gain)
        CHECK_STATUS(audio_mode_microphone_gain_read, p_audio_mode->audio_microphone_setting.setting.fm_mono.gain)
        CHECK_STATUS(audio_mode_microphone_output_bias_read, p_audio_mode->audio_microphone_setting.setting.fm_mono.output_bias)
        break;
      }
    }
#endif
    /* Audio speaker mode */
    CHECK_STATUS(audio_mode_speaker_mode_read, p_audio_mode->audio_speaker_setting.mode)

    /* Audio speaker setting */
    switch(p_audio_mode->audio_speaker_setting.mode)
    {
      case AUDIO_SPEAKER_HANDHELD:
      {
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
        CHECK_STATUS(audio_mode_speaker_highpass_filter_read, p_audio_mode->audio_speaker_setting.setting.handheld.audio_highpass_filter)
      #endif
	  #if (ANLG_FAM == 11)
        CHECK_STATUS(audio_mode_speaker_extra_gain_read, p_audio_mode->audio_speaker_setting.setting.handheld.extra_gain)
        #if (L1_AGC_DL == 1)
          CHECK_STATUS(audio_mode_agc_dl_read, p_audio_mode->audio_speaker_setting.setting.handheld.agc_dl)
        #endif        
	  #endif
        CHECK_STATUS(audio_mode_speaker_filter_read, p_audio_mode->audio_speaker_setting.setting.handheld.audio_filter)
        #if (PSP_STANDALONE != 1)
        #if (L1_IIR == 0)
          CHECK_STATUS(audio_mode_speaker_fir_read, p_audio_mode->audio_speaker_setting.setting.handheld.fir)
        #endif
        CHECK_STATUS(audio_mode_speaker_gain_read, p_audio_mode->audio_speaker_setting.setting.handheld.gain)
        #if (L1_IIR == 1 || L1_IIR == 2)
         CHECK_STATUS(audio_mode_speaker_iir_read, p_audio_mode->audio_speaker_setting.setting.handheld.iir)
        #endif
        #if (L1_DRC == 1)
         CHECK_STATUS(audio_mode_speaker_drc_read, p_audio_mode->audio_speaker_setting.setting.handheld.drc)
        #endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_read, p_audio_mode->audio_speaker_setting.setting.handheld.limiter)
        #endif
		#endif
        break;
      }
      case AUDIO_SPEAKER_HANDFREE:
      {
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
        CHECK_STATUS(audio_mode_speaker_highpass_filter_read, p_audio_mode->audio_speaker_setting.setting.handfree.audio_highpass_filter)
      #endif
	  #if (ANLG_FAM == 11)
        CHECK_STATUS(audio_mode_speaker_extra_gain_read, p_audio_mode->audio_speaker_setting.setting.handfree.extra_gain)	  
        #if (L1_AGC_DL == 1)
          CHECK_STATUS(audio_mode_agc_dl_read, p_audio_mode->audio_speaker_setting.setting.handfree.agc_dl)
        #endif        
	  #endif
        CHECK_STATUS(audio_mode_speaker_filter_read, p_audio_mode->audio_speaker_setting.setting.handfree.audio_filter)
        #if (PSP_STANDALONE != 1)
        #if (L1_IIR == 0)
          CHECK_STATUS(audio_mode_speaker_fir_read, p_audio_mode->audio_speaker_setting.setting.handfree.fir)
        #endif
        CHECK_STATUS(audio_mode_speaker_gain_read, p_audio_mode->audio_speaker_setting.setting.handfree.gain)
        #if (L1_IIR == 1 || L1_IIR == 2)
         CHECK_STATUS(audio_mode_speaker_iir_read, p_audio_mode->audio_speaker_setting.setting.handfree.iir)
        #endif
        #if (L1_DRC == 1)
         CHECK_STATUS(audio_mode_speaker_drc_read, p_audio_mode->audio_speaker_setting.setting.handfree.drc)
        #endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_read, p_audio_mode->audio_speaker_setting.setting.handfree.limiter)
        #endif
		#endif
        break;
      }
      case AUDIO_SPEAKER_HEADSET:
      {
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
        CHECK_STATUS(audio_mode_speaker_highpass_filter_read, p_audio_mode->audio_speaker_setting.setting.headset.audio_highpass_filter)
        #if (L1_AGC_DL == 1)
          CHECK_STATUS(audio_mode_agc_dl_read, p_audio_mode->audio_speaker_setting.setting.headset.agc_dl)
        #endif        
      #endif
        CHECK_STATUS(audio_mode_speaker_filter_read, p_audio_mode->audio_speaker_setting.setting.headset.audio_filter)
        #if (PSP_STANDALONE != 1)
        #if (L1_IIR == 0)
          CHECK_STATUS(audio_mode_speaker_fir_read, p_audio_mode->audio_speaker_setting.setting.headset.fir)
        #endif
        CHECK_STATUS(audio_mode_speaker_gain_read, p_audio_mode->audio_speaker_setting.setting.headset.gain)
        #if (L1_IIR == 1 || L1_IIR == 2)
         CHECK_STATUS(audio_mode_speaker_iir_read, p_audio_mode->audio_speaker_setting.setting.headset.iir)
        #endif
        #if (L1_DRC == 1)
         CHECK_STATUS(audio_mode_speaker_drc_read, p_audio_mode->audio_speaker_setting.setting.headset.drc)
        #endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_read, p_audio_mode->audio_speaker_setting.setting.headset.limiter)
        #endif
		#endif
        break;
      }
#if (ANLG_FAM != 11)
      case AUDIO_SPEAKER_BUZZER:
      {
        CHECK_STATUS(audio_mode_speaker_buzzer_read, p_audio_mode->audio_speaker_setting.setting.buzzer.activate)
        break;
      }
      case AUDIO_SPEAKER_HANDHELD_HANDFREE:
      {
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
        CHECK_STATUS(audio_mode_speaker_highpass_filter_read, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.audio_highpass_filter)
      #endif
        CHECK_STATUS(audio_mode_speaker_filter_read, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.audio_filter)
        #if (L1_IIR == 0)
         CHECK_STATUS(audio_mode_speaker_fir_read, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.fir)
        #endif
         CHECK_STATUS(audio_mode_speaker_gain_read, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.gain)
        #if (L1_IIR == 1 || L1_IIR == 2)
         CHECK_STATUS(audio_mode_speaker_iir_read, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.iir)
        #endif
        #if (L1_DRC == 1)
         CHECK_STATUS(audio_mode_speaker_drc_read, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.drc)
        #endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_read, p_audio_mode->audio_speaker_setting.setting.handheld_handfree.limiter)
        #endif
        break;
      }
      case AUDIO_SPEAKER_HANDHELD_8OHM:
      {
        CHECK_STATUS(audio_mode_speaker_highpass_filter_read, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.audio_highpass_filter)
        CHECK_STATUS(audio_mode_speaker_filter_read, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.audio_filter)
        #if (L1_IIR == 0)
          CHECK_STATUS(audio_mode_speaker_fir_read, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.fir)
        #endif
        CHECK_STATUS(audio_mode_speaker_extra_gain_read, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.extra_gain)
        CHECK_STATUS(audio_mode_speaker_gain_read, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.gain)
        #if (L1_IIR == 1 || L1_IIR == 2)
         CHECK_STATUS(audio_mode_speaker_iir_read, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.iir)
        #endif
        #if (L1_DRC == 1)
         CHECK_STATUS(audio_mode_speaker_drc_read, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.drc)
        #endif
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_read, p_audio_mode->audio_speaker_setting.setting.handheld_8ohm.limiter)
        #endif
        break;
      }
#else
      case AUDIO_SPEAKER_AUX:
      {
        CHECK_STATUS(audio_mode_speaker_highpass_filter_read, p_audio_mode->audio_speaker_setting.setting.aux.audio_highpass_filter)
        CHECK_STATUS(audio_mode_speaker_filter_read, p_audio_mode->audio_speaker_setting.setting.aux.audio_filter)
        #if (PSP_STANDALONE != 1)
        #if (L1_IIR == 0)
          CHECK_STATUS(audio_mode_speaker_fir_read, p_audio_mode->audio_speaker_setting.setting.aux.fir)
        #endif
        CHECK_STATUS(audio_mode_speaker_gain_read, p_audio_mode->audio_speaker_setting.setting.aux.gain)
        #if (L1_IIR == 1 || L1_IIR == 2)
          CHECK_STATUS(audio_mode_speaker_iir_read, p_audio_mode->audio_speaker_setting.setting.aux.iir)
        #endif
        #if (L1_DRC == 1)
          CHECK_STATUS(audio_mode_speaker_drc_read, p_audio_mode->audio_speaker_setting.setting.aux.drc)
        #endif
        #if (L1_AGC_DL == 1)
          CHECK_STATUS(audio_mode_agc_dl_read, p_audio_mode->audio_speaker_setting.setting.aux.agc_dl)
        #endif		
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_read, p_audio_mode->audio_speaker_setting.setting.aux.limiter)
        #endif
		#endif
        break;
      }

      case AUDIO_SPEAKER_CARKIT:
      {
        CHECK_STATUS(audio_mode_speaker_highpass_filter_read, p_audio_mode->audio_speaker_setting.setting.carkit.audio_highpass_filter)
        CHECK_STATUS(audio_mode_speaker_filter_read, p_audio_mode->audio_speaker_setting.setting.carkit.audio_filter)
        #if (PSP_STANDALONE != 1)
        #if (L1_IIR == 0)
          CHECK_STATUS(audio_mode_speaker_fir_read, p_audio_mode->audio_speaker_setting.setting.carkit.fir)
        #endif
        CHECK_STATUS(audio_mode_speaker_gain_read, p_audio_mode->audio_speaker_setting.setting.carkit.gain)
        #if (L1_IIR == 1 || L1_IIR == 2)
          CHECK_STATUS(audio_mode_speaker_iir_read, p_audio_mode->audio_speaker_setting.setting.carkit.iir)
        #endif
	#if (L1_DRC == 1)
          CHECK_STATUS(audio_mode_speaker_drc_read, p_audio_mode->audio_speaker_setting.setting.carkit.drc)
        #endif
        #if (L1_AGC_DL == 1)
          CHECK_STATUS(audio_mode_agc_dl_read, p_audio_mode->audio_speaker_setting.setting.carkit.agc_dl)
        #endif		
        #if (L1_LIMITER == 1)
          CHECK_STATUS(audio_mode_speaker_limiter_read, p_audio_mode->audio_speaker_setting.setting.carkit.limiter)
        #endif
		#endif
        break;
      }
#endif
    }

    #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))

    CHECK_STATUS(audio_mode_stereo_speaker_mode_read,  p_audio_mode->audio_stereo_speaker_setting.mode)

 
    /* Audio stereo speaker setting */
    switch(p_audio_mode->audio_stereo_speaker_setting.mode)
    {
      case AUDIO_STEREO_SPEAKER_HEADPHONE:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_stereo_mono_read,
          p_audio_mode->audio_stereo_speaker_setting.setting.headphone.stereo_mono)
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_read,
          p_audio_mode->audio_stereo_speaker_setting.setting.headphone.sampling_frequency)
      }
      break;
      case AUDIO_STEREO_SPEAKER_HANDHELD:
//TISH patch for OMAPS00102027
  	case AUDIO_SPEAKER_INVERTED_VOICE:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_read,
          p_audio_mode->audio_stereo_speaker_setting.setting.handheld.sampling_frequency)
      }
      break;
      case AUDIO_STEREO_SPEAKER_HANDFREE:
//TISH patch for OMAPS00102027
      case AUDIO_SPEAKER_HANDFREE_CLASS_D:	  	
      {
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_read,
          p_audio_mode->audio_stereo_speaker_setting.setting.handfree.sampling_frequency)
      }
      break;
#if (ANLG_FAM != 11)
      case AUDIO_STEREO_SPEAKER_HANDHELD_8OHM:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_read,
          p_audio_mode->audio_stereo_speaker_setting.setting.handheld_8ohm.sampling_frequency)
      }
      break;
#else
      case AUDIO_STEREO_SPEAKER_AUX:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_read,
          p_audio_mode->audio_stereo_speaker_setting.setting.aux.sampling_frequency)
      }
      break;
	  case AUDIO_STEREO_SPEAKER_CARKIT:
      {
        CHECK_STATUS(audio_mode_stereo_speaker_stereo_mono_read,
          p_audio_mode->audio_stereo_speaker_setting.setting.carkit.stereo_mono)
        CHECK_STATUS(audio_mode_stereo_speaker_sampling_freq_read,
          p_audio_mode->audio_stereo_speaker_setting.setting.carkit.sampling_frequency)
      }
#endif
    } // switch(p_audio_mode->audio_stereo_speaker_setting.mode)
  #endif

 #endif // AUDIO_DSP_FEATURES == 1

#if (PSP_STANDALONE != 1)
 /* Audio speaker microphone loop setting */
 CHECK_STATUS(audio_mode_aec_read, p_audio_mode->audio_microphone_speaker_loop_setting.aec)
#endif
 CHECK_STATUS(audio_mode_sidetone_gain_read, p_audio_mode->audio_microphone_speaker_loop_setting.sidetone_gain)

#if (PSP_STANDALONE != 1)
 #if (AUDIO_DSP_ONOFF)
   CHECK_STATUS(audio_mode_onoff_read,
         p_audio_mode->audio_microphone_speaker_setting.audio_onoff)
 #endif
#endif
    return(AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_volume_get                                         */
  /*                                                                              */
  /*    Purpose:  This function fill the audio volume structure.                  */
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
  T_AUDIO_RET audio_volume_get(T_AUDIO_SPEAKER_LEVEL *p_audio_volume)
  {
    CHECK_STATUS(audio_mode_speaker_volume_read, *p_audio_volume)

    return(AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_stereo_volume_get                                  */
  /*                                                                              */
  /*    Purpose:  This function fill the audio stereo volume structure.           */
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
#if ((ANLG_FAM ==3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
  T_AUDIO_RET audio_stereo_volume_get(T_AUDIO_STEREO_SPEAKER_LEVEL *p_audio_volume)
  {
    CHECK_STATUS(audio_mode_stereo_speaker_volume_read, *p_audio_volume)

    return(AUDIO_OK);
  }
#endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_save_manager                                  */
  /*                                                                              */
  /*    Purpose:  This function manage the audio mode save services.              */
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
  void audio_mode_save_manager (T_RV_HDR *p_message)
  {
    T_AUDIO_MODE *p_audio_mode;
    T_AUDIO_SPEAKER_LEVEL audio_volume;
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
    T_AUDIO_STEREO_SPEAKER_LEVEL audio_stereo_volume;
  #endif
    T_RVF_MB_STATUS       mb_status;

    #ifdef _WINDOWS
      INT8  *p_read, *p_write;
      UINT8 i;
    #endif
    /* allocate the buffer for the current Audio mode */
    mb_status = rvf_get_buf (p_audio_gbl_var->mb_internal,
                             sizeof (T_AUDIO_MODE),
                             (T_RVF_BUFFER **) (&p_audio_mode));
    /* If insufficient resources, then report a memory error and abort.               */
    if (mb_status == RVF_RED)
    {
      AUDIO_SEND_TRACE("AUDIO MODE SAVE: not enough memory to allocate the audio mode buffer", RV_TRACE_LEVEL_ERROR);
      audio_mode_save_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_SAVE_REQ *)p_message)->return_path);
      return;
    }

    /* Fill the audio mode structure */
    if ( (audio_mode_get(p_audio_mode)) == AUDIO_ERROR)
    {
      AUDIO_SEND_TRACE("AUDIO MODE SAVE: error in the the audio mode get function", RV_TRACE_LEVEL_ERROR);
      /* free the audio mode buffer */
      rvf_free_buf((T_RVF_BUFFER *)p_audio_mode);

      #ifndef _WINDOWS
        /* Close the files */
#if(AS_RFS_API == 1)
		rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_ffs_fd);
        rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#else
	    ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_ffs_fd);
        ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#endif
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
#if(AS_RFS_API == 1)
		rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
	    ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif
      #endif
      #endif

      /* send the status message */
      audio_mode_save_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_SAVE_REQ *)p_message)->return_path);
      return;
    }

    #ifndef _WINDOWS
      /* Save the audio mode structure to the FFS */
#if(AS_RFS_API == 1)
	  if ( (rfs_write (((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_ffs_fd,
                      p_audio_mode,
                      sizeof(T_AUDIO_MODE))) < RFS_EOK )
#else
	  if ( (ffs_write (((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_ffs_fd,
                      p_audio_mode,
                      sizeof(T_AUDIO_MODE))) < EFFS_OK )
#endif
      {
        AUDIO_SEND_TRACE("AUDIO MODE SAVE: impossible to save the current audio mode", RV_TRACE_LEVEL_ERROR);

        /* free the audio mode buffer */
        rvf_free_buf((T_RVF_BUFFER *)p_audio_mode);

        /* Close the files */
#if(AS_RFS_API == 1)
		rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_ffs_fd);
        rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#else
		ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_ffs_fd);
        ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#endif
      #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
#if(AS_RFS_API == 1)
	  rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
	  ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif
      #endif

        /* send the status message */
        audio_mode_save_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_SAVE_REQ *)p_message)->return_path);
        return;
      }

      /* Close the file */
#if(AS_RFS_API == 1)
	  rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_ffs_fd);
#else
	  ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_ffs_fd);
#endif
    #else
      #if ((AUDIO_REGR == SW_COMPILED) || (AUDIO_MISC == SW_COMPILED))
        p_read = (INT8 *)(p_audio_mode);
        p_write = (INT8 *)(&(p_audio_test->audio_mode_2));

        for (i=0; i<sizeof(T_AUDIO_MODE); i++)
        {
          *p_write++ = *p_read++;
        }
      #endif
    #endif

    /* free the audio mode buffer */
    rvf_free_buf((T_RVF_BUFFER *)p_audio_mode);

   #if (AUDIO_DSP_FEATURES == 0)
    /* Fill the audio volume structure */
    if ( (audio_volume_get(&audio_volume)) == AUDIO_ERROR)
    {
      AUDIO_SEND_TRACE("AUDIO MODE SAVE: error in the the audio speaker volume get function", RV_TRACE_LEVEL_ERROR);
      #ifndef _WINDOWS
        /* Close the file */
#if(AS_RFS_API == 1)
	  rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#else
	  ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#endif
      #endif

      /* send the status message */
      audio_mode_save_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_SAVE_REQ *)p_message)->return_path);
      return;
    }

    #ifndef _WINDOWS
      /* Save the audio speaker volume structure to the FFS */
#if(AS_RFS_API == 1)
	  if ( (rfs_write (((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd,
                      &audio_volume,
                      sizeof(T_AUDIO_SPEAKER_LEVEL))) < RFS_EOK )
#else
	  if ( (ffs_write (((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd,
                      &audio_volume,
                      sizeof(T_AUDIO_SPEAKER_LEVEL))) < EFFS_OK )
#endif
      {
        AUDIO_SEND_TRACE("AUDIO MODE SAVE: impossible to save the current speaker volume", RV_TRACE_LEVEL_ERROR);

        /* Close the files */
#if(AS_RFS_API == 1)
		rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#else
		ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#endif

        /* send the status message */
        audio_mode_save_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_SAVE_REQ *)p_message)->return_path);
        return;
      }

      /* Close the file */
#if(AS_RFS_API == 1)
	  rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#else
	  ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
#endif
    #else
      #if ((AUDIO_REGR == SW_COMPILED) || (AUDIO_MISC == SW_COMPILED))
        p_audio_test->speaker_volume_2.audio_speaker_level = audio_volume.audio_speaker_level;
      #endif
    #endif

  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
    /* Fill the audio volume structure */
    if ( (audio_stereo_volume_get(&audio_stereo_volume)) == AUDIO_ERROR)
    {
      AUDIO_SEND_TRACE("AUDIO MODE SAVE: error in the the audio stereo speaker volume get function", RV_TRACE_LEVEL_ERROR);
      #ifndef _WINDOWS
        /* Close the file */
#if(AS_RFS_API == 1)
	  rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
	  ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif
      #endif

      /* send the status message */
      audio_mode_save_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_SAVE_REQ *)p_message)->return_path);
      return;
    }

    /* Save the audio stereo speaker volume structure to the FFS */
#if(AS_RFS_API == 1)
	if ( (rfs_write (((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd,
                    &audio_stereo_volume,
                    sizeof(T_AUDIO_STEREO_SPEAKER_LEVEL))) < RFS_EOK )
#else
	if ( (ffs_write (((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd,
                    &audio_stereo_volume,
                    sizeof(T_AUDIO_STEREO_SPEAKER_LEVEL))) < EFFS_OK )
#endif
    {
      AUDIO_SEND_TRACE("AUDIO MODE SAVE: impossible to save the current stereo speaker volume", RV_TRACE_LEVEL_ERROR);

      /* Close the files */
#if(AS_RFS_API == 1)
	  rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
	  ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif

      /* send the status message */
      audio_mode_save_send_status (AUDIO_ERROR, ((T_AUDIO_MODE_SAVE_REQ *)p_message)->return_path);
      return;
    }

    /* Close the file */
#if(AS_RFS_API == 1)
	rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
	ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif
  #endif
 #else // AUDIO_DSP_FEATURES == 1
#if(AS_RFS_API == 1)
   rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
   rfs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#else
   ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_volume_ffs_fd);
   ffs_close(((T_AUDIO_MODE_SAVE_REQ *)p_message)->audio_stereo_volume_ffs_fd);
#endif
 #endif // AUDIO_DSP_FEATURES == 0

    /* send the status message */
    audio_mode_save_send_status (AUDIO_OK, ((T_AUDIO_MODE_SAVE_REQ *)p_message)->return_path);
  }
#endif /* RVM_AUDIO_MAIN_SWE */


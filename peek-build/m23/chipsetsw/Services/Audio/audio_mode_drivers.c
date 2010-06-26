/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_mode_drivers.c                                        */
/*                                                                          */
/*  Purpose:  This file contains all the drivers used for audio mode        */
/*            services.                                                     */
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
  #include "audio/audio_features_i.h"
  #include "audio/audio_ffs_i.h"
  #include "audio/audio_api.h"
  #include "audio/audio_structs_i.h"
  #include "audio/audio_error_hdlr_i.h"
  #include "audio/audio_var_i.h"
  #include "audio/audio_messages_i.h"
  #include "audio/audio_macro_i.h"
  #include "rvf/rvf_target.h"
  #include "audio/audio_const_i.h"
  #include "audio/audio_mode_i.h"

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
    #include "bspTwl3029_I2c.h"
    #include "types.h"
    #include "bspTwl3029_Audio.h"
  #endif

  /* write drivers */
  T_AUDIO_RET audio_mode_voice_path_write               (T_AUDIO_VOICE_PATH_SETTING *data,
                                                         UINT8 *message_to_confirm);
  T_AUDIO_RET audio_mode_microphone_mode_write          (INT8  *data);
  T_AUDIO_RET audio_mode_microphone_extra_gain_write    (INT8  *data);
  T_AUDIO_RET audio_mode_microphone_output_bias_write   (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_mode_write             (INT8  *data);
  #if(AS_FM_RADIO==1)
  T_AUDIO_RET audio_mode_fm_mode_write              (UINT8  *data); /*fm audio path*/
  #endif
  T_AUDIO_RET audio_mode_speaker_extra_gain_write       (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_filter_write           (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_highpass_filter_write  (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_buzzer_write           (INT8  *data);

  #if (L1_AEC == 1)
  T_AUDIO_RET audio_mode_aec_write                      (T_AUDIO_AEC_CFG *data);
  #endif
  #if (L1_AEC== 2)
      T_AUDIO_RET audio_mode_aec_write                      (T_AUDIO_AQI_AEC_CFG *data);
  #endif
  T_AUDIO_RET audio_mode_stereo_speaker_mode_write          (INT8  *data);
  T_AUDIO_RET audio_mode_stereo_speaker_stereo_mono_write   (INT8  *data);
  T_AUDIO_RET audio_mode_stereo_speaker_sampling_freq_write (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_volume_write           (T_AUDIO_SPEAKER_LEVEL *data);
  T_AUDIO_RET audio_mode_stereo_speaker_volume_write    (T_AUDIO_STEREO_SPEAKER_LEVEL *data);
  T_AUDIO_RET audio_mode_speaker_microphone_fir_write   (T_AUDIO_FIR_COEF *data_speaker,
                                                         T_AUDIO_FIR_COEF *data_microphone);

  #if (AUDIO_DSP_FEATURES == 1)
     T_AUDIO_RET audio_mode_volume_speed_write                      (INT16  *data);
     T_AUDIO_RET audio_mode_microphone_gain_write                   (INT16  *data);
     T_AUDIO_RET audio_mode_speaker_gain_write                      (INT16  *data);
     T_AUDIO_RET audio_mode_sidetone_gain_write                     (INT16  *data);
  #else
     T_AUDIO_RET audio_mode_microphone_gain_write                   (INT8  *data);
     T_AUDIO_RET audio_mode_speaker_gain_write                      (INT8  *data);
     T_AUDIO_RET audio_mode_sidetone_gain_write                     (INT8  *data);
  #endif

  #if (AUDIO_DSP_ONOFF == 1)
     T_AUDIO_RET audio_mode_onoff_write                             (INT8  *data);
  #endif
  #if (L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
     T_AUDIO_RET audio_mode_onoff_write                             (T_AUDIO_MODE_ONOFF  *data);
  #endif

  #if (L1_ANR == 1)
    T_AUDIO_RET audio_mode_microphone_anr_write  (T_AUDIO_ANR_CFG *data);
  #endif
  #if (L1_ANR == 2)
    T_AUDIO_RET audio_mode_microphone_anr_write  (T_AUDIO_AQI_ANR_CFG *data);
  #endif
  T_AUDIO_RET audio_mode_microphone_es_write  (T_AUDIO_ES_CFG *data);
#if( L1_IIR == 1)
  T_AUDIO_RET audio_mode_speaker_iir_write  (T_AUDIO_IIR_CFG *data);
#elif( L1_IIR == 2)
  T_AUDIO_RET audio_mode_speaker_iir_write  (T_AUDIO_IIR_DL_CFG *data);
#endif
 #if (L1_DRC == 1)
    T_AUDIO_RET audio_mode_speaker_drc_write (T_AUDIO_DRC_CFG *data);
  #endif
  T_AUDIO_RET audio_mode_speaker_limiter_write  (T_AUDIO_LIMITER_CFG *data);

  /* read drivers */
  T_AUDIO_RET audio_mode_voice_path_read               (T_AUDIO_VOICE_PATH_SETTING *data);
  T_AUDIO_RET audio_mode_microphone_mode_read          (INT8  *data);
  T_AUDIO_RET audio_mode_microphone_extra_gain_read    (INT8  *data);
  T_AUDIO_RET audio_mode_microphone_output_bias_read   (INT8  *data);
  T_AUDIO_RET audio_mode_microphone_fir_read           (T_AUDIO_FIR_COEF *data);
  #if(AS_FM_RADIO==1)
  T_AUDIO_RET audio_mode_fm_mode_read                      (UINT8  *data);
  #endif
  T_AUDIO_RET audio_mode_speaker_mode_read             (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_extra_gain_read       (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_filter_read           (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_highpass_filter_read  (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_fir_read              (T_AUDIO_FIR_COEF *data);
  T_AUDIO_RET audio_mode_speaker_buzzer_read           (INT8  *data);
  #if (L1_AEC == 1)
  extern T_AUDIO_RET audio_mode_aec_read                               (T_AUDIO_AEC_CFG *data);
  #endif
  #if (L1_AEC == 2)
  extern T_AUDIO_RET audio_mode_aec_read                               (T_AUDIO_AQI_AEC_CFG *data);
  #endif
  T_AUDIO_RET audio_mode_stereo_speaker_mode_read          (INT8  *data);
  T_AUDIO_RET audio_mode_stereo_speaker_stereo_mono_read   (INT8  *data);
  T_AUDIO_RET audio_mode_stereo_speaker_sampling_freq_read (INT8  *data);
  T_AUDIO_RET audio_mode_speaker_volume_read           (T_AUDIO_SPEAKER_LEVEL *data);
  T_AUDIO_RET audio_mode_stereo_speaker_volume_read    (T_AUDIO_STEREO_SPEAKER_LEVEL *data);
  #if (L1_ANR == 1)
    T_AUDIO_RET audio_mode_microphone_anr_read         (T_AUDIO_ANR_CFG *data);
  #endif
  #if (L1_ANR == 2)
    T_AUDIO_RET audio_mode_microphone_anr_read         (T_AUDIO_AQI_ANR_CFG *data);
  #endif
  T_AUDIO_RET audio_mode_microphone_es_read          (T_AUDIO_ES_CFG *data);
#if (L1_IIR == 1)
  T_AUDIO_RET audio_mode_speaker_iir_read            (T_AUDIO_IIR_CFG *data);
#elif (L1_IIR == 2)
  T_AUDIO_RET audio_mode_speaker_iir_read            (T_AUDIO_IIR_DL_CFG *data);
#endif
#if (L1_DRC == 1)
  T_AUDIO_RET audio_mode_speaker_drc_read            (T_AUDIO_DRC_CFG *data);
#endif
  T_AUDIO_RET audio_mode_speaker_limiter_read         (T_AUDIO_LIMITER_CFG *data);


  #if (AUDIO_DSP_FEATURES == 1)
     T_AUDIO_RET audio_mode_volume_speed_read                      (INT16  *data);
     T_AUDIO_RET audio_mode_microphone_gain_read                   (INT16  *data);
     T_AUDIO_RET audio_mode_speaker_gain_read                      (INT16  *data);
     T_AUDIO_RET audio_mode_sidetone_gain_read                     (INT16  *data);
  #else
     T_AUDIO_RET audio_mode_microphone_gain_read                   (INT8  *data);
     T_AUDIO_RET audio_mode_speaker_gain_read                      (INT8  *data);
     T_AUDIO_RET audio_mode_sidetone_gain_read                     (INT8  *data);
  #endif

  #if (AUDIO_DSP_ONOFF == 1)
     T_AUDIO_RET audio_mode_onoff_read                             (INT8  *data);
  #endif
  #if (L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
     T_AUDIO_RET audio_mode_onoff_read                             (T_AUDIO_MODE_ONOFF  *data);
  #endif

extern T_AUDIO_OUTEN_REG audio_outen_reg_as;
  #if (ANLG_FAM != 11)
  /* external functions */
  extern void ABB_CAL_UlVolume   (UWORD8 pga_index);
  extern void ABB_CAL_DlVolume   (UWORD8 volume_index, UWORD8 pga_index);
  extern void ABB_UlVolume       (UWORD8 volume_index);
  extern void ABB_DlVolume       (UWORD8 volume_index);
  extern void ABB_DlMute         (UWORD8 mute);
  extern void ABB_SideTone       (UWORD8 volume_index);
  extern void ABB_Audio_Config   (UWORD16 data);
  extern void ABB_Audio_Config_2 (UWORD16 data);
  extern void ABB_UlMute         (BOOL mute);
  extern void ABB_Audio_On_Off   (UWORD16 data);
  extern void ABB_Audio_Volume   (UWORD16 data);

  #if (AUDIO_DSP_FEATURES == 1)
    extern void L1_audio_sidetone_write(UWORD16 sidetone_value);
    extern void L1_audio_CAL_DlVolume_write(UWORD16 vol_value);
    extern void L1_audio_CAL_UlVolume_write(UWORD16 vol_value);
    extern void L1_audio_volume_speed_write(UWORD16 vol_speed_value);
  #endif
  /* external variable */
  /* Uplink PGA gain is coded on 5 bits, corresponding to -12 dB to +12 dB in 1dB steps */
  extern const UWORD8 ABB_uplink_PGA_gain[];
  /* Downlink volume: mute, -24dB to 0dB in 6dB steps */
  extern const UWORD8 ABB_volume_control_gain[];
  /* Downlink PGA gain is coded on 4 bits, corresponding to -6dB to 6dB in 1dB steps */
  extern const UWORD8 ABB_downlink_PGA_gain[];
  /* Side tone level: mute, -23dB to +4dB in 3dB steps */
  extern const UWORD8 ABB_sidetone_gain[];

  #else

  /* AGC */
  extern T_L1A_L1S_COM l1a_l1s_com;

  /* DRC */
  extern T_DRC_MCU_DSP *drc_ndb;


  #define MAX_VOL_DL   249

  // Uplink PGA gain is coded on 5 bits, corresponding to -12 dB to +12 dB in 1dB steps
  const UWORD8 ABB_uplink_PGA_gain[] =
  {
   	BSP_TWL3029_AUDIO_GAIN_DB_MINUS_12, BSP_TWL3029_AUDIO_GAIN_DB_MINUS_11,
	BSP_TWL3029_AUDIO_GAIN_DB_MINUS_10, BSP_TWL3029_AUDIO_GAIN_DB_MINUS_9,
	BSP_TWL3029_AUDIO_GAIN_DB_MINUS_8, BSP_TWL3029_AUDIO_GAIN_DB_MINUS_7,
	BSP_TWL3029_AUDIO_GAIN_DB_MINUS_6, BSP_TWL3029_AUDIO_GAIN_DB_MINUS_5,
	BSP_TWL3029_AUDIO_GAIN_DB_MINUS_4, BSP_TWL3029_AUDIO_GAIN_DB_MINUS_3,
	BSP_TWL3029_AUDIO_GAIN_DB_MINUS_2, BSP_TWL3029_AUDIO_GAIN_DB_MINUS_1,
   	BSP_TWL3029_AUDIO_GAIN_DB_0, BSP_TWL3029_AUDIO_GAIN_DB_1,
   	BSP_TWL3029_AUDIO_GAIN_DB_2, BSP_TWL3029_AUDIO_GAIN_DB_3,
   	BSP_TWL3029_AUDIO_GAIN_DB_4, BSP_TWL3029_AUDIO_GAIN_DB_5,
   	BSP_TWL3029_AUDIO_GAIN_DB_6, BSP_TWL3029_AUDIO_GAIN_DB_7,
   	BSP_TWL3029_AUDIO_GAIN_DB_8, BSP_TWL3029_AUDIO_GAIN_DB_9,
   	BSP_TWL3029_AUDIO_GAIN_DB_10, BSP_TWL3029_AUDIO_GAIN_DB_11,
   	BSP_TWL3029_AUDIO_GAIN_DB_12, BSP_TWL3029_AUDIO_GAIN_DB_MUTE
  };

  // Downlink volume: mute, -24dB to 0dB in 6dB steps
  const UWORD8 ABB_volume_control_gain[] =
  {
   	BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MUTE,
	BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MINUS_24DB,
	BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MINUS_18DB,
	BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MINUS_12DB,
	BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_MINUS_6DB,
	BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_COURSE_GAIN_0DB
  };

  // Downlink PGA gain is coded on 4 bits, corresponding to -6dB to 6dB in 1dB steps
const UWORD8 ABB_downlink_PGA_gain[] =
{
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_6DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_5DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_4DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_3DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_2DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_1DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_MINUS_0DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_1DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_2DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_3DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_4DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_5DB,
   BSP_TWL3029_AUDIO_VOICE_CODEC_DOWNLINK_FINE_GAIN_6DB
};

  // FM Gain: mute, -2dB to 14dB in 2dB steps
  const UWORD8 ABB_FM_gain[] =
  {
   	BSP_TWL3029_AUDIO_FMRADIO_GAIN_MINUS_2DB,
	BSP_TWL3029_AUDIO_FMRADIO_GAIN_0DB,
	BSP_TWL3029_AUDIO_FMRADIO_GAIN_2DB,
	BSP_TWL3029_AUDIO_FMRADIO_GAIN_4DB,
	BSP_TWL3029_AUDIO_FMRADIO_GAIN_6DB,
	BSP_TWL3029_AUDIO_FMRADIO_GAIN_8DB,
	BSP_TWL3029_AUDIO_FMRADIO_GAIN_10DB,
	BSP_TWL3029_AUDIO_FMRADIO_GAIN_12DB,
	BSP_TWL3029_AUDIO_FMRADIO_GAIN_14DB
  };

  const UWORD8 ABB_sidetone_gain[] =
  {
  	BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_23,
  	BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_20,
  	BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_17,
  	BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_14,
  	BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_11,
  	BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_8,
  	BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_5,
  	BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_MINUS_2,
  	BSP_TWL3029_AUDIO_SIDETONE_LEVEL_DB_1
  };

  typedef struct {
	INT8 audio_microphone_mode;
	INT8 audio_microphone_gain;
	INT8 audio_microphone_extra_gain;
	INT8 audio_microphone_output_bias;
	INT8 audio_speaker_mode;
	INT8 audio_speaker_gain;
	INT8 audio_speaker_extra_gain;
	INT8 audio_speaker_filter;
	INT8 audio_speaker_highpass_filter;
	INT16 audio_sidetone_gain;
	INT8 audio_stereo_speaker_mode;
	INT8 audio_stereo_speaker_stereo_mono;
	INT8 audio_Stereo_speaker_sampling_freq;
	T_AUDIO_SPEAKER_LEVEL audio_speaker_volume;
	T_AUDIO_STEREO_SPEAKER_LEVEL audio_stereo_speaker_volume;
  } T_AUDIO_CONFIGURATION;

T_AUDIO_CONFIGURATION audio_mode_configuration;
  #endif

  /********************************************************************************/
  /**********************     WRITE DRIVERS          ******************************/
  /********************************************************************************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_voice_path_write                              */
  /*                                                                              */
  /*    Purpose:  This function set a new voice path.                             */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new voice path.                                                       */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        number of message to confirm                                          */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_voice_path_write (T_AUDIO_VOICE_PATH_SETTING *data, UINT8 *message_to_confirm)
  {
    DummyStruct *p_send_message = NULL;
    T_AUDIO_VOICE_PATH_SETTING previous_audio_mode_path;

    *message_to_confirm = 0;

    /* Read if the current mode is a DAI mode */
    if (audio_mode_voice_path_read(&previous_audio_mode_path) == AUDIO_ERROR)
    {
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: voice path: can't read the current audio mode path",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    }

    switch (*data)
    {
      case AUDIO_GSM_VOICE_PATH:
      {
        if ( (previous_audio_mode_path == AUDIO_DAI_ENCODER) ||
             (previous_audio_mode_path == AUDIO_DAI_DECODER) ||
             (previous_audio_mode_path == AUDIO_DAI_ACOUSTIC) )
        {
          /* The current mode is a DAI test so it needs to be stopped */
          /* Send the stop DAI message to the L1*/
          /* allocate the buffer for the message to the L1 */
          p_send_message = audio_allocate_l1_message(0);
          if (p_send_message != NULL)
          {
            /* send the start command to the audio L1 */
            audio_send_l1_message(OML1_STOP_DAI_TEST_REQ, p_send_message);
          }
          else
          {
            AUDIO_SEND_TRACE("AUDIO MODE WRITE: voice path: the stop DAI message isn't sent",
              RV_TRACE_LEVEL_ERROR);
            return(AUDIO_ERROR);
          }
          /* confirm the DAI stop message */
          *message_to_confirm += 1;
        }

        #if (AUDIO_MODE)
          /* Send the Audio path configuration message */
          /* allocate the buffer for the message to the L1 */
          p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AUDIO_MODE));
          if (p_send_message != NULL)
          {
            /* Fill the parameter */
            ((T_MMI_AUDIO_MODE *)p_send_message)->audio_mode = *data;

            /* send the start command to the audio L1 */
            audio_send_l1_message(MMI_AUDIO_MODE_REQ, p_send_message);
          }
          else
          {
            AUDIO_SEND_TRACE("AUDIO MODE WRITE: voice path: the audio mode message isn't sent",
            RV_TRACE_LEVEL_ERROR);
            return(AUDIO_ERROR);
          }
          /* confirm the AUDIO MODE message */
          *message_to_confirm += 1;
        #endif
        break;
      }
      #if (AUDIO_MODE)
        case AUDIO_BLUETOOTH_CORDLESS_VOICE_PATH:
        case AUDIO_BLUETOOTH_HEADSET:
        {
          if ( (previous_audio_mode_path == AUDIO_DAI_ENCODER) ||
               (previous_audio_mode_path == AUDIO_DAI_DECODER) ||
               (previous_audio_mode_path == AUDIO_DAI_ACOUSTIC) )
          {
            /* The current mode is a DAI test so it needs to be stopped */
            /* Send the stop DAI message to the L1*/
            /* allocate the buffer for the message to the L1 */
            p_send_message = audio_allocate_l1_message(0);
            if (p_send_message != NULL)
            {
              /* send the start command to the audio L1 */
              audio_send_l1_message(OML1_STOP_DAI_TEST_REQ, p_send_message);
            }
            else
            {
              AUDIO_SEND_TRACE("AUDIO MODE WRITE: voice path: the stop DAI message isn't sent",
              RV_TRACE_LEVEL_ERROR);
              return(AUDIO_ERROR);
            }
            /* confirm the DAI stop message */
            *message_to_confirm += 1;
          }

          /* Send the Audio path configuration message */
          /* allocate the buffer for the message to the L1 */
          p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AUDIO_MODE));
          if (p_send_message != NULL)
          {
            /* Fill the parameter */
            ((T_MMI_AUDIO_MODE *)p_send_message)->audio_mode = *data;

            /* send the start command to the audio L1 */
            audio_send_l1_message(MMI_AUDIO_MODE_REQ, p_send_message);
          }
          else
          {
            AUDIO_SEND_TRACE("AUDIO MODE WRITE: voice path: the audio mode message isn't sent",
            RV_TRACE_LEVEL_ERROR);
            return(AUDIO_ERROR);
          }
          /* confirm the AUDIO MODE message */
          *message_to_confirm += 1;
          break;
        }
      #endif
      case AUDIO_DAI_ENCODER :
      case AUDIO_DAI_DECODER :
      case AUDIO_DAI_ACOUSTIC :
      {
        /* Send the DAI start message */
        /* allocate the buffer for the message to the L1 */
        p_send_message = audio_allocate_l1_message(sizeof(T_OML1_START_DAI_TEST_REQ));
        if (p_send_message != NULL)
        {
          /* Fill the parameter */
          ((T_OML1_START_DAI_TEST_REQ *)p_send_message)->tested_device = (UINT8)(*data - 10);

          /* send the start command to the audio L1 */
          audio_send_l1_message(OML1_START_DAI_TEST_REQ, p_send_message);
        }
        else
        {
          AUDIO_SEND_TRACE("AUDIO MODE WRITE: voice path: the start DAI message isn't sent",
          RV_TRACE_LEVEL_ERROR);
          return(AUDIO_ERROR);
        }
        /* confirm the DAI start message */
        *message_to_confirm += 1;
        break;
      }
      default :
      {
          AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: voice path: this mode isn't supported",
            *data,
            RV_TRACE_LEVEL_ERROR);
          return(AUDIO_ERROR);
      }
    }
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_microphone_mode_write                         */
  /*                                                                              */
  /*    Purpose:  This function set a new microphone mode.                        */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new microphone mode.                                                  */
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
  T_AUDIO_RET audio_mode_microphone_mode_write (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbcr;
    #if ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
      UINT16 vbcr2;
    #endif

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
      vbcr2 = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl2 >> 6);
    #endif

    /* Reset configuration */
    #if (ANLG_FAM == 1)
      vbcr &= ~(AUDIO_VBCR_VULSWITCH);
    #elif (ANLG_FAM == 2)
      vbcr &= ~(AUDIO_VBCR_VULSWITCH);
      vbcr2 &= (~(AUDIO_VBCR2_MICBIASEL | AUDIO_VBCR2_MICNAUX));
    #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr &= ~(AUDIO_VBCR_VULSWITCH);
      vbcr2 &= (~(AUDIO_VBCR2_MICBIASEL | AUDIO_VBCR2_HSMICSEL | AUDIO_VBCR2_HSDIF));
    #endif

    switch (*data)
    {
      case AUDIO_MICROPHONE_HANDHELD:
      {
        #if (ANLG_FAM == 1)
          vbcr |= AUDIO_VBCR_VULSWITCH;
          ABB_Audio_Config(vbcr);
        #elif ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
          vbcr |= AUDIO_VBCR_VULSWITCH;
          ABB_Audio_Config(vbcr);
          ABB_Audio_Config_2(vbcr2);
        #endif
        break;
      }
      case AUDIO_MICROPHONE_HEADSET:
      {
        #if (ANLG_FAM == 2)
          vbcr2 |= (AUDIO_VBCR2_MICBIASEL | AUDIO_VBCR2_MICNAUX);
          ABB_Audio_Config(vbcr);
          ABB_Audio_Config_2(vbcr2);
        #elif ((ANLG_FAM == 3)  || (ANLG_FAM == 4))
          vbcr2 |= (AUDIO_VBCR2_MICBIASEL | AUDIO_VBCR2_HSMICSEL);
          ABB_Audio_Config(vbcr);
          ABB_Audio_Config_2(vbcr2);
        #else
          AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: microphone mode: this mode isn't supported",
            *data, RV_TRACE_LEVEL_ERROR);
          return(AUDIO_ERROR);
        #endif
        break;
      }
      case AUDIO_MICROPHONE_HEADSET_DIFFERENTIAL:
      {
        #if ((ANLG_FAM == 3)  || (ANLG_FAM == 4))
          vbcr2 |= (AUDIO_VBCR2_MICBIASEL | AUDIO_VBCR2_HSMICSEL | AUDIO_VBCR2_HSDIF);
          ABB_Audio_Config(vbcr);
          ABB_Audio_Config_2(vbcr2);
        #else
          AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: microphone mode: this mode isn't supported",
            *data, RV_TRACE_LEVEL_ERROR);
          return(AUDIO_ERROR);
        #endif
        break;
      }
      case AUDIO_MICROPHONE_HANDFREE:
      {
        #if (ANLG_FAM == 1)
          ABB_Audio_Config(vbcr);
        #elif ((ANLG_FAM == 2)||(ANLG_FAM == 3)  || (ANLG_FAM == 4))
          ABB_Audio_Config(vbcr);
          ABB_Audio_Config_2(vbcr2);
        #endif
        break;
      }
      default :
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: microphone mode: this mode isn't supported",
          *data, RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
        break;
      }
    }
#else

    bspTwl3029_Audio_configureInputPath( NULL,  *data);
    //audio_mode_configuration.audio_microphone_mode = *data;

#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_microphone_gain_write                         */
  /*                                                                              */
  /*    Purpose:  This function set a new microphone gain.                        */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new microphone gain.                                                  */
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
 #if (AUDIO_DSP_FEATURES == 1)
  T_AUDIO_RET audio_mode_microphone_gain_write (INT16  *data)
   {
      L1_audio_CAL_UlVolume_write((UWORD16) (*data));

      l1a_l1s_com.audioIt_task.command.start = 1;
 #else
  T_AUDIO_RET audio_mode_microphone_gain_write (INT8  *data)
  {
#if (ANLG_FAM != 11)
    if (*data == AUDIO_MICROPHONE_MUTE)
    {
      ABB_UlMute(TRUE);
    }
    else
    {
      /* Unmute the microphone */
      ABB_UlMute(FALSE);
      ABB_CAL_UlVolume( (UINT8)(*data + 12) );
    }
#else
    BspTwl3029_Audio_GainSettings newGainSettings;

    newGainSettings.vulGain = ABB_uplink_PGA_gain[*data + 12];

    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_gainControl(NULL,
											BSP_TWL3029_AUDIO_GAIN_UPDATE_VUL,
											&newGainSettings ))
    {
    	return AUDIO_ERROR ;
    }

	//audio_mode_configuration.audio_microphone_gain = *data;

#endif /* ANLG_FAM == 11 */

#endif

    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_microphone_extra_gain_write                   */
  /*                                                                              */
  /*    Purpose:  This function set a new microphone extra gain.                  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new microphone extra gain.                                            */
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
  T_AUDIO_RET audio_mode_microphone_extra_gain_write (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbcr;

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
    #endif

    switch (*data)
    {
      case AUDIO_MICROPHONE_AUX_GAIN_28_2dB:
      {
        vbcr |= (AUDIO_VBCR_VBDFAUXG);
        ABB_Audio_Config(vbcr);
        break;
      }
      case AUDIO_MICROPHONE_AUX_GAIN_4_6dB:
      {
        vbcr &= ~(AUDIO_VBCR_VBDFAUXG);
        ABB_Audio_Config(vbcr);
        break;
      }
      default :
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: microphone extra gain: this extra gain isn't supported",
          *data,
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
        break;
      }
    }
#else
  /* Configure FM gain as part of Microphone extra-gain */
    BspTwl3029_Audio_GainSettings newGainSettings;

	if( *data != -2 )
	{
		newGainSettings.fmRadioGain = ABB_FM_gain [(*data / 2) + 1];
	}
	else
	{
		newGainSettings.fmRadioGain = BSP_TWL3029_AUDIO_FMRADIO_GAIN_MINUS_2DB;
	}

    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_gainControl(NULL,
											BSP_TWL3029_AUDIO_GAIN_UPDATE_RADIO,
											&newGainSettings ))
    {
    	return AUDIO_ERROR ;
    }

	//audio_mode_configuration.audio_microphone_extra_gain = *data;
#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_microphone_output_bias_write                  */
  /*                                                                              */
  /*    Purpose:  This function set a new microphone output bias.                 */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new microphone output bias.                                           */
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
  T_AUDIO_RET audio_mode_microphone_output_bias_write (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbcr;

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
    #endif

    switch (*data)
    {
      case AUDIO_MICROPHONE_OUTPUT_BIAS_2_5V:
      {
        vbcr |= (AUDIO_VBCR_MICBIAS);
        ABB_Audio_Config(vbcr);
        break;
      }
      case AUDIO_MICROPHONE_OUTPUT_BIAS_2_0V:
      {
        vbcr &= ~(AUDIO_VBCR_MICBIAS);
        ABB_Audio_Config(vbcr);
        break;
      }
      default :
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: microphone output bias: this output bias isn't supported",
          *data,
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
        break;
      }
    }
#else
    if( BSP_TWL3029_RETURN_CODE_SUCCESS !=
		bspTwl3029_Audio_configureMicBias( NULL, *data))
    {
	    return AUDIO_ERROR;
    }

	//audio_mode_configuration.audio_microphone_output_bias = *data;
#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
	 /*    Function Name:	audio_mode_fm_mode_write							 */
	 /* 																			 */
	 /*    Purpose:  This function set a new FM mode						 */
	 /* 																			 */
	 /*    Input Parameters:														 */
	 /* 	   new speaker mode.													 */
	 /* 																			 */
	 /*    Output Parameters:														 */
	 /* 	   Status.																 */
	 /* 																			 */
	 /*    Note:																	 */
	 /* 	   None 																 */
	 /* 																			 */
	 /*    Revision History:														 */
	 /* 	   None.																 */
	 /* 																			 */
	 /********************************************************************************/
#if(AS_FM_RADIO==1)

   T_AUDIO_RET  audio_mode_fm_mode_write (UINT8  *data)
   	{
   	BspTwl3029_Audio_OutputPathId outputPath = BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;
	BspTwl3029_AudioOutputSource outputSource[6]; // = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
  	BspTwl3029_Audio_OutputPathId outputPath_temp;
	Uint8 classDmode;
	 void *p_send_message;
	  /* FOUR Steps are required
   * 1. First reset the current Voice Path i.e. wherever voice is routed to
   * 2. Write triton registers to reset accordingly
   * 3. Set according what *data indicates i.e. route voice correctly
   * 4. Configure triton registers accordingly *
   */

  /* STEP 1 -> Reset current FM path settings and to
   * reset ONLY FM path settings i.e IF FM + voice is
   * routed to handheld, voice still needs to be routed to handheld */
		  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getOutputPath(&outputPath_temp, &outputSource))
			{
			  return AUDIO_ERROR;
			}
 // Headset
  {
    Uint8 headset_flag = 0;
	//if Audio+Voice,Reset Audio

	if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
			 BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO)
	   {

		 //if voice+FM ,then reset to Voice only
		 outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
		 outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
		 headset_flag = 1;
		 AUDIO_SEND_TRACE("headset voice+audio....reset to voice", RV_TRACE_LEVEL_DEBUG_LOW);

	   }

  	//FM only,reset
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
        BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM)
    {
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
      headset_flag = 1;
      AUDIO_SEND_TRACE("headset FM is prev", RV_TRACE_LEVEL_DEBUG_LOW);

    }
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM)
    {

      //if voice+FM ,then reset to Voice only
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE ;
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
      headset_flag = 1;
      AUDIO_SEND_TRACE("headset voice+FM prev", RV_TRACE_LEVEL_DEBUG_LOW);

    }


	if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
			BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM)
	  {

		//if Audio+FM ,then reset to Audio only
		outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO ;
		outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
		headset_flag = 1;
		AUDIO_SEND_TRACE("headset Audio+FM prev", RV_TRACE_LEVEL_DEBUG_LOW);

	  }

    if(headset_flag == 1){
      outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE |
            BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE);
    }

  }

//Handsfree & Class D


 {
	 Uint8 handsfree_flag = 0;
	 //if Class D,classDmode is set
	    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getClassD_mode(&classDmode))
   		 {
      			return AUDIO_ERROR;
    		}

	//if Audio+Voice,Reset voice
	 if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
		   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO)
	 {
	   //if voice+FM then reset to audio
	   outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
	   outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
	   handsfree_flag = 1;
	   AUDIO_SEND_TRACE("handfree voice+audio...reset to audio...", RV_TRACE_LEVEL_DEBUG_LOW);
	 }

	//FM only,reset
	 if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
		 BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM)
	 {
	   outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
	   outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
	   handsfree_flag = 1;
	   AUDIO_SEND_TRACE("handsfree FM is prev", RV_TRACE_LEVEL_DEBUG_LOW);
	 }
	 if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
		   BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM)
	 {

	   //if voice+FM then reset to Voice
	   outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE ;
	   outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
	   handsfree_flag = 1;
	   AUDIO_SEND_TRACE("handsfree voice+FM", RV_TRACE_LEVEL_DEBUG_LOW);
	 }


	 if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
			 BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM)
	   {

		 //if Audio+FM then reset to Audio only
		 outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO ;
		 outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] =BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
		 handsfree_flag = 1;
		 AUDIO_SEND_TRACE("handsfree Audio+FM", RV_TRACE_LEVEL_DEBUG_LOW);
	   }

	  if(handsfree_flag == 1)
	  	{
    		  if(classDmode == 1)
        			outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK_DCLASS);
      		else
        			outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK);
    		}
   }


 /* STEP2 -> Configure triton accordingly to reset current voice path settings */

		if( BSP_TWL3029_RETURN_CODE_SUCCESS !=
	bspTwl3029_Audio_configureOutputPath( NULL, outputPath,outputSource))
	  {
	  return AUDIO_ERROR;
  	  }

  /* STEP 3 -> Set audio path correctly as mentioned by *data */

  	outputPath = BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;
	switch(*data)
	{
	  case AUDIO_FM_SPEAKER_NONE:
   		 {
		 outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;
	  	break;
  		  }
	  case AUDIO_FM_STEREO_SPEAKER_HEADSET:

		{
			 outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE |
						BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE);
			//FM only
			if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE)
			{
				outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] =
        				BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM;
				outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] =
        				BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM;
        		         		AUDIO_SEND_TRACE("headset FM+Voice is conf", RV_TRACE_LEVEL_DEBUG_LOW);
			}
			//FM+Audio
			if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO)
			{
				outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] =
					BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM;
				outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] =
					BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM;
					AUDIO_SEND_TRACE("headset FM+Audio is conf", RV_TRACE_LEVEL_DEBUG_LOW);
			}
			//FM+Voice
			if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE)
			{
				outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] =
					BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM;
				outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] =
					BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM;
					AUDIO_SEND_TRACE("headset FM is conf", RV_TRACE_LEVEL_DEBUG_LOW);
			}
		  	break;
	  	}

	  case AUDIO_FM_MONO_SPEAKER_HANDFREE :
	  	case AUDIO_FM_MONO_SPEAKER_HANDFREE_CLASSD:

	  	{
			//If data  is ClassD set..
				if (*data==AUDIO_FM_MONO_SPEAKER_HANDFREE)
				{
		         				outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK;
				}
				else
				{
								AUDIO_SEND_TRACE("Class D is set", RV_TRACE_LEVEL_DEBUG_LOW);
								outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK_DCLASS;
				}

			//FM only
			if( outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET]==BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE)
      			{
       		 	outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET]=BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM;
       		 	AUDIO_SEND_TRACE("headfree FM is conf", RV_TRACE_LEVEL_DEBUG_LOW);
     	 		}
     	 		//FMAudio
			if( outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET]==BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO)
			{
			outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET]=BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM;
			AUDIO_SEND_TRACE("headfree FM+Audio is conf", RV_TRACE_LEVEL_DEBUG_LOW);
			}
			//FM+Voice
			if( outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET]==BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE)
			{
			outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET]=BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM;
			AUDIO_SEND_TRACE("headfree FM+Voice is conf", RV_TRACE_LEVEL_DEBUG_LOW);
			}


		break;
	  	}

 	default:
   		 {
      	     		 return(AUDIO_ERROR);
    	 }

   	}

	 /* STEP4 -> Configure triton accordingly to new voice path settings */


    if( BSP_TWL3029_RETURN_CODE_SUCCESS !=
		bspTwl3029_Audio_configureOutputPath( NULL, outputPath,outputSource))
    {
	    return AUDIO_ERROR;
    }

     /* Send Audio Outen Reg config command to L1  */
    p_send_message = (void *)audio_allocate_l1_message(sizeof(T_MMI_OUTEN_CFG_REQ));
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->outen1 = audio_outen_reg_as.outen1;
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->outen2 = audio_outen_reg_as.outen2;
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->outen3 = audio_outen_reg_as.outen3;
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->classD = audio_outen_reg_as.classD;

    if (p_send_message != NULL)
     {
       AUDIO_SEND_TRACE("AUDIO MODE WRITE: FM mode: MMI_OUTEN_CFG_REQ outen1 : ",
           RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("classD : ", audio_outen_reg_as.classD, RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("outen3 : ", audio_outen_reg_as.outen3, RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("outen2 : ", audio_outen_reg_as.outen2, RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("outen1 : ", audio_outen_reg_as.outen1, RV_TRACE_LEVEL_DEBUG_LOW);

       audio_send_l1_message(MMI_OUTEN_CFG_REQ, p_send_message);
     }

   	//audio_mode_configuration.audio_speaker_mode = *data;

    return (AUDIO_OK);

  }
#endif
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_mode_write                            */
  /*                                                                              */
  /*    Purpose:  This function set a new speaker mode.                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new speaker mode.                                                     */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_mode_write (INT8  *data)
  {
    void *p_send_message;

#if (ANLG_FAM != 11)
    UINT16 vbcr;
    #if (ANLG_FAM == 2)
      UINT16 vbcr2;
    #elif((ANLG_FAM == 3) || (ANLG_FAM == 4))
      UINT16 vbcr2;
      UINT16 vauoctrl;
    #endif

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif (ANLG_FAM == 2)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
      vbcr2 = (l1s_dsp_com.dsp_ndb_ptr->d_vbctrl2 >> 6);
    #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
      vbcr2 = (l1s_dsp_com.dsp_ndb_ptr->d_vbctrl2 >> 6);
      vauoctrl = (l1s_dsp_com.dsp_ndb_ptr->d_vauo_onoff >> 6);
    #endif

    /* Reset configuration */
    #if (ANLG_FAM == 1)
      vbcr &= (~(AUDIO_VBCR_VDLAUX|AUDIO_VBCR_VDLEAR));
    #elif (ANLG_FAM == 2)
      vbcr &= (~(AUDIO_VBCR_VDLAUX|AUDIO_VBCR_VDLEAR));
      vbcr2 &= (~(AUDIO_VBCR2_VDLHSO));
    #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
      /* Reset the HSO (but not audio part) */
      vauoctrl &= (~(AUDIO_VAUOCTRL_HSOL_VOICE | AUDIO_VAUOCTRL_HSOR_VOICE));
      /* Reset the AUX configuration (but not audio part) */
      vauoctrl &= (~AUDIO_VAUOCTRL_AUX_VOICE);
      /* Reset the SPK configuration (but not audio part) */
      vauoctrl &= (~AUDIO_VAUOCTRL_SPK_VOICE);
      /* Reset the EAR configuration (but not audio part) */
      vauoctrl &= (~AUDIO_VAUOCTRL_EAR_VOICE);
    #endif

    switch (*data)
    {
      case AUDIO_SPEAKER_HANDHELD:
      {
        #if (ANLG_FAM == 1)
          vbcr |= (AUDIO_VBCR_VDLEAR);
          ABB_Audio_Config(vbcr);
        #elif (ANLG_FAM == 2)
          vbcr |= (AUDIO_VBCR_VDLEAR);
          ABB_Audio_Config(vbcr);
          ABB_Audio_Config_2(vbcr2);
        #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
          vauoctrl |= (AUDIO_VAUOCTRL_EAR_VOICE);
          ABB_Audio_On_Off(vauoctrl);
        #endif
        break;
      }
      case AUDIO_SPEAKER_HANDFREE:
      {
        #if (ANLG_FAM == 1)
          vbcr |= (AUDIO_VBCR_VDLAUX);
          ABB_Audio_Config(vbcr);
        #elif (ANLG_FAM == 2)
          vbcr |= (AUDIO_VBCR_VDLAUX);
          ABB_Audio_Config(vbcr);
          ABB_Audio_Config_2(vbcr2);
        #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
          vauoctrl |= (AUDIO_VAUOCTRL_AUX_VOICE);
          ABB_Audio_On_Off(vauoctrl);
        #endif

        break;
      }
      case AUDIO_SPEAKER_HEADSET:
      {
        #if (ANLG_FAM == 2)
          vbcr2 |= (AUDIO_VBCR2_VDLHSO);
          ABB_Audio_Config(vbcr);
          ABB_Audio_Config_2(vbcr2);
        #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
          vauoctrl |= (AUDIO_VAUOCTRL_HSOR_VOICE | AUDIO_VAUOCTRL_HSOL_VOICE);
          ABB_Audio_On_Off(vauoctrl);
        #else
          AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker mode: this mode isn't supported",
            *data, RV_TRACE_LEVEL_ERROR);
          return(AUDIO_ERROR);
        #endif
        break;
      }
      case AUDIO_SPEAKER_HANDHELD_8OHM:
      {
        #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
          vauoctrl |= AUDIO_VAUOCTRL_SPK_VOICE;
          ABB_Audio_On_Off(vauoctrl);
        #else
          AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker mode: this mode isn't supported",
            *data, RV_TRACE_LEVEL_ERROR);
          return(AUDIO_ERROR);
        #endif
        break;
      }
      case AUDIO_SPEAKER_BUZZER:
      {
        #if (ANLG_FAM == 1)
          ABB_Audio_Config(vbcr);
        #else
          AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker mode: this mode isn't supported",
            *data, RV_TRACE_LEVEL_ERROR);
          return(AUDIO_ERROR);
        #endif
        break;
      }
      case AUDIO_SPEAKER_HANDHELD_HANDFREE:
      {
        #if (ANLG_FAM == 1)
          vbcr |= (AUDIO_VBCR_VDLAUX | AUDIO_VBCR_VDLEAR);
          ABB_Audio_Config(vbcr);
        #elif (ANLG_FAM == 2)
          vbcr |= (AUDIO_VBCR_VDLAUX | AUDIO_VBCR_VDLEAR);
          ABB_Audio_Config(vbcr);
          ABB_Audio_Config_2(vbcr2);
        #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
          vauoctrl |= (AUDIO_VAUOCTRL_EAR_VOICE | AUDIO_VAUOCTRL_AUX_VOICE);
          ABB_Audio_On_Off(vauoctrl);
        #endif
        break;
      }
      default:
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker mode: this mode isn't supported",
          *data, RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
        break;
      }
    }
#else
	BspTwl3029_Audio_OutputPathId outputPath = BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;
	BspTwl3029_AudioOutputSource outputSource[6]; // = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
  BspTwl3029_Audio_OutputPathId outputPath_temp;
	Uint8 classDmode;

  /* FOUR Steps are required
   * 1. First reset the current Voice Path i.e. wherever voice is routed to
   * 2. Write triton registers to reset accordingly
   * 3. Set according what *data indicates i.e. route voice correctly
   * 4. Configure triton registers accordingly *
   */

  /* STEP 1 -> Reset current Voice path settings and to
   * reset ONLY voice path settings i.e IF audio + voice is
   * routed to handheld, audio still needs to be routed to handheld */

  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getOutputPath(&outputPath_temp, &outputSource))
  {
    return AUDIO_ERROR;
  }


  // Handheld
  /* ideally
   * if (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET]
   *        == BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_ONLY)
   * {
   *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] =
   *      BSP_TWL3029_AUDIO_OUTPUT_PATH_OFF;
   * }
   * if (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET]
   *        == BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_AUDIO)audio_mode_stereo_speaker_stereo_mono_write(BSP_TWL3029_AUDIO_STEREO2MONO_OFF);
   * {
   *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] =
   *      BSP_TWL3029_AUDIO_OUTPUT_PATH_AUDIO;
   * }
   * Same could be established by AND with ~(0x01)
   */
  if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] & 0x1)
  {
    outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE;
    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] &= (~(0x1));
  }

  // AUX
  if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] & 0x1)
  {
    /* Same comment as handheld */
    outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_AUX;
    outputSource[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] &= (~(0x1));
  }

  // Carkit
  if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] & 0x1)
  {
    /* Same comment as handheld */
    outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_CARKIT;
    outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] &= (~(0x1));
  }

  // Headset inverted voice
  if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] ==
      BSP_TWL3029_AUDIO_OUTPUT_SOURCE_INVERT_VOICE)
  {
    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;

    outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE |
          BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE);
  }

  // Headset
  {
    Uint8 headset_flag = 0;
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
        BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE)
    {
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
      headset_flag = 1;
    }
    if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO) ||
        (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM))
    {
     /* BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO  = 4 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO (2)
      * BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM     = 5 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM (3)
      * */
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] -= 2;
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] -= 2;
      headset_flag = 1;
    }
    #if(AS_FM_RADIO==1)
    if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM))
      {
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
      headset_flag = 1;
	  }
	 #endif

    if(headset_flag == 1){
      outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE |
            BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE);
    }
  }

  // handfree and handfree class D Amplifier
  {
    Uint8 spk_flag = 0;
    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getClassD_mode(&classDmode))
    {
      return AUDIO_ERROR;
    }
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
        BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE)
    {
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
      spk_flag = 1;
    }
    if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO) ||
        (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM))
    {
     /* BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO  = 4 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO (2)
      * BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM     = 5 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM (3)
      * */
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] -= 2;
      spk_flag = 1;
    }
     #if(AS_FM_RADIO==1)
    if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM))
      {
		outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
		spk_flag=1;
      }
      #endif
    if(spk_flag == 1){
      if(classDmode == 1)
        outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK_DCLASS);
      else
        outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK);
    }
  }

  /* STEP2 -> Configure triton accordingly to reset current voice path settings */
  if( BSP_TWL3029_RETURN_CODE_SUCCESS !=
	bspTwl3029_Audio_configureOutputPath( NULL, outputPath,	outputSource))
  {
	  return AUDIO_ERROR;
  }
		outputPath = BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;

  /* STEP 3 -> Set voice path correctly as mentioned by *data */
	switch(*data)
	{
	  case AUDIO_SPEAKER_NONE:
    {
	  	outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;
	  	break;
    }
	  case AUDIO_SPEAKER_HANDHELD:
    {
      /* ideally
       * if (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET]
       *        == BSP_TWL3029_AUDIO_OUTPUT_PATH_OFF)
       * {
       *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] =
       *      BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_ONLY;
       * }
       * if (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET]
       *        == BSP_TWL3029_AUDIO_OUTPUT_PATH_AUDIO)
       * {
       *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] =
       *      BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_AUDIO;
       * }
       * Same could be established by OR with (0x01)
       */
	  	outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE;
	  	outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] |= 0x1;
	  	break;
    }
	case AUDIO_SPEAKER_AUX:
    {
      /* Same comment as handheld */
	  	outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_AUX;
	  	outputSource[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] |= 0x1;
	  	break;
    }
	case AUDIO_SPEAKER_CARKIT:
    {
      /* Same comment as handheld */
	  	outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_CARKIT;
	  	outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] |= 0x1;

      /* carkit alone might need a stereo->mono transition */
      if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] & 0x2)
      {
        INT8 dat;
        T_AUDIO_RET return_val;

        dat = AUDIO_MONO_LEFT;
        return_val = audio_mode_stereo_speaker_stereo_mono_write(&dat);
        if(return_val == AUDIO_ERROR){
          /* Stereo is already powered ON. Not able to change to Carkit + voice */
          return AUDIO_ERROR;
        }
      }
	  	break;
    }
	  case AUDIO_SPEAKER_HEADSET:
    {
	  	outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE |
	  		BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE);

      if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM)
      {
        /* In case of already AUDIO + FM return error */
        return AUDIO_ERROR;
      }
      if( outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE)
      {
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
      }
      else
      {
        /* BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO(2) -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO(4)
         * BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM(3)    -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM(5)
         * */
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] += 2;
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] += 2;
      }
	  	break;
    }
	case AUDIO_SPEAKER_INVERTED_VOICE:
    {
	  	outputPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE |
	  		BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE);

      if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM) ||
          (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO) ||
          (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM) )
      {
        /* In case of already AUDIO OR FM return error*/
        return AUDIO_ERROR;
      }
		outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_INVERT_VOICE;
		outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_INVERT_VOICE;
		break;
    }
	  case AUDIO_SPEAKER_HANDFREE:
	case AUDIO_SPEAKER_HANDFREE_CLASS_D:
    {
      /* */
      if(*data == AUDIO_SPEAKER_HANDFREE)
        outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK;
      else
        outputPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK_DCLASS;

      /* It is similar to AUDIO_SPEAKER_HEADSET */

      if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM)
      {
        /* In case of already AUDIO + FM return error*/
        return AUDIO_ERROR;
      }
      if( outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE)
      {
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE;
      }
      else
      {
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] += 2;
      }
	  	break;
    }
    default:
    {
      //AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker mode: this mode isn't supported",
       // (*data), RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    }
	}

  /* STEP4 -> Configure triton accordingly to new voice path settings */
    audio_outen_reg_as.classD = 0x00;

    if( BSP_TWL3029_RETURN_CODE_SUCCESS !=
		bspTwl3029_Audio_configureOutputPath( NULL, outputPath,	outputSource))
    {
	    return AUDIO_ERROR;
    }

     /* Send Audio Outen Reg config command to L1  */
    p_send_message = (void *)audio_allocate_l1_message(sizeof(T_MMI_OUTEN_CFG_REQ));
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->outen1 = audio_outen_reg_as.outen1;
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->outen2 = audio_outen_reg_as.outen2;
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->outen3 = audio_outen_reg_as.outen3;
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->classD = audio_outen_reg_as.classD;

    if (p_send_message != NULL)
    {
       AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker mode: MMI_OUTEN_CFG_REQ outen1 : ",
          audio_outen_reg_as.outen1, RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("classD : ", audio_outen_reg_as.classD, RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("outen3 : ", audio_outen_reg_as.outen3, RV_TRACE_LEVEL_DEBUG_LOW);

       audio_send_l1_message(MMI_OUTEN_CFG_REQ, p_send_message);
    }

   	//audio_mode_configuration.audio_speaker_mode = *data;
#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_gain_write                            */
  /*                                                                              */
  /*    Purpose:  This function set a new speaker gain.                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new speaker gain.                                                     */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
 #if (AUDIO_DSP_FEATURES == 1)
  T_AUDIO_RET audio_mode_speaker_gain_write (INT16  *data)
  {
      L1_audio_CAL_DlVolume_write((UWORD16) (*data));

      l1a_l1s_com.audioIt_task.command.start = 1;
 #else
  T_AUDIO_RET audio_mode_speaker_gain_write (INT8  *data)
  {
#if (ANLG_FAM != 11)
    T_AUDIO_SPEAKER_LEVEL volume;

    audio_mode_speaker_volume_read (&volume);

    ABB_CAL_DlVolume (volume.audio_speaker_level, (UINT8)(*data + 6));
#else
    BspTwl3029_Audio_GainVDLSettings newGainVDLSettings;

    newGainVDLSettings.vdlFineGain = ABB_downlink_PGA_gain[*data + 6];

    bspTwl3029_Audio_gainVDLConfig( NULL, BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_FINE,
								&newGainVDLSettings);

    //audio_mode_configuration.audio_speaker_gain = *data;
#endif
 #endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_extra_gain_write                      */
  /*                                                                              */
  /*    Purpose:  This function set a new speaker extra gain.                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new speaker gain.                                                     */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_extra_gain_write (INT8  *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
    UINT16 vbctrl2 = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl2 >> 6);

    switch (*data)
    {
      case AUDIO_SPEAKER_SPK_GAIN_8_5dB:
      {
        vbctrl2 |= (AUDIO_VBCR2_SPKG);
        ABB_Audio_Config_2(vbctrl2);
        break;
      }
      case AUDIO_SPEAKER_SPK_GAIN_2_5dB:
      {
        vbctrl2 &= ~(AUDIO_VBCR2_SPKG);
        ABB_Audio_Config_2(vbctrl2);
        break;
      }
      default :
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: microphone extra gain: this extra gain isn't supported",
          *data, RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
        break;
      }
    }
  #else
    AUDIO_SEND_TRACE("AUDIO MODE WRITE: speaker extra gain: feature not supported",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else
  BspTwl3029_Audio_GainSettings newGainSettings;
  INT8 speakerMode = BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;

  // audio_mode_stereo_speaker_mode_read  (&speakerMode);
  audio_mode_speaker_mode_read  (&speakerMode);

  if( AUDIO_SPEAKER_HANDHELD == speakerMode )
  {
	  newGainSettings.earGain = *data;
	  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_gainControl( NULL,
				  BSP_TWL3029_AUDIO_GAIN_UPDATE_EARGAIN,
				  &newGainSettings))
	  {
		  return(AUDIO_ERROR);
	  }
  }
  else if ( AUDIO_SPEAKER_HANDFREE == speakerMode || AUDIO_SPEAKER_HANDFREE_CLASS_D == speakerMode)
  {
	  newGainSettings.speakerAmpGain = *data;
	  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_gainControl( NULL,
				  BSP_TWL3029_AUDIO_GAIN_UPDATE_SPEAKERAMP,
				  &newGainSettings))
	  {
		  return(AUDIO_ERROR);
	  }
  }
  else
  {
	 AUDIO_SEND_TRACE("Check You are Wrong ", RV_TRACE_LEVEL_ERROR);
	 return(AUDIO_ERROR);
  }

  //audio_mode_configuration.audio_speaker_extra_gain = *data;

#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_stereo_speaker_extra_gain_write               */
  /*                                                                              */
  /*    Purpose:  This function set a new stereo speaker extra gain.              */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new speaker gain.                                                     */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_stereo_speaker_extra_gain_write (INT8  *data)
  {
#if (ANLG_FAM == 11)
  BspTwl3029_Audio_GainSettings newGainSettings;
  INT8 stereo_speakerMode = BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;

  audio_mode_stereo_speaker_mode_read  (&stereo_speakerMode);

  // AUDIO_SEND_TRACE_PARAM(" stereo spk mode : ", stereo_speakerMode, RV_TRACE_LEVEL_ERROR);

  if(AUDIO_STEREO_SPEAKER_HANDHELD == stereo_speakerMode )
  {
	  newGainSettings.earGain = *data;
	  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_gainControl( NULL,
				  BSP_TWL3029_AUDIO_GAIN_UPDATE_EARGAIN,
				  &newGainSettings))
	  {
		  return(AUDIO_ERROR);
	  }
  }
  else if(AUDIO_STEREO_SPEAKER_HANDFREE == stereo_speakerMode || AUDIO_STEREO_SPEAKER_HANDFREE_CLASS_D == stereo_speakerMode)
  {
	  newGainSettings.speakerAmpGain = *data;
	  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_gainControl( NULL,
				  BSP_TWL3029_AUDIO_GAIN_UPDATE_SPEAKERAMP,
				  &newGainSettings))
	  {
		  return(AUDIO_ERROR);
	  }
  }
  else if(AUDIO_STEREO_SPEAKER_HEADPHONE == stereo_speakerMode )
  {
  	  newGainSettings.earGain = *data;
  	  if(BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_gainControl(NULL,
  				  BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_RIGHT | BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_LEFT,
  				  &newGainSettings))
  	  {
  		  return(AUDIO_ERROR);
  	  }
  }
  else
  {
	 AUDIO_SEND_TRACE("Check You are Wrong ", RV_TRACE_LEVEL_ERROR);
	 return(AUDIO_ERROR);
  }
#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_filter_write                          */
  /*                                                                              */
  /*    Purpose:  This function enable/disable the DL filter.                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new DL filter state.                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_filter_write (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbcr;

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif ((ANLG_FAM == 2) || (ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
    #endif

    switch (*data)
    {
      case AUDIO_SPEAKER_FILTER_ON:
      {
        vbcr &= ~(AUDIO_VBCR_VFBYP);
        ABB_Audio_Config(vbcr);
        break;
      }
      case AUDIO_SPEAKER_FILTER_OFF:
      {
        vbcr |= (AUDIO_VBCR_VFBYP);
        ABB_Audio_Config(vbcr);
        break;
      }
      default :
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker filter: this state isn't supported",
          *data,
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
        break;
      }
    }
#else
    UINT8 filterSetting;

    switch (*data)
    {
      case AUDIO_SPEAKER_FILTER_ON:
      {
        filterSetting = BSP_TWL3029_AUDIO_FILTER_ENABLE;
        break;
      }
      case AUDIO_SPEAKER_FILTER_OFF:
      {
        filterSetting = BSP_TWL3029_AUDIO_FILTER_BYPASS;
        break;
      }
      default :
      {
        //AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker filter: this state isn't supported",
         // (*data),
          //RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    }
    if( BSP_TWL3029_RETURN_CODE_SUCCESS !=
	 	bspTwl3029_Audio_setFilterState(NULL,
	 						BSP_TWL3029_AUDIO_FILTER_VOICE_DOWNLINK_PATH,
							filterSetting))
    {
	    return(AUDIO_ERROR);
    }

    //audio_mode_configuration.audio_speaker_filter = *data;
#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_highpass_filter_write                 */
  /*                                                                              */
  /*    Purpose:  This function enable/disable the highpass DL filter.            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new DL filter state.                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_highpass_filter_write (INT8  *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
    UINT16 vaudctrl = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vaud_cfg >> 6);

    switch (*data)
    {
      case AUDIO_SPEAKER_HIGHPASS_FILTER_ON:
      {
        vaudctrl &= ~(AUDIO_VAUDCTRL_HPFBYP);
        ABB_Audio_Control(vaudctrl);
        break;
      }
      case AUDIO_SPEAKER_HIGHPASS_FILTER_OFF:
      {
        vaudctrl |= (AUDIO_VAUDCTRL_HPFBYP);
        ABB_Audio_Control(vaudctrl);
        break;
      }
      default :
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker highpass filter: this state isn't supported",
          *data, RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
        break;
      }
    }
  #else
    AUDIO_SEND_TRACE("AUDIO MODE WRITE: speaker highpass filter: feature not supported",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else
    UINT8 filterSetting;

    switch (*data)
    {
      case AUDIO_SPEAKER_HIGHPASS_FILTER_ON:
      {
        filterSetting = BSP_TWL3029_AUDIO_FILTER_ENABLE;
        break;
      }
      case AUDIO_SPEAKER_HIGHPASS_FILTER_OFF:
      {
        filterSetting = BSP_TWL3029_AUDIO_FILTER_BYPASS;
        break;
      }
      default :
      {
        //AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker highpass filter: this state isn't supported",
         // (*data), RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    }

    if( BSP_TWL3029_RETURN_CODE_SUCCESS !=
		bspTwl3029_Audio_setFilterState(NULL,
			     BSP_TWL3029_AUDIO_FILTER_VOICE_UPLINK_HIGH_PASS,
			     filterSetting))
    {
	     return(AUDIO_ERROR);
    }

    //audio_mode_configuration.audio_speaker_highpass_filter = *data;
#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_buzzer_write                           */
  /*                                                                              */
  /*    Purpose:  This function enable/disable the buzzer.                        */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new buzzer state.                                                     */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_buzzer_write (INT8  *data)
  {
    #if (ANLG_FAM== 1)
      UINT16 vbcr;

      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);

      switch (*data)
      {
        case AUDIO_SPEAKER_BUZZER_OFF:
        {
          /* Reset aux output amplifier and the ear amplifier */
          vbcr &= ~(AUDIO_VBCR_VDLEAR | AUDIO_VBCR_VDLAUX);

          vbcr &= ~(AUDIO_VBCR_VBUZ);
          ABB_Audio_Config(vbcr);
          break;
        }
        case AUDIO_SPEAKER_BUZZER_ON:
        {
          /* Reset aux output amplifier and the ear amplifier */
          vbcr &= ~(AUDIO_VBCR_VDLEAR | AUDIO_VBCR_VDLAUX);

          vbcr |= (AUDIO_VBCR_VBUZ);
          ABB_Audio_Config(vbcr);
          break;
        }
        default :
        {
          AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: buzzer: this state isn't supported",
            *data,
            RV_TRACE_LEVEL_ERROR);
          return(AUDIO_ERROR);
          break;
        }
      }
      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: buzzer: this feature isn't supported by the current analog base band",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_sidetone_gain_write                            */
  /*                                                                              */
  /*    Purpose:  This function set the sidetone gain.                            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new sidetone gain.                                                    */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
 #if (AUDIO_DSP_FEATURES == 1)
  T_AUDIO_RET audio_mode_sidetone_gain_write (INT16  *data)
  {
      L1_audio_sidetone_write((UWORD16) (*data));

      l1a_l1s_com.audioIt_task.command.start = 1;
 #else
  T_AUDIO_RET audio_mode_sidetone_gain_write (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT8  sidetone_index;

    sidetone_index = (UINT8)(25*((*data + 26)/3));

    ABB_SideTone (sidetone_index);
#else
    BspTwl3029_Audio_GainSettings newGainSettings;

	if(AUDIO_SIDETONE_MUTE == *data)
	{
    	newGainSettings.sidetoneLevel = BSP_TWL3029_AUDIO_SIDETONE_LEVEL_MUTE;
	}
	else
	{
	    newGainSettings.sidetoneLevel = ABB_sidetone_gain[(*data + 23) / 3];
	}

    if( BSP_TWL3029_RETURN_CODE_SUCCESS !=
		bspTwl3029_Audio_gainControl( NULL,
			    BSP_TWL3029_AUDIO_GAIN_UPDATE_SIDETONE,
			    &newGainSettings))
    {
	    return(AUDIO_ERROR);
    }

	//audio_mode_configuration.audio_sidetone_gain = *data;
#endif
 #endif
    return (AUDIO_OK);
  }


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_aec_write                                      */
  /*                                                                              */
  /*    Purpose:  This function set the AEC.                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new AEC setting.                                                      */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  #if (L1_AEC == 1)
  T_AUDIO_RET audio_mode_aec_write (T_AUDIO_AEC_CFG *data)
  {
    #if (L1_NEW_AEC)
      DummyStruct *p_send_message = NULL;

      /* Send the FIR configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AEC_REQ));
      if (p_send_message != NULL)
      {
        /* Fill the parameter */
      #if (L1_ANR == 0)
        ((T_MMI_AEC_REQ *)p_send_message)->aec_control =
                           (UINT16)( (data->aec_enable | 0x0080) |
                           (data->noise_suppression_enable | 0x0100) |
                           (data->noise_suppression_level) |
                           (data->aec_visibility) );
      #else
                ((T_MMI_AEC_REQ *)p_send_message)->aec_control =
                           (UINT16)( (data->aec_enable | 0x0080) |
                           (data->aec_visibility) );
      #endif

        ((T_MMI_AEC_REQ *)p_send_message)->cont_filter     = data->continuous_filtering;
        ((T_MMI_AEC_REQ *)p_send_message)->granularity_att = data->granularity_attenuation;
        ((T_MMI_AEC_REQ *)p_send_message)->coef_smooth     = data->smoothing_coefficient;
        ((T_MMI_AEC_REQ *)p_send_message)->es_level_max    = data->max_echo_suppression_level;
        ((T_MMI_AEC_REQ *)p_send_message)->fact_vad        = data->vad_factor;
        ((T_MMI_AEC_REQ *)p_send_message)->thrs_abs        = data->absolute_threshold;
        ((T_MMI_AEC_REQ *)p_send_message)->fact_asd_fil    = data->factor_asd_filtering;
        ((T_MMI_AEC_REQ *)p_send_message)->fact_asd_mut    = data->factor_asd_muting;

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_AEC_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: AEC: the audio AEC message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    #elif (L1_AEC == 1)
        DummyStruct *p_send_message = NULL;

      /* Send the FIR configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AEC_REQ));
      if (p_send_message != NULL)
      {
        /* Fill the parameter */
        ((T_MMI_AEC_REQ *)p_send_message)->aec_control =
                 (UINT16)( (data->aec_enable | 0x0080) |
                           (data->aec_mode) |
                           (data->echo_suppression_level)
                          #if (L1_ANR == 0)
                           |
                           (data->noise_suppression_enable | 0x0100) |
                           (data->noise_suppression_level)
                          #endif
                           );

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_AEC_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: AEC: the audio AEC message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    #else
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: AEC: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
    return (AUDIO_OK);
  }
  #endif
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_aec_write                                      */
  /*                                                                              */
  /*    Purpose:  This function set the AEC.                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new AEC setting.                                                      */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  #if (L1_AEC == 2)
  T_AUDIO_RET audio_mode_aec_write (T_AUDIO_AQI_AEC_CFG *data)
  {
      DummyStruct *p_send_message = NULL;

      /* Send the FIR configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AQI_AEC_REQ));
      if (p_send_message != NULL)
      {
        /* Fill the parameter */
              ((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_control = (T_AEC_CONTROL)data->aec_control;//omaps00090550
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.aec_mode = data->aec_mode;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.mu = data->mu;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.cont_filter = data->cont_filter;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.scale_input_ul = data->scale_input_ul;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.scale_input_dl = data->scale_input_dl;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.div_dmax = data->div_dmax;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.div_swap_good = data->div_swap_good;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.div_swap_bad = data->div_swap_bad;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.block_init = data->block_init;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.fact_vad = data->fact_vad;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.fact_asd_fil = data->fact_asd_fil;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.fact_asd_mut = data->fact_asd_mut;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.thrs_abs = data->thrs_abs;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.es_level_max = data->es_level_max;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.granularity_att = data->granularity_att;
		((T_MMI_AQI_AEC_REQ *)p_send_message)->aec_parameters.coef_smooth = data->coef_smooth;

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_AQI_AEC_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: AEC: the audio AEC message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
        return(AUDIO_OK);
  }
  #endif


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_stereo_speaker_mode_write                     */
  /*                                                                              */
  /*    Purpose:  This function set a new stereo speaker mode.                    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new speaker mode.                                                     */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_stereo_speaker_mode_write (INT8  *data)
  {
    void *p_send_message;
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM== 3) || (ANLG_FAM == 4))
    UINT16 vauoctrl = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vauo_onoff >> 6);
    INT8 *dat;

    /* Reset configuration */
    /* Reset the HSO (but not audio part) */
    vauoctrl &= (~(AUDIO_VAUOCTRL_HSOL_AUDIO | AUDIO_VAUOCTRL_HSOR_AUDIO));
    /* Reset the AUX configuration (but not audio part) */
    vauoctrl &= (~AUDIO_VAUOCTRL_AUX_AUDIO);
    /* Reset the SPK configuration (but not audio part) */
    vauoctrl &= (~AUDIO_VAUOCTRL_SPK_AUDIO);
    /* Reset the EAR configuration (but not audio part) */
    vauoctrl &= (~AUDIO_VAUOCTRL_EAR_AUDIO);

    switch (*data)
    {
      case AUDIO_STEREO_SPEAKER_HEADPHONE:
      {
        vauoctrl |= (AUDIO_VAUOCTRL_HSOR_AUDIO | AUDIO_VAUOCTRL_HSOL_AUDIO);
        ABB_Audio_On_Off(vauoctrl);
      }
      break;
      case AUDIO_STEREO_SPEAKER_HANDHELD:
      {
        vauoctrl |= (AUDIO_VAUOCTRL_EAR_AUDIO);
        ABB_Audio_On_Off(vauoctrl);
        *dat = AUDIO_MONO_LEFT;
        audio_mode_stereo_speaker_stereo_mono_write(dat);
      }
      break;
      case AUDIO_STEREO_SPEAKER_HANDFREE:
      {
        vauoctrl |= (AUDIO_VAUOCTRL_AUX_AUDIO);
        ABB_Audio_On_Off(vauoctrl);
        *dat = AUDIO_MONO_LEFT;
        audio_mode_stereo_speaker_stereo_mono_write(dat);
      }
      break;
      case AUDIO_STEREO_SPEAKER_HANDHELD_8OHM:
      {
        vauoctrl |= AUDIO_VAUOCTRL_SPK_AUDIO;
        ABB_Audio_On_Off(vauoctrl);
        *dat = AUDIO_MONO_LEFT;
        audio_mode_stereo_speaker_stereo_mono_write(dat);
      }
      break;
      default:
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: speaker mode: this mode isn't supported",
          *data, RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
      break;
    }
    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("AUDIO MODE WRITE: stereo speaker mode: this feature isn't supported by the current analog base band",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif // #if(ANLG_FAM == 3) || (ANLG_FAM == 4)
#else
    BspTwl3029_Audio_OutputPathId audioPath = BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;
    BspTwl3029_Audio_OutputPathId audioPath_temp;
    INT8 dat;
    BspTwl3029_AudioOutputSource outputSource[6];
    Uint8 classDmode;

    /* FOUR Steps are required
    * 1. First reset the current Audio Path i.e. wherever audio is routed to
    * 2. Write the triton registers to reset first
    * 3. Set according what *data indicates i.e. route audio correctly
    * 4. Configure triton registers accordingly *
    */

    /* STEP 1 -> Reset current Audio path settings and to
     * reset ONLY audio path settings i.e IF audio + voice is
     * routed to handheld, voice still needs to be routed to handheld */

    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getOutputPath(&audioPath_temp, &outputSource))
    {
      return AUDIO_ERROR;
    }

    // Handheld
    /* ideally
     * if (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET]
     *        == BSP_TWL3029_AUDIO_OUTPUT_PATH_AUDIO)
     * {
     *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] =
     *      BSP_TWL3029_AUDIO_OUTPUT_PATH_OFF;
     * }
     * if (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET]
     *        == BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_AUDIO)
     * {
     *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] =
     *      BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE;
     * }
     * Same could be established by AND with ~(0x02)
     */
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] & 0x2)
    {
      audioPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE;
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] &= (~(0x2));
    }

    // AUX
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] & 0x2)
    {
      /* Same comment as handheld */
      audioPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_AUX;
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] &= (~(0x2));
    }

    // Carkit
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] & 0x2)
    {
      /* Same comment as handheld */
      audioPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_CARKIT;
      outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] &= (~(0x2));
    }


    // Headset
    {
      Uint8 headset_flag = 0;
      if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO)
      {
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
        headset_flag = 1;
      }
      if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO) ||
          (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM))
      {
       /* BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO  = 4 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE (1)
        * BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM     = 6 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM (3)
        * */
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] -= 3;
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] -= 3;
        headset_flag = 1;
      }
       #if(AS_FM_RADIO==1)
	    if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM))
      {
       /* BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO  = 4 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE (1)
        * BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM     = 6 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM (3)
        * */
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET]= BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
        headset_flag = 1;
      }
		#endif
      if(headset_flag == 1){
        audioPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE |
              BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE);
      }
    }

    // handfree and handfree class D Amplifier
    {
      Uint8 spk_flag = 0;
      if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getClassD_mode(&classDmode))
      {
        return AUDIO_ERROR;
      }
      if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
          BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO)
      {
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE;
        spk_flag = 1;
      }
      if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO) ||
          (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM))
      {
       /* BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO  = 4 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE (1)
        * BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM     = 6 -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM (3)
        * */
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] -= 3;
        spk_flag = 1;
      }
       #if(AS_FM_RADIO==1)
	  if((outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM))
	  	{
			 outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET]=  BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
			 spk_flag = 1;
	  	}
		#endif


      if(spk_flag == 1){
        if(classDmode == 1)
          audioPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK_DCLASS);
        else
          audioPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK);
      }
    }

    /* STEP2: reset triton register correctly */
    if ( BSP_TWL3029_RETURN_CODE_SUCCESS !=
             bspTwl3029_Audio_configureOutputPath( NULL,audioPath,outputSource))
    {
	    return(AUDIO_ERROR);
    }
    audioPath = BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE;

    /* STEP 3-> Set AUDIO path correctly as mentioned by *data */
    switch (*data)
    {
      case AUDIO_STEREO_SPEAKER_NONE:
      {
        audioPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_NONE);
      }
      break;
      case AUDIO_STEREO_SPEAKER_HANDHELD:
      {
        /* ideally
         * if (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET]
         *        == BSP_TWL3029_AUDIO_OUTPUT_PATH_OFF)
         * {
         *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] =
         *      BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
         * }
         * if (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET]
         *        == BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_ONLY)
         * {
         *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] =
         *      BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_AUDIO;
         * }
         * Same could be established by OR with (0x02)
         */
        audioPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE);
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] |= 0x2;
        dat = AUDIO_MONO_LEFT;
        if (audio_mode_stereo_speaker_stereo_mono_write(&dat) == AUDIO_ERROR){
          return AUDIO_ERROR;
        }
      }
      break;

      case AUDIO_STEREO_SPEAKER_AUX:
      {
        /* same comment as AUDIO_STEREO_SPEAKER_HANDHELD */
        audioPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_AUX);
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] |= 0x2;
        dat = AUDIO_MONO_LEFT;
        if (audio_mode_stereo_speaker_stereo_mono_write(&dat) == AUDIO_ERROR){
          return AUDIO_ERROR;
        }
      }
      break;
	  case AUDIO_STEREO_SPEAKER_CARKIT:
      {
        audioPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_CARKIT);
        outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] |= 0x2;

        /* if voice is ON then carkit needs to be mono, else it needs to be stereo */
        if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] & 0x1){
          dat = AUDIO_MONO_LEFT;
          if (audio_mode_stereo_speaker_stereo_mono_write(&dat) == AUDIO_ERROR){
            return AUDIO_ERROR;
          }
        }
      }
      break;

      case AUDIO_STEREO_SPEAKER_HEADPHONE:
      {
        audioPath |= (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE |
                       BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE);

        if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
              BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM) ||
            (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] ==
              BSP_TWL3029_AUDIO_OUTPUT_SOURCE_INVERT_VOICE) )
        {
          /* In case of already AUDIO + FM return error*/
          return AUDIO_ERROR;
        }
        if( outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] ==
              BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE)
        {
          outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
          outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
        }
        else
        {
          /* BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE(1) -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO(4)
           * BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM(3)    -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM(6)
           * */
          outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] += 3;
          outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] += 3;
        }
        dat = AUDIO_STEREO;
        audio_mode_stereo_speaker_stereo_mono_write(&dat);
      }
      break;
      case AUDIO_SPEAKER_HANDFREE:
	case AUDIO_STEREO_SPEAKER_HANDFREE_CLASS_D:
      {
        dat = AUDIO_MONO_LEFT;
        if (audio_mode_stereo_speaker_stereo_mono_write(&dat) == AUDIO_ERROR){
          return AUDIO_ERROR;
        }

        if(*data == AUDIO_SPEAKER_HANDFREE){
          audioPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK;
        }
        else{
       		audioPath |= BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK_DCLASS;
        }

        /* It is similar to AUDIO_SPEAKER_HEADSET */
        if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
            BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM)
        {
          /* In case of already VOICE + FM return error*/
          return AUDIO_ERROR;
        }

        if( outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] ==
              BSP_TWL3029_AUDIO_OUTPUT_SOURCE_NONE)
        {
          outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] = BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO;
        }
        else
        {
          /* BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE(1) -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO(4)
           * BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM(3)    -> BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM(6)
           * */
          outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] += 3;
        }
      }
      break;
      default:
      {
       // AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: stereo speaker mode: this mode isn't supported",
        //  (*data), RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    }


    // STEP4: write the triton registers for the new audio path
    audio_outen_reg_as.classD = 0x00;

	  if (BSP_TWL3029_RETURN_CODE_SUCCESS !=
            bspTwl3029_Audio_configureOutputPath( NULL,audioPath,outputSource))
    {
	    return(AUDIO_ERROR);
    }
     /* Send Audio Outen Reg config command to L1  */
    p_send_message = (void *)audio_allocate_l1_message(sizeof(T_MMI_OUTEN_CFG_REQ));
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->outen1 = audio_outen_reg_as.outen1;
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->outen2 = audio_outen_reg_as.outen2;
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->outen3 = audio_outen_reg_as.outen3;
    ((T_MMI_OUTEN_CFG_REQ *)p_send_message)->classD = audio_outen_reg_as.classD;


    if (p_send_message != NULL)
    {
       AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: stereo speaker mode: MMI_OUTEN_CFG_REQ outen1 : ",
          audio_outen_reg_as.outen1, RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("classD : ", audio_outen_reg_as.classD, RV_TRACE_LEVEL_DEBUG_LOW);
       AUDIO_SEND_TRACE_PARAM("outen3 : ", audio_outen_reg_as.outen3, RV_TRACE_LEVEL_DEBUG_LOW);

       audio_send_l1_message(MMI_OUTEN_CFG_REQ, p_send_message);
    }


	return (AUDIO_OK);

#endif

  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_stereo_speaker_stereo_mono_write              */
  /*                                                                              */
  /*    Purpose:  This function set a new stereo speaker stereo/mono conversion   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new stereo/mono conversion.                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_stereo_speaker_stereo_mono_write(INT8  *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
    UINT16 vaudctrl = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vaud_cfg >> 6);

    /* Reset configuration => this is AUDIO_STEREO configuration */
    vaudctrl &= (~(AUDIO_VAUDCTRL_MONOR | AUDIO_VAUDCTRL_MONOL));

    if (*data & AUDIO_MONO_RIGHT)
    {
      vaudctrl |= AUDIO_VAUDCTRL_MONOR;
    }
    if (*data & AUDIO_MONO_LEFT)
    {
      vaudctrl |= AUDIO_VAUDCTRL_MONOL;
    }
    ABB_Audio_Control(vaudctrl);
    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("AUDIO MODE WRITE: stereo speaker stereo/mono: this feature isn't supported by the current analog base band",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else
    /* Separate function has to be created to set/reset audio mono option */
    AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: stereo/mono",*data,RV_TRACE_LEVEL_ERROR);

    if( BSP_TWL3029_RETURN_CODE_SUCCESS != \
		bspTwl3029_Audio_stereoMonoConfigure (NULL, *data))
    {
	return (AUDIO_ERROR);
    }

    return AUDIO_OK;

#endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_stereo_speaker_sampling_freq_write            */
  /*                                                                              */
  /*    Purpose:  This function set a new stereo speaker sampling rate freq       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new sampling rate freq.                                               */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_stereo_speaker_sampling_freq_write   (INT8  *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
    UINT16 vaudctrl = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vaud_cfg >> 6);

    /* Reset configuration */
    vaudctrl &= (~(AUDIO_VAUDCTRL_SRW2 | AUDIO_VAUDCTRL_SRW1 | AUDIO_VAUDCTRL_SRW0));

    switch(*data)
    {
      case AUDIO_STEREO_SAMPLING_FREQUENCY_48KHZ:     vaudctrl |= AUDIO_VAUDCTRL_SRW_48KHZ; break;
      case AUDIO_STEREO_SAMPLING_FREQUENCY_44_1KHZ:   vaudctrl |= AUDIO_VAUDCTRL_SRW_44_1KHZ; break;
      case AUDIO_STEREO_SAMPLING_FREQUENCY_32KHZ:     vaudctrl |= AUDIO_VAUDCTRL_SRW_32KHZ; break;
      case AUDIO_STEREO_SAMPLING_FREQUENCY_22_05KHZ:  vaudctrl |= AUDIO_VAUDCTRL_SRW_22_05KHZ; break;
      case AUDIO_STEREO_SAMPLING_FREQUENCY_16KHZ:     vaudctrl |= AUDIO_VAUDCTRL_SRW_16KHZ; break;
      case AUDIO_STEREO_SAMPLING_FREQUENCY_11_025KHZ: vaudctrl |= AUDIO_VAUDCTRL_SRW_11_025KHZ; break;
      case AUDIO_STEREO_SAMPLING_FREQUENCY_8KHZ:      vaudctrl |= AUDIO_VAUDCTRL_SRW_8KHZ; break;
      default:
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: stereo speaker sampling frequency: this frequency isn't supported",
          *data, RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
      break;
    }
    ABB_Audio_Control(vaudctrl);
    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("AUDIO MODE WRITE: stereo speaker sampling frequency: this feature isn't supported by the current analog base band",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else
    /* Separate function has to be created as part of TRITON driver to set/reset audio
     * mono option */
    BspTwl3029_Audio_StereoSamplingRate samplingFrequency;

    switch(*data)
    {
      case AUDIO_STEREO_SAMPLING_FREQUENCY_48KHZ:
      {
	  samplingFrequency = BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_48KHZ;
	  break;
      }
      case AUDIO_STEREO_SAMPLING_FREQUENCY_44_1KHZ:
      {
	  samplingFrequency = BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_44_1KHZ;
	  break;
      }
      case AUDIO_STEREO_SAMPLING_FREQUENCY_32KHZ:
      {
          samplingFrequency = BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_32KHZ;
	  break;
      }
      case AUDIO_STEREO_SAMPLING_FREQUENCY_24KHZ:
      {
          samplingFrequency = BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_24KHZ;
	  break;
      }
      case AUDIO_STEREO_SAMPLING_FREQUENCY_22_05KHZ:
      {
	  samplingFrequency = BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_22_05KHZ;
	  break;
      }
      case AUDIO_STEREO_SAMPLING_FREQUENCY_16KHZ:
      {
	  samplingFrequency = BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_16KHZ;
	  break;
      }
      case AUDIO_STEREO_SAMPLING_FREQUENCY_12KHZ:
      {
	  samplingFrequency = BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_12KHZ;
	  break;
      }
      case AUDIO_STEREO_SAMPLING_FREQUENCY_11_025KHZ:
      {
	  samplingFrequency = BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_11_025KHZ;
	  break;
      }
      case AUDIO_STEREO_SAMPLING_FREQUENCY_8KHZ:
      {
	  samplingFrequency = BSP_TWL3029_AUDIO_STEREO_DAC_SAMPLING_RATE_8KHZ;
	  break;
      }
      default:
      {
        //AUDIO_SEND_TRACE_PARAM("AUDIO MODE WRITE: stereo speaker sampling frequency: this frequency isn't supported",
         // (*data), RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    }

    if(BSP_TWL3029_RETURN_CODE_SUCCESS !=
		    bspTwl3029_Audio_stereoSamplingRateConfigure( NULL,
			    samplingFrequency))
    {
	  return(AUDIO_ERROR);
    }

    //audio_mode_configuration.audio_Stereo_speaker_sampling_freq = *data;

    return  (AUDIO_OK);
#endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_volume_write                           */
  /*                                                                              */
  /*    Purpose:  This function set the speaker volume.                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new speaker volume.                                                   */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_volume_write (T_AUDIO_SPEAKER_LEVEL *data)
  {
#if (ANLG_FAM != 11)
    ABB_DlVolume (data->audio_speaker_level);
#else
    BspTwl3029_Audio_GainVDLSettings newGainVDLSettings;
	UINT8 volume_index = data->audio_speaker_level;
    UINT16 volume;

    // Normalize volume (0 to 5)
    if (volume_index > MAX_VOL_DL) volume_index=MAX_VOL_DL;   //clip

  	if (volume_index)
    	volume = (volume_index / 50) + 1;
  	else
    	volume = volume_index;

    if( volume >= 6)
	return (AUDIO_ERROR);

    newGainVDLSettings.vdlCourseGain = ABB_volume_control_gain[volume];

    bspTwl3029_Audio_gainVDLConfig( NULL, BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_COURSE,
								&newGainVDLSettings);

   // audio_mode_configuration.audio_speaker_volume.audio_speaker_level =
	//					data->audio_speaker_level;

#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_volume_write                           */
  /*                                                                              */
  /*    Purpose:  This function set the speaker volume.                           */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new speaker volume.                                                   */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/

  T_AUDIO_RET audio_mode_stereo_speaker_volume_write (T_AUDIO_STEREO_SPEAKER_LEVEL *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
    UINT16 vausctrl = 0;

    if (data->audio_stereo_speaker_level_left == AUDIO_STEREO_SPEAKER_VOLUME_MUTE)
    {
      vausctrl |= AUDIO_VAUSCTRL_AULGA0 | AUDIO_VAUSCTRL_AULGA1 | AUDIO_VAUSCTRL_AULGA2
                  | AUDIO_VAUSCTRL_AULGA3 | AUDIO_VAUSCTRL_AULGA4;
    }
    else
    {
      vausctrl |= (UINT16)data->audio_stereo_speaker_level_left;
    }

    if (data->audio_stereo_speaker_level_right == AUDIO_STEREO_SPEAKER_VOLUME_MUTE)
    {
      vausctrl |= AUDIO_VAUSCTRL_AURGA0 | AUDIO_VAUSCTRL_AURGA1 | AUDIO_VAUSCTRL_AURGA2
                  | AUDIO_VAUSCTRL_AURGA3 | AUDIO_VAUSCTRL_AURGA4;
    }
    else
    {
      vausctrl |= ((UINT16)(data->audio_stereo_speaker_level_right) << 5);
    }

    ABB_Audio_Volume (vausctrl);

    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("AUDIO MODE WRITE: stereo speaker volume: this feature isn't supported by the current analog base band",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else
    BspTwl3029_Audio_GainSettings newGainSettings;

    newGainSettings.stereoVolRight = data->audio_stereo_speaker_level_right;
    newGainSettings.stereoVolLeft = data->audio_stereo_speaker_level_left ;

    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_gainControl( NULL,
		    BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_RIGHT | \
		    BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_LEFT,
		    &newGainSettings))
    {
        return(AUDIO_ERROR);
    }
#if 0
    audio_mode_configuration.audio_stereo_speaker_volume.audio_stereo_speaker_level_left = \
		data->audio_stereo_speaker_level_left;

    audio_mode_configuration.audio_stereo_speaker_volume.audio_stereo_speaker_level_right = \
		data->audio_stereo_speaker_level_right;
#endif
	return AUDIO_OK;

#endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_microphone_fir_write                  */
  /*                                                                              */
  /*    Purpose:  This function set the speaker and microphone FIR.               */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        new speaker or/and microphone FIR.                                    */
  /*        Note: if one of this two pointer equals NULL, it means corresponding  */
  /*              FIR doesn't need to be updated.                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        The data buffer must be deallocate only when the FIR confirmation is  */
  /*        received.                                                             */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_microphone_fir_write   (T_AUDIO_FIR_COEF *data_speaker,
                                                         T_AUDIO_FIR_COEF *data_microphone)
  {
    #if (FIR)
      DummyStruct *p_send_message = NULL;

      if ( (data_speaker == NULL) &&
           (data_microphone == NULL) )
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: FIR: wrong arguments",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }

      /* Send the FIR configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AUDIO_FIR_REQ));
      if (p_send_message != NULL)
      {
        if (data_speaker == NULL)
        {
          /* Fill the parameter */
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->update_fir = UL_FIR;
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->fir_loop = FALSE;
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->fir_dl_coefficient = NULL;
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->fir_ul_coefficient = &(data_microphone->coefficient[0]);
        }
        else
        if (data_microphone == NULL)
        {
          /* Fill the parameter */
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->update_fir = DL_FIR;
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->fir_loop = FALSE;
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->fir_dl_coefficient = &(data_speaker->coefficient[0]);
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->fir_ul_coefficient = NULL;
        }
        else
        {
          /* Fill the parameter */
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->update_fir = UL_DL_FIR;
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->fir_loop = FALSE;
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->fir_dl_coefficient = &(data_speaker->coefficient[0]);
          ((T_MMI_AUDIO_FIR_REQ *)p_send_message)->fir_ul_coefficient = &(data_microphone->coefficient[0]);
        }

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_AUDIO_FIR_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: FIR: the audio FIR message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    #else
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: FIR: FIR isn't supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif

    return (AUDIO_OK);
  }

  #if (AUDIO_DSP_FEATURES == 1)
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_volume_speed_write                            */
  /*                                                                              */
  /*    Purpose:  This function set the speed of the volume change                */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        data  : value of the speed for the volume change                      */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_volume_speed_write   (INT16* data)
  {
    L1_audio_volume_speed_write((UWORD16) (*data));

    l1a_l1s_com.audioIt_task.command.start = 1;
    return(AUDIO_OK);
  }
  #endif


  #if (AUDIO_DSP_ONOFF == 1 )
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_onoff_write                                   */
  /*                                                                              */
  /*    Purpose:  This function enable or disable the audio automatic switch off  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        AUDIO_ON  : audio part of the ABB should always stay switch on        */
  /*        AUDIO_OFF : automatic audio ABB switch off is enabled                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_onoff_write   (INT8* data)
  {
  #if (PSP_STANDALONE != 1)
    void *p_send_message = NULL;

    /* allocate the buffer for the message to the L1 */
    p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AUDIO_ONOFF_REQ));

    if (p_send_message != NULL)
    {
      /* Fill the parameter */
      ((T_MMI_AUDIO_ONOFF_REQ *)p_send_message)->onoff_value = *data;

      /* send the start command to the audio L1 */
      audio_send_l1_message(MMI_AUDIO_ONOFF_REQ, p_send_message);
    }
    else
    {
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: AUDIO ON/OFF: the audio ON/OFF message isn't sent",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    }
   return (AUDIO_OK);
   #else
   return (AUDIO_ERROR);
   #endif
  }
  #endif

#if (L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1 )
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_onoff_write                                   */
  /*                                                                              */
  /*    Purpose:  This function enable or disable the audio automatic switch off  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        AUDIO_ON  : audio part of the ABB should always stay switch on        */
  /*        AUDIO_OFF : automatic audio ABB switch off is enabled                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_onoff_write   (T_AUDIO_MODE_ONOFF* data)
  {
  #if (PSP_STANDALONE != 1)
    void *p_send_message = NULL;

    /* allocate the buffer for the message to the L1 */
    p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AUDIO_ONOFF_REQ));

    if (p_send_message != NULL)
    {
      /* Fill the parameter */
      ((T_MMI_AUDIO_ONOFF_REQ *)p_send_message)->vul_onoff_value = data->vul_onoff;
      ((T_MMI_AUDIO_ONOFF_REQ *)p_send_message)->vdl_onoff_value = data->vdl_onoff;

      /* send the start command to the audio L1 */
      audio_send_l1_message(MMI_AUDIO_ONOFF_REQ, p_send_message);
    }
    else
    {
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: AUDIO ON/OFF: the audio ON/OFF message isn't sent",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    }
   return (AUDIO_OK);
   #else
   return (AUDIO_ERROR);
   #endif
  }
  #endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_microphone_anr_write                           */
  /*                                                                              */
  /*    Purpose:  This function set the ANR.                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        ANR setting.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_ANR ==1)

  T_AUDIO_RET audio_mode_microphone_anr_write (T_AUDIO_ANR_CFG *data)
  {
    #if (L1_ANR ==1)
      DummyStruct *p_send_message = NULL;

      /* Send the ANR configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_ANR_REQ));
      if (p_send_message != NULL)
      {
        /* Fill the parameter */
        ((T_MMI_ANR_REQ *)p_send_message)->ns_level         = data->ns_level;
        ((T_MMI_ANR_REQ *)p_send_message)->div_factor_shift = data->div_factor_shift;
        ((T_MMI_ANR_REQ *)p_send_message)->min_gain         = data->min_gain;
        ((T_MMI_ANR_REQ *)p_send_message)->anr_enable       = data->anr_enable;

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_ANR_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: ANR: the audio ANR message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    #else
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: ANR: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
    return (AUDIO_OK);
  }
#endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_microphone_anr_write                           */
  /*                                                                              */
  /*    Purpose:  This function set the ANR.                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        ANR setting.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  #if (L1_ANR == 2)
  T_AUDIO_RET audio_mode_microphone_anr_write (T_AUDIO_AQI_ANR_CFG *data)
  {
    #if (L1_ANR == 2)
      DummyStruct *p_send_message = NULL;

      /* Send the ANR configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AQI_ANR_REQ));
      if (p_send_message != NULL)
      {
        ((T_MMI_AQI_ANR_REQ *)p_send_message)->anr_ul_control        = (T_ANR_CONTROL)data->anr_ul_control;

        /* Fill the parameter */
        ((T_MMI_AQI_ANR_REQ *)p_send_message)->parameters.control  = data->parameters.control;
        ((T_MMI_AQI_ANR_REQ *)p_send_message)->parameters.ns_level = data->parameters.ns_level;
        ((T_MMI_AQI_ANR_REQ *)p_send_message)->parameters.tone_cnt_th = data->parameters.tone_cnt_th;
        ((T_MMI_AQI_ANR_REQ *)p_send_message)->parameters.tone_ene_th = data->parameters.tone_ene_th;

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_AQI_ANR_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: ANR: the audio ANR message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    #else
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: ANR: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
    return (AUDIO_OK);
  }
  #endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_microphone_es_write                            */
  /*                                                                              */
  /*    Purpose:  This function set the ES.                                       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        ES setting.                                                           */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_ES ==1)
  T_AUDIO_RET audio_mode_microphone_es_write (T_AUDIO_ES_CFG *data)
  {
      DummyStruct *p_send_message = NULL;
      UINT8       i;

      /* Send the ES configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_ES_REQ));
      if (p_send_message != NULL)
      {
        /* Fill the parameter */
        ((T_MMI_ES_REQ *)p_send_message)->es_enable             = data->es_enable;
        ((T_MMI_ES_REQ *)p_send_message)->es_behavior           = data->es_behavior;

        ((T_MMI_ES_REQ *)p_send_message)->es_mode               = data->es_mode;
        ((T_MMI_ES_REQ *)p_send_message)->es_gain_dl            = data->es_gain_dl;
        ((T_MMI_ES_REQ *)p_send_message)->es_gain_ul_1          = data->es_gain_ul_1;
        ((T_MMI_ES_REQ *)p_send_message)->es_gain_ul_2          = data->es_gain_ul_2;
        ((T_MMI_ES_REQ *)p_send_message)->tcl_fe_ls_thr         = data->tcl_fe_ls_thr;
        ((T_MMI_ES_REQ *)p_send_message)->tcl_dt_ls_thr         = data->tcl_dt_ls_thr;
        ((T_MMI_ES_REQ *)p_send_message)->tcl_fe_ns_thr         = data->tcl_fe_ns_thr;
        ((T_MMI_ES_REQ *)p_send_message)->tcl_dt_ns_thr         = data->tcl_dt_ns_thr;
        ((T_MMI_ES_REQ *)p_send_message)->tcl_ne_thr            = data->tcl_ne_thr;
        ((T_MMI_ES_REQ *)p_send_message)->ref_ls_pwr            = data->ref_ls_pwr;
        ((T_MMI_ES_REQ *)p_send_message)->switching_time        = data->switching_time;
        ((T_MMI_ES_REQ *)p_send_message)->switching_time_dt     = data->switching_time_dt;
        ((T_MMI_ES_REQ *)p_send_message)->hang_time             = data->hang_time;
        ((T_MMI_ES_REQ *)p_send_message)->gain_lin_dl_vect[0]   = data->gain_lin_dl_vect[0];
        ((T_MMI_ES_REQ *)p_send_message)->gain_lin_dl_vect[1]   = data->gain_lin_dl_vect[1];
        ((T_MMI_ES_REQ *)p_send_message)->gain_lin_dl_vect[2]   = data->gain_lin_dl_vect[2];
        ((T_MMI_ES_REQ *)p_send_message)->gain_lin_dl_vect[3]   = data->gain_lin_dl_vect[3];
        ((T_MMI_ES_REQ *)p_send_message)->gain_lin_ul_vect[0]   = data->gain_lin_ul_vect[0];
        ((T_MMI_ES_REQ *)p_send_message)->gain_lin_ul_vect[1]   = data->gain_lin_ul_vect[1];
        ((T_MMI_ES_REQ *)p_send_message)->gain_lin_ul_vect[2]   = data->gain_lin_ul_vect[2];
        ((T_MMI_ES_REQ *)p_send_message)->gain_lin_ul_vect[3]   = data->gain_lin_ul_vect[3];

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_ES_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: ES: the audio ES message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    return (AUDIO_OK);
  }
#endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_iir_write                              */
  /*                                                                              */
  /*    Purpose:  This function set the IIR.                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        IIR setting.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_IIR ==1 || L1_IIR == 2)

#if (L1_IIR ==1)
  T_AUDIO_RET audio_mode_speaker_iir_write (T_AUDIO_IIR_CFG *data)
#elif (L1_IIR == 2)
  T_AUDIO_RET audio_mode_speaker_iir_write (T_AUDIO_IIR_DL_CFG *data)
#endif
  {
    #if (L1_IIR == 1)
      DummyStruct *p_send_message = NULL;
      UINT16  i;

      /* Send the IIR configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_IIR_REQ));
      if (p_send_message != NULL)
      {
        /* Fill the parameter */
        ((T_MMI_IIR_REQ *)p_send_message)->fir_scaling    = data->fir_scaling ;
        ((T_MMI_IIR_REQ *)p_send_message)->input_gain_scaling  = data->input_gain_scaling;
        ((T_MMI_IIR_REQ *)p_send_message)->input_scaling  = data->input_scaling;
        ((T_MMI_IIR_REQ *)p_send_message)->nb_fir_coefs   = data->nb_fir_coefs;
        ((T_MMI_IIR_REQ *)p_send_message)->nb_iir_blocks  = data->nb_iir_blocks;
        ((T_MMI_IIR_REQ *)p_send_message)->output_gain    = data ->output_gain;
        ((T_MMI_IIR_REQ *)p_send_message)->output_gain_scaling = data->output_gain_scaling;
        ((T_MMI_IIR_REQ *)p_send_message)->feedback       = data->feedback;
        ((T_MMI_IIR_REQ *)p_send_message)->fir_coefs      = &(data->fir_coefs[0]);
        ((T_MMI_IIR_REQ *)p_send_message)->iir_coefs      = &(data->iir_coefs[0]);
        ((T_MMI_IIR_REQ *)p_send_message)->iir_enable     = data->iir_enable;

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_IIR_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: IIR: the audio IIR message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    #elif(L1_IIR == 2)
	DummyStruct *p_send_message = NULL;
	UINT16  i;

	/* Allocate L1 IIR 4.x request message*/
	p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AQI_IIR_DL_REQ));
	/* Populate the request message */
	if (p_send_message != NULL)
       {
         /* Fill the parameter */
	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->iir_dl_control = (T_IIR_CONTROL)data->iir_dl_control;
	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.control = data->parameters.control;
	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.frame_size = data->parameters.frame_size;
	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.fir_swap = data->parameters.fir_swap;
	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.fir_filter.fir_enable = data->parameters.fir_filter.fir_enable;
	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.fir_filter.fir_length = data->parameters.fir_filter.fir_length;
	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.fir_filter.fir_shift = data->parameters.fir_filter.fir_shift;
	  for(i = 0; i < AUDIO_IIR_MAX_FIR_TAPS; i++)
	  {
	  	((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.fir_filter.fir_taps[i] =
	  		data->parameters.fir_filter.fir_taps[i];
	  }
	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_enable = data->parameters.sos_filter.sos_enable;
	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_number = data->parameters.sos_filter.sos_number;

	  for(i = 0; i <AUDIO_IIR_MAX_IIR_BLOCKS ; i++)
	  {
	  	((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_filter[i].sos_fact =
			data->parameters.sos_filter.sos_filter[i].sos_fact ;
		((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_filter[i].sos_fact_form =
			data->parameters.sos_filter.sos_filter[i].sos_fact_form;
		((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_filter[i].sos_den[0] =
			data->parameters.sos_filter.sos_filter[i].sos_den[0];
		((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_filter[i].sos_den[1] =
			data->parameters.sos_filter.sos_filter[i].sos_den[1];
		((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_filter[i].sos_num[0] =
			data->parameters.sos_filter.sos_filter[i].sos_num[0];
		((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_filter[i].sos_num[1] =
			data->parameters.sos_filter.sos_filter[i].sos_num[1];
		((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_filter[i].sos_num[2] =
			data->parameters.sos_filter.sos_filter[i].sos_num[2];
		((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.sos_filter.sos_filter[i].sos_num_form =
			data->parameters.sos_filter.sos_filter[i].sos_num_form;
	  }

	  ((T_MMI_AQI_IIR_DL_REQ *)p_send_message)->parameters.gain = data->parameters.gain;

      	  /* Send the start message */
	  audio_send_l1_message(MMI_AQI_IIR_DL_REQ, p_send_message);
	}
	else
       {
         AUDIO_SEND_TRACE("AUDIO MODE WRITE: IIR: the audio IIR message isn't sent",
          RV_TRACE_LEVEL_ERROR);
         return(AUDIO_ERROR);
       }
    #else
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: IIR: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
    return (AUDIO_OK);
  }

#endif

 /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_drc_write                              */
  /*                                                                              */
  /*    Purpose:  This function set the DRC.                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        DRC setting.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_DRC ==1)

  T_AUDIO_RET audio_mode_speaker_drc_write (T_AUDIO_DRC_CFG *data)
  {
      DummyStruct *p_send_message = NULL;
      UINT16  i;

      /* Send the DRC configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AQI_DRC_REQ));

      if (p_send_message != NULL)
      {
        /* Fill the parameter */
     ((T_MMI_AQI_DRC_REQ *)p_send_message)->drc_dl_control    = (T_DRC_CONTROL)data->drc_dl_control ;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.speech_mode_samp_f   =
	 		data->parameters.speech_mode_samp_f ;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.num_subbands   =
	 		data->parameters.num_subbands;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.frame_len   =
	 		data->parameters.frame_len;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.expansion_knee_fb_bs   =
	 		data->parameters.expansion_knee_fb_bs;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.expansion_knee_md_hg   =
	 		data->parameters.expansion_knee_md_hg;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.expansion_ratio_fb_bs   =
	 		data->parameters.expansion_ratio_fb_bs;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.expansion_ratio_md_hg   =
	 		data->parameters.expansion_ratio_md_hg;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.max_amplification_fb_bs   =
	 		data->parameters.max_amplification_fb_bs;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.max_amplification_md_hg   =
	 		data->parameters.max_amplification_md_hg;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.compression_knee_fb_bs   =
	 		data->parameters.compression_knee_fb_bs;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.compression_knee_md_hg   =
	 		data->parameters.compression_knee_md_hg;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.compression_ratio_fb_bs   =
	 		data->parameters.compression_ratio_fb_bs;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.compression_ratio_md_hg   =
	 		data->parameters.compression_ratio_md_hg;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.energy_limiting_th_fb_bs   =
	 		data->parameters.energy_limiting_th_fb_bs;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.energy_limiting_th_md_hg   =
	 		data->parameters.energy_limiting_th_md_hg;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.limiter_threshold_fb   =
	 		data->parameters.limiter_threshold_fb;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.limiter_threshold_bs   =
	 		data->parameters.limiter_threshold_bs;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.limiter_threshold_md   =
	 		data->parameters.limiter_threshold_md;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.limiter_threshold_hg   =
	 		data->parameters.limiter_threshold_hg;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.limiter_hangover_spect_preserve   =
	 		data->parameters.limiter_hangover_spect_preserve;
	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.limiter_release_fb_bs   =
	 		data->parameters.limiter_release_fb_bs;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.limiter_release_md_hg   =
	 		data->parameters.limiter_release_md_hg;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.gain_track_fb_bs   =
	 		data->parameters.gain_track_fb_bs;
 	 ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.gain_track_md_hg   =
	 		data->parameters.gain_track_md_hg;

	for(i = 0; i < 17; i++)
	{
	   ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.low_pass_filter[i]   =
	 		data->parameters.low_pass_filter[i];
	   ((T_MMI_AQI_DRC_REQ *)p_send_message)->parameters.mid_band_filter[i]   =
	 		data->parameters.mid_band_filter[i];
	}
        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_AQI_DRC_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: DRC: the audio DRC message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }


    return (AUDIO_OK);
  }
#endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_limiter_write                          */
  /*                                                                              */
  /*    Purpose:  This function set the Limiter.                                  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        Limiter setting.                                                      */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_LIMITER ==1)
  T_AUDIO_RET audio_mode_speaker_limiter_write (T_AUDIO_LIMITER_CFG *data)
  {
    #if (L1_LIMITER)
      DummyStruct *p_send_message = NULL;
      UINT16  i;

      /* Send the Limiter configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_LIMITER_REQ));
      if (p_send_message != NULL)
      {
        /* Fill the parameter */
        ((T_MMI_LIMITER_REQ *)p_send_message)->block_size     = data->block_size;
        ((T_MMI_LIMITER_REQ *)p_send_message)->gain_fall      = data->gain_fall;
        ((T_MMI_LIMITER_REQ *)p_send_message)->gain_rise      = data->gain_rise;
        ((T_MMI_LIMITER_REQ *)p_send_message)->nb_fir_coefs   = data->nb_fir_coefs;
        ((T_MMI_LIMITER_REQ *)p_send_message)->slope_update_period= data->slope_update_period;
        ((T_MMI_LIMITER_REQ *)p_send_message)->thr_high_0     = data ->thr_high_0;
        ((T_MMI_LIMITER_REQ *)p_send_message)->thr_high_slope = data->thr_high_slope;
        ((T_MMI_LIMITER_REQ *)p_send_message)->thr_low_0      = data->thr_low_0;
        ((T_MMI_LIMITER_REQ *)p_send_message)->thr_low_slope  = data->thr_low_slope;
        ((T_MMI_LIMITER_REQ  *)p_send_message)->filter_coefs  = &(data->filter_coefs[0]);
        ((T_MMI_LIMITER_REQ *)p_send_message)->limiter_enable         = data->limiter_enable;

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_LIMITER_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: LIMITER: the audio LIMITER message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    #else
      AUDIO_SEND_TRACE("AUDIO MODE WRITE: LIMITER: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
    return (AUDIO_OK);
  }
#endif
/********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_agc_ul_write                           */
  /*                                                                              */
  /*    Purpose:  This function set the AGC_UL.                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        AGC PARAMS setting.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_AGC_UL ==1)
  T_AUDIO_RET audio_mode_agc_ul_write (T_AUDIO_AQI_AGC_UL_REQ *data)
  {
      DummyStruct *p_send_message = NULL;

      /* Send the ANR configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AQI_AGC_UL_REQ));
      if (p_send_message != NULL)
      {
        /* Fill the control value */
        ((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->agc_ul_control = (T_AGC_CONTROL)data->agc_ul_control;

        /* Fill the parameter */
        ((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.control = data->parameters.control;
        ((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.frame_size = data->parameters.frame_size;
        ((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.targeted_level         = data->parameters.targeted_level;
        ((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.signal_up       = data->parameters.signal_up;
        ((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.signal_down       = data->parameters.signal_down;
        ((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.max_scale       = data->parameters.max_scale;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_smooth_alpha       = data->parameters.gain_smooth_alpha;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_smooth_alpha_fast       = data->parameters.gain_smooth_alpha_fast;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_smooth_beta       = data->parameters.gain_smooth_beta;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_smooth_beta_fast       = data->parameters.gain_smooth_beta_fast;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_intp_flag       = data->parameters.gain_intp_flag;

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_AQI_AGC_UL_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: AGC_UL: the audio AGC_UL message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }

    return (AUDIO_OK);
  }
#endif

/********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_agc_dl_write                           */
  /*                                                                              */
  /*    Purpose:  This function set the AGC_DL.                                      */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        AGC PARAMS setting.                                                          */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_AGC_DL ==1)
  T_AUDIO_RET audio_mode_agc_dl_write (T_AUDIO_AQI_AGC_DL_REQ *data)
  {
      DummyStruct *p_send_message = NULL;

      /* Send the ANR configuration message */
      /* allocate the buffer for the message to the L1 */
      p_send_message = audio_allocate_l1_message(sizeof(T_MMI_AQI_AGC_DL_REQ));
      if (p_send_message != NULL)
      {
        /* Fill the control value */
        ((T_MMI_AQI_AGC_DL_REQ *)p_send_message)->agc_dl_control = (T_AGC_CONTROL)data->agc_dl_control;

        /* Fill the parameter */
        ((T_MMI_AQI_AGC_DL_REQ *)p_send_message)->parameters.control         = data->parameters.control;
        ((T_MMI_AQI_AGC_DL_REQ *)p_send_message)->parameters.frame_size = data->parameters.frame_size;
        ((T_MMI_AQI_AGC_DL_REQ *)p_send_message)->parameters.targeted_level         = data->parameters.targeted_level;
        ((T_MMI_AQI_AGC_DL_REQ *)p_send_message)->parameters.signal_up       = data->parameters.signal_up;
        ((T_MMI_AQI_AGC_DL_REQ *)p_send_message)->parameters.signal_down       = data->parameters.signal_down;
        ((T_MMI_AQI_AGC_DL_REQ *)p_send_message)->parameters.max_scale       = data->parameters.max_scale;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_smooth_alpha       = data->parameters.gain_smooth_alpha;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_smooth_alpha_fast       = data->parameters.gain_smooth_alpha_fast;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_smooth_beta       = data->parameters.gain_smooth_beta;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_smooth_beta_fast       = data->parameters.gain_smooth_beta_fast;
		((T_MMI_AQI_AGC_UL_REQ *)p_send_message)->parameters.gain_intp_flag       = data->parameters.gain_intp_flag;

        /* send the start command to the audio L1 */
        audio_send_l1_message(MMI_AQI_AGC_DL_REQ, p_send_message);
      }
      else
      {
        AUDIO_SEND_TRACE("AUDIO MODE WRITE: AGC_DL: the audio AGC_DL message isn't sent",
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    return (AUDIO_OK);
  }
#endif

  /********************************************************************************/
  /**********************     READ DRIVERS           ******************************/
  /********************************************************************************/

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_voice_path_read                               */
  /*                                                                              */
  /*    Purpose:  This function reads the current voice path.                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current path voice.                                                   */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_voice_path_read  (T_AUDIO_VOICE_PATH_SETTING *data)
  {
  #if (PSP_STANDALONE != 1)
    #ifndef _WINDOWS
      UINT16 audio_init, dai_mode;

      dai_mode   = (((l1s_dsp_com.dsp_ndb_ptr->d_dai_onoff)>>11) & 0x0003);
      audio_init = l1s_dsp_com.dsp_ndb_ptr->d_audio_init;

      // Note :
      //         tested_device  | dai_mode (bit 11 and 12)  |    test
      //         ---------------|---------------------------|----------------
      //              0         |     0                     |  no test
      //              1         |     2                     |  speech decoder
      //              2         |     1                     |  speech encoder
      //              3         |     0                     |  no test
      //              4         |     3                     |  Acouustic devices

      if(dai_mode == 0)
      /* There's no DAI mode */
      {
        #if (AUDIO_MODE)
          if (audio_init & B_GSM_ONLY)
          {
            *data = AUDIO_GSM_VOICE_PATH;
          }
          else
          if (audio_init & B_BT_CORDLESS)
          {
            *data = AUDIO_BLUETOOTH_CORDLESS_VOICE_PATH;
          }
          else
          if (audio_init & B_BT_HEADSET)
          {
            *data = AUDIO_BLUETOOTH_HEADSET;
          }
          else
          {
            AUDIO_SEND_TRACE_PARAM("AUDIO MODE READ: voice path: error in audio path mode",
              audio_init,
              RV_TRACE_LEVEL_ERROR);
            return(AUDIO_ERROR);
          }
        #else
          *data = AUDIO_GSM_VOICE_PATH;
        #endif
      }
      else
      {
        switch (dai_mode)
        {
          case 1:
          {
            *data = AUDIO_DAI_DECODER;
            break;
          }
          case 2:
          {
            *data = AUDIO_DAI_ENCODER;
            break;
          }
          case 3:
          {
            *data = AUDIO_DAI_ACOUSTIC;
            break;
          }
          default :
          {
            AUDIO_SEND_TRACE_PARAM("AUDIO MODE READ: voice path: error in DAI mode",
              dai_mode,
              RV_TRACE_LEVEL_ERROR);
            return(AUDIO_ERROR);
            //break;
          }
        }
      }
  #else
    *data = AUDIO_GSM_VOICE_PATH;
  #endif
    return (AUDIO_OK);
  #else
    return AUDIO_ERROR;
  #endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_microphone_mode_read                          */
  /*                                                                              */
  /*    Purpose:  This function reads the current microphone mode.                */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Microphone mode.                                                      */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_microphone_mode_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbcr;
    #if ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
      UINT16 vbcr2;
    #endif

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
      vbcr2 = (l1s_dsp_com.dsp_ndb_ptr->d_vbctrl2 >> 6);
    #endif

    /* HANDHELD is checked with 1 bit */
    if (vbcr & AUDIO_VBCR_VULSWITCH)
    {
      *data = AUDIO_MICROPHONE_HANDHELD;
    }
    /* HANDFREE or HEADSET */
    else
    {
      #if (ANLG_FAM == 1)
        *data = AUDIO_MICROPHONE_HANDFREE;
      #elif (ANLG_FAM== 2)
        if (vbcr2 & (AUDIO_VBCR2_MICBIASEL | AUDIO_VBCR2_MICNAUX))
        {
          *data = AUDIO_MICROPHONE_HEADSET;
        }
        else
        {
          *data = AUDIO_MICROPHONE_HANDFREE;
        }
      #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
        if (vbcr2 & (AUDIO_VBCR2_HSDIF | AUDIO_VBCR2_HSMICSEL))
        {
          *data = AUDIO_MICROPHONE_HEADSET_DIFFERENTIAL;
        }
        else if (vbcr2 & AUDIO_VBCR2_HSMICSEL)
        {
          *data = AUDIO_MICROPHONE_HEADSET_DIFFERENTIAL;
        }
        else
        {
          *data = AUDIO_MICROPHONE_HANDFREE;
        }
      #endif
    }
#else

    //*data = audio_mode_configuration.audio_microphone_mode;
    BspTwl3029_Audio_InputPathId inputPath;


    if ( BSP_TWL3029_RETURN_CODE_FAILURE == bspTwl3029_Audio_getInputPath(&inputPath))
		return AUDIO_ERROR;

	*data = inputPath;

#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_microphone_gain_read                          */
  /*                                                                              */
  /*    Purpose:  This function reads the current microphone gain.                */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current microphone gain.                                              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
 #if (AUDIO_DSP_FEATURES == 1)
  T_AUDIO_RET audio_mode_microphone_gain_read  (INT16  *data)
  {
      *data =  (INT16) l1s_dsp_com.dsp_ndb_ptr->d_vol_ul_level;
 #else
  T_AUDIO_RET audio_mode_microphone_gain_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT8 i = 0;
    UINT16 vbur, vulpg;


    vbur = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbuctrl >> 6);
    if (vbur & AUDIO_VBUR_DXEN)
    {
      *data = AUDIO_MICROPHONE_MUTE;
    }
    else
    /* Microphone not muted */
    {
      vulpg = (UINT16)(vbur & AUDIO_VBUR_VULPG);
      while (ABB_uplink_PGA_gain[i] != vulpg)
      {
        i++;
        if (i == 25)
        {
          AUDIO_SEND_TRACE_PARAM("AUDIO MODE READ: microphone gain: error in gain value",
            vulpg,
            RV_TRACE_LEVEL_ERROR);
          return(AUDIO_ERROR);
        }
      }
      *data = (INT8)(i - 12);
    }
#else

  //*data = audio_mode_configuration.audio_microphone_gain;
    BspTwl3029_Audio_GainSettings newGainSettings;
	INT8 count;

    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getGain (
											BSP_TWL3029_AUDIO_GAIN_UPDATE_VUL,
											&newGainSettings ))
    {
    	return AUDIO_ERROR ;
    }

	for(count = 0; count <= 25 && ABB_uplink_PGA_gain[count] != newGainSettings.vulGain; count++);

	if( count > 25 )
		return AUDIO_ERROR;

	*data = count - 12;

#endif
 #endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_microphone_extra_gain_read                    */
  /*                                                                              */
  /*    Purpose:  This function reads the current microphone extra gain.          */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current microphone extra gain.                                        */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_microphone_extra_gain_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbcr;

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
    #endif

    if (vbcr & AUDIO_VBCR_VBDFAUXG)
    {
      *data = AUDIO_MICROPHONE_AUX_GAIN_28_2dB;
    }
    else
    {
      *data = AUDIO_MICROPHONE_AUX_GAIN_4_6dB;
    }
#else

    //*data = audio_mode_configuration.audio_microphone_extra_gain;
      /* Configure FM gain as part of Microphone extra-gain */
    BspTwl3029_Audio_GainSettings newGainSettings;

	if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getGain (
											BSP_TWL3029_AUDIO_GAIN_UPDATE_RADIO,
											&newGainSettings ))
    {
    	return AUDIO_ERROR ;
    }

	if( BSP_TWL3029_AUDIO_FMRADIO_GAIN_MINUS_2DB == newGainSettings.fmRadioGain )
	{
		*data = -2;
	}
	else
	{
		*data = (2 * newGainSettings.fmRadioGain);
	}

#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_microphone_output_bias_read                   */
  /*                                                                              */
  /*    Purpose:  This function reads the current microphone output bias.         */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current microphone output bias.                                       */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_microphone_output_bias_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbcr;

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
    #endif

    if (vbcr & AUDIO_VBCR_MICBIAS)
    {
      *data = AUDIO_MICROPHONE_OUTPUT_BIAS_2_5V;
    }
    else
    {
      *data = AUDIO_MICROPHONE_OUTPUT_BIAS_2_0V;
    }
#else

    //*data = audio_mode_configuration.audio_microphone_output_bias;
    BspTwl3029_Audio_MicBiasId micBiasId;

    if ( BSP_TWL3029_RETURN_CODE_FAILURE == bspTwl3029_Audio_getMicBias( &micBiasId ))
    {
		return AUDIO_ERROR;
    }

	*data = micBiasId;
#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_microphone_fir_read                           */
  /*                                                                              */
  /*    Purpose:  This function reads the current microphone FIR.                 */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current microphone FIR.                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_microphone_fir_read  (T_AUDIO_FIR_COEF *data)
  {
  #if (PSP_STANDALONE != 1)
    #if (FIR)
      UINT8 i;

      for (i=0; i<31; i++)
      {
        #if (DSP == 33) || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37) || (DSP == 38) || (DSP == 39)
          // For this DSP code the FIR coefficients are in API param memory
          data->coefficient[i] = l1s_dsp_com.dsp_param_ptr->a_fir31_uplink[i];
        #else
          data->coefficient[i] = l1s_dsp_com.dsp_ndb_ptr->a_fir31_uplink[i];
        #endif
      }
      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("AUDIO MODE READ: microphone FIR: FIR isn't supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
  #else
  	return AUDIO_ERROR;
  #endif
  }


 /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_fm_mode_read                             */
  /*                                                                              */
  /*    Purpose:  This function reads the current fm mode.                   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current fm mode.                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/

#if(AS_FM_RADIO==1)


 T_AUDIO_RET  audio_mode_fm_mode_read(UINT8 *data)
 	{
	BspTwl3029_Audio_OutputPathId outputPathId;
	BspTwl3029_AudioOutputSource outputSource[BSP_TWL3029_AUDIO_OUTPUT_SOURCE_MAX_SIZE];
	Uint8 classDmode;

  *data = AUDIO_SPEAKER_NONE;

    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getOutputPath(&outputPathId, &outputSource))
   	{
   		return AUDIO_ERROR;
   	}
//headset
	if( outputPathId & (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE | BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE))
		{
			 if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM) ||
	         (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM) ||
         		(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM) )

			*data = AUDIO_FM_STEREO_SPEAKER_HEADSET;
 		}


	if(outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK)
		{
		 if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_FM) ||
	         (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM) ||
         		(outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM) )
		 	{
		 	      AUDIO_SEND_TRACE("goto in speaker mode", RV_TRACE_LEVEL_DEBUG_LOW);
	 			if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getClassD_mode(&classDmode))
   				{
   					return AUDIO_ERROR;
   				}
		 	if( 1==classDmode)
		 		*data= AUDIO_FM_MONO_SPEAKER_HANDFREE_CLASSD;
			else
				*data= AUDIO_FM_MONO_SPEAKER_HANDFREE;

			}
 		}
	  return (AUDIO_OK);
 }
 #endif
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_mode_read                             */
  /*                                                                              */
  /*    Purpose:  This function reads the current speaker mode.                   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current speaker mode.                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_mode_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
    #if (ANLG_FAM == 1)
      UINT16 vbcr;
    #endif
    #if (ANLG_FAM == 2)
      UINT16 vbcr;
      UINT16 vbcr2;
    #endif
    #if ((ANLG_FAM== 3) || (ANLG_FAM == 4))
      UINT16 vauoctrl;
    #endif

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif (ANLG_FAM == 2)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
      vbcr2 = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl2 >> 6);
    #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
      vauoctrl = (l1s_dsp_com.dsp_ndb_ptr->d_vauo_onoff >> 6);
    #endif

    #if (ANLG_FAM == 1)
      if ( (vbcr & AUDIO_VBCR_VDLEAR) &&
           (vbcr & AUDIO_VBCR_VDLAUX) )
      {
        *data = AUDIO_SPEAKER_HANDHELD_HANDFREE;
      }
      else
      if (vbcr & AUDIO_VBCR_VDLEAR)
      {
        *data = AUDIO_SPEAKER_HANDHELD;
      }
      else
      if (vbcr & AUDIO_VBCR_VDLAUX)
      {
        *data = AUDIO_SPEAKER_HANDFREE;
      }
      else
      {
        *data = AUDIO_SPEAKER_BUZZER;
      }
    #elif (ANLG_FAM == 2)
      if (vbcr2 & AUDIO_VBCR2_VDLHSO)
      {
        *data = AUDIO_SPEAKER_HEADSET;
      }
      if ( (vbcr & AUDIO_VBCR_VDLEAR) &&
           (vbcr & AUDIO_VBCR_VDLAUX) )
      {
        *data = AUDIO_SPEAKER_HANDHELD_HANDFREE;
      }
      else
      if (vbcr & AUDIO_VBCR_VDLEAR)
      {
        *data = AUDIO_SPEAKER_HANDHELD;
      }
      else
      if (vbcr & AUDIO_VBCR_VDLAUX)
      {
        *data = AUDIO_SPEAKER_HANDFREE;
      }
    #elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
      if ( (vauoctrl & AUDIO_VAUOCTRL_HSOL_VOICE) &&
           (vauoctrl & AUDIO_VAUOCTRL_HSOR_VOICE) )
      {
        *data = AUDIO_SPEAKER_HEADSET;
      }
      else
      if ( (vauoctrl & AUDIO_VAUOCTRL_EAR_VOICE) &&
           (vauoctrl & AUDIO_VAUOCTRL_AUX_VOICE) )
      {
        *data = AUDIO_SPEAKER_HANDHELD_HANDFREE;
      }
      else
      if (vauoctrl & AUDIO_VAUOCTRL_EAR_VOICE)
      {
        *data = AUDIO_SPEAKER_HANDHELD;
      }
      else
      if (vauoctrl & AUDIO_VAUOCTRL_AUX_VOICE)
      {
        *data = AUDIO_SPEAKER_HANDFREE;
      }
      else
      if (vauoctrl & AUDIO_VAUOCTRL_SPK_VOICE)
      {
        *data = AUDIO_SPEAKER_HANDHELD_8OHM;
      }
    #endif
#else
	BspTwl3029_Audio_OutputPathId outputPathId;
	BspTwl3029_AudioOutputSource outputSource[BSP_TWL3029_AUDIO_OUTPUT_SOURCE_MAX_SIZE];
	Uint8 classDmode;

  *data = AUDIO_SPEAKER_NONE;

    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getOutputPath(&outputPathId, &outputSource))
   	{
   		return AUDIO_ERROR;
   	}


	if( outputPathId & (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE | BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE))
	{
		 if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_R_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_INVERT_VOICE)
     {
                *data = AUDIO_SPEAKER_INVERTED_VOICE;
     }
     else if( (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE) ||
         (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO) ||
         (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM) )
	{
       *data = AUDIO_SPEAKER_HEADSET;
     }
	}
	if (outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE )
	{
    /* ideally,
     * if outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_ONLY ||
     *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_AUDIO
     * *data = AUDIO_SPEAKER_HANDHELD;
     * But an AND with 0x1 will do the same
     * */
     if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] & 0x1)
     {
       *data = AUDIO_SPEAKER_HANDHELD;
     }
	}
	if (outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_AUX )
	{
    /* Same comment as HANDSET */
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] & 0x1)
	{
		*data = AUDIO_SPEAKER_AUX;
	}
	}
	if (outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_CARKIT )
	{
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] & 0x1)
	{
		*data = AUDIO_SPEAKER_CARKIT;
	}
	}
	if (outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK )
	{
    if((outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE) ||
       (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO) ||
       (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_FM) )
	{
	 	if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getClassD_mode(&classDmode))
   		{
   			return AUDIO_ERROR;
   		}
		if(1 == classDmode)
			*data = AUDIO_SPEAKER_HANDFREE_CLASS_D;
		else
			*data = AUDIO_SPEAKER_HANDFREE;
	}
	}
  return (AUDIO_OK);
#endif

  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_gain_read                             */
  /*                                                                              */
  /*    Purpose:  This function reads the current speaker gain.                   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current speaker gain.                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
 #if (AUDIO_DSP_FEATURES == 1)
  T_AUDIO_RET audio_mode_speaker_gain_read  (INT16  *data)
  {
     *data = (INT16) l1s_dsp_com.dsp_ndb_ptr->d_vol_dl_level;
 #else
  T_AUDIO_RET audio_mode_speaker_gain_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbdr, vdlpg;
    UINT8 i=0;

    vbdr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbdctrl >> 6);
    vdlpg = (UINT16)(vbdr & AUDIO_VBDR_VDLPG);

    while (ABB_downlink_PGA_gain[i] != vdlpg)
    {
      i++;
      if (i == 13)
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE READ: speaker gain: error in gain value",
          vdlpg,
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    }
    *data = (INT8)(i - 6);
#else

    //*data = audio_mode_configuration.audio_speaker_gain;
    BspTwl3029_Audio_GainVDLSettings newGainVDLSettings;

    if ( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getVDLGain( BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_FINE,
													&newGainVDLSettings))
    {
    	return AUDIO_ERROR;
    }

	*data = newGainVDLSettings.vdlFineGain - 6;

	if( *data > 6 )
		*data = -6;

#endif
  #endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_extra_gain_read                       */
  /*                                                                              */
  /*    Purpose:  This function reads the current speaker gain.                   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current speaker gain.                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_extra_gain_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
    UINT16 vbctrl2 = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl2 >> 6);

    if (vbctrl2 & AUDIO_VBCR2_SPKG)
    {
      *data = AUDIO_SPEAKER_SPK_GAIN_8_5dB;
    }
    else
    {
      *data = AUDIO_SPEAKER_SPK_GAIN_2_5dB;
    }
  #else
    AUDIO_SEND_TRACE("AUDIO MODE READ: speaker extra gain: feature not supported",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else

    //*data = audio_mode_configuration.audio_speaker_extra_gain;
    BspTwl3029_Audio_GainSettings newGainSettings;
    INT8 speakerMode = AUDIO_SPEAKER_NONE;

    if (audio_mode_speaker_mode_read  (&speakerMode) == AUDIO_ERROR)
    {
      return AUDIO_ERROR;
    }

    if( AUDIO_SPEAKER_HANDHELD == speakerMode )
    {
 	  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getGain(
				  BSP_TWL3029_AUDIO_GAIN_UPDATE_EARGAIN,
				  &newGainSettings))
	  {
		  return(AUDIO_ERROR);
	  }

	  *data = newGainSettings.earGain ;

    }
    else if ( AUDIO_SPEAKER_HANDFREE == speakerMode || AUDIO_SPEAKER_HANDFREE_CLASS_D == speakerMode)
    {

	  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getGain(
				  BSP_TWL3029_AUDIO_GAIN_UPDATE_SPEAKERAMP,
				  &newGainSettings))
	  {
		  return(AUDIO_ERROR);
	  }

	  *data = newGainSettings.speakerAmpGain;
    }
    else
    {
	 return(AUDIO_ERROR);
    }


#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_stereo_speaker_extra_gain_read                */
  /*                                                                              */
  /*    Purpose:  This function reads the current stereo speaker gain.            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current stereo speaker gain.                                          */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_stereo_speaker_extra_gain_read  (INT8  *data)
  {
#if (ANLG_FAM == 11)
    //*data = audio_mode_configuration.audio_speaker_extra_gain;
    BspTwl3029_Audio_GainSettings newGainSettings;
    INT8 stereo_speakerMode = AUDIO_STEREO_SPEAKER_NONE;

    if (audio_mode_stereo_speaker_mode_read  (&stereo_speakerMode) == AUDIO_ERROR)
    {
      return AUDIO_ERROR;
    }

    if( AUDIO_STEREO_SPEAKER_HANDHELD == stereo_speakerMode )
    {
 	  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getGain(
				  BSP_TWL3029_AUDIO_GAIN_UPDATE_EARGAIN,
				  &newGainSettings))
	  {
		  return(AUDIO_ERROR);
	  }

	  *data = newGainSettings.earGain ;

    }
    else if ( AUDIO_STEREO_SPEAKER_HANDFREE == stereo_speakerMode || AUDIO_STEREO_SPEAKER_HANDFREE_CLASS_D == stereo_speakerMode)
    {

	  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getGain(
				  BSP_TWL3029_AUDIO_GAIN_UPDATE_SPEAKERAMP,
				  &newGainSettings))
	  {
		  return(AUDIO_ERROR);
	  }

	  *data = newGainSettings.speakerAmpGain;
    }
    else if( AUDIO_STEREO_SPEAKER_HEADPHONE == stereo_speakerMode )
    {
 	  if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getGain(
				  BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_RIGHT | BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_LEFT,
				  &newGainSettings))
	  {
		  return(AUDIO_ERROR);
	  }

	  *data = newGainSettings.earGain ;

    }
    else
    {
	 return(AUDIO_ERROR);
    }

#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_filter_read                           */
  /*                                                                              */
  /*    Purpose:  This function reads the current state of the DL filter.         */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current DL filter state.                                              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_filter_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbcr;

    #if (ANLG_FAM == 1)
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);
    #elif ((ANLG_FAM == 2)||(ANLG_FAM == 3) || (ANLG_FAM == 4))
      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl1 >> 6);
    #endif

    if (vbcr & AUDIO_VBCR_VFBYP)
    {
      *data = AUDIO_SPEAKER_FILTER_OFF;
    }
    else
    {
      *data = AUDIO_SPEAKER_FILTER_ON;
    }
#else

    BspTwl3029_Audio_FilterState filterState;

	if(BSP_TWL3029_RETURN_CODE_FAILURE == bspTwl3029_Audio_getFilterState (BSP_TWL3029_AUDIO_FILTER_VOICE_DOWNLINK_PATH,&filterState))
	{
		return AUDIO_ERROR;
	}

	*data = filterState;

#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_highpass_filter_read                  */
  /*                                                                              */
  /*    Purpose:  This function reads the current state of the highpass DL filter */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current DL filter state.                                              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_highpass_filter_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
    UINT16 vaudctrl = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vaud_cfg >> 6);

    if (vaudctrl & AUDIO_VAUDCTRL_HPFBYP)
    {
      *data = AUDIO_SPEAKER_HIGHPASS_FILTER_OFF;
    }
    else
    {
      *data = AUDIO_SPEAKER_HIGHPASS_FILTER_ON;
    }
  #else
    AUDIO_SEND_TRACE("AUDIO MODE READ: speaker highpass filter: feature not supported",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else

    BspTwl3029_Audio_FilterState filterState;

	if(BSP_TWL3029_RETURN_CODE_FAILURE == bspTwl3029_Audio_getFilterState (BSP_TWL3029_AUDIO_FILTER_VOICE_UPLINK_HIGH_PASS,&filterState))
	{
		return AUDIO_ERROR;
	}

	*data = filterState;

#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_speaker_fir_read                              */
  /*                                                                              */
  /*    Purpose:  This function reads the speaker FIR.                            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current speaker FIR.                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_fir_read  (T_AUDIO_FIR_COEF *data)
  {
  #if (PSP_STANDALONE != 1)
    #if (FIR)
      UINT8 i;

      for (i=0; i<31; i++)
      {
        #if (DSP == 33) || (DSP == 34) || (DSP == 35) || (DSP == 36) || (DSP == 37) || (DSP == 38) || (DSP == 39)
          // For this DSP code the FIR coefficients are in API param memory
          #if (L1_IIR == 1)
             data->coefficient[i] = l1s_dsp_com.dsp_ndb_ptr->a_iir_fir_coefs[i];
          #else
          data->coefficient[i] = l1s_dsp_com.dsp_param_ptr->a_fir31_downlink[i];
          #endif
        #else
          data->coefficient[i] = l1s_dsp_com.dsp_ndb_ptr->a_fir31_downlink[i];
        #endif
      }
      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("AUDIO MODE READ: speaker FIR: FIR isn't supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
    //return (AUDIO_OK);
  #else
  	return AUDIO_ERROR;
  #endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_buzzer_read                            */
  /*                                                                              */
  /*    Purpose:  This function reads the current state of the buzzer.            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current buzzer state.                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_buzzer_read  (INT8  *data)
  {
  #if (PSP_STANDALONE != 1)
    #if (ANLG_FAM== 1)
      UINT16 vbcr;

      vbcr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbctrl >> 6);

      if (vbcr & AUDIO_VBCR_VBUZ)
      {
        *data = AUDIO_SPEAKER_BUZZER_ON;
      }
      else
      {
        *data = AUDIO_SPEAKER_BUZZER_OFF;
      }
      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("AUDIO MODE READ: buzzer: this feature isn't supported by the current analog base band",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
  #else
  	return AUDIO_ERROR;
  #endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_sidetone_gain_read                             */
  /*                                                                              */
  /*    Purpose:  This function reads the current sidetone gain.                  */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current sidetone gain.                                                */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
 #if (AUDIO_DSP_FEATURES == 1)
  T_AUDIO_RET audio_mode_sidetone_gain_read  (INT16  *data)
  {
      *data = (INT16) l1s_dsp_com.dsp_ndb_ptr->d_sidetone_level;
 #else
  T_AUDIO_RET audio_mode_sidetone_gain_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbur, vdlst;
    UINT8 i=0;

    vbur = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbuctrl >> 11);
    vdlst = (UINT16)(vbur & AUDIO_VBUR_VDLST);

    while (ABB_sidetone_gain[i] != vdlst)
    {
      i++;
      if (i == 10)
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE READ: sidetone gain: error in gain value",
          vdlst,
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    }
    *data = (INT8)((3 * i) - 26);
#else

    //*data = audio_mode_configuration.audio_sidetone_gain;
    BspTwl3029_Audio_GainSettings newGainSettings;
	INT8 count;

	if(AUDIO_SIDETONE_MUTE == *data)
	{
    	newGainSettings.sidetoneLevel = BSP_TWL3029_AUDIO_SIDETONE_LEVEL_MUTE;
	}
	else
	{
	    newGainSettings.sidetoneLevel = ABB_sidetone_gain[(*data + 23) / 3];
	}

    if( BSP_TWL3029_RETURN_CODE_SUCCESS !=
		bspTwl3029_Audio_getGain( BSP_TWL3029_AUDIO_GAIN_UPDATE_SIDETONE,
			    &newGainSettings))
    {
	    return(AUDIO_ERROR);
    }

	for( count = 0; count <= 8 && ABB_sidetone_gain[count] != newGainSettings.sidetoneLevel ; count++);

	if (count > 8)
		*data = AUDIO_SIDETONE_MUTE;
	else
	    *data = (count * 3) - 23;


#endif
 #endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_aec_read                                       */
  /*                                                                              */
  /*    Purpose:  This function reads the current AEC setting.                    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current AEC setting.                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_AEC == 1)
  T_AUDIO_RET audio_mode_aec_read  (T_AUDIO_AEC_CFG *data)
  {
  #if (PSP_STANDALONE != 1)
    #if (L1_NEW_AEC)
      UINT16 aec_ctrl;

      aec_ctrl = l1s.aec.aec_control;

      data->aec_enable               = (UINT16)(aec_ctrl & AUDIO_AEC_ENABLE);
      data->aec_visibility           = (UINT16)(aec_ctrl & AUDIO_AEC_VISIBILITY_ENABLE);
      #if (L1_ANR == 0)
        data->noise_suppression_enable = (UINT16)(aec_ctrl & AUDIO_NOISE_SUPPRESSION_ENABLE);
        data->noise_suppression_level  = (UINT16)(aec_ctrl & AUDIO_NOISE_18dB);
      #endif

	if(aec_ctrl){
      data->continuous_filtering       = l1s_dsp_com.dsp_ndb_ptr->d_cont_filter;
      data->granularity_attenuation    = l1s_dsp_com.dsp_ndb_ptr->d_granularity_att;
      data->smoothing_coefficient      = l1s_dsp_com.dsp_ndb_ptr->d_coef_smooth;
      data->max_echo_suppression_level = l1s_dsp_com.dsp_ndb_ptr->d_es_level_max;
      data->vad_factor                 = l1s_dsp_com.dsp_ndb_ptr->d_fact_vad;
      data->absolute_threshold         = l1s_dsp_com.dsp_ndb_ptr->d_thrs_abs;
      data->factor_asd_filtering       = l1s_dsp_com.dsp_ndb_ptr->d_fact_asd_fil;
      data->factor_asd_muting          = l1s_dsp_com.dsp_ndb_ptr->d_fact_asd_mut;
	}

      return (AUDIO_OK);

    #elif (L1_AEC == 1)
      UINT16 aec_ctrl;

      aec_ctrl = l1s.aec.aec_control;

      data->aec_enable = (UINT16)(aec_ctrl & AUDIO_AEC_ENABLE);
      data->aec_mode = (UINT16)(aec_ctrl & AUDIO_SHORT_ECHO);
      data->echo_suppression_level = (UINT16)(aec_ctrl & AUDIO_ECHO_18dB);
      #if (L1_ANR == 0)
        data->noise_suppression_enable = (UINT16)(aec_ctrl & AUDIO_NOISE_SUPPRESSION_ENABLE);
        data->noise_suppression_level = (UINT16)(aec_ctrl & AUDIO_NOISE_18dB);
      #endif
      return (AUDIO_OK);
    #else
      AUDIO_SEND_TRACE("AUDIO MODE READ: AEC: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
  #else
  	return(AUDIO_ERROR);
  #endif
  }
#endif// L1_AEC == 1
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_aec_read                                       */
  /*                                                                              */
  /*    Purpose:  This function reads the current AEC setting.                    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current AEC setting.                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_AEC == 2)
  T_AUDIO_RET audio_mode_aec_read  (T_AUDIO_AQI_AEC_CFG *data)
  {

  #if (PSP_STANDALONE != 1)
	if(l1s_dsp_com.dsp_ndb_ptr->d_aqi_status &  B_AEC_STATE)
	{

	      data->aec_control =     AUDIO_AQI_AEC_START; // AUDIO_ANR_START;  //omaps00090550
	      data->aec_mode        = l1s_dsp_com.dsp_ndb_ptr->d_aec_mode;
	      data->mu              = l1s_dsp_com.dsp_ndb_ptr->d_mu;
	      data->cont_filter     = l1s_dsp_com.dsp_ndb_ptr->d_cont_filter;
	      data->scale_input_ul  = l1s_dsp_com.dsp_ndb_ptr->d_scale_input_ul;
	      data->scale_input_dl  = l1s_dsp_com.dsp_ndb_ptr->d_scale_input_dl;
	      data->div_dmax        = l1s_dsp_com.dsp_ndb_ptr->d_div_dmax;
	      data->div_swap_good   = l1s_dsp_com.dsp_ndb_ptr->d_div_swap_good;
	      data->div_swap_bad    = l1s_dsp_com.dsp_ndb_ptr->d_div_swap_bad;
	      data->block_init      = l1s_dsp_com.dsp_ndb_ptr->d_block_init;
	      data->fact_vad        = l1s_dsp_com.dsp_ndb_ptr->d_fact_vad;
	      data->fact_asd_fil    = l1s_dsp_com.dsp_ndb_ptr->d_fact_asd_fil;
	      data->fact_asd_mut    = l1s_dsp_com.dsp_ndb_ptr->d_fact_asd_mut;
	      data->thrs_abs        = l1s_dsp_com.dsp_ndb_ptr->d_thrs_abs;
	      data->es_level_max    = l1s_dsp_com.dsp_ndb_ptr->d_es_level_max;
	      data->granularity_att = l1s_dsp_com.dsp_ndb_ptr->d_granularity_att;
	      data->coef_smooth     = l1s_dsp_com.dsp_ndb_ptr->d_coef_smooth;
	}
	else
	{
	      data->aec_control = AUDIO_AQI_AEC_STOP; //AUDIO_ANR_STOP;  //omaps00090550
	}
      return (AUDIO_OK);
  #else
  	return(AUDIO_ERROR);
  #endif
}
#endif
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_stereo_speaker_mode_read                      */
  /*                                                                              */
  /*    Purpose:  This function reads the current stereo speaker mode.            */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current speaker mode.                                                 */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_stereo_speaker_mode_read  (INT8  *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM== 3) || (ANLG_FAM == 4))
    UINT16 vauoctrl;

    vauoctrl = (l1s_dsp_com.dsp_ndb_ptr->d_vauo_onoff >> 6);

    if ( (vauoctrl & AUDIO_VAUOCTRL_HSOL_AUDIO) &&
         (vauoctrl & AUDIO_VAUOCTRL_HSOR_AUDIO) )
    {
      *data = AUDIO_STEREO_SPEAKER_HEADPHONE;
    }
    else
    if (vauoctrl & AUDIO_VAUOCTRL_EAR_AUDIO)
    {
      *data = AUDIO_STEREO_SPEAKER_HANDHELD;
    }
    else
    if (vauoctrl & AUDIO_VAUOCTRL_AUX_AUDIO)
    {
      *data = AUDIO_STEREO_SPEAKER_HANDFREE;
    }
    else
    if (vauoctrl & AUDIO_VAUOCTRL_SPK_AUDIO)
    {
      *data = AUDIO_STEREO_SPEAKER_HANDHELD_8OHM;
    }
    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("AUDIO MODE READ: stereo speaker mode: this feature isn't supported by the current analog base band",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif // #if (ANLG_FAM == 3) || (ANLG_FAM == 4)
#else

	BspTwl3029_Audio_OutputPathId outputPathId;
	BspTwl3029_AudioOutputSource outputSource[BSP_TWL3029_AUDIO_OUTPUT_SOURCE_MAX_SIZE];
	Uint8 classDmode=0;
  *data = AUDIO_STEREO_SPEAKER_NONE;
    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getOutputPath(&outputPathId, &outputSource))
   	{
   		return AUDIO_ERROR;
   	}


	if(outputPathId &
      (BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_R_EARPIECE | BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HEADSET_L_EARPIECE))
	{
    if((outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO) ||
       (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO) ||
       (outputSource[BSP_TWL3029_AUDIO_OUTPUT_HEADSET_L_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM) )
     {
       *data = AUDIO_STEREO_SPEAKER_HEADPHONE;
     }
  }
	if (outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_HANDSET_EARPIECE )
	{
    /* ideally,
     * if outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_PATH_AUDIO ||
     *    outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_PATH_VOICE_AUDIO
     *   *data = AUDIO_STEREO_SPEAKER_HANDHELD;
     * But an AND with 0x2 will do the same
     * */
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_HANDSET_EARPIECE_OFFSET] & 0x2){
      *data = AUDIO_STEREO_SPEAKER_HANDHELD;
    }
	}
	if (outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_AUX )
	{
    /* Same comment as handheld */
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_AUX_OFFSET] & 0x2){
		*data = AUDIO_STEREO_SPEAKER_AUX;
	}
  }
	if (outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_CARKIT )
	{
    /* Same comment as handheld */
    if(outputSource[BSP_TWL3029_AUDIO_OUTPUT_CARKIT_OFFSET] & 0x2){
		*data = AUDIO_STEREO_SPEAKER_CARKIT;
	}
	}
	if (outputPathId & BSP_TWL3029_AUDIO_OUTPUT_PATH_ID_SPK )
	{
    if((outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO) ||
       (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_VOICE_AND_AUDIO) ||
       (outputSource[BSP_TWL3029_AUDIO_OUTPUT_SPK_OFFSET] == BSP_TWL3029_AUDIO_OUTPUT_SOURCE_AUDIO_AND_FM))
	{
		if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getClassD_mode(&classDmode))
   		{
   			return AUDIO_ERROR;
   		}
		if(1 == classDmode)
			*data = AUDIO_STEREO_SPEAKER_HANDFREE_CLASS_D;
		else
			*data = AUDIO_STEREO_SPEAKER_HANDFREE;
	}
	}

	return AUDIO_OK;
#endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_stereo_speaker_stereo_mono_read               */
  /*                                                                              */
  /*    Purpose:  This function reads the stereo speaker stereo/mono conversion   */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*                                                                              */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_stereo_speaker_stereo_mono_read(INT8  *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
    UINT16 vaudctrl = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vaud_cfg >> 6);

    *data = AUDIO_STEREO; // AUDIO STEREO configuration is 0!!

    if (vaudctrl & AUDIO_VAUDCTRL_MONOR)
    {
      *data |= AUDIO_MONO_RIGHT;
    }
    if (vaudctrl & AUDIO_VAUDCTRL_MONOL)
    {
      *data |= AUDIO_MONO_LEFT;
    }
    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("AUDIO MODE READ: stereo speaker stereo/mono: this feature isn't supported by the current analog base band",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else

    BspTwl3029_Audio2MonoState mono;

	if(BSP_TWL3029_RETURN_CODE_FAILURE == bspTwl3029_Audio_getStereoMonoConfigData (&mono ))
	{
		return AUDIO_ERROR;
	}

	*data = mono;

	return (AUDIO_OK);

#endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_stereo_speaker_sampling_freq_read             */
  /*                                                                              */
  /*    Purpose:  This function reads the stereo speaker sampling rate freq       */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*                                                                              */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_stereo_speaker_sampling_freq_read(INT8  *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
    UINT16 vaudctrl = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vaud_cfg >> 6);
    UINT16 srw = vaudctrl & (AUDIO_VAUDCTRL_SRW2|AUDIO_VAUDCTRL_SRW1|AUDIO_VAUDCTRL_SRW0);

    switch(srw)
    {
      case AUDIO_VAUDCTRL_SRW_48KHZ:     *data = AUDIO_STEREO_SAMPLING_FREQUENCY_48KHZ; break;
      case AUDIO_VAUDCTRL_SRW_44_1KHZ:   *data = AUDIO_STEREO_SAMPLING_FREQUENCY_44_1KHZ; break;
      case AUDIO_VAUDCTRL_SRW_32KHZ:     *data = AUDIO_STEREO_SAMPLING_FREQUENCY_32KHZ; break;
      case AUDIO_VAUDCTRL_SRW_22_05KHZ:  *data = AUDIO_STEREO_SAMPLING_FREQUENCY_22_05KHZ; break;
      case AUDIO_VAUDCTRL_SRW_16KHZ:     *data = AUDIO_STEREO_SAMPLING_FREQUENCY_16KHZ; break;
      case AUDIO_VAUDCTRL_SRW_11_025KHZ: *data = AUDIO_STEREO_SAMPLING_FREQUENCY_11_025KHZ; break;
      case AUDIO_VAUDCTRL_SRW_8KHZ:      *data = AUDIO_STEREO_SAMPLING_FREQUENCY_8KHZ; break;
      default:
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE READ: stereo speaker sampling frequency: this configuration is not used",
          *data, RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
      break;
    }
    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("AUDIO MODE READ: stereo speaker sampling frequency: this feature isn't supported by the current analog base band",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else

    BspTwl3029_Audio_StereoSamplingRate  samplingRate ;
	if(BSP_TWL3029_RETURN_CODE_FAILURE == bspTwl3029_Audio_getStereoSamplingRate (&samplingRate ))
	{
		return AUDIO_ERROR;
	}

	*data = (8 - samplingRate);

	return (AUDIO_OK);

#endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_volume_read                            */
  /*                                                                              */
  /*    Purpose:  This function reads the current speaker volume.                 */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current speaker volume.                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_volume_read  (T_AUDIO_SPEAKER_LEVEL *data)
  {
#if (ANLG_FAM != 11)
    UINT16 vbdr, volctl;
    UINT8 i=0;

    vbdr = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vbdctrl >> 10);
    volctl = (UINT16)(vbdr & AUDIO_VBDR_VOLCTL);

    while (ABB_volume_control_gain[i] != volctl)
    {
      i++;
      if (i == 6)
      {
        AUDIO_SEND_TRACE_PARAM("AUDIO MODE READ: speaker volume: error in volume value",
          volctl,
          RV_TRACE_LEVEL_ERROR);
        return(AUDIO_ERROR);
      }
    }
    if ( (i == 0) ||
         (i == 1) )
    {
      data->audio_speaker_level = i;
    }
    else
    {
      data->audio_speaker_level = (UINT8)((i - 1) * 50);
    }
#else

    //*data = audio_mode_configuration.audio_speaker_volume;

    //*data = audio_mode_configuration.audio_speaker_gain;
    BspTwl3029_Audio_GainVDLSettings newGainVDLSettings;
	INT8 count;

    if ( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getVDLGain( BSP_TWL3029_AUDIO_GAIN_UPDATE_VDL_COURSE,
													&newGainVDLSettings))
    {
    	return AUDIO_ERROR;
    }

	for( count = 0; count <= 5 && ABB_volume_control_gain[count] != newGainVDLSettings.vdlCourseGain; count++);

	if( count == 0 || count > 5)
	{
		data->audio_speaker_level = 0;
	}
	else if ( count == 1)
	{
		data->audio_speaker_level = 1;
	}
	else
	{
		data->audio_speaker_level = (UINT8)((count - 1) * 50);
	}

#endif
    return (AUDIO_OK);
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_stereo_speaker_volume_read                     */
  /*                                                                              */
  /*    Purpose:  This function reads the current stereo_speaker volume.          */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current speaker volume.                                               */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_stereo_speaker_volume_read  (T_AUDIO_STEREO_SPEAKER_LEVEL *data)
  {
#if (ANLG_FAM != 11)
  #if ((ANLG_FAM ==3) || (ANLG_FAM == 4))
    UINT16 vausctrl = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_vaus_vol >> 6);
    UINT16 vausctrl_left = vausctrl & (AUDIO_VAUSCTRL_AULGA0 | AUDIO_VAUSCTRL_AULGA1 | AUDIO_VAUSCTRL_AULGA2
                                       | AUDIO_VAUSCTRL_AULGA3 |AUDIO_VAUSCTRL_AULGA4);
    UINT16 vausctrl_right = vausctrl & (AUDIO_VAUSCTRL_AURGA0 | AUDIO_VAUSCTRL_AURGA1 | AUDIO_VAUSCTRL_AURGA2
                                       | AUDIO_VAUSCTRL_AURGA3 |AUDIO_VAUSCTRL_AURGA4);

    if (vausctrl_left == (AUDIO_VAUSCTRL_AULGA0 | AUDIO_VAUSCTRL_AULGA1 | AUDIO_VAUSCTRL_AULGA2
                          | AUDIO_VAUSCTRL_AULGA3 |AUDIO_VAUSCTRL_AULGA4)
       )
    {
      data->audio_stereo_speaker_level_left = AUDIO_STEREO_SPEAKER_VOLUME_MUTE;
    }
    else
    {
      data->audio_stereo_speaker_level_left = vausctrl_left;
    }

    if (vausctrl_right == (AUDIO_VAUSCTRL_AURGA0 | AUDIO_VAUSCTRL_AURGA1 | AUDIO_VAUSCTRL_AURGA2
                          | AUDIO_VAUSCTRL_AURGA3 |AUDIO_VAUSCTRL_AURGA4)
       )
    {
      data->audio_stereo_speaker_level_right = AUDIO_STEREO_SPEAKER_VOLUME_MUTE;
    }
    else
    {
      data->audio_stereo_speaker_level_right = (vausctrl_right >> 5);
    }

    return (AUDIO_OK);
  #else
    AUDIO_SEND_TRACE("AUDIO MODE READ: stereo speaker volume: this feature isn't supported by the current analog base band",
      RV_TRACE_LEVEL_ERROR);
    return(AUDIO_ERROR);
  #endif
#else

    BspTwl3029_Audio_GainSettings newGainSettings;

    if( BSP_TWL3029_RETURN_CODE_SUCCESS != bspTwl3029_Audio_getGain( 		    BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_RIGHT | \
		    BSP_TWL3029_AUDIO_GAIN_UPDATE_STEREO_LEFT,
		    &newGainSettings))
    {
        return(AUDIO_ERROR);
    }

    data->audio_stereo_speaker_level_right = newGainSettings.stereoVolRight;
    data->audio_stereo_speaker_level_left = newGainSettings.stereoVolLeft;

	return AUDIO_OK;

#endif
  }

  #if (AUDIO_DSP_FEATURES == 1)
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_volume_speed_read                             */
  /*                                                                              */
  /*    Purpose:  This function reads the volume change speed                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        volume speed value                                                    */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_volume_speed_read  (INT16 *data)
  {
  #if (PSP_STANDALONE != 1)
     *data = (INT16)(l1s_dsp_com.dsp_ndb_ptr->d_vol_speed);
     return (AUDIO_OK);
  #else
  	return AUDIO_ERROR;
  #endif
  }
  #endif

  #if (AUDIO_DSP_ONOFF == 1)
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_onoff_read                                    */
  /*                                                                              */
  /*    Purpose:  This function reads the audio on/off status                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Audio onoff value                                                     */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_onoff_read  (INT8 *data)
  {
  #if (PSP_STANDALONE != 1)
    UINT16 onoff_value = (UINT16)(l1s_dsp_com.dsp_ndb_ptr->d_toneskb_status & B_AUDIO_ON_STATUS);

    if (onoff_value)
    {
      *data = AUDIO_ON;
    }
    else
    {
      *data = AUDIO_OFF;
    }

    return (AUDIO_OK);
  #else
  	return AUDIO_ERROR;
  #endif
  }
  #endif

  #if (L1_EXT_MCU_AUDIO_VOICE_ONOFF == 1)
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:   audio_mode_onoff_read                                    */
  /*                                                                              */
  /*    Purpose:  This function reads the audio on/off status                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Audio onoff value                                                     */
  /*                                                                              */
  /*    Note:                                                                     */
  /*        None.                                                                 */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_onoff_read  (T_AUDIO_MODE_ONOFF *data)
  {
  #if (PSP_STANDALONE != 1)
   if(l1s.audio_state[L1S_AUDIO_UL_ONOFF_STATE] < L1_AUDIO_UL_ON)
	data->vul_onoff = AUDIO_VUL_OFF;
   else
	data->vul_onoff = AUDIO_VUL_ON;

   if(l1s.audio_state[L1S_AUDIO_DL_ONOFF_STATE] < L1_AUDIO_DL_ON)
	data->vdl_onoff = AUDIO_VDL_OFF;
   else
	data->vdl_onoff = AUDIO_VDL_ON;

    return (AUDIO_OK);
  #else
  	return AUDIO_ERROR;
  #endif
  }
  #endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_microphone_anr_read                            */
  /*                                                                              */
  /*    Purpose:  This function reads the current ANR setting.                    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current ANR setting.                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  #if (L1_ANR == 1)
  T_AUDIO_RET audio_mode_microphone_anr_read  (T_AUDIO_ANR_CFG *data)
  {
  #if (PSP_STANDALONE != 1)
    #if (L1_ANR == 1)
      data->anr_enable             = (UINT16) ((l1s_dsp_com.dsp_ndb_ptr->d_aqi_status & B_ANR_STATE) >> 12);

	if(data->anr_enable){
      data->div_factor_shift       = l1s_dsp_com.dsp_ndb_ptr->d_anr_div_factor_shift;
      data->min_gain               = l1s_dsp_com.dsp_ndb_ptr->d_anr_min_gain;
      data->ns_level               = l1s_dsp_com.dsp_ndb_ptr->d_anr_ns_level;
	}

      return (AUDIO_OK);

    #else
      AUDIO_SEND_TRACE("AUDIO MODE READ: ANR: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
  #else
  	return AUDIO_ERROR;
  #endif
  }
  #endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_microphone_anr_read                            */
  /*                                                                              */
  /*    Purpose:  This function reads the current ANR setting.                    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current ANR setting.                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  #if (L1_ANR == 2)
  T_AUDIO_RET audio_mode_microphone_anr_read  (T_AUDIO_AQI_ANR_CFG *data)
  {
  #if (PSP_STANDALONE != 1)
    #if (L1_ANR == 2)
#if 0
      data->parameters.control = (UINT16) l1a_l1s_com.anr_task.parameters.control;

	if(data->parameters.control){
      data->anr_ul_control = l1a_l1s_com.anr_task.parameters.anr_ul_control;
      data->parameters.ns_level = l1a_l1s_com.anr_task.parameters.ns_level;
      data->parameters.tone_cnt_th = l1a_l1s_com.anr_task.parameters.tone_cnt_th;
      data->parameters.tone_ene_th = l1a_l1s_com.anr_task.parameters.tone_ene_th;
	}
#endif


	if( l1s_dsp_com.dsp_ndb_ptr->d_aqi_status & B_ANR_UL_STATE)
	{
		data->anr_ul_control = AUDIO_ANR_START;
		data->parameters.control = l1s_dsp_com.dsp_ndb_ptr->d_anr_control;
		data->parameters.ns_level = l1s_dsp_com.dsp_ndb_ptr->d_anr_ns_level ;
       	data->parameters.tone_cnt_th = l1s_dsp_com.dsp_ndb_ptr->d_anr_tone_cnt_th;
       	data->parameters.tone_ene_th = l1s_dsp_com.dsp_ndb_ptr->d_anr_tone_ene_th;
	}
	else
	{
		data->anr_ul_control = AUDIO_ANR_STOP;
	}

	return (AUDIO_OK);

    #else
      AUDIO_SEND_TRACE("AUDIO MODE READ: ANR: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
  #else
  	return AUDIO_ERROR;
  #endif
  }
  #endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_microphone_es_read                             */
  /*                                                                              */
  /*    Purpose:  This function reads the current ES setting.                     */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current ES setting.                                                   */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_microphone_es_read  (T_AUDIO_ES_CFG *data)
  {
  #if (PSP_STANDALONE != 1)
    #if (L1_ES == 1)
      UINT8   i;

      data->es_enable             = (UINT16) ((l1s_dsp_com.dsp_ndb_ptr->d_aqi_status & B_ES_STATE) >> 13);
	if (data->es_enable){
      data->es_behavior           = l1a_l1s_com.es_task.parameters.es_behavior;

      data->es_mode               = l1s_dsp_com.dsp_ndb_ptr->d_es_mode;
      data->es_gain_dl            = l1s_dsp_com.dsp_ndb_ptr->d_es_gain_dl;
      data->es_gain_ul_1          = l1s_dsp_com.dsp_ndb_ptr->d_es_gain_ul_1;
      data->es_gain_ul_2          = l1s_dsp_com.dsp_ndb_ptr->d_es_gain_ul_2;
      data->tcl_fe_ls_thr         = l1s_dsp_com.dsp_ndb_ptr->d_es_tcl_fe_ls_thr;
      data->tcl_dt_ls_thr         = l1s_dsp_com.dsp_ndb_ptr->d_es_tcl_dt_ls_thr;
      data->tcl_fe_ns_thr         = l1s_dsp_com.dsp_ndb_ptr->d_es_tcl_fe_ns_thr;
      data->tcl_dt_ns_thr         = l1s_dsp_com.dsp_ndb_ptr->d_es_tcl_dt_ns_thr;
      data->tcl_ne_thr            = l1s_dsp_com.dsp_ndb_ptr->d_es_tcl_ne_thr;
      data->ref_ls_pwr            = l1s_dsp_com.dsp_ndb_ptr->d_es_ref_ls_pwr;
      data->switching_time        = l1s_dsp_com.dsp_ndb_ptr->d_es_switching_time;
      data->switching_time_dt     = l1s_dsp_com.dsp_ndb_ptr->d_es_switching_time_dt;
      data->hang_time             = l1s_dsp_com.dsp_ndb_ptr->d_es_hang_time;
      data->gain_lin_dl_vect[0]   = l1s_dsp_com.dsp_ndb_ptr->a_es_gain_lin_dl_vect[0];
      data->gain_lin_dl_vect[1]   = l1s_dsp_com.dsp_ndb_ptr->a_es_gain_lin_dl_vect[1];
      data->gain_lin_dl_vect[2]   = l1s_dsp_com.dsp_ndb_ptr->a_es_gain_lin_dl_vect[2];
      data->gain_lin_dl_vect[3]   = l1s_dsp_com.dsp_ndb_ptr->a_es_gain_lin_dl_vect[3];
      data->gain_lin_ul_vect[0]   = l1s_dsp_com.dsp_ndb_ptr->a_es_gain_lin_ul_vect[0];
      data->gain_lin_ul_vect[1]   = l1s_dsp_com.dsp_ndb_ptr->a_es_gain_lin_ul_vect[1];
      data->gain_lin_ul_vect[2]   = l1s_dsp_com.dsp_ndb_ptr->a_es_gain_lin_ul_vect[2];
      data->gain_lin_ul_vect[3]   = l1s_dsp_com.dsp_ndb_ptr->a_es_gain_lin_ul_vect[3];
	}
      return (AUDIO_OK);

    #else
      AUDIO_SEND_TRACE("AUDIO MODE READ: ES: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
  #else
  	return AUDIO_ERROR;
  #endif

  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_iir_read                               */
  /*                                                                              */
  /*    Purpose:  This function reads the current IIR setting.                    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current IIR setting.                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_IIR == 1 || L1_IIR == 2)

#if( L1_IIR == 1)
  T_AUDIO_RET audio_mode_speaker_iir_read  (T_AUDIO_IIR_CFG *data)
#elif( L1_IIR == 2 )
  T_AUDIO_RET audio_mode_speaker_iir_read  (T_AUDIO_IIR_DL_CFG *data)
#endif
  {
  #if (PSP_STANDALONE != 1)
    #if (L1_IIR == 1)

      UINT16 i;

      data->iir_enable          = (UINT16)((l1s_dsp_com.dsp_ndb_ptr->d_aqi_status & B_IIR_STATE) >> 4);

	if (data->iir_enable){
      data->feedback            = l1s_dsp_com.dsp_ndb_ptr->d_iir_feedback;
      data->nb_fir_coefs        = l1s_dsp_com.dsp_ndb_ptr->d_iir_nb_fir_coefs;
      data->nb_iir_blocks       = l1s_dsp_com.dsp_ndb_ptr->d_iir_nb_iir_blocks;

      for (i=0; i<data->nb_fir_coefs; i++)
      {
        data->fir_coefs[i]           = l1s_dsp_com.dsp_ndb_ptr->a_iir_fir_coefs[i];
      }
      for (i=0; i<8*(data->nb_iir_blocks); i++)
      {
        data->iir_coefs[i]           = l1s_dsp_com.dsp_ndb_ptr->a_iir_iir_coefs[i];
      }
      data->fir_scaling         = l1s_dsp_com.dsp_ndb_ptr->d_iir_fir_scaling;
      data->input_gain_scaling  = l1s_dsp_com.dsp_ndb_ptr->d_iir_input_gain_scaling;
      data->input_scaling       = l1s_dsp_com.dsp_ndb_ptr->d_iir_input_scaling;
      data->output_gain         = l1s_dsp_com.dsp_ndb_ptr->d_iir_output_gain;
      data->output_gain_scaling = l1s_dsp_com.dsp_ndb_ptr->d_iir_output_gain_scaling;
	}
      return (AUDIO_OK);

    #elif(L1_IIR == 2)
	UINT16 i;


	/* Read from l1s_dsp_com and populate the parameter */
	if ( l1s_dsp_com.dsp_ndb_ptr->d_aqi_status & B_IIR_DL_STATE)
	{
		data->iir_dl_control = AUDIO_IIR_START; //l1a_l1s_com.iir_task.parameters->iir_dl_control;

		data->parameters.control = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_control;
		data->parameters.frame_size = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_frame_size;
		data->parameters.fir_swap = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_fir_swap;
		data->parameters.fir_filter.fir_enable = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_fir_enable;
		data->parameters.fir_filter.fir_length = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_fir_length;
		data->parameters.fir_filter.fir_shift = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_fir_shift;

	 	for(i = 0; i < AUDIO_IIR_MAX_FIR_TAPS; i++)
	  	{
	  		data->parameters.fir_filter.fir_taps[i] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_fir_taps[i];
	  	}

		data->parameters.sos_filter.sos_enable = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_enable;
	  	data->parameters.sos_filter.sos_number = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_number;

      	  	data->parameters.sos_filter.sos_filter[0].sos_fact = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_1;
	  	data->parameters.sos_filter.sos_filter[0].sos_fact_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_form_1;
	  	data->parameters.sos_filter.sos_filter[0].sos_den[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_1[0];
	  	data->parameters.sos_filter.sos_filter[0].sos_den[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_1[1];
         	data->parameters.sos_filter.sos_filter[0].sos_num[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_1[0];
	  	data->parameters.sos_filter.sos_filter[0].sos_num[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_1[1];
	  	data->parameters.sos_filter.sos_filter[0].sos_num[2] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_1[2];
	  	data->parameters.sos_filter.sos_filter[0].sos_num_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_num_form_1;

	  	data->parameters.sos_filter.sos_filter[1].sos_fact = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_2;
	  	data->parameters.sos_filter.sos_filter[1].sos_fact_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_form_2;
	  	data->parameters.sos_filter.sos_filter[1].sos_den[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_2[0];
	  	data->parameters.sos_filter.sos_filter[1].sos_den[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_2[1];
         	data->parameters.sos_filter.sos_filter[1].sos_num[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_2[0];
	  	data->parameters.sos_filter.sos_filter[1].sos_num[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_2[1];
	  	data->parameters.sos_filter.sos_filter[1].sos_num[2] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_2[2];
	  	data->parameters.sos_filter.sos_filter[1].sos_num_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_num_form_2;

	  	data->parameters.sos_filter.sos_filter[2].sos_fact = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_3;
	  	data->parameters.sos_filter.sos_filter[2].sos_fact_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_form_3;
	  	data->parameters.sos_filter.sos_filter[2].sos_den[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_3[0];
	  	data->parameters.sos_filter.sos_filter[2].sos_den[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_3[1];
         	data->parameters.sos_filter.sos_filter[2].sos_num[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_3[0];
	  	data->parameters.sos_filter.sos_filter[2].sos_num[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_3[1];
	  	data->parameters.sos_filter.sos_filter[2].sos_num[2] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_3[2];
	  	data->parameters.sos_filter.sos_filter[2].sos_num_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_num_form_3;

	  	data->parameters.sos_filter.sos_filter[3].sos_fact = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_4;
	  	data->parameters.sos_filter.sos_filter[3].sos_fact_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_form_4;
	  	data->parameters.sos_filter.sos_filter[3].sos_den[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_4[0];
	  	data->parameters.sos_filter.sos_filter[3].sos_den[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_4[1];
         	data->parameters.sos_filter.sos_filter[3].sos_num[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_4[0];
	  	data->parameters.sos_filter.sos_filter[3].sos_num[1] =  l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_4[1];
	  	data->parameters.sos_filter.sos_filter[3].sos_num[2] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_4[2];
	  	data->parameters.sos_filter.sos_filter[3].sos_num_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_num_form_4;

	  	data->parameters.sos_filter.sos_filter[4].sos_fact = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_5;
	  	data->parameters.sos_filter.sos_filter[4].sos_fact_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_form_5;
	  	data->parameters.sos_filter.sos_filter[4].sos_den[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_5[0];
	  	data->parameters.sos_filter.sos_filter[4].sos_den[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_5[1];
         	data->parameters.sos_filter.sos_filter[4].sos_num[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_5[0];
	  	data->parameters.sos_filter.sos_filter[4].sos_num[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_5[1];
	  	data->parameters.sos_filter.sos_filter[4].sos_num[2] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_5[2];
	  	data->parameters.sos_filter.sos_filter[4].sos_num_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_num_form_5;

	  	data->parameters.sos_filter.sos_filter[5].sos_fact = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_6;
	  	data->parameters.sos_filter.sos_filter[5].sos_fact_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_fact_form_6;
	  	data->parameters.sos_filter.sos_filter[5].sos_den[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_6[0];
	  	data->parameters.sos_filter.sos_filter[5].sos_den[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_den_6[1];
         	data->parameters.sos_filter.sos_filter[5].sos_num[0] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_6[0];
	  	data->parameters.sos_filter.sos_filter[5].sos_num[1] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_6[1];
	  	data->parameters.sos_filter.sos_filter[5].sos_num[2] = l1s_dsp_com.dsp_ndb_ptr->a_iir4x_sos_num_6[2];
	  	data->parameters.sos_filter.sos_filter[5].sos_num_form = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_sos_num_form_6;

	  	data->parameters.gain = l1s_dsp_com.dsp_ndb_ptr->d_iir4x_gain;
	}
	else
	{
		data->iir_dl_control = AUDIO_IIR_STOP;
	}

  	return AUDIO_OK;
    #else
      AUDIO_SEND_TRACE("AUDIO MODE READ: IIR: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
  #else
  	return AUDIO_ERROR;
  #endif
  }
#endif


  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_drc_read                               */
  /*                                                                              */
  /*    Purpose:  This function reads the current DRC setting.                    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current DRC setting.                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  #if (L1_DRC == 1)
  T_AUDIO_RET audio_mode_speaker_drc_read  (T_AUDIO_DRC_CFG *data)
  {
  #if (PSP_STANDALONE != 1)

	UINT16 i;

	if(l1s_dsp_com.dsp_ndb_ptr->d_aqi_status & B_DRC_DL_STATE)
	{
		data->drc_dl_control = AUDIO_DRC_START;
		data->parameters.speech_mode_samp_f = drc_ndb->d_drc_speech_mode_samp_f;
	 	data->parameters.num_subbands = drc_ndb->d_drc_num_subbands;
	 	data->parameters.frame_len =  drc_ndb->d_drc_frame_len;
	 	data->parameters.expansion_knee_fb_bs =  drc_ndb->d_drc_expansion_knee_fb_bs;
	 	data->parameters.expansion_knee_md_hg = drc_ndb->d_drc_expansion_knee_md_hg;
	 	data->parameters.expansion_ratio_md_hg =  drc_ndb->d_drc_expansion_ratio_md_hg;
        	data->parameters.expansion_ratio_fb_bs =  drc_ndb->d_drc_expansion_ratio_fb_bs;
	 	data->parameters.max_amplification_fb_bs =  drc_ndb->d_drc_max_amplification_fb_bs;
	 	data->parameters.max_amplification_md_hg =  drc_ndb->d_drc_max_amplification_md_hg;
	 	data->parameters.compression_knee_fb_bs =  drc_ndb->d_drc_compression_knee_fb_bs;
	 	data->parameters.compression_knee_md_hg =  drc_ndb->d_drc_compression_knee_md_hg;
	 	data->parameters.compression_ratio_fb_bs =  drc_ndb->d_drc_compression_ratio_fb_bs;
	 	data->parameters.compression_ratio_md_hg =  drc_ndb->d_drc_compression_ratio_md_hg;
	 	data->parameters.energy_limiting_th_fb_bs =  drc_ndb->d_drc_energy_limiting_th_fb_bs;
	 	data->parameters.energy_limiting_th_md_hg =  drc_ndb->d_drc_energy_limiting_th_md_hg;
	 	data->parameters.limiter_threshold_fb =  drc_ndb->d_drc_limiter_threshold_fb;
	 	data->parameters.limiter_threshold_bs =  drc_ndb->d_drc_limiter_threshold_bs;
	 	data->parameters.limiter_threshold_md =  drc_ndb->d_drc_limiter_threshold_md;
	 	data->parameters.limiter_threshold_hg =  drc_ndb->d_drc_limiter_threshold_hg;
	 	data->parameters.limiter_hangover_spect_preserve =  drc_ndb->d_drc_limiter_hangover_spect_preserve;
	 	data->parameters.limiter_release_fb_bs =  drc_ndb->d_drc_limiter_release_fb_bs;
	 	data->parameters.limiter_release_md_hg =  drc_ndb->d_drc_limiter_release_md_hg;
	 	data->parameters.gain_track_fb_bs =  drc_ndb->d_drc_gain_track_fb_bs;
	 	data->parameters.gain_track_md_hg =  drc_ndb->d_drc_gain_track_md_hg;

       	for(i = 0; i < 17; i++)
	 	{
	   		data->parameters.low_pass_filter[i] = drc_ndb->a_drc_low_pass_filter[i];
	   		data->parameters.mid_band_filter[i] = drc_ndb->a_drc_mid_band_filter[i];
	 	}
	}
	else
	{
		data->drc_dl_control = AUDIO_DRC_STOP;
	}

	return (AUDIO_OK);
  #else
  	return AUDIO_ERROR;
  #endif
  }
  #endif
  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_speaker_limiter_read                           */
  /*                                                                              */
  /*    Purpose:  This function reads the current LIMITER setting.                */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current LIMITER setting.                                              */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
  T_AUDIO_RET audio_mode_speaker_limiter_read  (T_AUDIO_LIMITER_CFG *data)
  {
  #if (PSP_STANDALONE != 1)
    #if (L1_LIMITER)
      UINT16 i;

      data->limiter_enable      = (UINT16)((l1s_dsp_com.dsp_ndb_ptr->d_aqi_status & B_LIM_STATE) >> 5);
	if(data->limiter_enable){
      data->block_size          = l1s_dsp_com.dsp_ndb_ptr->d_lim_block_size;
      data->nb_fir_coefs        = l1s_dsp_com.dsp_ndb_ptr->d_lim_nb_fir_coefs;

      for (i=0; i<data->nb_fir_coefs; i++)
      {
        data->filter_coefs[i]    = l1s_dsp_com.dsp_ndb_ptr->a_lim_filter_coefs[i];
      }
      data->gain_fall           = l1s_dsp_com.dsp_ndb_ptr->d_lim_gain_fall_q15;
      data->gain_rise           = l1s_dsp_com.dsp_ndb_ptr->d_lim_gain_rise_q15;
      data->slope_update_period = l1s_dsp_com.dsp_ndb_ptr->d_lim_slope_update_period;
      data->thr_high_0          = l1a_l1s_com.limiter_task.parameters.thr_high_0;
      data->thr_high_slope      = l1a_l1s_com.limiter_task.parameters.thr_high_slope;
      data->thr_low_0           = l1a_l1s_com.limiter_task.parameters.thr_low_0;
      data->thr_low_slope       = l1a_l1s_com.limiter_task.parameters.thr_low_slope;
	}
      return (AUDIO_OK);

    #else
      AUDIO_SEND_TRACE("AUDIO MODE READ: Limiter: feature not supported",
        RV_TRACE_LEVEL_ERROR);
      return(AUDIO_ERROR);
    #endif
  #else
  	return AUDIO_ERROR;
  #endif
  }

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_agc_ul_read                            */
  /*                                                                              */
  /*    Purpose:  This function reads the current AGC_UL setting.                    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current AGC_UL setting.                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_AGC_UL == 1)
  T_AUDIO_RET audio_mode_agc_ul_read  (T_AUDIO_AQI_AGC_UL_REQ *data)
  {
  #if (PSP_STANDALONE != 1)
#if 0
    data->parameters.control = l1a_l1s_com.agc_ul_task.parameters.control;
	if(data->parameters.control){
        data->agc_ul_control = (UINT16) l1a_l1s_com.agc_ul_task.parameters.agc_ul_control;
        data->parameters.frame_size = l1a_l1s_com.agc_ul_task.parameters.frame_size;
        data->parameters.targeted_level = l1a_l1s_com.agc_ul_task.parameters.targeted_level;
        data->parameters.signal_up = l1a_l1s_com.agc_ul_task.parameters.signal_up;
        data->parameters.signal_down = l1a_l1s_com.agc_ul_task.parameters.signal_down;
        data->parameters.max_scale = l1a_l1s_com.agc_ul_task.parameters.max_scale;

		data->parameters.gain_smooth_alpha = l1a_l1s_com.agc_ul_task.parameters.gain_smooth_alpha;
        data->parameters.gain_smooth_alpha_fast = l1a_l1s_com.agc_ul_task.parameters.gain_smooth_alpha_fast;
        data->parameters.gain_smooth_beta = l1a_l1s_com.agc_ul_task.parameters.gain_smooth_beta;
        data->parameters.gain_smooth_beta_fast = l1a_l1s_com.agc_ul_task.parameters.gain_smooth_beta_fast;
        data->parameters.gain_intp_flag = l1a_l1s_com.agc_ul_task.parameters.gain_intp_flag;
	}
#endif

	if(l1s_dsp_com.dsp_ndb_ptr->d_aqi_status & B_AGC_UL_STATE)
	{
		data->agc_ul_control = AUDIO_AGC_START;
		data->parameters.control = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_control;
		data->parameters.frame_size = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_frame_size;
		data->parameters.targeted_level = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_targeted_level;
		data->parameters.signal_up = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_signal_up;
		data->parameters.signal_down = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_signal_down;
		data->parameters.max_scale = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_max_scale;
		data->parameters.gain_smooth_alpha = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_gain_smooth_alpha;
		data->parameters.gain_smooth_alpha_fast = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_gain_smooth_alpha_fast;
		data->parameters.gain_smooth_beta = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_gain_smooth_beta;
		data->parameters.gain_smooth_beta_fast = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_gain_smooth_beta_fast;
		data->parameters.gain_intp_flag = l1s_dsp_com.dsp_ndb_ptr->d_agc_ul_gain_intp_flag;
	}
	else
	{
		data->agc_ul_control = AUDIO_AGC_STOP;
	}

      return (AUDIO_OK);

    #else
      return(AUDIO_ERROR);
    #endif
  }
#endif

  /********************************************************************************/
  /*                                                                              */
  /*    Function Name:  audio_mode_agc_dl_read                            */
  /*                                                                              */
  /*    Purpose:  This function reads the current AGC_DL setting.                    */
  /*                                                                              */
  /*    Input Parameters:                                                         */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Output Parameters:                                                        */
  /*        Status.                                                               */
  /*        Current AGC_DL setting.                                                  */
  /*                                                                              */
  /*    Note:                                                                     */
  /*       None.                                                                  */
  /*                                                                              */
  /*    Revision History:                                                         */
  /*        None.                                                                 */
  /*                                                                              */
  /********************************************************************************/
#if (L1_AGC_DL == 1)
  T_AUDIO_RET audio_mode_agc_dl_read  (T_AUDIO_AQI_AGC_DL_REQ *data)
  {
  #if (PSP_STANDALONE != 1)

#if 0
    data->parameters.control = l1a_l1s_com.agc_dl_task.parameters.control;
	if(data->parameters.control){
        data->agc_dl_control = (UINT16) l1a_l1s_com.agc_dl_task.parameters.agc_dl_control;
        data->parameters.frame_size = l1a_l1s_com.agc_dl_task.parameters.frame_size;
        data->parameters.targeted_level = l1a_l1s_com.agc_dl_task.parameters.targeted_level;
        data->parameters.signal_up = l1a_l1s_com.agc_dl_task.parameters.signal_up;
        data->parameters.signal_down = l1a_l1s_com.agc_dl_task.parameters.signal_down;
        data->parameters.max_scale = l1a_l1s_com.agc_dl_task.parameters.max_scale;

 	 data->parameters.gain_smooth_alpha = l1a_l1s_com.agc_dl_task.parameters.gain_smooth_alpha;
        data->parameters.gain_smooth_alpha_fast = l1a_l1s_com.agc_dl_task.parameters.gain_smooth_alpha_fast;
        data->parameters.gain_smooth_beta = l1a_l1s_com.agc_dl_task.parameters.gain_smooth_beta;
        data->parameters.gain_smooth_beta_fast = l1a_l1s_com.agc_dl_task.parameters.gain_smooth_beta_fast;
        data->parameters.gain_intp_flag = l1a_l1s_com.agc_dl_task.parameters.gain_intp_flag;
	}
#endif

	if(l1s_dsp_com.dsp_ndb_ptr->d_aqi_status &  B_AGC_DL_STATE)
	{
		data->agc_dl_control = AUDIO_AGC_START;
		data->parameters.control = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_control;
		data->parameters.frame_size = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_frame_size;
		data->parameters.targeted_level = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_targeted_level;
		data->parameters.signal_up = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_signal_up;
		data->parameters.signal_down = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_signal_down;
		data->parameters.max_scale = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_max_scale;
		data->parameters.gain_smooth_alpha = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_gain_smooth_alpha;
		data->parameters.gain_smooth_alpha_fast = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_gain_smooth_alpha_fast;
		data->parameters.gain_smooth_beta = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_gain_smooth_beta;
		data->parameters.gain_smooth_beta_fast = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_gain_smooth_beta_fast;
		data->parameters.gain_intp_flag = l1s_dsp_com.dsp_ndb_ptr->d_agc_dl_gain_intp_flag;
	}
	else
	{
		data->agc_dl_control = AUDIO_AGC_STOP;
	}

      return (AUDIO_OK);
  #else
  	return AUDIO_ERROR;
  #endif
  }
#endif
#endif /* RVM_AUDIO_MAIN_SWE */

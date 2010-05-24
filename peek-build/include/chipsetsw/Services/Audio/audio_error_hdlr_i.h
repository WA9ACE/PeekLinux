/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_error_hdlr_i.h                                        */
/*                                                                          */
/*  Purpose:  This file contains routines used to report unrecoverable      */
/*            memory errors that might occur.                               */
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
  #ifndef  _AUDIO_ERROR_HDLR_I_H
    #define  _AUDIO_ERROR_HDLR_I_H

    #include "l1_confg.h"

    /* error trace functions prototypes */
    #if (KEYBEEP)
      void audio_keybeep_error_trace(UINT8 error_id);
    #endif

    #if (TONE)
      void audio_tones_error_trace(UINT8 error_id);
    #endif

    #if (MELODY_E1)
      void audio_melody_E1_error_trace(UINT8 error_id);
    #endif

    #if (MELODY_E2)
      void audio_melody_E2_error_trace(UINT8 error_id);
    #endif

    #if (VOICE_MEMO)
      void audio_voice_memo_error_trace(UINT8 error_id);
    #endif

    void audio_mode_error_trace(UINT8 error_id);

    #if (SPEECH_RECO)
      void audio_sr_error_trace(UINT8 error_id);
    #endif

  #if (MELODY_E1) || (VOICE_MEMO) || (SPEECH_RECO)
    void audio_ffs_error_trace(UINT8 error_id);
  #endif

    #if (L1_VOICE_MEMO_AMR) || (L1_MP3) || (L1_AAC)
      void audio_new_ffs_error_trace(UINT8 error_id);
    #endif

    #if (L1_VOICE_MEMO_AMR)
      void audio_voice_memo_amr_error_trace(UINT8 error_id);
    #endif

    #if (L1_AUDIO_DRIVER)
      void audio_driver_error_trace(UINT8 error_id);
    #endif

    #if (L1_MP3==1)
      void audio_mp3_error_trace(UINT8 error_id);
    #endif

    #if (L1_AAC==1)
      void audio_aac_error_trace(UINT8 error_id);
    #endif

    /* extern prototype */
    extern void audio_error_trace(UINT8 error_id);

    #if (L1_EXT_AUDIO_MGT==1)
      void audio_midi_error_trace(UINT8 error_id);
    #endif
    
    /* Id of the error trace */
    #define AUDIO_ENTITY_NOT_START             (0)
    #define AUDIO_ENTITY_NO_MEMORY             (1)
    #define AUDIO_ERROR_STOP_EVENT             (2)
    #define AUDIO_ERROR_START_EVENT            (3)
    #define AUDIO_ERROR_INIT_EVENT             (4)
    #define AUDIO_ERROR_FREE_EVENT             (5)
    #define AUDIO_ENTITY_BAD_PARAMETER         (6)
    #define AUDIO_ENTITY_BAD_MESSAGE           (7)
    #define AUDIO_ERROR_MODE                   (8)
    #define AUDIO_ENTITY_FILE_ERROR            (9)
    #define AUDIO_ENTITY_FILE_NO_SAVED         (10)
    #define AUDIO_ENTITY_FILE_ALREADY_EXIST    (11)
    #define AUDIO_ENTITY_FILE_NO_CLOSE         (12)
    #define AUDIO_ENTITY_SPEECH_NO_SAVED       (13)
    #define AUDIO_ENTITY_MODEL_NO_SAVED        (14)
    #define AUDIO_ENTITY_BAD_DATABASE          (15)
    #define AUDIO_ENTITY_MODEL_NO_LOAD         (16)
    #define AUDIO_ENTITY_AUDIO_MODE_NO_LOADED  (17)
    #define AUDIO_ERROR_DOWNLOAD               (18)
    #define AUDIO_ERROR_NOT_DOWNLOAD           (19)
    #define AUDIO_ENTITY_LOAD_FILE_INSTR_ERROR (20)
    #define AUDIO_ERROR_INSTRUMENT_SIZE        (21)
    #define AUDIO_FEATURE_NOT_COMPILED         (22)
    #define AUDIO_ERROR_L1_START_EVENT         (23)
    #define AUDIO_ERROR_L1_STOP_EVENT          (24)
    #define AUDIO_ERROR_L1_PAUSE_EVENT         (25)
    #define AUDIO_ERROR_L1_RESUME_EVENT        (26)
    #define AUDIO_ERROR_L1_RESTART_EVENT       (27)
    #define AUDIO_ERROR_PAUSE_EVENT            (28)
    #define AUDIO_ERROR_RESUME_EVENT           (29)
    #define AUDIO_ERROR_L1_INFO_EVENT          (30)
    #define AUDIO_ERROR_INFO_EVENT             (31)
    #define AUDIO_ERROR_RESTART_EVENT          (32)

  #endif
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */

/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_const_i.h                                             */
/*                                                                          */
/*  Purpose:  Internal constants used by AUDIO instance                     */
/*                                                                          */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  18 May 2001  Create                                                     */
/*                                                                          */
/*  Author      Francois Mazard - Stephanie Gerthoux                        */
/*                                                                          */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"
#include "l1_confg.h"
#ifdef RVM_AUDIO_MAIN_SWE

  #ifndef __AUDIO_CONST_I_H_
    #define __AUDIO_CONST_I_H_

    #ifdef __cplusplus
      extern "C"
        {
    #endif

    /* Dummy definition */
    #define MMI_QUEUE (0)

    /* Define a mask used to identify the events                       */
    #define AUDIO_EVENT_EXTERN                      (0x0C00)
    #define AUDIO_EVENT_INTERN                      (0x0300)

    /* The FFS isn't activated: the audio task run without any time out */
    #define AUDIO_NOT_TIME_OUT                          (0)

    /* The FFS downloader is waked up by a timer */
    #define AUDIO_FFS_TIMER                             (RVF_TIMER_0)

    /* The audio entity process only the message send to the following mail box */
    #define AUDIO_MBOX                                (RVF_TASK_MBOX_0)

    /* The FFS manager start only with a message */
    #define AUDIO_FFS_MBOX                            (RVF_TASK_MBOX_0)

    /* The audio entity takes into account only the following events: */
    #define AUDIO_TASK_MBOX_EVT_MASK                  (RVF_TASK_MBOX_0_EVT_MASK)
    #define AUDIO_FFS_TIMER_EVT_MASK                  (RVF_TIMER_0_EVT_MASK)

    // simulator's events
    #define AUDIO_MIDI_L1_SIMUL_TIMER            (RVF_TIMER_1)
    #define AUDIO_MIDI_L1_SIMUL_TIMER_EVT_MASK   (RVF_TIMER_1_EVT_MASK)
    #define AUDIO_MIDI_L1_SIMUL_ONE_SHOT_TIMER   (FALSE)

    #define AUDIO_VM_AMR_RECORD_L1_SIMUL_TIMER            (RVF_TIMER_1)
    #define AUDIO_VM_AMR_RECORD_L1_SIMUL_TIMER_EVT_MASK   (RVF_TIMER_1_EVT_MASK)
    #define AUDIO_VM_AMR_RECORD_L1_SIMUL_ONE_SHOT_TIMER   (FALSE)

    #define AUDIO_VM_AMR_PLAY_L1_SIMUL_TIMER              (RVF_TIMER_1)
    #define AUDIO_VM_AMR_PLAY_L1_SIMUL_TIMER_EVT_MASK     (RVF_TIMER_1_EVT_MASK)
    #define AUDIO_VM_AMR_PLAY_L1_SIMUL_ONE_SHOT_TIMER     (FALSE)

    /* The audio entity wait all event type */
    #define AUDIO_ALL_EVENT_FLAGS                       (0xFFFF)

    #if (KEYBEEP)
      /* Define all the state of the Audio Keepbeep State Machine */
      #define AUDIO_KEYBEEP_IDLE                          (1)
      #define AUDIO_KEYBEEP_WAIT_START_CON                (2)
      #define AUDIO_KEYBEEP_WAIT_STOP_COMMAND             (3)
      #define AUDIO_KEYBEEP_WAIT_START_CON_TO_STOP        (4)
      #define AUDIO_KEYBEEP_WAIT_STOP_CON                 (5)
    #endif

    #if (TONE)
      /* Define all the state of the Audio Tones State Machine */
      #define AUDIO_TONES_IDLE                            (1)
      #define AUDIO_TONES_WAIT_START_CON                  (2)
      #define AUDIO_TONES_WAIT_STOP_COMMAND               (3)
      #define AUDIO_TONES_WAIT_START_CON_TO_STOP          (4)
      #define AUDIO_TONES_WAIT_STOP_CON                   (5)
    #endif

    #if (KEYBEEP) || (TONE) || (VOICE_MEMO)
      /* Definition of the minimum and maximum of each parameter */
      #define FREQUENCY_BEEP_MIN                          (0)    /* Hz */
      #define FREQUENCY_BEEP_MAX                          (2000) /* Hz */
      #define AMPLITUDE_BEEP_MIN                          (-48)  /* dB */
      #define AMPLITUDE_BEEP_MAX                          (0)    /* dB */
      #define DURATION_BEEP_MIN                           (20)
    #endif

    #if (MELODY_E1)
      /* Define all the state of the Audio Melody_E1 number 0 State Machine */
      #define AUDIO_MELODY_E1_IDLE                       (1)
      #define AUDIO_MELODY_E1_WAIT_INIT_FFS              (2)
      #define AUDIO_MELODY_E1_WAIT_START_CON             (3)
      #define AUDIO_MELODY_E1_WAIT_STOP_COMMAND          (4)
      #define AUDIO_MELODY_E1_WAIT_START_CON_TO_STOP     (5)
      #define AUDIO_MELODY_E1_WAIT_STOP_CON              (6)
      #define AUDIO_MELODY_E1_WAIT_STOP_FFS              (7)
      #define AUDIO_MELODY_E1_WAIT_INIT_DONE             (8)

      /* Melody E1 number ID */
      #define AUDIO_MELODY_E1_0     (0)
      #define AUDIO_MELODY_E1_1     (1)
      #define AUDIO_MELODY_E1_NONE  (0xFF)
    #endif

    #if (MELODY_E2)
      /* Define all the state of the Audio Melody_E2 number 0 State Machine */
      #define AUDIO_MELODY_E2_IDLE                       (1)
      #define AUDIO_MELODY_E2_WAIT_INIT_FFS              (2)
      #define AUDIO_MELODY_E2_WAIT_START_CON             (3)
      #define AUDIO_MELODY_E2_WAIT_STOP_COMMAND          (4)
      #define AUDIO_MELODY_E2_WAIT_START_CON_TO_STOP     (5)
      #define AUDIO_MELODY_E2_WAIT_STOP_CON              (6)
      #define AUDIO_MELODY_E2_WAIT_STOP_FFS              (7)
      #define AUDIO_MELODY_E2_WAIT_INIT_DONE             (8)

      /* Melody E2 number ID */
      #define AUDIO_MELODY_E2_0     (0)
      #define AUDIO_MELODY_E2_1     (1)
      #define AUDIO_MELODY_E2_NONE  (0xFF)
    #endif

    #if (VOICE_MEMO)
      /* Define all the state of the Audio Voice Memorization Play State Machine */
      #define AUDIO_VM_PLAY_IDLE                       (1)
      #define AUDIO_VM_PLAY_WAIT_INIT_FFS              (2)
      #define AUDIO_VM_PLAY_WAIT_START_CON             (3)
      #define AUDIO_VM_PLAY_WAIT_STOP_COMMAND          (4)
      #define AUDIO_VM_PLAY_WAIT_START_CON_TO_STOP     (5)
      #define AUDIO_VM_PLAY_WAIT_STOP_CON              (6)
      #define AUDIO_VM_PLAY_WAIT_STOP_FFS              (7)
      #define AUDIO_VM_PLAY_WAIT_INIT_DONE             (8)

      /* Define all the state of the Audio Voice Memorization Record State Machine */
      #define AUDIO_VM_RECORD_IDLE                       (1)
      #define AUDIO_VM_RECORD_WAIT_INIT_FFS              (2)
      #define AUDIO_VM_RECORD_WAIT_START_CON             (3)
      #define AUDIO_VM_RECORD_WAIT_STOP_COMMAND          (4)
      #define AUDIO_VM_RECORD_WAIT_START_CON_TO_STOP     (5)
      #define AUDIO_VM_RECORD_WAIT_STOP_CON              (6)
      #define AUDIO_VM_RECORD_WAIT_STOP_FFS              (7)
      #define AUDIO_VM_RECORD_WAIT_INIT_DONE             (8)

      /* Voice Memo number ID */
      #define AUDIO_VM_PLAY       (2)
      #define AUDIO_VM_RECORD     (3)
      #define AUDIO_VM_NONE       (0xFF)

    #endif

#if (L1_PCM_EXTRACTION) //VENKAT
	#define AUDIO_PCM_SAMPLES_PER_SEC (8000)

      #define AUDIO_VM_PCM_PLAY_IDLE                       (1)
      #define AUDIO_VM_PCM_PLAY_WAIT_INIT_FFS              (2)
      #define AUDIO_VM_PCM_PLAY_WAIT_START_CON             (3)
      #define AUDIO_VM_PCM_PLAY_WAIT_STOP_COMMAND          (4)
      #define AUDIO_VM_PCM_PLAY_WAIT_START_CON_TO_STOP     (5)
      #define AUDIO_VM_PCM_PLAY_WAIT_STOP_CON              (6)
      #define AUDIO_VM_PCM_PLAY_WAIT_STOP_FFS              (7)
      #define AUDIO_VM_PCM_PLAY_WAIT_INIT_DONE             (8)


/* Define all the state of the Audio Voice Memorization Record State Machine */
      #define AUDIO_VM_PCM_RECORD_IDLE                       (1)
      #define AUDIO_VM_PCM_RECORD_WAIT_INIT_FFS              (2)
      #define AUDIO_VM_PCM_RECORD_WAIT_START_CON             (3)
      #define AUDIO_VM_PCM_RECORD_WAIT_STOP_COMMAND          (4)
      #define AUDIO_VM_PCM_RECORD_WAIT_START_CON_TO_STOP     (5)
      #define AUDIO_VM_PCM_RECORD_WAIT_STOP_CON              (6)
      #define AUDIO_VM_PCM_RECORD_WAIT_STOP_FFS              (7)
      #define AUDIO_VM_PCM_RECORD_WAIT_INIT_DONE             (8)
	  
	#define AUDIO_VM_PCM_PLAY		(2)
	#define AUDIO_VM_PCM_RECORD	(3)
	#define AUDIO_VM_PCM_NONE		(0xFF)
	#define AUDIO_VMEMO_PCM_SESSION (1)		

      #define AUDIO_VBUF_PCM_PLAY_IDLE                       (1)
      #define AUDIO_VBUF_PCM_PLAY_WAIT_INIT_FFS              (2)
      #define AUDIO_VBUF_PCM_PLAY_WAIT_START_CON             (3)
      #define AUDIO_VBUF_PCM_PLAY_WAIT_STOP_COMMAND          (4)
      #define AUDIO_VBUF_PCM_PLAY_WAIT_START_CON_TO_STOP     (5)
      #define AUDIO_VBUF_PCM_PLAY_WAIT_STOP_CON              (6)
      #define AUDIO_VBUF_PCM_PLAY_WAIT_STOP_FFS              (7)
      #define AUDIO_VBUF_PCM_PLAY_WAIT_INIT_DONE             (8)


/* Define all the state of the Audio Voice Memorization Record State Machine */
      #define AUDIO_VBUF_PCM_RECORD_IDLE                       (1)
      #define AUDIO_VBUF_PCM_RECORD_WAIT_INIT_FFS              (2)
      #define AUDIO_VBUF_PCM_RECORD_WAIT_START_CON             (3)
      #define AUDIO_VBUF_PCM_RECORD_WAIT_STOP_COMMAND          (4)
      #define AUDIO_VBUF_PCM_RECORD_WAIT_START_CON_TO_STOP     (5)
      #define AUDIO_VBUF_PCM_RECORD_WAIT_STOP_CON              (6)
      #define AUDIO_VBUF_PCM_RECORD_WAIT_STOP_FFS              (7)
      #define AUDIO_VBUF_PCM_RECORD_WAIT_INIT_DONE             (8)
      #define AUDIO_VBUF_PCM_RECORD_WAIT_STOP_PLAY		(9)
	  
	#define AUDIO_VBUF_PCM_PLAY		(2)
	#define AUDIO_VBUF_PCM_RECORD	(3)
	#define AUDIO_VBUF_PCM_NONE		(0xFF)
	#define AUDIO_VBUF_PCM_SESSION (2)
#endif
	
	
    #if (L1_VOICE_MEMO_AMR)
      /* Voice Memo number ID */
      #define AUDIO_VM_AMR_PLAY       (0)
      #define AUDIO_VM_AMR_RECORD     (1)
      #define AUDIO_VM_AMR_NONE       (0xFF)

      // RX_TYPE or TX_TYPE (See 06.93)
      #define AUDIO_VM_AMR_RXTX_SPEECH_GOOD     (0<<3)
      #define AUDIO_VM_AMR_RXTX_SPEECH_DEGRADED (1<<3)
      #define AUDIO_VM_AMR_RXTX_ONSET           (2<<3)
      #define AUDIO_VM_AMR_RXTX_SPEECH_BAD      (3<<3)
      #define AUDIO_VM_AMR_RXTX_SID_FIRST       (4<<3)
      #define AUDIO_VM_AMR_RXTX_SID_UPDATE      (5<<3)
      #define AUDIO_VM_AMR_RXTX_SID_BAD         (6<<3)
      #define AUDIO_VM_AMR_RXTX_NO_DATA         (7<<3)

      // Speech channel type
      #define AUDIO_VM_AMR_SPEECH_475      0
      #define AUDIO_VM_AMR_SPEECH_515      1
      #define AUDIO_VM_AMR_SPEECH_59       2
      #define AUDIO_VM_AMR_SPEECH_67       3
      #define AUDIO_VM_AMR_SPEECH_74       4
      #define AUDIO_VM_AMR_SPEECH_795      5
      #define AUDIO_VM_AMR_SPEECH_102      6
      #define AUDIO_VM_AMR_SPEECH_122      7

      // FT mapping for MMS storage
      #define AUDIO_MMS_SID_FRAME_TYPE     (8)
      #define AUDIO_MMS_NO_DATA_FRAME_TYPE (15)

      #define AUDIO_MMS_FRAME_TYPE_SHIFT   (3)
      #define AUDIO_MMS_FRAME_TYPE_MASK    (0x78)

      // quality mapping for MMS storage
      #define AUDIO_MMS_GOOD_QUALITY       (1)
      #define AUDIO_MMS_BAD_QUALITY        (0)

      #define AUDIO_MMS_QUALITY_SHIFT      (2)
      #define AUDIO_MMS_QUALITY_MASK       (0x04)

      #define AUDIO_MMS_STI_BIT         (0x10)
      #define AUDIO_MMS_MODE_INDICATION (0xC)

      #define AUDIO_VM_AMR_SPEECH_475_DATA_SIZE  12
      #define AUDIO_VM_AMR_SPEECH_515_DATA_SIZE  13
      #define AUDIO_VM_AMR_SPEECH_590_DATA_SIZE  15
      #define AUDIO_VM_AMR_SPEECH_670_DATA_SIZE  17
      #define AUDIO_VM_AMR_SPEECH_740_DATA_SIZE  19
      #define AUDIO_VM_AMR_SPEECH_795_DATA_SIZE  20
      #define AUDIO_VM_AMR_SPEECH_102_DATA_SIZE  26
      #define AUDIO_VM_AMR_SPEECH_122_DATA_SIZE  31

      #define AUDIO_VM_AMR_SID_DATA_SIZE          5

      #define AUDIO_VM_AMR_NO_DATA_DATA_SIZE      0

      #define AUDIO_VM_AMR_ONSET_DATA_SIZE        0

      #define AUDIO_VM_AMR_MAX_SAMPLE_SIZE_16BIT  ((1+31) >> 1)
    #endif

    #if (L1_EXT_AUDIO_MGT==1)
      #define AUDIO_MIDI 0
      #define AUDIO_MIDI_NONE 0xFF
    #endif

      // driver
      #define AUDIO_PLAY_API_STATE_GET_BUF  (1)
      #define AUDIO_PLAY_API_STATE_PLAY_BUF (2)

      #define AUDIO_DMA_CHANNEL             (1)

      /* Define all the state of a session */
      #define AUDIO_IDLE                                  (0)
      #define AUDIO_WAIT_CHANNEL_ID                       (1)
      #define AUDIO_WAIT_CHANNEL_ID_TO_STOP               (2)
      #define AUDIO_WAIT_STOP                             (3)
      #define AUDIO_WAIT_STOP_OR_NOTIFICATION             (4)
      #define AUDIO_WAIT_STOP_CON                         (5)
      #define AUDIO_WAIT_DRIVER_STOP_CON                  (6)
      #define AUDIO_WAIT_STOP_PAUSE_OR_NOTIFICATION       (7)
      #define AUDIO_WAIT_STOP_OR_RESUME                   (8)
      #define AUDIO_WAIT_STOP_OR_RESUME_OR_RESTART        (9)

      /* Define all the state of channel handling in audio driver */
      #define AUDIO_DRIVER_CHANNEL_WAIT_INIT               (1)
      #define AUDIO_DRIVER_CHANNEL_WAIT_START              (2)
      #define AUDIO_DRIVER_CHANNEL_WAIT_START_CON          (3)
      #define AUDIO_DRIVER_CHANNEL_WAIT_STOP               (4)
      #define AUDIO_DRIVER_CHANNEL_WAIT_STOP_CON           (5)
      #define AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_STOP  (6)
      #define AUDIO_DRIVER_CHANNEL_WAIT_FREE               (7)
      #define AUDIO_DRIVER_CHANNEL_WAIT_START_CON_TO_PAUSE (8)

      // FFS manager
      /* Define all the state of FFS manager */
      #define AUDIO_MEM_IDLE                          (0)
      #define AUDIO_MEM_WAIT_NOTIFICATION_OR_STOP     (1)

      #define AUDIO_DRIVER_SWITCH                       (0)
      #define AUDIO_DRIVER_VM_AMR_RECORD_SESSION_SWITCH (1)
      #define AUDIO_DRIVER_VM_AMR_PLAY_SESSION_SWITCH   (2)
      #define AUDIO_DRIVER_MIDI_SESSION_SWITCH          (3)
      #define AUDIO_DRIVER_MP3_SESSION_SWITCH           (4)
      #define AUDIO_DRIVER_AAC_SESSION_SWITCH           (5)
      #define AUDIO_DRIVER_NONE                         (0xFF)

      #define AUDIO_DRIVER_MAX_BUFFER_PER_SESSION (10)
      #define AUDIO_DRIVER_MIN_BUFFER_PER_SESSION (2)
      #define AUDIO_DRIVER_MAX_CHANNEL            (1)

    #if (MELODY_E1) || (VOICE_MEMO) || (MELODY_E2) || (L1_PCM_EXTRACTION)//VENKAT
      /* id for the session of Flash to Ram download */
      #define AUDIO_FFS_FLASH_2_RAM_SESSION   (0)
      /* id for the session of Ram to Flashdownload */
      #define AUDIO_FFS_RAM_2_FLASH_SESSION   (1)
    #endif

    #if (SPEECH_RECO)
      /* Define the lengh of the speech recognition sample flash file name */
      #define AUDIO_SR_PATH_NAME_MAX_SIZE (AUDIO_PATH_NAME_MAX_SIZE + 4)

      /* define the threshold constant to switch between update and update-check */
      #define AUDIO_SR_UPDATE_UPDATECHECK_THRESHOLD      (1)

      /* Define all the state of the Audio speech reco enroll state machine */
      #define AUDIO_SR_ENROLL_IDLE                          (1)
      #define AUDIO_SR_ENROLL_WAIT_START_CON                (2)
      #define AUDIO_SR_ENROLL_WAIT_STOP_COMMAND             (3)
      #define AUDIO_SR_ENROLL_WAIT_START_CON_TO_STOP        (4)
      #define AUDIO_SR_ENROLL_WAIT_STOP_CON                 (5)

      /* Define all the state of the Audio speech reco update state machine */
      #define AUDIO_SR_UPDATE_IDLE                          (1)
      #define AUDIO_SR_UPDATE_WAIT_START_CON                (2)
      #define AUDIO_SR_UPDATE_WAIT_STOP_COMMAND             (3)
      #define AUDIO_SR_UPDATE_WAIT_START_CON_TO_STOP        (4)
      #define AUDIO_SR_UPDATE_WAIT_STOP_CON                 (5)

      /* Define all the state of the Audio speech reco state machine */
      #define AUDIO_SR_RECO_IDLE                          (1)
      #define AUDIO_SR_RECO_WAIT_START_CON                (2)
      #define AUDIO_SR_RECO_WAIT_STOP_COMMAND             (3)
      #define AUDIO_SR_RECO_WAIT_START_CON_TO_STOP        (4)
      #define AUDIO_SR_RECO_WAIT_STOP_CON                 (5)

    #endif

    /* Define all the state of the Audio mode full write state machine */
    #define AUDIO_MODE_FULL_WRITE_IDLE                  (1)
    #define AUDIO_MODE_FULL_WRITE_WAIT_MESSAGE_CONF     (2)

    /* Define all the state of the Audio mode load state machine */
    #define AUDIO_MODE_LOAD_IDLE                  (1)
    #define AUDIO_MODE_LOAD_WAIT_MESSAGE_CONF     (2)

    /* Audio mode number ID */
    #define AUDIO_FULL_ACCESS_WRITE     (0)
    #define AUDIO_MODE_SAVE             (1)
    #define AUDIO_MODE_LOAD             (2)
    #define AUDIO_MODE_NONE             (3)
    #define AUDIO_SPEAKER_VOLUME        (4)
    #define AUDIO_STEREO_SPEAKER_VOLUME (5)

    /* MP3 number ID */
    #define AUDIO_MP3                   (4)
    #define AUDIO_MP3_NONE              (0xFF)
    #define AUDIO_MP3_DMA_CHANNEL       (1)

    /* AAC */
    #define AUDIO_AAC_NONE              (0xFF)
    #define AUDIO_AAC                   (5)


    #ifdef __cplusplus
    }
    #endif
  #endif /* __AUDIO_CONST_I_H_ */
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */

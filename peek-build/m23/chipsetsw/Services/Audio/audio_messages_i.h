/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_messages_i.h                                          */
/*                                                                          */
/*  Purpose:  Internal messages used by AUDIO instance                      */
/*                                                                          */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------------------                                    */
/*  18/05/2001  Create                                                      */
/*                                                                          */
/*  Author      Francois Mazard - Stephanie Gerthoux                        */
/*                                                                          */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE
  #ifndef __AUDIO_MESSAGES_I_H_
    #define __AUDIO_MESSAGES_I_H_

    #include "l1_confg.h"

/*************to be removed ********/
#ifndef AS_RFS_API
#define AS_RFS_API 1
#endif
/*************to be removed ********/

#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#endif
	#include "ffs/ffs_api.h"



    #ifdef __cplusplus
    extern "C"
    {
    #endif


#if(AS_RFS_API == 1)
#define RFS_RDWREX_ALL  0x0777 // All permissions to user/group/others
#endif


    /******************************* KEY BEEP *****************************************/
    #if (KEYBEEP)
      /* Entity->Audio event */
      #define AUDIO_KEYBEEP_START_REQ                       (0x0001 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_AUDIO_KEYBEEP_PARAMETER keybeep_parameter;
        T_RV_RETURN               return_path;
      } T_AUDIO_KEYBEEP_START;

      /* Entity->Audio event */
      #define AUDIO_KEYBEEP_STOP_REQ                        (0x0002 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_RV_RETURN               return_path;
      } T_AUDIO_KEYBEEP_STOP;
    #endif /* #if (KEYBEEP) */

    /******************************* TONES *****************************************/
    #if (TONE)
      /* Entity->Audio event */
      #define AUDIO_TONES_START_REQ                       (0x0003 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_AUDIO_TONES_PARAMETER   tones_parameter;
        T_RV_RETURN               return_path;
      } T_AUDIO_TONES_START;

      /* Entity->Audio event */
      #define AUDIO_TONES_STOP_REQ                        (0x0004 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_RV_RETURN               return_path;
      } T_AUDIO_TONES_STOP;
    #endif

    /******************************* DRIVER ****************************************/
    #define AUDIO_DRIVER_START_SESSION                    (0x0005 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      UINT8                         channel_id;
      T_RV_RETURN                   return_path;
    } T_AUDIO_DRIVER_HANDLE_SESSION;

    #define AUDIO_DRIVER_STOP_SESSION                     (0x0006 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)

    #define AUDIO_DRIVER_FREE_SESSION                     (0x0007 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    #define AUDIO_DRIVER_PAUSE_SESSION                    (0x0008 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    #define AUDIO_DRIVER_RESUME_SESSION                   (0x0009 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    #define AUDIO_DRIVER_RESTART_SESSION                  (0x000A | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    #define AUDIO_DRIVER_INFO_SESSION                     (0x000B | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)

    /******************************* MELODY_E1 *****************************************/
    #if (MELODY_E1)
      /* Entity->Audio event */
      #define AUDIO_MELODY_E1_START_REQ                       (0x000C | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        T_AUDIO_MELODY_E1_PARAMETER   melody_E1_parameter;
#if(AS_RFS_API == 1)
		T_RFS_FD                    audio_ffs_fd;
#else
		T_FFS_FD                    audio_ffs_fd;
#endif
        T_RV_RETURN                   return_path;
      } T_AUDIO_MELODY_E1_START;

      /* Entity->Audio event */
      #define AUDIO_MELODY_E1_STOP_REQ                        (0x000D | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
#if(AS_RFS_API == 1) 
		T_WCHAR      melody_name[AUDIO_PATH_NAME_MAX_SIZE];       /* File name of the melody */ 
#else
        char         melody_name[AUDIO_PATH_NAME_MAX_SIZE];       /* File name of the melody */
#endif
        T_RV_RETURN               return_path;
      } T_AUDIO_MELODY_E1_STOP;

    #endif

    /******************************* MELODY_E2 *****************************************/
    #if (MELODY_E2)
      /* Entity->Audio event */
      #define AUDIO_MELODY_E2_START_REQ                       (0x000E | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        T_AUDIO_MELODY_E2_PARAMETER   melody_E2_parameter;
#if(AS_RFS_API == 1)
		T_RFS_FD                      audio_E2_ffs_fd;
#else
		T_FFS_FD                      audio_E2_ffs_fd;
#endif
        T_RV_RETURN                   return_path;
      } T_AUDIO_MELODY_E2_START;

      /* Entity->Audio event */
      #define AUDIO_MELODY_E2_STOP_REQ                        (0x000F | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
#if(AS_RFS_API == 1)
		T_WCHAR                   melody_E2_name[AUDIO_PATH_NAME_MAX_SIZE];
#else
		char                      melody_E2_name[AUDIO_PATH_NAME_MAX_SIZE];
#endif
                                  /* File name of the melody */
        T_RV_RETURN               return_path;
      } T_AUDIO_MELODY_E2_STOP;

    #endif


    /******************************* FFS interface ************************************/
    #if (MELODY_E1) || (MELODY_E2) || (VOICE_MEMO) || (L1_PCM_EXTRACTION)
      /* FFS FLASH to RAM interface */
      /* start message */
      #define AUDIO_FFS_FLASH_2_RAM_START_REQ                 (0x0010 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR  os_hdr;
        BOOLEAN   loop;
        UINT16    initial_size;
        UINT8     session_id;
#if(AS_RFS_API == 1)
		T_RFS_FD  audio_ffs_fd;
#else
		 T_FFS_FD  audio_ffs_fd;
#endif
      } T_AUDIO_FFS_FLASH_2_RAM_START;

      /* start confirmation message */
      #define AUDIO_FFS_INIT_DONE                             (0x0011 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)

      typedef struct
      {
        T_RV_HDR  os_hdr;
        UINT8  session_id;
        UINT8  channel_id;
      } T_AUDIO_FFS_INIT;

      /* stop message */
      #define AUDIO_FFS_STOP_REQ                              (0x0012 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR  os_hdr;
        UINT8     session_id;
      } T_AUDIO_FFS_STOP_REQ;

      /* stop confirmation message */
      #define AUDIO_FFS_STOP_CON                              (0x0013 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR  os_hdr;
        UINT8     session_id;
      } T_AUDIO_FFS_STOP_CON;

      /* FFS RAM to FLASH interface */
      /* start message */
      #define AUDIO_FFS_RAM_2_FLASH_START_REQ                 (0x0014 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR  os_hdr;
        UINT16    initial_size;
        UINT8     session_id;
#if(AS_RFS_API == 1)
		T_RFS_FD  audio_ffs_fd;
#else
		 T_FFS_FD  audio_ffs_fd;
#endif
      } T_AUDIO_FFS_RAM_2_FLASH_START;
    #endif

   /******************************* VOICE MEMO *****************************************/
    #if (VOICE_MEMO)

      /* ------------------------------- */
      /*      Voice Memo PLAY            */
      /* ------------------------------- */
      /* Entity->Audio event */
      #define AUDIO_VM_PLAY_START_REQ                       (0x0015 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
#if(AS_RFS_API == 1)
		T_RFS_FD                    audio_ffs_fd;
#else
		 T_FFS_FD                    audio_ffs_fd;
#endif
        T_RV_RETURN                   return_path;
      } T_AUDIO_VM_PLAY_START;

      /* Entity->Audio event */
      #define AUDIO_VM_PLAY_STOP_REQ                        (0x0016 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_RV_RETURN               return_path;
      } T_AUDIO_VM_PLAY_STOP;

      /* ------------------------------- */
      /*      Voice Memo RECORD          */
      /* ------------------------------- */
      /* Entity->Audio event */
      #define AUDIO_VM_RECORD_START_REQ                     (0x0017 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT32                        memo_duration;
        BOOL                          compression_mode;
        UINT16                        microphone_gain;
        UINT16                        network_gain;
#if(AS_RFS_API == 1)
		T_RFS_FD                    audio_ffs_fd;
#else
		 T_FFS_FD                    audio_ffs_fd;
#endif
        T_AUDIO_TONES_PARAMETER       tones_parameter;
        T_RV_RETURN                   return_path;
      } T_AUDIO_VM_RECORD_START;

      /* Entity->Audio event */
      #define AUDIO_VM_RECORD_STOP_REQ                      (0x0018 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_RV_RETURN               return_path;
      } T_AUDIO_VM_RECORD_STOP;
    #endif


    #if (L1_VOICE_MEMO_AMR)
      #define AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION       (0x0019 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        UINT32                    memo_duration;
        BOOL                      compression_mode;
        UINT16                    microphone_gain;
        UINT8                     amr_vocoder;
        T_AUDIO_DRIVER_PARAMETER  driver_parameter;
        T_RV_RETURN               return_path;
      } T_AUDIO_DRIVER_INIT_VM_AMR_RECORD_SESSION;

      #define AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION       (0x001A | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        T_AUDIO_DRIVER_PARAMETER      driver_parameter;
        T_RV_RETURN                   return_path;
      } T_AUDIO_DRIVER_INIT_VM_AMR_PLAY_SESSION;

      #define AUDIO_VM_AMR_RECORD_TO_FFS_START_REQ         (0x001B | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT32                        memo_duration;
        BOOL                          compression_mode;
        UINT16                        microphone_gain;
        UINT8                         amr_vocoder;
#if(AS_RFS_API == 1)
		T_RFS_FD                      audio_ffs_fd;
#else
		 T_FFS_FD                      audio_ffs_fd;
#endif
        T_RV_RETURN                   return_path;
      } T_AUDIO_VM_AMR_RECORD_TO_FFS_START;

      #define AUDIO_VM_AMR_RECORD_TO_RAM_START_REQ         (0x001C | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT32                        memo_duration;
        BOOL                          compression_mode;
        UINT16                        microphone_gain;
        UINT8                         amr_vocoder;
        UINT8*                        p_buffer;
        T_RV_RETURN                   return_path;
      } T_AUDIO_VM_AMR_RECORD_TO_RAM_START;

      #define AUDIO_VM_AMR_PLAY_FROM_FFS_START_REQ           (0x001D | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
#if(AS_RFS_API == 1)
		T_RFS_FD                      audio_ffs_fd;
#else
		 T_FFS_FD                      audio_ffs_fd;
#endif
        T_RV_RETURN                   return_path;
      } T_AUDIO_VM_AMR_PLAY_FROM_FFS_START;

      #define AUDIO_VM_AMR_PLAY_FROM_RAM_START_REQ           (0x001E | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT8                         *p_buffer;
		UINT16                        buffer_size;
        T_RV_RETURN                   return_path;
      } T_AUDIO_VM_AMR_PLAY_FROM_RAM_START;

      #define AUDIO_VM_AMR_RECORD_TO_FFS_STOP_REQ           (0x001F | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      #define AUDIO_VM_AMR_RECORD_TO_RAM_STOP_REQ           (0x0020 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
      } T_AUDIO_VM_AMR_RECORD_STOP;

      #define AUDIO_VM_AMR_PLAY_FROM_FFS_STOP_REQ             (0x0021 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      #define AUDIO_VM_AMR_PLAY_FROM_RAM_STOP_REQ             (0x0022 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
      } T_AUDIO_VM_AMR_PLAY_STOP;

      #define AUDIO_VM_AMR_PLAY_FROM_FFS_PAUSE_REQ             (0x004C | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      #define AUDIO_VM_AMR_PLAY_FROM_RAM_PAUSE_REQ             (0x004D | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        T_RV_RETURN                   return_path;
      } T_AUDIO_VM_AMR_PLAY_PAUSE;

      #define AUDIO_VM_AMR_PLAY_FROM_FFS_RESUME_REQ             (0x004E | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      #define AUDIO_VM_AMR_PLAY_FROM_RAM_RESUME_REQ             (0x004F | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        T_RV_RETURN                   return_path;
      } T_AUDIO_VM_AMR_PLAY_RESUME;


      #define AUDIO_MEM_START_REQ                       (0x0023 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT8                         channel_id;
        UINT8                         session_id;
      #if (AUDIO_NEW_FFS_MANAGER)
         T_FFS_FD                      audio_ffs_fd;
      #endif
      #if (AUDIO_RAM_MANAGER)
        UINT8                         *audio_ram_fd;
        UINT16                        audio_ram_size;
      #endif
        UINT16                        size;
      } T_AUDIO_MEM_START;

      #define AUDIO_MEM_STOP_REQ                         (0x0024 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT8                         channel_id;
        T_RV_RETURN                   return_path;
      } T_AUDIO_MEM_STOP;

      #define AUDIO_MEM_STATUS_MSG                       (0x0025 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        INT8                          status;
        UINT8                         status_type;
        UINT8                         channel_id;
        UINT32                        recorded_size;
      } T_AUDIO_MEM_STATUS;

    #endif

   /******************************* SPEECH RECO *****************************************/
    #if (SPEECH_RECO)
      /* ------------------------------- */
      /*      Speech Reco enrollment     */
      /* ------------------------------- */
      /* Entity->Audio event */
      #define AUDIO_SR_ENROLL_START_REQ                     (0x0026 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        
#if(AS_RFS_API == 1)
		T_RFS_FD                    sr_ffs_fd;
		T_RFS_FD                    voice_ffs_fd;
#else
		T_FFS_FD                    sr_ffs_fd;
		T_FFS_FD                    voice_ffs_fd;
#endif
        T_RV_RETURN                   return_path;
      } T_AUDIO_SR_ENROLL_START;

      /* Entity->Audio event */
      #define AUDIO_SR_ENROLL_STOP_REQ                      (0x0027 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        T_RV_RETURN                   return_path;
      } T_AUDIO_SR_ENROLL_STOP;

      /* ------------------------------- */
      /*      Speech Reco update         */
      /* ------------------------------- */
      /* Entity->Audio event */
      #define AUDIO_SR_UPDATE_START_REQ                     (0x0028 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT8                         vocabulary_size;
        UINT8                         model_index;
        void                          *p_database;
        BOOL                          record_speech;
        T_RV_RETURN                   return_path;
      } T_AUDIO_SR_UPDATE_START;

      /* Entity->Audio event */
      #define AUDIO_SR_UPDATE_STOP_REQ                      (0x0029 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        T_RV_RETURN                   return_path;
      } T_AUDIO_SR_UPDATE_STOP;

      /* ------------------------------- */
      /*      Speech Reco reco           */
      /* ------------------------------- */
      /* Entity->Audio event */
      #define AUDIO_SR_RECO_START_REQ                       (0x002A | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT8                         vocabulary_size;
        void                          *p_database;
        T_RV_RETURN                   return_path;
      } T_AUDIO_SR_RECO_START;

      /* Entity->Audio event */
      #define AUDIO_SR_RECO_STOP_REQ                        (0x002B | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        T_RV_RETURN                   return_path;
      } T_AUDIO_SR_RECO_STOP;
    #endif /* SPEECH_RECO */

    /******************************* AUDIO_MODE *****************************************/
    /* ------------------------------- */
    /*      Full access write          */
    /* ------------------------------- */
    /* Entity->Audio event */
    #define AUDIO_MODE_WRITE_REQ                       (0x002C | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_AUDIO_FULL_ACCESS_WRITE     audio_parameter;
      T_RV_RETURN                   return_path;
    } T_AUDIO_MODE_FULL_ACCESS_WRITE_REQ;

    /* ------------------------------- */
    /*      save audio mode            */
    /* ------------------------------- */
    /* Entity->Audio event */
    #define AUDIO_MODE_SAVE_REQ                         (0x002D | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
#if(AS_RFS_API == 1)
	  T_RFS_FD                      audio_ffs_fd;
      T_RFS_FD                      audio_volume_ffs_fd;
#else
	  T_FFS_FD                      audio_ffs_fd;
      T_FFS_FD                      audio_volume_ffs_fd;
#endif
    #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
#if(AS_RFS_API == 1)
	T_RFS_FD                      audio_stereo_volume_ffs_fd;
#else
	T_FFS_FD                      audio_stereo_volume_ffs_fd;
#endif
    #endif
      T_RV_RETURN                   return_path;
    } T_AUDIO_MODE_SAVE_REQ;

    /* ------------------------------- */
    /*      load audio mode            */
    /* ------------------------------- */
    /* Entity->Audio event */
    #define AUDIO_MODE_LOAD_REQ                         (0x002E | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
#if(AS_RFS_API == 1)
		T_RFS_FD                      audio_ffs_fd;
      T_RFS_FD                      audio_volume_ffs_fd;
#else
      T_FFS_FD                      audio_ffs_fd;
      T_FFS_FD                      audio_volume_ffs_fd;
#endif
    #if ((ANLG_FAM == 3) || (ANLG_FAM == 4) || (ANLG_FAM == 11))
#if(AS_RFS_API == 1)
	T_RFS_FD                      audio_stereo_volume_ffs_fd;
#else
	T_FFS_FD                      audio_stereo_volume_ffs_fd;
#endif
    #endif
#if(AS_RFS_API == 1)
	T_WCHAR                          audio_mode_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#else
	char                          audio_mode_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#endif
      T_RV_RETURN                   return_path;
    } T_AUDIO_MODE_LOAD_REQ;

    /* ------------------------------- */
    /*      audio speaker volume       */
    /* ------------------------------- */
    /* Entity->Audio event */
    #define AUDIO_SPEAKER_VOLUME_REQ                    (0x002F | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_AUDIO_SPEAKER_VOLUME        volume;
      T_RV_RETURN                   return_path;
    } T_AUDIO_SPEAKER_VOLUME_REQ;

    /* -------------------------------- */
    /*      audio stereo speaker volume */
    /* -------------------------------- */
    /* Entity->Audio event */
    #define AUDIO_STEREO_SPEAKER_VOLUME_REQ             (0x0030 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_AUDIO_STEREO_SPEAKER_VOLUME volume;
      T_RV_RETURN                   return_path;
    } T_AUDIO_STEREO_SPEAKER_VOLUME_REQ;


    /* -------------------------------------------------------- */
    /* WARNING: TTY Messages ID are defined as Audio messages.  */
    /* To avoid any conflict have a look in tty_i.h & tty_api.h */
    /* -------------------------------------------------------- */


   /******************************* MIDI *****************************************/
   /* ------------------- */
   /*      MIDI           */
   /* ------------------- */
   /* Entity->Audio event */
   #define AUDIO_DRIVER_INIT_MIDI_SESSION            (0x0031 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
   typedef struct
   {
     T_RV_HDR                      os_hdr;
     T_AUDIO_DRIVER_PARAMETER      driver_parameter;
     T_RV_RETURN                   return_path;
   } T_AUDIO_DRIVER_INIT_MIDI_SESSION;

   #define AUDIO_MIDI_START_REQ                      (0x0032 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)

   typedef struct
   {
     T_RV_HDR                      os_hdr;
     UINT16                        sampling_rate;
     UINT16                        data_type;
     T_AUDIO_EXT_MIDI_TYPE         bit_type;
     UINT8                         channel;
     UINT16                        frame_number;
     T_AUDIO_DRIVER_PARAMETER      driver_parameter;
     T_RV_RETURN                   return_path;
   } T_AUDIO_MIDI_START;

   #define AUDIO_MIDI_STOP_REQ                       (0x0033 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
   typedef struct
   {
     T_RV_HDR                      os_hdr;
     T_RV_RETURN                   return_path;
   } T_AUDIO_MIDI_STOP;

   /******************************* MP3 *****************************************/
   /* ------------------- */
   /*      MP3            */
   /* ------------------- */
   /* Entity->Audio event */
   #if (L1_MP3 == 1)
    #define AUDIO_DRIVER_INIT_MP3_SESSION            (0x0034 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_AUDIO_DRIVER_PARAMETER      driver_parameter;
      T_RV_RETURN                   return_path;
    } T_AUDIO_DRIVER_INIT_MP3_SESSION;

    #define AUDIO_MP3_START_REQ                      (0x0035 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)

    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_AUDIO_MP3_PARAMETER         mp3_parameter;
#if(AS_RFS_API == 1)
		T_RFS_FD                      audio_ffs_fd;
#else
	   T_FFS_FD                      audio_ffs_fd;
#endif
      T_FFS_SIZE       audio_filesize;
      UINT8  play_bar_on; /* set 0 to deconfigure the playbar */
      T_AUDIO_DRIVER_PARAMETER      driver_parameter;
      T_RV_RETURN                   return_path;
    } T_AUDIO_MP3_START;

    #define AUDIO_MP3_STOP_REQ                       (0x0036 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_RV_RETURN                   return_path;
    } T_AUDIO_MP3_STOP;

    #define AUDIO_MP3_PAUSE_REQ                      (0x0037 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_RV_RETURN                   return_path;
    } T_AUDIO_MP3_PAUSE;

    #define AUDIO_MP3_RESUME_REQ                     (0x0038 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_RV_RETURN                   return_path;
    } T_AUDIO_MP3_RESUME;

    #define AUDIO_MP3_INFO_REQ                       (0x0039 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_RV_RETURN                   return_path;
    } T_AUDIO_MP3_INFO;

  #endif

   /******************************* AAC *****************************************/
   /* ------------------- */
   /*      AAC            */
   /* ------------------- */

   /* Entity->Audio event */
   #if (L1_AAC == 1)
    #define AUDIO_DRIVER_INIT_AAC_SESSION            (0x003A | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_AUDIO_DRIVER_PARAMETER      driver_parameter;
      T_RV_RETURN                   return_path;
    } T_AUDIO_DRIVER_INIT_AAC_SESSION;

    #define AUDIO_AAC_START_REQ                      (0x003B | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)

    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_AUDIO_AAC_PARAMETER         aac_parameter;
#if(AS_RFS_API == 1)
		T_RFS_FD                      audio_ffs_fd;
#else
	   T_FFS_FD                      audio_ffs_fd;
#endif
       T_FFS_SIZE       audio_filesize; /* Total filesize to be played */
       UINT8  play_bar_on; /* set 0 to deconfigure the playbar */
	 T_AUDIO_DRIVER_PARAMETER      driver_parameter;
      T_RV_RETURN                   return_path;
    } T_AUDIO_AAC_START;

    #define AUDIO_AAC_STOP_REQ                       (0x003C | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_RV_RETURN                   return_path;
    } T_AUDIO_AAC_STOP;

    #define AUDIO_AAC_PAUSE_REQ                      (0x003D | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_RV_RETURN                   return_path;
    } T_AUDIO_AAC_PAUSE;

    #define AUDIO_AAC_RESUME_REQ                     (0x003E | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_RV_RETURN                   return_path;
    } T_AUDIO_AAC_RESUME;

    #define AUDIO_AAC_INFO_REQ                       (0x003F | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_RV_RETURN                   return_path;
    } T_AUDIO_AAC_INFO;

    #define AUDIO_DRIVER_AAC_INFO_SESSION             (0x0040 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)

    #define AUDIO_AAC_RESTART_REQ                     (0x0041 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
      T_AUDIO_AAC_RESTART_PARAMETER aac_restart_parameter;
      T_RV_RETURN                   return_path;
    } T_AUDIO_AAC_RESTART;
    #define AUDIO_DRIVER_INFO_AAC_SESSION             (0x0042 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)


  #endif

/*VENKAT*/
	/******************************* VOICE MEMO PCM*****************************************/
  #if(L1_PCM_EXTRACTION)//VENKAT

      /* ------------------------------- */
      /*      Voice Memo PLAY            */
      /* ------------------------------- */
      /* Entity->Audio event */
      #define AUDIO_VM_PCM_PLAY_START_REQ                       (0x0043 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR          os_hdr;
  	 UINT32               memo_duration;    /* maximum duration of the voice memo */
  	 UINT8                 download_ul_gain;
        UINT8                 download_dl_gain;
#if(AS_RFS_API == 1)
		T_RFS_FD            audio_ffs_fd;
#else
		 T_FFS_FD            audio_ffs_fd;
#endif
        T_RV_RETURN     return_path;
      } T_AUDIO_VM_PCM_PLAY_START;

      /* Entity->Audio event */
      #define AUDIO_VM_PCM_PLAY_STOP_REQ                        (0x0044 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_RV_RETURN               return_path;
      } T_AUDIO_VM_PCM_PLAY_STOP;

      /* ------------------------------- */
      /*      Voice Memo RECORD          */
      /* ------------------------------- */
      /* Entity->Audio event */
      #define AUDIO_VM_PCM_RECORD_START_REQ                     (0x0045 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT32                        memo_duration;
        UINT8                        upload_ul_gain;
        UINT8                        upload_dl_gain;
#if(AS_RFS_API == 1)
		T_RFS_FD                    audio_ffs_fd;
#else
		 T_FFS_FD                    audio_ffs_fd;
#endif
        T_RV_RETURN                   return_path;
      } T_AUDIO_VM_PCM_RECORD_START;

      /* Entity->Audio event */
      #define AUDIO_VM_PCM_RECORD_STOP_REQ                      (0x0046 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_RV_RETURN               return_path;
      } T_AUDIO_VM_PCM_RECORD_STOP;
	/*!VENKAT*/


/*VENKAT*/
	/******************************* VOICE BUFFERING PCM*****************************************/
      /* ------------------------------- */
      /*      Voice Buffering PLAY            */
      /* ------------------------------- */
      /* Entity->Audio event */
      #define AUDIO_VBUF_PCM_PLAY_START_REQ                       (0x0047 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR          os_hdr;
  	 UINT32               memo_duration;    /* maximum duration of the voice memo */
  	 UINT8                 download_ul_gain;
        UINT8                 download_dl_gain;
#if(AS_RFS_API == 1)
		T_RFS_FD            audio_ffs_fd;
#else
        T_FFS_FD            audio_ffs_fd;
#endif
        T_RV_RETURN     return_path;
      } T_AUDIO_VBUF_PCM_PLAY_START;

      /* Entity->Audio event */
      #define AUDIO_VBUF_PCM_PLAY_STOP_REQ                        (0x0048 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_RV_RETURN               return_path;
      } T_AUDIO_VBUF_PCM_PLAY_STOP;

      /* ------------------------------- */
      /*      Voice Buffering RECORD          */
      /* ------------------------------- */
      /* Entity->Audio event */
      #define AUDIO_VBUF_PCM_RECORD_START_REQ                     (0x0049 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        UINT32                        memo_duration;
        UINT8                        upload_ul_gain;
        UINT8                        upload_dl_gain;
#if(AS_RFS_API == 1)
		T_RFS_FD                    audio_ffs_fd;
#else
        T_FFS_FD                    audio_ffs_fd;
#endif
        T_RV_RETURN                   return_path;
#if(AS_RFS_API == 1)
	T_WCHAR  memo_name[AUDIO_PATH_NAME_MAX_SIZE];
#else
	 char memo_name[AUDIO_PATH_NAME_MAX_SIZE];
#endif
      } T_AUDIO_VBUF_PCM_RECORD_START;

      /* Entity->Audio event */
      #define AUDIO_VBUF_PCM_RECORD_STOP_REQ                      (0x004A | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
        T_RV_RETURN               return_path;
      } T_AUDIO_VBUF_PCM_RECORD_STOP;

#define AUDIO_VBUF_PCM_PLAY_STOP_CON                      (0x004B | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
      typedef struct
      {
        T_RV_HDR                  os_hdr;
      } T_AUDIO_VBUF_PCM_PLAY_STOP_CON;

    #endif //#if(L1_PCM_EXTRACTION)
	/*!VENKAT*/

#if(L1_BT_AUDIO)
#define AUDIO_BT_CFG_REQ                       (0x005E | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)
    typedef struct
    {
      T_RV_HDR                      os_hdr;
     T_AUDIO_BT_PARAMETER bt_parameter;
      T_RV_RETURN                   return_path;
    } T_AUDIO_BT_CFG_REQ;

#endif
  #ifdef __cplusplus
  }
  #endif

  #endif /* __AUDIO_MESSAGES_I_ */

#endif /* #ifdef RVM_AUDIO_MAIN_SWE */

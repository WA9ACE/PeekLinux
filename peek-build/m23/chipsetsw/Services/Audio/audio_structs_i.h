/********************************************************************************/
/*                                                                              */
/*    File Name:   audio_structs_i.h                                            */
/*                                                                              */
/*    Purpose:     This file contains constants, data type, and data            */
/*                 structures that are used by the AUDIO's task.                */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/*    Revision History:                                                         */
/*      11 May 01     Francois Mazard - Stephanie Gerthoux        Create        */
/*                                                                              */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved.   */
/*                                                                              */
/********************************************************************************/
#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE

  #ifndef  _AUDIO_STRUCTS_I_
    #define  _AUDIO_STRUCTS_I_

/*************to be removed ********/
#ifndef AS_RFS_API
#define AS_RFS_API 1
#endif
/*************to be removed ********/

	#ifndef _WINDOWS
      #include "sys.cfg"
      #include "l1sw.cfg"
      #include "chipset.cfg"
    #endif
    #include "l1_confg.h"
    #include "rvm/rvm_gen.h"
    #include "Audio/audio_features_i.h"
    #include "Audio/audio_ffs_i.h"

    #include "Audio/audio_const_i.h"
#if (AS_RFS_API == 1)
  #include "rfs/rfs_api.h"
#else
    #include "ffs/ffs_api.h"
#endif

    #ifdef _WINDOWS
      #define L1_MP3  1
      #define L1_AAC  1
    #endif

    #ifdef __cplusplus
      extern "C"
        {
    #endif

    /****************************** AUDIO'S ENVIRONMENT *****************************/
    /* Define a structure used to store all information related to the AUDIO's task */
    /* & memory bank identifiers.                                                   */

    #if (KEYBEEP)
      typedef struct
      {
        UINT8         state;              /* keybeep state */
        T_RVF_ADDR_ID task_id;            /* keybeep task_id */
        T_RV_RETURN   return_path;        /* return path of the function that previously start the keybeep */
      } T_AUDIO_KEYBEEP_VAR;
    #endif /* #if (KEYBEEP) */

    #if (TONE)
      typedef struct
      {
        UINT8         state;              /* tones state */
        T_RVF_ADDR_ID task_id;            /* tones task_id */
        T_RV_RETURN   return_path;        /* return path of the function that previously start the tones */
      } T_AUDIO_TONES_VAR;
    #endif

    #if (MELODY_E1)
      typedef struct
      {
        UINT8           state;             /* melody state */
        T_RVF_ADDR_ID   task_id;           /* melody task_id */
#if(AS_RFS_API == 1)
		T_WCHAR         melody_name[AUDIO_PATH_NAME_MAX_SIZE];  /* name and path of the word to enroll */
#else
		char            melody_name[AUDIO_PATH_NAME_MAX_SIZE];  /* name and path of the word to enroll */
#endif
        BOOLEAN         loopback;
        T_RV_RETURN     return_path;       /* return path of the function that previously start the melody */
      } T_AUDIO_MELODY_E1_VAR;
    #endif

    #if (MELODY_E2)
      typedef struct
      {
        UINT8           state;             /* melody state */
        T_RVF_ADDR_ID   task_id;           /* melody task_id */
#if(AS_RFS_API == 1)
		T_WCHAR           melody_name[AUDIO_PATH_NAME_MAX_SIZE];  /* name of the melody E2 */
#else
		char            melody_name[AUDIO_PATH_NAME_MAX_SIZE];  /* name of the melody E2 */
#endif
        BOOLEAN         loopback;
        T_RV_RETURN     return_path;       /* return path of the function that previously start the melody */
      } T_AUDIO_MELODY_E2_VAR;

      typedef struct
      {
        BOOLEAN       file_downloaded;
        UINT8         nb_of_instruments;
        char          instrument_file_name[AUDIO_PATH_NAME_MAX_SIZE];
      } T_AUDIO_MELODY_E2_LOAD_FILE_VAR;

      typedef struct
      {
        INT8            id;             /* id of the melody in the .lsi file*/
#if(AS_RFS_API == 1)
		T_WCHAR            melody_name[AUDIO_PATH_NAME_MAX_SIZE];  /* name of the melody E2 */
#else
		char            melody_name[AUDIO_PATH_NAME_MAX_SIZE];  /* name of the melody E2 */
#endif

      } T_AUDIO_MELODY_E2_ID_NAME;


    #endif


    #if (VOICE_MEMO)
      typedef struct
      {
        UINT8         state;                /* voice memo state */
        T_RVF_ADDR_ID task_id;              /* voice memo task_id */
        UINT32        recorded_size;        /* voice memo recorded size */
        T_RV_RETURN   return_path;          /* return path of the function that previously start the voice memo  */
        DummyStruct   *p_l1_send_message;   /* address to the message to send to the L1 */
      } T_AUDIO_VOICE_MEMO_RECORD_VAR;

      typedef struct
      {
        UINT8         state;                /* voice memo state */
        T_RVF_ADDR_ID task_id;              /* voice memo task_id */
        T_RV_RETURN   return_path;          /* return path of the function that previously start the voice memo  */
      } T_AUDIO_VOICE_MEMO_PLAY_VAR;

    #endif

  #if (L1_PCM_EXTRACTION) //VENKAT


      typedef struct
      {
        UINT8         state;                /* voice memo state */
        T_RVF_ADDR_ID task_id;              /* voice memo task_id */
        UINT32        recorded_size;        /* voice memo recorded size */
        T_RV_RETURN   return_path;          /* return path of the function that previously start the voice memo  */
        DummyStruct *p_l1_send_message;   /* address to the message to send to the L1 */
	 UINT32 size_copied_to_flash;
      } T_AUDIO_VOICE_MEMO_PCM_RECORD_VAR;

      typedef struct
      {
        UINT8         state;                /* voice memo state */
        T_RVF_ADDR_ID task_id;              /* voice memo task_id */
        T_RV_RETURN   return_path;          /* return path of the function that previously start the voice memo  */
 	DummyStruct *p_l1_send_message; /* address to the message to send to the L1 */
	 UINT32 size_read_from_flash;
      } T_AUDIO_VOICE_MEMO_PCM_PLAY_VAR;

	  typedef struct
      {
        UINT8         state;                /* voice memo state */
        T_RVF_ADDR_ID task_id;              /* voice memo task_id */
        UINT32        recorded_size;        /* voice memo recorded size */
        T_RV_RETURN   return_path;          /* return path of the function that previously start the voice memo  */
        DummyStruct *p_l1_send_message;   /* address to the message to send to the L1 */
#if(AS_RFS_API == 1)
	T_WCHAR    memo_name[AUDIO_PATH_NAME_MAX_SIZE]; /* File name of the Voice Memo play */
#else
	 char    memo_name[AUDIO_PATH_NAME_MAX_SIZE]; /* File name of the Voice Memo play */
#endif
#if(AS_RFS_API == 1)
	T_RFS_FD vbuf_ffs_fd;
#else
	 T_FFS_FD vbuf_ffs_fd;
#endif	 

	 UINT32 ffs_write;

	 
	 UINT32 delete_after_play;
	 UINT32 size_copied_to_flash;
      } T_AUDIO_VBUF_PCM_RECORD_VAR;

      typedef struct
      {
        UINT8         state;                /* voice memo state */
        T_RVF_ADDR_ID task_id;              /* voice memo task_id */
        T_RV_RETURN   return_path;          /* return path of the function that previously start the voice memo  */

	 UINT32 ffs_read;

 	DummyStruct *p_l1_send_message; /* address to the message to send to the L1 */
	 UINT32 size_read_from_flash;
      } T_AUDIO_VBUF_PCM_PLAY_VAR;

    #endif //L1_PCM_EXTRACTION //VENKAT

    #if (L1_VOICE_MEMO_AMR)
      typedef struct
      {
        UINT8         state;                /* voice memo state */
        T_RV_RETURN   return_path;          /* return path of the function that previously start the voice memo  */
        UINT8         channel_id;
      #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
	  T_RFS_FD      ffs_fd;
#else
	  T_FFS_FD      ffs_fd;
#endif
      #endif
      #if (AUDIO_RAM_MANAGER)
        UINT8         *audio_ram_fd;
      #endif
        UINT8         session_id;
        UINT16        size;
        UINT8         size_left; // sample split between 2 RAM buffers
        UINT8         previous_type; // last sample type processed
        UINT32        recorded_size; /* voice memo recorded size */
        BOOL          stop_req_allowed;// TRUE first then FALSE
      #ifdef _WINDOWS
        UINT8 l1_state;
      #endif
      } T_AUDIO_VOICE_MEMO_AMR_RECORD_TO_MEM_VAR;

      typedef struct
      {
        UINT8         state;                /* voice memo state */
        T_RV_RETURN   return_path;          /* return path of the function that previously start the voice memo  */
        UINT8         channel_id;
      #if (AUDIO_NEW_FFS_MANAGER)
#if(AS_RFS_API == 1)
		T_RFS_FD      ffs_fd;
#else
	  T_FFS_FD      ffs_fd;
#endif
      #endif
      #if (AUDIO_RAM_MANAGER)
        UINT8         *audio_ram_fd;
        UINT16        audio_ram_size;
      #endif
        UINT8         session_id;
        UINT16        size;
        UINT8         size_left; // sample split between 2 RAM buffers
        UINT8         previous_type; // last sample type processed
        UINT32        recorded_size; /* voice memo recorded size */
        BOOL          stop_req_allowed;// TRUE first then FALSE
        BOOL          last_stop_msg;    /* FALSE first then TRUE */
        BOOL          last_pause_msg;    /* FALSE first then TRUE */
        BOOL          last_resume_msg;    /* FALSE first then TRUE */
        BOOL          pause_req_allowed;/* TRUE first then FALSE */
        BOOL          start_req;    
      #ifdef _WINDOWS
        UINT8 l1_state;
      #endif
      } T_AUDIO_VOICE_MEMO_AMR_PLAY_FROM_MEM_VAR;
    #endif

    #if (MELODY_E1) || (MELODY_E2) || (VOICE_MEMO) || (L1_MP3) || (L1_AAC) || (L1_PCM_EXTRACTION)



      typedef struct
      {
        UINT8  *p_start_pointer;
        UINT8  *p_stop_pointer;
        UINT16  size;
      } T_AUDIO_FFS_BUFFER_INFO;




      typedef struct
      {
        /* validity of the session: */
        /* 0: this session isn't started */
        /* 1: this session is running */
        BOOLEAN   valid_channel;

#if(AS_RFS_API == 1)
		/* file descriptor */
        T_RFS_FD  audio_ffs_fd;

#else
        /* file descriptor */
        T_FFS_FD  audio_ffs_fd;
#endif

        /*initial size requested before to start the L1 */
        UINT16    size;
        /* loop mode is used or not during this session */
        BOOLEAN   loop_mode;
        /* mode of the session: */
        /* 0: FLASH to RAM download */
        /* 1: RAM to FLASH download */
        BOOLEAN   session_mode;
        UINT8     session_id;
      } T_AUDIO_FFS_SESSION_REQ;

      typedef struct
      {
        /* index of the next buffer will be used by the L1 */
        UINT8                   index_l1;
        /* index of the current buffer used by the FFS */
        UINT8                   index_ffs;
        T_AUDIO_FFS_BUFFER_INFO buffer[AUDIO_MAX_FFS_BUFFER_PER_SESSION];
        /* state of the cust_get_pointer function */
        UINT8                  cust_get_pointer_state;
      } T_AUDIO_FFS_SESSION_INFO;

      typedef struct
      {
        /* Request of the current session */
        T_AUDIO_FFS_SESSION_REQ   session_req;
        /* information of the current session */
        T_AUDIO_FFS_SESSION_INFO  session_info;
      } T_AUDIO_FFS_SESSION;

    #endif

    #if (MELODY_E1) || (MELODY_E2) || (VOICE_MEMO) || (L1_MP3) || (L1_AAC) || (L1_PCM_EXTRACTION)

      typedef struct
      {
        UINT16  *p_start_pointer;
        UINT16  size;
      } T_AUDIO_DRIVER_BUFFER_INFO;

      typedef struct
      {
        UINT16    size; // initial size requested before to start the L1
        UINT8     nb_buffer; // nb buffer
        UINT8     session_id;
        DummyStruct *p_l1_send_message;   /* address to the message to send to the L1 */
        T_RV_RETURN return_path;
      } T_AUDIO_DRIVER_SESSION_REQ;

      typedef struct
      {
        UINT8                   index_l1; // index of the next buffer will be used by the L1
        UINT8                   index_appli; // index of the current buffer used by the FFS
        UINT8                   index_last_buffer; // index of the last buffer used by the FFS
        UINT16                  buffer_size; // size of the buffer sent to the Layer1

        T_AUDIO_DRIVER_BUFFER_INFO *buffer; // pointers on temporary RAM buffers

        UINT8                   state;
        UINT8                   play_api_state;// only play
        UINT8                   stop_request;// 0 first then 1 when stop request
        UINT8                   pause_request;// 0 first then 1 when pause request
        UINT8                   resume_request;// 0 first then 1 when resume request
        UINT8                   restart_request;// 0 first then 1 when restart request
        UINT8                   info_request;// 0 first then 1 when info request
      } T_AUDIO_DRIVER_SESSION_INFO;

      typedef struct
      {
        /* Request of the current session */
        T_AUDIO_DRIVER_SESSION_REQ   session_req;
        /* information of the current session */
        T_AUDIO_DRIVER_SESSION_INFO  session_info;
      } T_AUDIO_DRIVER_SESSION;
    #endif

    #if (SPEECH_RECO)
      typedef struct
      {
        /* enroll speech reco state */
        UINT8       state;
        /* voice sample file descriptor */
        
        /* speech reco sample file descriptor */
#if(AS_RFS_API == 1)
		T_RFS_FD  sr_ffs_fd;
		T_RFS_FD  voice_ffs_fd;
#else
		T_FFS_FD  sr_ffs_fd;
		T_FFS_FD  voice_ffs_fd;
#endif
        /* task_id */
        UINT16       task_id;
        /* Speech buffer address */
        UINT16       *p_speech_address;
        /* return path of the function that previously start the sppeech reco */
        T_RV_RETURN return_path;
      } T_AUDIO_SR_ENROLL_VAR;

      typedef struct
      {
        /* update speech reco state */
        UINT8       state;
        /* task_id */
        UINT16       task_id;
        /* Speech buffer address */
        UINT16       *p_speech_address;
        /* Temporary model buffer address */
        UINT16       *p_model_address;
        /* vocabulary size */
        UINT8        vocabulary_size;
        /* model index to update */
        UINT8        model_index;
        /* flag to mention if the speech must be recorded */
        BOOL         record_speech;
        /* return path of the function that previously start the sppeech reco */
        T_RV_RETURN return_path;
        /* pointer to the vocabulary database */
        void  *p_database;
      } T_AUDIO_SR_UPDATE_VAR;

      typedef struct
      {
        /* update speech reco state */
        UINT8       state;
        /* task_id */
        UINT16       task_id;
        /* vocabulary size */
        UINT8        vocabulary_size;
        /* return path of the function that previously start the sppeech reco */
        T_RV_RETURN return_path;
        /* pointer to the vocabulary database */
        void  *p_database;
      } T_AUDIO_SR_RECO_VAR;

      typedef struct
      {
        /* speech reco enroll variables */
        T_AUDIO_SR_ENROLL_VAR sr_enroll;
        T_AUDIO_SR_UPDATE_VAR sr_update;
        T_AUDIO_SR_RECO_VAR sr_reco;
      } T_AUDIO_SPEECH_RECO_VAR;
    #endif

    #if (L1_EXT_AUDIO_MGT==1)
      typedef struct
      {
        UINT8         state;                /* midi state */
        T_RV_RETURN   return_path;          /* return path of the function that previously started midi */
        UINT8         channel_id;
        UINT16        size;
#if(AS_RFS_API == 1)
		T_RFS_FD      ffs_fd;
#else
        T_FFS_FD      ffs_fd;
#endif
        BOOL          stop_req_allowed;
        UINT16        sampling_rate;
        UINT8         nb_buffer;
        UINT16        data_type;
        UINT8         bit_type;
        UINT8         channel ;
        UINT16        frame_number;
        UINT16*       midi_play_array[2];
        #ifdef _WINDOWS
          UINT8 l1_state;
          INT16 counter;
        #endif
		    UINT16		    get_play_counter;    /* counter incremented at each "audio_driver_get_play_buffer" function call*/
		    UINT16		    layer1_counter;	     /* counter incremented at each "AUDIO_DRIVER_NOTIFICATION_MSG" reception */
      } T_AUDIO_MIDI_VAR;
    #endif


    /* audio configuration variable */
    typedef struct
    {
      /* full access write state */
      UINT8   state;
      /* Number of message to receive */
      UINT8 number_of_message;
      /* address to the data buffer */
      void   *buffer;
      /* flag to know if the data buffer need to be deallocate or not */
      BOOLEAN             deallocate_buffer;
      /* return path of the function that previously start the full access write */
      T_RV_RETURN return_path;
    }
    T_AUDIO_FULL_ACCESS_WRITE_VAR;

    typedef struct
    {
      /* state of the audio mode load state machine */
      UINT8 state;
      /* Number of message to receive */
      UINT8 number_of_message;
      /* pointer to the RAM buffer */
      T_AUDIO_MODE *p_audio_mode;
      /* return path */
      T_RV_RETURN return_path;
    }
    T_AUDIO_MODE_LOAD_VAR;

    typedef struct
    {
      T_AUDIO_FULL_ACCESS_WRITE_VAR full_access_write_var;
      T_AUDIO_MODE_LOAD_VAR         audio_mode_load_var;
#if(AS_RFS_API == 1)
	  T_WCHAR  audio_mode_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#else
	  char  audio_mode_path_name[AUDIO_PATH_NAME_MAX_SIZE];
#endif
    } T_AUDIO_MODE_VAR;

    #if (L1_MP3 == 1)
      typedef struct
        {
          UINT8         state;        /* mp3 state */
          T_RV_RETURN   return_path;  /* return path of the function that previously start the mp3  */
          UINT8         channel_id;
#if(AS_RFS_API == 1)
		  T_RFS_FD      ffs_fd;
#else
          T_FFS_FD      ffs_fd;
#endif
     //     T_FFS_SIZE       filesize;
          UINT8         session_id;
          UINT16        size;
          UINT8         size_left;    /* sample split between 2 RAM buffers */
          UINT8         previous_type;/* last sample type processed */
          BOOL          stop_req_allowed;/* TRUE first then FALSE */
          BOOL          pause_req_allowed;/* TRUE first then FALSE */
          BOOL          last_stop_msg;    /* FALSE first then TRUE */
          BOOL          last_pause_msg;    /* FALSE first then TRUE */
          BOOL          last_resume_msg;    /* FALSE first then TRUE */
          BOOL          last_info_msg;    /* FALSE first then TRUE */
          UINT8         index_last_buffer;
          UINT32        size_file_start;
          UINT32        size_played;
          BOOL          buff_notify_msg;
#if(AS_RFS_API == 1)  
          T_RFS_SIZE filesize;
#else
         T_FFS_SIZE       filesize;
#endif
          BOOL   play_bar_on;   /* set to zero to switch off the playbar */
          BOOL   mp3_mono_stereo_mmi_command;  /* 0-mono 1-stereo */
        #ifdef _WINDOWS
          UINT8 l1_state;
          INT16 counter;
        #endif
   BOOL          start_req; /* TRUE first , after start confirmation make it FALSE */
        } T_AUDIO_MP3_VAR;


   /* For tests on PC */
   #ifdef _WINDOWS
      typedef struct
      {
        T_RV_HDR  header;
        UINT16    frequency;
        UINT16    bitrate;
        UINT8     mpeg_id;
        UINT8     layer;
        BOOL      padding;
        UINT8     private;
        UINT8     channel;
        BOOL      copyright;
        BOOL      original;
        UINT8     emphasis;
      } T_L1A_MP3_INFO_CON;
    #endif

   /*
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        INT8                          status;
        UINT8                         status_type;
        UINT8                         channel_id;
        T_L1A_MP3_INFO_CON            info;
      } T_AUDIO_DRIVER_INFO_STATUS;

      typedef struct
      {
        T_RV_HDR os_hdr;
        INT8     status;
        T_L1A_MP3_INFO_CON            info;
      } T_AUDIO_MP3_INFO_STATUS;
      */

    #endif


   #if (L1_AAC == 1)
      typedef struct
        {
          UINT8         state;        /* aac state */
          T_RV_RETURN   return_path;  /* return path of the function that previously start the aac  */
          UINT8         channel_id;
#if(AS_RFS_API == 1)
		  T_RFS_FD      ffs_fd;
#else
          T_FFS_FD      ffs_fd;
#endif
          UINT8         session_id;
          UINT16        size;
          UINT8         size_left;    /* sample split between 2 RAM buffers */
          UINT8         previous_type;/* last sample type processed */
          BOOL          stop_req_allowed;/* TRUE first then FALSE */
          BOOL          pause_req_allowed;/* TRUE first then FALSE */
          BOOL          last_stop_msg;    /* FALSE first then TRUE */
          BOOL          last_pause_msg;    /* FALSE first then TRUE */
          BOOL          last_resume_msg;    /* FALSE first then TRUE */
          BOOL          last_restart_msg;    /* FALSE first then TRUE */
          BOOL          last_info_msg;    /* FALSE first then TRUE */
          UINT8         index_last_buffer;
          UINT32        size_file_start;
          UINT32        size_played;
          UINT8         aac_format;
          BOOL          buff_notify_msg;
#if(AS_RFS_API == 1)
	   T_RFS_SIZE audio_filesize;
#else
          T_FFS_SIZE       audio_filesize;/* Total filesize to be played */
#endif
          BOOL   aac_mono_stereo_mmi_command;  /* 0-mono 1-stereo */          
          BOOL   play_bar_on;   /* set to zero to switch off the playbar */
          BOOL   start_req; /* TRUE first , after start confirmation make it FALSE */
		  BOOL   stop_aac_true;
        #ifdef _WINDOWS
          UINT8 l1_state;
          INT16 counter;
        #endif
        } T_AUDIO_AAC_VAR;

  #endif
#if(L1_BT_AUDIO==1)
typedef struct{
  T_RVF_ADDR_ID task_id;            /* keybeep task_id */
  T_RV_RETURN   return_path;        /* return path of the function that previously start the keybeep */
}T_AUDIO_BT_VAR;
#endif

    typedef struct
    {
      T_RVF_ADDR_ID          addrId;            /* AUDIO address ID.       */
      T_RVF_ADDR_ID          audio_ffs_addrId;  /* AUDIO FFS manager address ID.       */

      T_RVF_MB_ID            mb_internal;   /* AUDIO internal memory bank. */
      T_RVF_MB_ID            mb_external;   /* AUDIO external memory bank. */
      T_RVF_MB_ID            mb_audio_ffs;  /* AUDIO FFS memory bank */
      T_RVM_RETURN           (*callBackFct)  (T_RVM_NAME        SWEntName,
                                              T_RVM_RETURN      errorCause,
                                              T_RVM_ERROR_TYPE  errorType,
                                              T_RVM_STRING      errorMsg);

      BOOLEAN               message_processed;

      #if (KEYBEEP)
        /* Keybeep global variable */
        T_AUDIO_KEYBEEP_VAR     keybeep;
      #endif
      #if (TONE)
        /* Tones global variable */
        T_AUDIO_TONES_VAR     tones;
      #endif
      #if (MELODY_E1)
        /* Melody_E1 global variable */
        BOOLEAN                   melody_E1_mode;
        T_AUDIO_MELODY_E1_VAR     melody_E1_0;
        T_AUDIO_MELODY_E1_VAR     melody_E1_1;
      #endif
      #if (MELODY_E2)
        /* Melody_E2 global variable */
        BOOLEAN                                melody_E2_mode;
        T_AUDIO_MELODY_E2_VAR                  melody_E2_0;
        T_AUDIO_MELODY_E2_VAR                  melody_E2_1;
        T_AUDIO_MELODY_E2_LOAD_FILE_VAR melody_E2_load_file_instruments;
      #endif
      #if (VOICE_MEMO)
        /* Voice Memo global variable */
        T_AUDIO_VOICE_MEMO_PLAY_VAR       vm_play;
        T_AUDIO_VOICE_MEMO_RECORD_VAR     vm_record;
      #endif


	  /*VENKAT*/
	#if (L1_PCM_EXTRACTION)
        /* Voice Memo global variable */

        T_AUDIO_VOICE_MEMO_PCM_PLAY_VAR       vm_pcm_play;
        T_AUDIO_VOICE_MEMO_PCM_RECORD_VAR     vm_pcm_record;

        /* Voice buffering global variable */
        T_AUDIO_VBUF_PCM_PLAY_VAR       vbuf_pcm_play;
        T_AUDIO_VBUF_PCM_RECORD_VAR     vbuf_pcm_record;
      #endif
	  /*!VENKAT*/

      #if (L1_VOICE_MEMO_AMR)
        /* Voice Memo global variable */
        T_AUDIO_VOICE_MEMO_AMR_RECORD_TO_MEM_VAR audio_vm_amr_record;
        T_AUDIO_VOICE_MEMO_AMR_PLAY_FROM_MEM_VAR audio_vm_amr_play;
      #endif
      #if (SPEECH_RECO)
        /* Speech reco global variable */
        T_AUDIO_SPEECH_RECO_VAR       speech_reco;
      #endif

      #if (L1_EXT_AUDIO_MGT==1)
        // Midi global variable
        T_AUDIO_MIDI_VAR midi;
      #endif

      #if (L1_MP3 == 1)
        // MP3 global variable
        T_AUDIO_MP3_VAR audio_mp3;
      #endif

      #if (L1_AAC == 1)
        // AAC global variable
        T_AUDIO_AAC_VAR audio_aac;
      #endif

      /* audio mode variable */
      T_AUDIO_MODE_VAR  audio_mode_var;

      #if (MELODY_E1) ||(MELODY_E2) || (VOICE_MEMO) || (L1_MP3) || (L1_AAC) || (L1_PCM_EXTRACTION)
        /* Audio FFS global variable */
        T_AUDIO_FFS_SESSION audio_ffs_session[AUDIO_FFS_MAX_CHANNEL];
      #endif
      #if (MELODY_E1) ||(MELODY_E2) || (VOICE_MEMO) || (L1_VOICE_MEMO_AMR) || (L1_MP3) || (L1_AAC) || (L1_PCM_EXTRACTION)
        /* Audio FFS global variable */
        T_AUDIO_DRIVER_SESSION audio_driver_session[AUDIO_DRIVER_MAX_CHANNEL];
      #endif
	  /*VENKAT*/
	  #if (L1_PCM_EXTRACTION)
	UINT8 vmemo_vbuf_session;
	  #endif
	 /*!VENKAT*/
#if(L1_BT_AUDIO==1)
       // BT global variable
T_AUDIO_BT_VAR bt;
#endif
    }
    T_AUDIO_ENV_CTRL_BLK;

    #ifdef __cplusplus
       }
    #endif
  #endif /* _AUDIO_STRUCTS_I_ */
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */

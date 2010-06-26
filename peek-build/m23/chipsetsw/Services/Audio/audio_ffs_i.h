/********************************************************************************/
/*                                                                              */
/*    File Name:   audio_ffs_i.h                                                */
/*                                                                              */
/*    Purpose:     This header contains the audio ffs constants related to      */
/*                 the ffs behavior.                                            */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/*    Revision History:                                                         */
/*       11 May 01     Francois Mazard - Stephanie Gerthoux        Create       */
/*                                                                              */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved.   */
/*                                                                              */
/********************************************************************************/
  #ifndef _AUDIO_FFS_I_H_
    #define _AUDIO_FFS_I_H_
/*VENKAT*/
//#define VOICE_MEMO_PCM
/*!VENKAT*/
    #ifdef __cplusplus
      extern "C"
      {
    #endif

      #define AUDIO_MEM_MAX_CHANNEL            (1)
      #define AUDIO_VM_AMR_PLAY_SIZE           (500)// 16 bit unit
      #define AUDIO_VM_AMR_PLAY_NB_BUFFER      (2)
      #define AUDIO_VM_AMR_RECORD_SIZE         (500)// 16 bit unit
      #define AUDIO_VM_AMR_RECORD_NB_BUFFER    (2)      
      #define AUDIO_MIDI_SIZE                  (500) // 16 bit unit
      #define AUDIO_MIDI_NB_BUFFER             (2)

      /* Buffer size and nb of buffers for MP3*/
      #define AUDIO_MP3_SIZE                    (2048) //(1500) //(4096)//(500)  /*  size in 16 bits units */
      #define AUDIO_MP3_NB_BUFFER               (2)     /* Nb of buffers */
     
      /* Buffer size and nb of buffers for AAC*/
      #define AUDIO_AAC_SIZE                    (2048)  /*  size in 16 bits units */
      #define AUDIO_AAC_NB_BUFFER               (2)     /* Nb of buffers */

      #if (MELODY_E1) || (MELODY_E2) || (VOICE_MEMO) || (L1_PCM_EXTRACTION)				
        /* maximum number of FFS channel */
        #define AUDIO_FFS_MAX_CHANNEL             (2)

        /* list of the session_id depending on the audio features */
        /* session id of the melody E1 number 0 */
        #define AUDIO_FFS_SESSION_MELODY_E1_0     (0)
        /* session id of the melody E1 number 1 */
        #define AUDIO_FFS_SESSION_MELODY_E1_1     (1)
        /* session id of the melody E2 number 0 */
        #define AUDIO_FFS_SESSION_MELODY_E2_0     (2)
        /* session id of the melody E2 number 1 */
        #define AUDIO_FFS_SESSION_MELODY_E2_1     (3)

        /* session id of the voice memo play */
        #define AUDIO_FFS_SESSION_VM_PLAY         (4)
        /* session id ofthe voice memo record */
        #define AUDIO_FFS_SESSION_VM_RECORD       (5)
        /* size of the data to download to the RAM for each features */

	/*VENKAT*/
		        /* session id of the voice buffering play */
        #define AUDIO_FFS_SESSION_VM_PCM_PLAY         (6)
        /* session id ofthe voice buffering record */
        #define AUDIO_FFS_SESSION_VM_PCM_RECORD       (7)
        /* size of the data to download to the RAM for each features */

        /* session id of the voice buffering play */
        #define AUDIO_FFS_SESSION_VBUF_PCM_PLAY         (8)
        /* session id ofthe voice buffering record */
        #define AUDIO_FFS_SESSION_VBUF_PCM_RECORD       (9)
        /* size of the data to download to the RAM for each features */
	/*!VENKAT*/

        /* using the FLASH to RAM session*/
        #define AUDIO_MELODY_E1_SIZE              (512)
        #define AUDIO_MELODY_E2_SIZE              (512)
        #define AUDIO_VM_PLAY_SIZE                (1024)
        /* size to allocate in RAM for each features */
        /* using the RAM to FLASH session */
        #define AUDIO_VM_RECORD_INITIAL_SIZE      (1024)
/*VENKAT*/
	#define AUDIO_VM_PCM_RECORD_SIZE	(8000) //4000 16 bit units will be recorded in 500 ms;audio ffs timer value is 500ms
	#define AUDIO_VM_PCM_PLAY_SIZE	(8000) //4000 16 bit units will be played in 500 ms;audio ffs timer value is 500ms
/*!VENKAT*/

/*VENKAT*/
	#define AUDIO_VBUF_PCM_RECORD_SIZE	(40960) //4000 16 bit units will be recorded in 500 ms;audio ffs timer value is 500ms
	#define AUDIO_VBUF_PCM_PLAY_SIZE	       (40960) //4000 16 bit units will be played in 500 ms;audio ffs timer value is 500ms
/*!VENKAT*/

        /* maximum number of buffer per session */
        #define AUDIO_MAX_FFS_BUFFER_PER_SESSION  (2)

        /* The audio FFS is activated: the audio task run with a time out */
        #ifndef _WINDOWS
		/*VENKAT*/
	#define 	AUDIO_FFS_TIME_OUT_PCM		(RVF_MS_TO_TICKS(65))
          #define AUDIO_FFS_TIME_OUT                (RVF_MS_TO_TICKS(350))
		  /*!VENKAT*/
        #else
          #define AUDIO_FFS_TIME_OUT                (RVF_MS_TO_TICKS(10))
        #endif

/*VENKAT*/
        /* The audio FFS manager need to be activate asap */
        #define AUDIO_FFS_ACTIVE_NOW              (0)
/*!VENKAT*/

        /* The audio FFS timer is in one shot mode */
        #define AUDIO_FFS_ONE_SHOT_TIMER          (FALSE)

        /* The audio FFS timer is in continuous mode */
        #define AUDIO_FFS_CONTINUOUS_TIMER        (TRUE)

        /* state of the cust_get_pointer state machine */
        #define  AUDIO_CUST_GET_POINTER_INIT      (0)
        #define  AUDIO_CUST_GET_POINTER_NORMAL    (1)
        #define  AUDIO_CUST_GET_POINTER_LOOP      (2)
      #endif /* MELODY_E1 || MELODY_E2 || VOICE_MEMO */

    #ifdef __cplusplus
      }
    #endif
  #endif /* _AUDIO_FFS_I_ */

/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_compatibilities.h                                     */
/*                                                                          */
/*  Purpose:  This file contains the compatibilities table between the      */
/*            audio feature as describe in the S916 specification (annex)   */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date         Modification                                               */
/*  ------------------------------------                                    */
/*  12 November 2001  Create                                                */
/*                                                                          */
/*  Author       Francois Mazard                                            */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE

  #ifndef __AUDIO_COMPATIBILITIES_H_
    #define __AUDIO_COMPATIBILITIES_H_

    #ifdef __cplusplus
      extern "C"
        {
    #endif

        /* Define the number of audio features */
	/*VENKAT*/
        #define AUDIO_NUMBER_OF_FEATURES        (19)
	/*!VENKAT*/
        typedef struct {
          BOOLEAN feature[AUDIO_NUMBER_OF_FEATURES];
        } T_AUDIO_ACTIVE_FEATURE;

        /* Define the list of the audio features */
        #define AUDIO_KEYBEEP_FEATURE           (0)
        #define AUDIO_TONES_FEATURE             (1)
        #define AUDIO_MELODY_E1_FEATURE         (2)
        #define AUDIO_VOICE_MEMO_PLAY_FEATURE   (3)
        #define AUDIO_VOICE_MEMO_RECORD_FEATURE (4)
        #define AUDIO_SR_ENROLL_FEATURE         (5)
        #define AUDIO_SR_UPDATE_FEATURE         (6)
        #define AUDIO_SR_RECO_FEATURE           (7)
        #define AUDIO_MELODY_E2_FEATURE         (8)
        #define AUDIO_TTY_FEATURE               (9)
        #define AUDIO_VOICE_MEMO_AMR_PLAY_FEATURE   (10)
        #define AUDIO_VOICE_MEMO_AMR_RECORD_FEATURE (11)
        #define AUDIO_MIDI_FEATURE              (12)
        #define AUDIO_MP3_FEATURE               (13)
        #define AUDIO_AAC_FEATURE               (14)
	 /*VENKAT*/
   	 #define AUDIO_VOICE_MEMO_PCM_PLAY_FEATURE   (15)
        #define AUDIO_VOICE_MEMO_PCM_RECORD_FEATURE (16)
   	 #define AUDIO_VBUF_PCM_PLAY_FEATURE   (17)
        #define AUDIO_VBUF_PCM_RECORD_FEATURE (18)

        /*!VENKAT*/

     /* It isn't a starting message */
        #define AUDIO_NO_START_MESSAGE      (-1)

        typedef struct {
          BOOLEAN compatibilities_flag[AUDIO_NUMBER_OF_FEATURES];
        } T_AUDIO_COMPATIBILITES;

    #ifdef __cplusplus
      } 
    #endif

  #endif /* __AUDIO_COMPATIBILITIES_H_ */
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */

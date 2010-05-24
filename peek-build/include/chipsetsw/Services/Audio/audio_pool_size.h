/**
 * @file	audio_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	Vincent Oberle
 * @version 0.1
 */

/*
 * History:
 *
 *  Date       	Author               Modification
 *  -------------------------------------------------------------------
 *  07/08/2003	Vincent Oberle       Extracted from rvf_pool_size.h
 *  09/06/2004  Frederic Maria       Split pool into external and internal memory.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __AUDIO_POOL_SIZE_H_
#define __AUDIO_POOL_SIZE_H_


#ifndef _WINDOWS
  #include "l1sw.cfg"
  #include "swconfig.cfg"
#endif


/*
 * Values used in audio_env_i.h
 */
#if (LOCOSTO_LITE)
 #define AUDIO_STACK_SIZE (1256)
#elif(L1_PCM_EXTRACTION)				
 #define AUDIO_STACK_SIZE (4328)//(2280)(new value to be optimised)
#else
 #define AUDIO_STACK_SIZE (2280)
#endif

#define AUDIO_MB1_SIZE   (1612)   //(1612)
#define AUDIO_MB2_SIZE   (1868)   //(1768)

#if (GSMLITE)
  #define AUDIO_MB3_SIZE   (0)
#elif (LOCOSTO_LITE)
  #define AUDIO_MB3_SIZE   (0)
  /*VENKAT*/
#elif (L1_PCM_EXTRACTION) //VENKAT
  #define AUDIO_MB3_SIZE   (43292 +163840 ) //if voice memo on PCM needs 2 buffers of 8K each for play and record so increased MB3 size from 7382 to 41174(to be optimized)
/*!VENKAT*/
#elif ((L1_MP3) || (L1_AAC))
  #define AUDIO_MB3_SIZE   (9500) 
#elif (SPEECH_RECO)
  #define AUDIO_MB3_SIZE   (7382)
#else
  #define AUDIO_MB3_SIZE   (3048)
#endif

#define AUDIO_POOL_SIZE      (AUDIO_MB1_SIZE + AUDIO_MB2_SIZE + AUDIO_MB3_SIZE)
#define AUDIO_INT_POOL_SIZE  (AUDIO_STACK_SIZE)


#define AUDIO_BGD_STACK_SIZE (512)
#define AUDIO_BGD_MB1_SIZE   (256)

#define AUDIO_BGD_POOL_SIZE      (AUDIO_BGD_MB1_SIZE)
#define AUDIO_BGD_INT_POOL_SIZE  (AUDIO_BGD_STACK_SIZE)


#endif /*__AUDIO_POOL_SIZE_H_*/

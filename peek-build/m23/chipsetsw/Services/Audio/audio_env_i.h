/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_env_i.h                                               */
/*                                                                          */
/*  Purpose:  This file contains prototypes for BT Environment related      */
/*            functions used to get info, start and stop the audio block.   */
/*                                                                          */
/*  Version   0.1                                                           */
/*                                                                          */
/*  Date         Modification                                               */
/*  ------------------------------------                                    */
/*  18 May 2001  Create                                                     */
/*                                                                          */
/*  Author       Francois Mazard - Stephanie Gerthoux                       */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#ifndef _WINDOWS
  #include "l1sw.cfg"
  #include "swconfig.cfg"
#endif

#include "rv/rv_defined_swe.h"
#ifdef RVM_AUDIO_MAIN_SWE
  #include "l1audio_cust.h"
  #include "Audio/audio_structs_i.h"

  #include "Audio/audio_pool_size.h"	/* Stack & Memory Bank sizes definitions */

  #ifndef __AUDIO_ENV_H_
    #define __AUDIO_ENV_H_

/*VENKAT*/
//#define VOICE_MEMO_PCM
/*!VENKAT*/

    /* memory bank size and watermark */
    #define AUDIO_MB_EXTERNAL_SIZE         AUDIO_MB1_SIZE
    #define AUDIO_MB_EXTERNAL_WATERMARK    AUDIO_MB_EXTERNAL_SIZE

    #define AUDIO_MB_INTERNAL_SIZE         AUDIO_MB2_SIZE
    #define AUDIO_MB_INTERNAL_WATERMARK    AUDIO_MB_INTERNAL_SIZE

    #if (!GSMLITE)
      #if ((L1_MP3) || (L1_AAC))
        #define AUDIO_MB_FFS_SIZE              AUDIO_MB3_SIZE
      #else
        #if (SPEECH_RECO)
          /* In case of the speech reco, the RAM used is for the word speech sample recording */
          /* 2 corresponds to sizeof(UINT16) */
          #define AUDIO_MB_FFS_SIZE              ( (SC_SR_MMI_2_L1_SPEECH_SIZE * 2)\
                                                    + (SC_SR_MMI_DB_MODEL_SIZE * 2) +\
                                                    (AUDIO_SR_MAX_VOCABULARY_NAME * AUDIO_PATH_NAME_MAX_SIZE)\
                                                    + 100 )
        #else
            #define AUDIO_MB_FFS_SIZE              AUDIO_MB3_SIZE
        #endif
      #endif
      #define AUDIO_MB_FFS_WATERMARK         AUDIO_MB_FFS_SIZE
    #endif

    /* generic functions declarations */
    T_RVM_RETURN audio_get_info (T_RVM_INFO_SWE  *infoSWEnt);

    T_RVM_RETURN audio_set_info(T_RVF_ADDR_ID addrId,
                                T_RV_RETURN   return_path[],
                                T_RVF_MB_ID   mbId[],
                                T_RVM_RETURN  (*callBackFct) ( T_RVM_NAME SWEntName,
                                                               T_RVM_RETURN errorCause,
                                                               T_RVM_ERROR_TYPE errorType,
                                                               T_RVM_STRING errorMsg) );

    T_RVM_RETURN audio_init (void);

    T_RVM_RETURN audio_stop (void);

    T_RVM_RETURN audio_kill (void);

  #endif /*__AUDIO_ENV_H_*/

#endif /* #ifdef RVM_AUDIO_MAIN_SWE */

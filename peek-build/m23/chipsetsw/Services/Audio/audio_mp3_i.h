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
    #include "l1sw.cfg"

    #include "l1_confg.h"
    #include "rvm/rvm_gen.h"
    /* include the usefull L1 header */
    #include "l1mp3_msgty.h"


    #ifdef _WINDOWS
      #define L1_MP3  1
    #endif

    #if (L1_MP3 == 1)
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
#if 0
// moved to audio_api.h as it is used by other modules.
     typedef struct
      {
        T_RV_HDR os_hdr;
        INT8     status;
        UINT32  u32TotalTimePlayed;
        UINT32 u32totalTimeEst;
      } T_AUDIO_MP3_STATUS_BAR;
#endif

    #endif

    #ifdef __cplusplus
       }
    #endif
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */

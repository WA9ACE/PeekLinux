/********************************************************************************/
/*                                                                              */
/*    File Name:   audio_aac_i.h                                                */
/*                                                                              */
/*    Purpose:     This file contains constants, data type, and data            */
/*                 structures that are used by the AAC AUDIO's task.            */
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
    #include "l1aac_msgty.h"


    #ifdef _WINDOWS
      #define L1_AAC  1
    #endif

    #if (L1_AAC == 1)
      typedef struct
      {
        T_RV_HDR                      os_hdr;
        INT8                          status;
        UINT8                         status_type;
        UINT8                         channel_id;
        T_L1A_AAC_INFO_CON            info;
      } T_AUDIO_DRIVER_AAC_INFO_STATUS;

      typedef struct
      {
        T_RV_HDR os_hdr;
        INT8     status;
        T_L1A_AAC_INFO_CON            info;
      } T_AUDIO_AAC_INFO_STATUS;

#if 0
     typedef struct
      {
        T_RV_HDR os_hdr;
        INT8     status;
        UINT32  u32TotalTimePlayed;
        UINT16 u16totalTimeEst;
      } T_AUDIO_AAC_STATUS_BAR;

#endif
    #endif

    #ifdef __cplusplus
       }
    #endif
#endif /* #ifdef RVM_AUDIO_MAIN_SWE */

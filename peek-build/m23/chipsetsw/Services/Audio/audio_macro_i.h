/********************************************************************************/
/*                                                                              */
/*    File Name:   audio_macro_i.h                                              */
/*                                                                              */
/*    Purpose:     This header contains the generic structures and constants    */
/*                 that are used by the AUDIO layer.                            */
/*                                                                              */
/*    Note:                                                                     */
/*        None.                                                                 */
/*                                                                              */
/*    Revision History:                                                         */
/*       11/05/01     Francois Mazard - Stephanie Gerthoux        Create        */
/*                                                                              */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved.   */
/*                                                                              */
/********************************************************************************/

#include "rv/rv_defined_swe.h"
#include "rvm/rvm_use_id_list.h"

#ifdef RVM_AUDIO_MAIN_SWE
  #ifndef _AUDIO_MACRO_I_H
    #define _AUDIO_MACRO_I_H

    #ifdef __cplusplus
      extern "C"
        {
    #endif

    /*
    ** AUDIO trace macros
    */
    #define AUDIO_SEND_TRACE(string,type) rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,type,AUDIO_USE_ID)
    #define AUDIO_SEND_TRACE_PARAM(string,param,type) rvf_send_trace (string,(sizeof(string)-1),(UINT32)param,type,AUDIO_USE_ID)
    #define AUDIO_TRACE_WARNING(string) rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_WARNING,AUDIO_USE_ID)

    #ifdef __cplusplus
       }
    #endif

  #endif

#endif /* #ifdef RVM_AUDIO_MAIN_SWE */



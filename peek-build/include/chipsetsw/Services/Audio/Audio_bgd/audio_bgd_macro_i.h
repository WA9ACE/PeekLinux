/********************************************************************************/
/*                                                                              */
/*    File Name:   audio_bgd_macro_i.h                                          */
/*                                                                              */
/*    Purpose:     This header contains the generic structures and constants    */
/*                 that are used by the AUDIO BACKGROUND layer.                 */
/*                                                                              */
/*  Version   1                                                                 */
/*                                                                              */
/*  Date          Modification                                                  */
/*  ------------------------------------                                        */
/*  19 June 2003  Creation                                                      */
/*                                                                              */
/*  Author      Frederic Turgis                                                 */
/*                                                                              */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved.   */
/*                                                                              */
/********************************************************************************/

#include "rv/rv_defined_swe.h"

#ifdef RVM_AUDIO_BGD_SWE

  #ifndef _AUDIO_BGD_MACRO_I_H
  #define _AUDIO_BGD_MACRO_I_H

  #include "rv/rv_trace.h"
  #include "rvf/rvf_api.h"
  #include "rvm/rvm_use_id_list.h"

  #ifdef __cplusplus
    extern "C"
    {
  #endif

  /*
  ** AUDIO trace macros
  */
  #define AUDIO_BGD_SEND_TRACE(string,type) rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,type,AUDIO_BGD_USE_ID)
  #define AUDIO_BGD_SEND_TRACE_PARAM(string,param,type) rvf_send_trace (string,(sizeof(string)-1),(UINT32)param,type,AUDIO_BGD_USE_ID)
  #define AUDIO_BGD_TRACE_WARNING(string) rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_WARNING,AUDIO_BGD_USE_ID)

  #ifdef __cplusplus
    }
  #endif

  #endif // #ifndef _AUDIO_BGD_MACRO_I_H

#endif // #ifdef RVM_AUDIO_BGD_SWE


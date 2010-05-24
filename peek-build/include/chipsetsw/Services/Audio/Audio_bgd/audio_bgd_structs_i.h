/********************************************************************************/
/*                                                                              */
/*    File Name:   audio_structs_i.h                                            */
/*                                                                              */
/*    Purpose:     This file contains constants, data type, and data            */
/*                 structures that are used by the AUDIO's task.                */
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

  #ifndef  _AUDIO_BGD_STRUCTS_I_
  #define  _AUDIO_BGD_STRUCTS_I_

  #include "rv/rv_general.h"
  #include "rvf/rvf_api.h"
  #include "rvm/rvm_api.h"
  #include "rvm/rvm_gen.h"

  #ifdef __cplusplus
    extern "C"
    {
  #endif

  typedef struct
  {
    T_RVF_ADDR_ID          addrId;            /* AUDIO address ID.       */

    T_RVF_MB_ID            mb;   /* AUDIO internal memory bank. */

    T_RVM_RETURN           (*callBackFct)  (T_RVM_NAME        SWEntName,
                                            T_RVM_RETURN      errorCause,
                                            T_RVM_ERROR_TYPE  errorType,
                                            T_RVM_STRING      errorMsg);

    BOOLEAN                message_processed;
  }
  T_AUDIO_BGD_ENV_CTRL_BLK;

  #ifdef __cplusplus
    }
  #endif

  #endif // #ifndef  _AUDIO_BGD_STRUCTS_I_

#endif // #ifdef RVM_AUDIO_BGD_SWE

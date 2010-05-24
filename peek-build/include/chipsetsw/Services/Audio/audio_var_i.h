/********************************************************************************/
/*                                                                              */
/*    File Name:   audio_var_i.h                                                */
/*                                                                              */
/*    Purpose:     This header contains the global variables                    */
/*                 that are used by the AUDIO layer.                            */
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
  #ifndef _AUDIO_VAR_I_H_
    #define _AUDIO_VAR_I_H_

    #ifdef __cplusplus
      extern "C"
      {
    #endif

    /* Define a pointer to the Global Environment Control block  */
  extern T_AUDIO_ENV_CTRL_BLK *p_audio_gbl_var;

    #ifdef __cplusplus
      }
    #endif
  #endif /* _AUDIO_VAR_I_ */

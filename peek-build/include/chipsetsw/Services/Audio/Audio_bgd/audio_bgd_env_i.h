/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_bgd_env_i.h                                           */
/*                                                                          */
/*  Purpose:  This file contains prototypes for BT Environment related      */
/*            functions used to get info, start and stop the audio block.   */
/*                                                                          */
/*  Version   1                                                             */
/*                                                                          */
/*  Date          Modification                                              */
/*  ------------------------------------                                    */
/*  19 June 2003  Creation                                                  */
/*                                                                          */
/*  Author      Frederic Turgis                                             */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"

#ifdef RVM_AUDIO_BGD_SWE

  #ifndef __AUDIO_BGD_ENV_H_
  #define __AUDIO_BGD_ENV_H_

  extern T_AUDIO_BGD_ENV_CTRL_BLK *p_audio_bgd_gbl_var;

  #endif // #ifndef __AUDIO_BGD_ENV_H_

#endif // #ifdef RVM_AUDIO_BGD_SWE

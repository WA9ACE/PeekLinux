/****************************************************************************/
/*                                                                          */
/*  File Name:  audio_bgd_const_i.h                                         */
/*                                                                          */
/*  Purpose:  Internal constants used by AUDIO BACKGROUND instance          */
/*                                                                          */
/*                                                                          */
/*  Version   1                                                             */
/*                                                                          */
/*  Date          Modification                                              */
/*  ------------------------------------                                    */
/*  19 June 2003  Creation                                                  */
/*                                                                          */
/*  Author      Frederic Turgis                                             */
/*                                                                          */
/*                                                                          */
/* (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved*/
/****************************************************************************/

#include "rv/rv_defined_swe.h"

#ifdef RVM_AUDIO_BGD_SWE

  #ifndef __AUDIO_CONST_I_H_
  #define __AUDIO_CONST_I_H_

  #include "rvf/rvf_api.h"

  #ifdef __cplusplus
    extern "C"
    {
  #endif

  /* the task processes events with no time-out */
  #define AUDIO_BGD_NOT_TIME_OUT             (0)

  /* The audio background entity processes only the message sent to the following mail box */
  #define AUDIO_BGD_MBOX                     (RVF_TASK_MBOX_0)

  /* The audio entity takes into account only the following events: */
  #define AUDIO_BGD_TASK_MBOX_EVT_MASK       (RVF_TASK_MBOX_0_EVT_MASK)

  /* The audio background entity waits all event type */
  #define AUDIO_BGD_ALL_EVENT_FLAGS                       (0xFFFF)

  #ifdef __cplusplus
  }
  #endif

  #endif // #ifndef __AUDIO_CONST_I_H_
#endif // #ifdef RVM_AUDIO_BGD_SWE

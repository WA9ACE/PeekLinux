/**
 * @file	tty_api.h
 *
 * API Definition for TTY SWE.
 *
 * @author	Frederic Turgis (f-turgis@ti.com) & Gerard Cauvy (g-cauvy@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 * 	Date       	Modification
 *  ------------------------------------
 *  01/27/2003	Create
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __TTY_API_H_
#define __TTY_API_H_

#include "rvm/rvm_gen.h"		/* Generic RVM types and functions. */

/* TTY entity (Background Task) is highly linked to the Audio entity. */
#include "Audio/audio_api.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
  UINT8  Mode;
  UINT8  tty_mode; /* 0-No_TTY, 1-TTY_VCO, 2-TTY_HCO, 3-TTY_All */
  UINT16 ThresholdRead;
  UINT16 ThreshHoldWrite;
}
T_AUDIO_TTY_CONFIG_PARAMETER;


typedef enum
{
   TTY_API_NOTTY=0,
   TTY_API_VCO,
   TTY_API_HCO,
   TTY_API_ALL
}T_TTY_API_CMD;

#define TTY_STOP      (0)
#define TTY_EXT_START (1)

T_AUDIO_RET audio_tty_set_config (T_AUDIO_TTY_CONFIG_PARAMETER *parameter,
                                  T_RV_RETURN *return_path);

#define AUDIO_TTY_STATUS_MSG (0x00F2 | AUDIO_EVENT_EXTERN | AUDIO_EVENTS_MASK)

typedef struct
{
  T_RV_HDR  os_hdr;
  INT8      status;
}
T_AUDIO_TTY_STATUS;

#ifdef __cplusplus
}
#endif


#endif /*__TTY_API_H_*/


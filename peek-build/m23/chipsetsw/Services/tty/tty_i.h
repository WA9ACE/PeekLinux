/**
 * @file	tty_i.h
 *
 * Internal definitions for TTY.
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

#ifndef __TTY_I_H_
#define __TTY_I_H_


#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"

/* TTY entity (Background Task) is HIGHLY linked to the Audio entity. */
#include "Audio/audio_api.h"

/* Macro definition for sending trace */
#define TTY_SEND_TRACE(text,level) rvf_send_trace(text, sizeof(text)-1, NULL_PARAM, level, TTY_USE_ID )

/* Events Masks for TTY SWE. */
#define TTY_DATA_EVENT_MASK (EVENT_MASK(RVF_APPL_EVT_0))
#define TTY_INIT_EVENT_MASK (EVENT_MASK(RVF_APPL_EVT_1))
#define TTY_EXIT_EVENT_MASK (EVENT_MASK(RVF_APPL_EVT_2))

/* TTY Control Block. */
typedef struct
{
	/**
	 * Return path of the input result (should be the return path to access 
	 * to the SWE we are communicating with).
	 */
	T_RV_RETURN_PATH return_path;

	/** Pointer to the error function */
	T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,	
							 T_RVM_RETURN error_cause,
							 T_RVM_ERROR_TYPE error_type,
							 T_RVM_STRING error_msg);

	/** Mem bank id. */
	T_RVF_MB_ID prim_mb_id;

	/** Address ID of TTY SWE */
	T_RVF_G_ADDR_ID addr_id;

	UINT8 state;

} T_TTY_ENV_CTRL_BLK;

/* Internal messages definition */
#define TTY_START_REQ (0x00F0 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)

typedef struct
{
  T_RV_HDR    os_hdr;
  UINT8       tty_mode; 
  T_RV_RETURN return_path;
} T_TTY_START;

#define TTY_STOP_REQ (0x00F1 | AUDIO_EVENT_INTERN | AUDIO_EVENTS_MASK)

typedef struct
{
  T_RV_HDR    os_hdr;
  T_RV_RETURN return_path;
} T_TTY_STOP;

/* Define all the state of the TTY State Machine */
#define TTY_IDLE                   (1)
#define TTY_WAIT_START_CON         (2)
#define TTY_WAIT_STOP_COMMAND      (3)
#define TTY_WAIT_START_CON_TO_STOP (4)
#define TTY_WAIT_STOP_CON          (5)

#endif /* __TTY_I_H_ */

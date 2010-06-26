/**
 * @file	rng_i.h
 *
 * Internal definitions for RNG.
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	6/10/2003	  R. Kramer   		Create.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __RNG_INST_I_H_
#define __RNG_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "rng/rng_cfg.h"
#include "rng/rng_api.h"


/** Macro used for tracing RNG messages. */
#define RNG_SEND_TRACE(string, trace_level) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, RNG_USE_ID)

/* definiton of the size of numbers that are maximal in the queue */
#define MAX_QUEUE_SIZE  5

/* RNG register mapping */
#define RNG_OUT             (MEM_RNG)
#define RNG_STAT            (MEM_RNG+0x04)
#define RNG_CTRL            (MEM_RNG+0x08)
#define RNG_ENTA            (MEM_RNG+0x0C)
#define RNG_ENTB            (MEM_RNG+0x10)
#define RNG_X0              (MEM_RNG+0x14)
#define RNG_X1              (MEM_RNG+0x18)
#define RNG_X2              (MEM_RNG+0x1C)
#define RNG_COUNT           (MEM_RNG+0x20)
#define RNG_ALARM           (MEM_RNG+0x24)
#define RNG_CONFIG          (MEM_RNG+0x28)
#define RNG_LFSR1_0         (MEM_RNG+0x2C)
#define RNG_LFSR1_1         (MEM_RNG+0x30)
#define RNG_LFSR2_0         (MEM_RNG+0x34)
#define RNG_LFSR2_1         (MEM_RNG+0x38)
#define RNG_REV             (MEM_RNG+0x3C)
#define RNG_MASK            (MEM_RNG+0x40)
#define RNG_SYSSTATUS       (MEM_RNG+0x44)

/* Bit masks */
#define RNG_AUTO_IDLE_BIT   0x000001
#define RNG_BUSY_BIT_MASK   0x01

/**
 * The Control Block buffer of RNG, which gathers all 'Global variables'
 * used by RNG instance.
 *
 * A structure should gathers all the 'global variables' of RNG instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_RNG_ENV_CTRL_BLK buffer is allocated when creating RNG instance and is 
 * then always refered by RNG instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
	/** Pointer to the error function */
	T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,	
							 T_RVM_RETURN error_cause,
							 T_RVM_ERROR_TYPE error_type,
							 T_RVM_STRING error_msg);
	/** Mem bank id. */
	T_RVF_MB_ID prim_mb_id;

	T_RVF_ADDR_ID	addr_id;

  BOOL rng_initialised; /* Used for indicating if RNG is initialised */

} T_RNG_ENV_CTRL_BLK;


/** External ref "global variables" structure. */
extern T_RNG_ENV_CTRL_BLK	*rng_env_ctrl_blk_p;

extern BOOL rng_idle;

#endif /* __RNG_INST_I_H_ */

/**
 * @file  mpk_i.h
 *
 * Internal definitions for MPK.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  5/30/2003 Dennis Gallas ()    Create.
 *  22/07/2003	 TI		  Update   Flag _WINDOWS for exclude mem.h from PC build * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __MPK_INST_I_H_
#define __MPK_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "mpk/mpk_cfg.h"
#include "mpk/mpk_api.h"
#ifndef _WINDOWS
	#include "memif/mem.h"
#endif


/** Macro used for tracing MPK messages. */
#define MPK_SEND_TRACE(string, trace_level) \
  rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, MPK_USE_ID)

#define MPK_ID_REG      (MEM_DEV_ID0 + 0xC)

#endif /* __MPK_INST_I_H_ */

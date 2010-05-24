/**
 * @file	die_i.h
 *
 * Internal definitions for DIE.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	5/30/2003	 ()		Create.
 *	22/07/2003	 TI		Modif		Flag _WINDOWS for exclude mem.h from PC build
 *
 * (C) Copyright 2003 by ICT Embedded, All Rights Reserved
 */

#ifndef __DIE_INST_I_H_
#define __DIE_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "die/die_cfg.h"
#include "die/die_api.h"

#ifndef _WINDOWS
	#include "memif/mem.h"
#endif



/** Macro used for tracing DIE messages. */
#define DIE_SEND_TRACE(string, trace_level) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, DIE_USE_ID)

/* DIE ID register */
#define DIE_ID_REG    (MEM_DEV_ID0 + 0x4)
#if (CHIPSET==15)
#define DIE_ID_REG1    (MEM_DEV_ID0 + 0x40)
#endif

#endif /* __DIE_INST_I_H_ */

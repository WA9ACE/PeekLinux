/**
 * @file	hash_hw_version.h
 *
 * TASK Definition for hash SWE.
 *  BOARD file
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/31/2003	Eddy Oude Middendorp (ICT)  split from common &Completion.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __HASH_HW_FUNCTIONS_H_
#define __HASH_HW_FUNCTIONS_H_


#include "rvm/rvm_gen.h"            /* Generic RVM types and functions. */
#include "dmg/dmg_api.h" 

#ifdef __cplusplus
extern "C"
{
#endif

#define HASH_REGISTER_OFFSET (0x09800000u)

#define HASH_REG32(x) (*((volatile UINT32 *)(HASH_REGISTER_OFFSET+(x))))

#define HASH_DIGEST_A_REG   HASH_REG32(0x00)
#define HASH_DIGEST_B_REG   HASH_REG32(0x04)
#define HASH_DIGEST_C_REG   HASH_REG32(0x08)
#define HASH_DIGEST_D_REG   HASH_REG32(0x0c)
#define HASH_DIGEST_E_REG   HASH_REG32(0x10)
#define HASH_DIGCNT_REG     HASH_REG32(0x14)
#define HASH_CTRL_REG       HASH_REG32(0x18)
#define HASH_REV_REG        HASH_REG32(0x5c)
#define HASH_MASK_REG       HASH_REG32(0x60)
#define HASH_SYSSTATUS_REG  HASH_REG32(0x64)
#define HASH_DIN_ARRAY8 ((volatile UINT8 *)(HASH_REGISTER_OFFSET+0x1c))
#define HASH_DIN_ARRAY32 ((volatile UINT32 *)(HASH_REGISTER_OFFSET+0x1c))

#define HASH_CTRL_CLOSE       (0x10)
#define HASH_CTRL_BEGIN       (0x08)
#define HASH_CTRL_SHA1        (0x04)
#define HASH_CTRL_LENGTH_SHL  (5)
#define HASH_DMA_ENABLE       (0x8)
#define HASH_DMA_DISABLE      (0x0)

#ifndef __HASH_CFG_H_
#define HASH_AUTO_IDLE_ENABLE (1)
#endif

extern UINT32 hash_hw_get_hardware_version ();
extern T_RVM_RETURN hash_hw_exec_hash (T_HASH_CLIENT_STATE * context_p,
                                       UINT8 * data_p, UINT32 data_size,
                                       T_HASH_MSG * rsp_msg);
extern void hash_hw_process_byte_string (T_HASH_CLIENT_STATE * context_p,
                                         UINT8 * data, UINT32 length);
extern T_RV_RET hash_hw_process_byte_string_DMG (T_HASH_CLIENT_STATE * 
                                context_p, UINT8 * data_p, UINT32 length);

extern void hash_hw_init (void);

static T_DMG_CHANNEL hash_dmg_wait_status_OK (void);

/*@}*/


#ifdef __cplusplus
}
#endif


#endif /*__hash_HW_FUNCTIONS_H_*/

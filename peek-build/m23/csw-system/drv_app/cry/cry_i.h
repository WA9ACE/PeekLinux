/**
 * @file	cry_i.h
 *
 * Internal definitions for CRY.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/14/2003	 ()		Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __CRY_INST_I_H_
#define __CRY_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "cry/cry_cfg.h"
#include "cry/cry_api.h"
#include "cry/cry_message.h"

#include "cry/cry_state_i.h"


/** Macro used for tracing CRY messages. */
#define CRY_SEND_TRACE(string, trace_level) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, CRY_USE_ID)

#define CRY_SEND_TRACE_PARAM(string, param, trace_level) \
	rvf_send_trace (string, (sizeof(string) - 1), (UINT32)param, trace_level, CRY_USE_ID)



/**
 * The Control Block buffer of CRY, which gathers all 'Global variables'
 * used by CRY instance.
 *
 * A structure should gathers all the 'global variables' of CRY instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_CRY_ENV_CTRL_BLK buffer is allocated when creating CRY instance and is 
 * then always refered by CRY instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
	/** Store the current state of the CRY instance */
	T_CRY_INTERNAL_STATE state;

	/** Pointer to the error function */
  T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name,	
                T_RVM_RETURN error_cause,
                T_RVM_ERROR_TYPE error_type,
                T_RVM_STRING error_msg);
	/** Mem bank id. */
  T_RVF_MB_ID prim_mb_id;

  T_RVF_ADDR_ID	addr_id;

  BOOL cry_initialised;// = FALSE; /* Used for indicating if CRY is initialised */
  struct
  {
	T_RVF_G_ADDR_ID addr_id;
	UINT16 event;
	T_RVF_BUFFER_Q queue_obj;
  }
  return_queue;
  T_RV_RETURN_PATH path_to_return_queue;
  T_CRYPTO_DMA_MODE dma_mode;

} T_CRY_ENV_CTRL_BLK;

/**
 *  Structure used for storing keys and algorithm
 *
 */
typedef struct{
              T_CRYPTO_ALGORITM algorithm;
              UINT8 key1_len;
              UINT8 key1[8];
              UINT8 key2_len;
              UINT8 key2[8];
              UINT8 key3_len;
              UINT8 key3[8];
} T_CRYPTO_KEY;

/**
 *  Structure used for storing the context to be used
 *
 */
typedef struct{
              T_CRYPTO_MODE mode;
              T_CRYPTO_KEY *des_key;
              T_RV_RETURN return_path;
              UINT16 ivlen;
              UINT8 iv[8];
} T_CRYPTO_CONTEXT;


/** External ref "global variables" structure. */
extern T_CRY_ENV_CTRL_BLK	*cry_env_ctrl_blk_p;

/* Memory addresses for the registers of DES3DES */
#define DES_KEY3_L    MEM_DES
#define DES_KEY3_H    (MEM_DES + 0x04)
#define DES_KEY2_L    (MEM_DES + 0x08)
#define DES_KEY2_H    (MEM_DES + 0x0C)
#define DES_KEY1_L    (MEM_DES + 0x10)
#define DES_KEY1_H    (MEM_DES + 0x14)
#define DES_IV_L      (MEM_DES + 0x18)
#define DES_IV_H      (MEM_DES + 0x1C)
#define DES_CTRL      (MEM_DES + 0x20)
#define DES_DATA_L    (MEM_DES + 0x24)
#define DES_DATA_H    (MEM_DES + 0x28)
#define DES_REV       (MEM_DES + 0x2C)
#define DES_MASK      (MEM_DES + 0x30)
#define DES_SYSSTATUS (MEM_DES + 0x34)

#define DES_AUTO_IDLE_BIT 0x00000001
#define DES_DIRECTION_BIT 0x00000004
#define DES_TDES_BIT      0x00000008
#define DES_CBC_BIT       0x00000010
#define DES_START_BIT     0x00000020
#define DES_READY_BIT     0x00000001
#define DES_DMA_REQ_IN_BIT (1u<<2)
#define DES_DMA_REQ_OUT_BIT (1u<<3)

#define CRY_RETURN_QUEUE_EVENT (1u<<14)
extern void *cry_return_queue_get_msg (void);
extern void cry_return_queue_store_msg (void *msg_p);
extern void cry_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id,
								   UINT16 rq_event,
								   T_RV_RETURN_PATH * path_to_return_queue_p);
#endif /* __CRY_INST_I_H_ */

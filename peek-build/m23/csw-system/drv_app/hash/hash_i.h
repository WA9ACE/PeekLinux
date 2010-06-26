/**
 * @file	hash_i.h
 *
 * Internal definitions for SHA.
 *
 * @author	()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/23/2003	raymond zandbergen (ICT)		    Create.
 *	7/31/2003	Eddy Oude Middendorp (ICT)  Completion.
 *
 * (C) Copyright 2003 by ICT, All Rights Reserved
 */

#ifndef __HASH_INST_I_H_
#define __HASH_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "hash/hash_cfg.h"
#include "hash/hash_api.h"
#include "hash/hash_message.h"
#include "hash/hash_state_i.h"

#define HASH_ENABLE_DMA
// #undef HASH_ENABLE_DMA

/* Software version */
#define HASH_MAJOR  0
#define HASH_MINOR  2
#define HASH_BUILD  0

//sizes in bytes
#define HASH_MAX_BLOCKSIZE	((1L<<27))
#define HASH_HW_FIFOSIZE	  (64)

/** Macro used for tracing SHA messages. */
#define HASH_SEND_TRACE(string, trace_level) \
	rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, HASH_USE_ID)
#define HASH_SEND_TRACE_PARAM(text,param,level) \
  rvf_send_trace(text, sizeof(text)-1, param, level, HASH_USE_ID )

#define hash_lock()   rvf_lock_mutex (&hash_env_ctrl_blk_p->mutex)
#define hash_unlock() rvf_unlock_mutex (&hash_env_ctrl_blk_p->mutex)

#define HASH_DMA_MODE hash_env_ctrl_blk_p->hash_dma_mode

#define HASH_RETURN_QUEUE_EVENT (1u<<14)

typedef union
{
  T_RV_HDR                      os_hdr;
  T_HASH_MD5_HASH_REQ_MSG       md5_hash;
  T_HASH_MD5_REQ_DATA_RSP_MSG   md5_req_data;
  T_HASH_MD5_RESULT_RSP_MSG     md5_result;
  T_HASH_SHA1_HASH_REQ_MSG      sha1_hash;
  T_HASH_SHA1_REQ_DATA_RSP_MSG  sha1_req_data;
  T_HASH_SHA1_RESULT_RSP_MSG    sha1_result;
} T_HASH_MSG;


typedef enum {
  HASH_TYPE_INVALID,
  HASH_TYPE_SHA1,
  HASH_TYPE_MD5
} T_HASH_TYPE;

typedef struct {
  T_HASH_TYPE hash_type;
  BOOL        force_init;
  BOOL        last_block;
  UINT8       rest_cnt;     //number of bytes in data_rest[]
  T_RV_RETURN return_path;
  UINT32      digest_a;
  UINT32      digest_b;
  UINT32      digest_c;
  UINT32      digest_d;
  UINT32      digest_e;
  UINT32      digcnt;
  UINT32      data_rest[HASH_HW_FIFOSIZE/sizeof(UINT32)];
} T_HASH_CLIENT_STATE;

/**
 * The Control Block buffer of SHA, which gathers all 'Global variables'
 * used by SHA instance.
 *
 * A structure should gathers all the 'global variables' of SHA instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_HASH_ENV_CTRL_BLK buffer is allocated when creating SHA instance and is 
 * then always refered by SHA instance when access to 'global variable' 
 * is necessary.
 */

typedef struct
{
        /** Store the current state of the SHA instance */
  T_HASH_INTERNAL_STATE state;

        /** Pointer to the error function */
  T_RVM_RETURN (*error_ft) (T_RVM_NAME swe_name,
                            T_RVM_RETURN error_cause,
                            T_RVM_ERROR_TYPE error_type,
                            T_RVM_STRING error_msg);
        /** Mem bank id. */
  T_RVF_MB_ID prim_mb_id;

  T_RVF_ADDR_ID addr_id;
  BOOL initialised;
  T_RVF_MUTEX mutex;
  struct
  {
	T_RVF_G_ADDR_ID addr_id;
	UINT16 event;
	T_RVF_BUFFER_Q queue_obj;
  }
  return_queue;
  T_RV_RETURN_PATH path_to_return_queue;
  UINT8 hash_dma_mode;
}
T_HASH_ENV_CTRL_BLK;

/** External ref "global variables" structure. */
extern T_HASH_ENV_CTRL_BLK *hash_env_ctrl_blk_p;
extern void *hash_return_queue_get_msg (void);
extern void hash_return_queue_init (T_RVF_G_ADDR_ID rq_addr_id,
								   UINT16 rq_event,
								   T_RV_RETURN_PATH * path_to_return_queue_p);

#endif /* __HASH_INST_I_H_ */

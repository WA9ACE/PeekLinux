/**
 * @file  rfs_i.h
 *
 * Internal definitions for RFS.
 *
 * @author   ()
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  1/23/2004  ()   Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __RFS_INST_I_H_
#define __RFS_INST_I_H_


#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "rfs/fscore_types.h"

#include "rfs/rfs_cfg.h"
#include "rfs/rfs_api.h"
#include "rfs/rfs_message.h"
#include "rfs/rfs_message_i.h"
#include "rfs/rfs_state_i.h"
#include "rfs/rfs_media_manager.h"

/* Include interface header files of underlaying SWE's */
#include "gbi/gbi_api.h"
#include "gbi/gbi_message.h"

#include "rfsfat/rfsfat_api.h"

#include "rfsnand/rfsnand_api.h"
#include "rfsnand/rfsnand_message.h"

/* Contains information of GBI */
#include "rfs/rfs_fm.h"

#define EOSTR                   (0)
/* #define ENABLE_RFS_TRACE		 */


#ifdef ENABLE_RFS_TRACE		
/** Macro used for tracing RFS messages. */
#define RFS_SEND_TRACE(string, trace_level) \
  rvf_send_trace (string, (sizeof(string) - 1), NULL_PARAM, trace_level, RFS_USE_ID)
#define RFS_SEND_TRACE_PARAM_ERROR(string, param) \
	rvf_send_trace (string, (sizeof(string) - 1), param, RV_TRACE_LEVEL_ERROR, RFS_USE_ID)

/** Macro used for tracing RFS messages with a parameter. */
#define RFS_SEND_TRACE_PARAM(text, param, level) \
  rvf_send_trace(text, sizeof(text)-1, (UINT32)(param), level, RFS_USE_ID )
#else		// Andrew
/** Macro used for tracing RFS messages. */
#define RFS_SEND_TRACE(string, trace_level)

/* Error Trace */
#define RFS_SEND_TRACE_PARAM_ERROR(string, param)

/** Macro used for tracing RFS messages with a parameter. */
#define RFS_SEND_TRACE_PARAM(text, param, level)
#endif		// Andrew

#define FSCORE_ILLEGAL_CORE_NMB ((UINT8)(~1u))
#define FSCORE_ILLEGAL_PAIR_ID (-1)
#define FSCORE_ILLEGAL_FD (-1)

#define rfs_lock_mutex_pair_id()    rvf_lock_mutex   (&rfs_env_ctrl_blk_p->mutex_pair_id)
#define rfs_unlock_mutex_pair_id()  rvf_unlock_mutex (&rfs_env_ctrl_blk_p->mutex_pair_id)


#define FSCORE_MAX_NR_OPEN_FSCORE_FILES  10
/**
 * This structure maps client to core pair ids / fds
 */
typedef struct {
	BOOL is_fd;
	UINT8 fs_core_nmb;
	T_RFS_PAIR_VALUE pair_value[2]; /* 0 == client, 1 == core */
	T_RFS_RETURN_PATH client_return_path;
  UINT8       op_mode;            /* Opened file in synchronous/asynchronous mode */
} T_RFS_PAIR_VALUE_MAP;


/* 
 * RFS API: Default values
 */

#define RFS_FD_DEF_VALUE         0x0000
#define RFS_ADDR_ID_DEF_VALUE    0x00
#define RFS_CB_DEF_VALUE         NULL
#define RFS_OPMODE_DEF_VALUE     O_SIO

#define RFS_MAX_MOUNT_POINTS     5

/* Commented: probably not used in the future */
/*
 * Second mailbox definitions *-/
#define RFS_RETURN_QUEUE_EVENT (1<<15)

typedef struct {
  T_RVF_G_ADDR_ID addr_id;
  UINT16          event;
  T_RVF_BUFFER_Q  queue_obj;
} T_RFS_RETURN_QUEUE;
*/

/**
 * The Control Block buffer of RFS, which gathers all 'Global variables'
 * used by RFS instance.
 *
 * A structure should gathers all the 'global variables' of RFS instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_RFS_ENV_CTRL_BLK buffer is allocated when creating RFS instance and is 
 * then always refered by RFS instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
  /** Store the current state of the RFS instance */
  T_RFS_INTERNAL_STATE state;

 
  /** Pointer to the error function */
  T_RVM_RETURN (*error_ft)(T_RVM_NAME swe_name, 
               T_RVM_RETURN error_cause,
               T_RVM_ERROR_TYPE error_type,
               T_RVM_STRING error_msg);
  /** Mem bank id. */
  T_RVF_MB_ID     prim_mb_id;
  T_RVF_ADDR_ID   addr_id;

  T_RFS_PAIR_VALUE_MAP pair_value_map[2*RFS_MAX_NR_OPEN_FILES];
  T_RFS_FD fd_slots[RFS_MAX_NR_OPEN_FILES];
  T_RFS_FD last_fd_issued;

  /* media, partition and mountpoint information */
  T_GBI_MEDIA_INFO       *media_info_p;
  UINT8                  nmb_of_media;
  UINT8                  nmb_of_fs; 
  T_FSCORE_PARTITION_TABLE      fscore_part_table[RFS_NMB_FSCORE];

  /* Administration regarding statistics and directory handling */
  UINT8                  open_file_cnt;
  UINT8                  reserved_file_cnt;
  UINT8                  open_dir_cnt;

  T_RFS_RET pair_id;		//for msg req-resp pairing: cnt
  T_RVF_MUTEX mutex_pair_id;	//for msg req-resp pairing
  

} T_RFS_ENV_CTRL_BLK;


/** External ref "global variables" structure. */
extern T_RFS_ENV_CTRL_BLK *rfs_env_ctrl_blk_p;


/* Commented: probably not used in the future */
/*

extern void rfs_return_queue_init (T_RVF_G_ADDR_ID  rq_addr_id,
                                   UINT16           rq_event,
                                   T_RV_RETURN_PATH *path_to_return_queue_p);
extern void *rfs_return_queue_get_msg (void); 
*/

#endif /* __RFS_INST_I_H_ */

/**
 * @file	relfs_env.h
 *
 * Internal definitions for reliance .
 *
 * @author	
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	               			
 *
 * 
 */

#ifndef __REL_ENV_H__
#define __REL_ENV_H__




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "vsi.h"
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */
#include "rv/rv_general.h"		/* General Riviera definitions. */
#include "rvf/rvf_api.h"


#define FFXDRV_INC_LOCOSTO_DATALIGHT_PLUGIN
#include "ffxdrv.h"


#define pei_create ffs_pei_create

#if RELFS_TRACE_ENABLE

#define RELFS_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )

#else

#define RELFS_TRACE_DEBUG_HIGH(string)

#endif

#define MAX_ASCII_ARRAY_SIZE 260

/**
 * The Control Block buffer of reliance, which gathers all 'Global variables'
 * used by reliance instance.
 *
 * A structure should gathers all the 'global variables' of reliance instance.
 * Indeed, global variable must not be defined in order to avoid using static memory.
 * A T_RIL_ENV_CTRL_BLK buffer is allocated when creating reliance instance
 * and is
 * then always refered by reliance instance when access to 'global variable' 
 * is necessary.
 */
typedef struct
{
  BOOL initialised;

  T_RVF_MB_ID	mbid;
  T_RVF_ADDR_ID   addr_id;

  /* store task ID*/
  T_HANDLE task_id;

  long pair_id;  //for msg req-resp pairing: cnt
  UINT8  fscore_nmb;    /* File system Number issued by RFS */

  T_HANDLE mutex_pair_id;  //for msg req-resp pairing

  T_RV_RETURN rfs_stream_return_path;
  T_RV_RETURN rfs_file_return_path;

  char temp_buf1[MAX_ASCII_ARRAY_SIZE+1];
  char temp_buf2[MAX_ASCII_ARRAY_SIZE+1];
  DISK_PARTITION_PARAMS rel_sDiskPartitionParams;
  DISK_MEDIA_PARAMS     rel_sDiskMediaParams;
  	
  UINT16 id;

  BOOL Garbage_Collection_Process;
 
}T_RIL_ENV_CTRL_BLK;


/** External ref "global variables" structure. */
extern T_RIL_ENV_CTRL_BLK *ril_env_ctrl_blk_p;



void relfs_rfs_stream_clbk_func(void * result_p);
void relfs_rfs_file_clbk_func(void * result_p);


#endif /* __REL_ENV_H__ */


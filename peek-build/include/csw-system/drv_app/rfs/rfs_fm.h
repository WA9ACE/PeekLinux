/**
 * @file   rfs_fm.h
 *
 * Header file containing definitions of rfs_fm.c
 *
 * @author  
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  4/27/2004  ()   Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#include "gbi/gbi_api.h"    /* Generic rfs types and definitions */
#include "rfs/fscore_types.h"
#include "rfs/rfs_cfg.h"
#include "rfs/rfs_api.h"    /* Generic rfs types and definitions */

#ifndef _RFS_FM_H_
#define _RFS_FM_H_

typedef struct
{
  UINT8 partition_name[GBI_MAX_PARTITION_NAME];
  UINT8 media_nmb;
  UINT8 partition_nmb;
  T_GBI_FS_TYPE filesystem_type;
  UINT8 filesystem_name[GBI_MAX_FS_NAME];
  UINT8 fs_core_nmb;
  BOOL mounted;
} T_RFS_MOUNT_TABLE;

extern T_FSCORE_PARTITION_TABLE *rfs_get_mount_entry_by_pathname (const char *, UINT16* part_index);
extern T_FSCORE_PARTITION_TABLE *rfs_get_mount_entry_by_pathname_uc (const T_WCHAR *, UINT16* part_index);



typedef struct
{
  T_RV_RETURN return_path;
  T_RVF_MUTEX *mutex;
  T_RFS_RET *result;
} T_RFS_RETURN_PATH;

/* flags for rfs_map_pair_value() */
/* ALERT: dont change these values; these should be unique in nature */
#define RFS_PAIR_MAP_TO_CORE 			(1<<0)	/* second pv parameter unused */
#define RFS_PAIR_MAP_NEW 				(1<<1)	/* create new mapping, both pv parameters used */
#define RFS_PAIR_MAP_TO_CLIENT  		(1<<2)	/* second pv parameter unused */
#define RFS_PAIR_MAP_FD 				(1<<3)
#define RFS_PAIR_MAP_PAIR_ID 			(1<<4)
#define RFS_PAIR_MAP_DELETE 			(1<<5)	/* delete if found */
#define RFS_PAIR_MAP_UPDATE_CLIENT_PATH (1<<6)    /* update the client pair value and return path */



/* wrapper for rfs_map_pair_value() to hide ugly casts  -  we need 
 * to be able to use both T_RFS_PAIR_VALUE and T_FSCORE_PAIR_VALUE */

#define rfs_map_pair_value(pv1, pv2, return_path_p, fscore_nmb_p, flags) \
  rfs_map_pair_value_private(*(T_RFS_PAIR_VALUE*)&(pv1), \
					 *(T_RFS_PAIR_VALUE*)&(pv2), \
					 (return_path_p), \
					 (fscore_nmb_p), \
					 (flags))
extern T_RFS_PAIR_VALUE
rfs_map_pair_value_private (T_RFS_PAIR_VALUE, T_RFS_PAIR_VALUE,
							T_RFS_RETURN_PATH *, UINT8 *, UINT32);
extern void rfs_map_pair_value_init (void);
extern T_RFS_FD rfs_new_fd (void);
extern void rfs_fd_list_init (void);
extern T_RFS_RET rfs_delete_fd (T_RFS_FD doomed_fd);
extern T_RFS_FD rfs_reserve_fd (void);
extern void rfs_unreserve_fd (void);


/* CORE DISPATCHER */
/* call the appropriate function pointer for this core */
/*lint -emacro(717, rfs_core_dispatch)*/
#define rfs_core_dispatch(ret_val_p, fs_core_nmb, func_and_params) \
	  do { \
		  T_FSCORE_FUNC_TABLE func_table; \
		  T_FSCORE_RET (*get_func_table[RFS_NMB_FSCORE]) (T_FSCORE_FUNC_TABLE *) = {RFS_FSCORE_FUNC_TABLE}; \
		  (void)get_func_table[(fs_core_nmb)](&func_table); \
		  *(ret_val_p) = func_table.func_and_params ; \
	  } while (0)

#endif /* #ifndef _RFS_FM_H_ */

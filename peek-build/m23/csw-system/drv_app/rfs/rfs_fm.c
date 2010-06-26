/**
 * @file	rfs_fm.c
 *
 * Contains the parts 'Request info and pairing', 'open files Handling',
 * 'Path Resolver' and 'Mount table' of the 'File Management' component.
 * The 'Core Dispatcher' is integrated in this file.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	4/27/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */
#include <string.h>
#include "rv/rv_general.h"
#include "rfs/rfs_i.h"


#undef RFS_PAIR_MAP_VERBOSE

static const T_RFS_RETURN_PATH rfs_default_fd_return_path =
  { {RVF_INVALID_ADDR_ID, NULL}, NULL, NULL };



#if (FFS_UNICODE ==1)


T_FSCORE_PARTITION_TABLE * rfs_get_mount_entry_by_pathname_uc (const T_WCHAR *path_p,UINT16* part_index)
{
  char *mpt_end_ptr;
  UINT32 mpt_len;
  UINT32 i, fs_index;
  char char_path[GBI_MAX_PARTITION_NAME+1];
  char *char_path_p=char_path;

  /* check parameter */
  if (path_p == NULL)
	{
	  RFS_SEND_TRACE ("RFS rfs_get_mount_entry_by_pathname: path == NULL",
					  RV_TRACE_LEVEL_ERROR);
	  return NULL;
	}

  convert_unicode_to_u8_length(path_p,char_path, GBI_MAX_PARTITION_NAME);

  char_path_p=char_path;

  /* find end of mountpoint */
  /* skip leading slashes */
  while (char_path_p[0] == '/')
	{
	  char_path_p++;
	}
  mpt_end_ptr = (char *) strchr (char_path_p, '/');
  mpt_len = mpt_end_ptr ? (UINT32) (mpt_end_ptr - char_path_p) : strlen (char_path_p);


  /* Search in file systems core */
for(fs_index=0;fs_index < rfs_env_ctrl_blk_p->nmb_of_fs; fs_index++)
{

   /* find matching partition table entry */
  for (i = 0; i < rfs_env_ctrl_blk_p->fscore_part_table[fs_index].nbr_partitions; i++)
	{
	  T_FSCORE_PARTITION_INFO *pt_entry_p = rfs_env_ctrl_blk_p->fscore_part_table[fs_index].partition_info_table+i;
	  const char *partition_name = (const char *) pt_entry_p->partition_name;

	  while (partition_name[0] == '/')
		{
		  partition_name++;
		}
	  if (strncmp (char_path_p, partition_name, mpt_len) == 0)
		{
		  *part_index = i;
		  return (&rfs_env_ctrl_blk_p->fscore_part_table[fs_index]);
		}
	}

 }
  return NULL;
}

#else

T_FSCORE_PARTITION_TABLE * rfs_get_mount_entry_by_pathname (const char *path_p,UINT16* part_index)
{
  char *mpt_end_ptr;
  UINT32 mpt_len;
  UINT32 i,fs_index;

  /* check parameter */
  if (path_p == NULL)
	{
	  RFS_SEND_TRACE ("RFS rfs_get_mount_entry_by_pathname: path == NULL",
					  RV_TRACE_LEVEL_ERROR);
	  return NULL;
	}

  /* find end of mountpoint */
  /* skip leading slashes */
  while (path_p[0] == '/')
	{
	  path_p++;
	}
  mpt_end_ptr = (char *) strchr (path_p, '/');
  mpt_len = mpt_end_ptr ? (UINT32) (mpt_end_ptr - path_p) : strlen (path_p);

  /* sanity check */
  if (mpt_len >= GBI_MAX_PARTITION_NAME)
	{
	  RFS_SEND_TRACE ("RFS rfs_get_mount_entry_by_pathname: path too long",
					  RV_TRACE_LEVEL_ERROR);
	  return NULL;
	}

  /* Search in file systems core */
for(fs_index=0;fs_index < rfs_env_ctrl_blk_p->nmb_of_fs; fs_index++)
{

   /* find matching partition table entry */
  for (i = 0; i < rfs_env_ctrl_blk_p->fscore_part_table[fs_index].nbr_partitions; i++)
	{
	  T_FSCORE_PARTITION_INFO *pt_entry_p = rfs_env_ctrl_blk_p->fscore_part_table[fs_index].partition_info_table+i;
	  const char *partition_name = (const char *) pt_entry_p->partition_name;

	  while (partition_name[0] == '/')
		{
		  partition_name++;
		}
	  if (strncmp (char_path_p, partition_name, mpt_len) == 0)
		{
		  *part_index = i;
		  return (&rfs_env_ctrl_blk_p->fscore_part_table[fs_index]);
		}
	}

 }
  return NULL;
}

#endif



void
rfs_map_pair_value_init (void)
{
  UINT32 i;
  for (i = 0; i < sizeof rfs_env_ctrl_blk_p->pair_value_map /
	   sizeof rfs_env_ctrl_blk_p->pair_value_map[0]; i++)
	{
	  T_RFS_PAIR_VALUE_MAP *pvm_p = rfs_env_ctrl_blk_p->pair_value_map + i;
	  pvm_p->is_fd = FALSE;
	  pvm_p->pair_value[0].pair_id = FSCORE_ILLEGAL_PAIR_ID;
	  pvm_p->pair_value[1].pair_id = FSCORE_ILLEGAL_PAIR_ID;
	  pvm_p->fs_core_nmb = FSCORE_ILLEGAL_CORE_NMB;
	  pvm_p->op_mode = RFS_OPMODE_DEF_VALUE;

	  /* Not needed to do, but just nice */
	  pvm_p->client_return_path.return_path.addr_id = RFS_ADDR_ID_DEF_VALUE;
	  pvm_p->client_return_path.return_path.callback_func = RFS_CB_DEF_VALUE;
	}
  
}

#ifdef RFS_PAIR_MAP_VERBOSE
#define trace_pair_value(s, pv) do { \
	if (flags & RFS_PAIR_MAP_FD) { \
	  RFS_SEND_TRACE_PARAM ("pair value " s " fd ", (pv).fd, RV_TRACE_LEVEL_ERROR); \
	} else { \
	  RFS_SEND_TRACE_PARAM ("pair value " s " pid ", (pv).pair_id, RV_TRACE_LEVEL_ERROR); \
	} \
} while (0)
#endif

/*lint -efunc(514, rfs_map_pair_value_private)*/
T_RFS_PAIR_VALUE
rfs_map_pair_value_private (T_RFS_PAIR_VALUE pair_value_in,
							T_RFS_PAIR_VALUE pair_value_in2,
							T_RFS_RETURN_PATH * client_return_path_p,
							UINT8 * fs_core_nmb_p, UINT32 flags)
{
  UINT32 i;
  T_RFS_PAIR_VALUE ret_val;
  BOOL found_entry = FALSE;

  /* initialise return value as 'not found' */
  if (flags & RFS_PAIR_MAP_FD)
	{
	  ret_val.fd = FSCORE_ILLEGAL_FD;
	}
  else
	{
	  ret_val.pair_id = FSCORE_ILLEGAL_PAIR_ID;
	}

#ifdef RFS_PAIR_MAP_VERBOSE
  if (flags & RFS_PAIR_MAP_NEW)
	{
	  trace_pair_value ("new client", pair_value_in);
	  trace_pair_value ("new core", pair_value_in2);
	}
  else
	{
	  if (flags & RFS_PAIR_MAP_TO_CORE)
		{
		  RFS_SEND_TRACE ("pair value map to CORE", RV_TRACE_LEVEL_ERROR);
		}
	  else
		{
		  RFS_SEND_TRACE ("pair value map to CLIENT", RV_TRACE_LEVEL_ERROR);
		}
	  if (flags & RFS_PAIR_MAP_DELETE)
		{
		  trace_pair_value ("delete", pair_value_in);
		}
	  else
		{
		  trace_pair_value ("find", pair_value_in);
		}
	}
#endif

  /* search loop */
  for (i = 0; i < sizeof rfs_env_ctrl_blk_p->pair_value_map /
	   sizeof rfs_env_ctrl_blk_p->pair_value_map[0] && !found_entry; i++)
	{
	  T_RFS_PAIR_VALUE_MAP *pvm_p = rfs_env_ctrl_blk_p->pair_value_map + i;
	  /* IF new entry requested THEN test for unused entry ELSE test existing entry */
	  if (flags & RFS_PAIR_MAP_NEW)
		{
		  /* IF unused entry THEN store caller data and leave */
		  if (!pvm_p->is_fd
			  && pvm_p->pair_value[0].pair_id == FSCORE_ILLEGAL_PAIR_ID
			  && pvm_p->pair_value[1].pair_id == FSCORE_ILLEGAL_PAIR_ID)
			{
			  pvm_p->is_fd = (flags & RFS_PAIR_MAP_FD);
			  pvm_p->pair_value[0] = pair_value_in;
			  pvm_p->pair_value[1] = pair_value_in2;
			  pvm_p->fs_core_nmb = *fs_core_nmb_p;
			  if (client_return_path_p)
				{
				  pvm_p->client_return_path = *client_return_path_p;
				}
			  found_entry = TRUE;
			  pvm_p->op_mode = O_SIO; 
			}
		}
	  else
		{
		  /* looking for existing entry: fd */
		  if ((flags & RFS_PAIR_MAP_FD) && pvm_p->is_fd
			  && pvm_p->pair_value[!(flags & RFS_PAIR_MAP_TO_CORE)].fd ==
			  pair_value_in.fd && ((flags & RFS_PAIR_MAP_TO_CORE)
								   || pvm_p->fs_core_nmb == *fs_core_nmb_p))
			{
			  ret_val.fd =
				pvm_p->pair_value[!!(flags & RFS_PAIR_MAP_TO_CORE)].fd;
			  found_entry = TRUE;
			}
		  /* looking for existing entry: pair id */
		  if (!(flags & RFS_PAIR_MAP_FD) && !pvm_p->is_fd
			  && pvm_p->pair_value[!(flags & RFS_PAIR_MAP_TO_CORE)].
			  pair_id == pair_value_in.pair_id && ((flags &
													RFS_PAIR_MAP_TO_CORE)
												   || pvm_p->fs_core_nmb ==
												   *fs_core_nmb_p))
			{
			  ret_val.pair_id =
				pvm_p->pair_value[!!(flags & RFS_PAIR_MAP_TO_CORE)].pair_id;
			  found_entry = TRUE;
			}
		  if (found_entry)
			{
			  /* IF mapping to core THEN caller needs core number */
			  if (flags & RFS_PAIR_MAP_TO_CORE)
				{
				  *fs_core_nmb_p = pvm_p->fs_core_nmb;
				}


			   if(flags&RFS_PAIR_MAP_UPDATE_CLIENT_PATH)
			   	{
			   	    /* update the client pair value */
					  pvm_p->pair_value[0] = pair_value_in;
			   	}


			  if (client_return_path_p)
				{
				    /* Update the client return path */
				 	 if(flags&RFS_PAIR_MAP_UPDATE_CLIENT_PATH)
			  			{
			  			    pvm_p->client_return_path = *client_return_path_p;
			  			}
					 else
					 	{
					 	 	 /* IF caller is interested in return path THEN store it */
					   *client_return_path_p = pvm_p->client_return_path;
					 	}
				}
			  /* IF caller wants to delete this entry THEN delete it */
			  if (flags & RFS_PAIR_MAP_DELETE)
				{
				  pvm_p->is_fd = FALSE;
				  pvm_p->pair_value[0].pair_id = FSCORE_ILLEGAL_PAIR_ID;
				  pvm_p->pair_value[1].pair_id = FSCORE_ILLEGAL_PAIR_ID;
				  pvm_p->fs_core_nmb = FSCORE_ILLEGAL_CORE_NMB;
				  pvm_p->client_return_path=rfs_default_fd_return_path;
				  pvm_p->op_mode = O_SIO;
				}
			}
		  else
		  	{
		  	      /*Entry not found */
                 if(flags&RFS_PAIR_MAP_TO_CLIENT) 
                 	{
                 	  /* client return path is required, but entry not found */
                 	 *client_return_path_p =rfs_default_fd_return_path ;   
					 			/* fill the defatult client return path; NULL*/
                 	}
				  
		  	}
		}
	}							/* end search loop FOR i */

  if (found_entry == FALSE)
	{
	  RFS_SEND_TRACE ("RFS File manager: no pair entry found",
					  RV_TRACE_LEVEL_ERROR);
	}
#ifdef RFS_PAIR_MAP_VERBOSE
  trace_pair_value ("returning", ret_val);
#endif
  return ret_val;
}

void
rfs_fd_list_init (void)
{
  UINT32 i;
  for (i = 0; i < RFS_MAX_NR_OPEN_FILES; i++)
	{
	  rfs_env_ctrl_blk_p->fd_slots[i] = RFS_ENOSPACE;
	}
  rfs_env_ctrl_blk_p->last_fd_issued = 666;
}

void
rfs_unreserve_fd (void)
{
  if (rfs_env_ctrl_blk_p->reserved_file_cnt == 0)
	{
	  RFS_SEND_TRACE ("rfs_unreserve_fd: nothing to unreserve",
					  RV_TRACE_LEVEL_ERROR);
	}
  else
	{
	  rfs_env_ctrl_blk_p->reserved_file_cnt--;
	}
}

T_RFS_FD
rfs_reserve_fd (void)
{
  if (rfs_env_ctrl_blk_p->open_file_cnt +
	  rfs_env_ctrl_blk_p->reserved_file_cnt < RFS_MAX_NR_OPEN_FILES)
	{
	  rfs_env_ctrl_blk_p->reserved_file_cnt++;
	  return RFS_EOK;
	}
  return RFS_ENUMFD;
}

T_RFS_FD
rfs_new_fd (void)
{
  UINT32 i;
  UINT32 free_entry = RFS_MAX_NR_OPEN_FILES;	/* mark as invalid */
  BOOL reject;

  do
	{
	  /* compensate for wrap around */
	  if (++rfs_env_ctrl_blk_p->last_fd_issued <= 0)
		{
		  rfs_env_ctrl_blk_p->last_fd_issued = 1;
		}
	  reject = FALSE;
	  for (i = 0; i < RFS_MAX_NR_OPEN_FILES; i++)
		{
		  if (rfs_env_ctrl_blk_p->fd_slots[i] < 0)
			{
			  free_entry = i;
			}
		  if (rfs_env_ctrl_blk_p->fd_slots[i] ==
			  rfs_env_ctrl_blk_p->last_fd_issued)
			{
			  reject = TRUE;
			  break;
			}
		}
	}
  while (reject);
  if (free_entry == RFS_MAX_NR_OPEN_FILES)
	{
	  return RFS_ENOSPACE;
	}
  rfs_env_ctrl_blk_p->fd_slots[free_entry] =
	rfs_env_ctrl_blk_p->last_fd_issued;
  rfs_env_ctrl_blk_p->open_file_cnt++;
  rfs_env_ctrl_blk_p->reserved_file_cnt--;
  RFS_SEND_TRACE_PARAM ("rfs_new_fd: # fds",
						rfs_env_ctrl_blk_p->open_file_cnt,
						RV_TRACE_LEVEL_DEBUG_HIGH);
  return rfs_env_ctrl_blk_p->last_fd_issued;
}

T_RFS_RET
rfs_delete_fd (T_RFS_FD doomed_fd)
{
  UINT32 i;

  for (i = 0; i < RFS_MAX_NR_OPEN_FILES; i++)
	{
	  if (rfs_env_ctrl_blk_p->fd_slots[i] == doomed_fd)
		{
		  rfs_env_ctrl_blk_p->fd_slots[i] = RFS_ENOSPACE;
		  rfs_env_ctrl_blk_p->open_file_cnt--;
		  RFS_SEND_TRACE_PARAM ("rfs_delete_fd: # fds",
								rfs_env_ctrl_blk_p->open_file_cnt,
								RV_TRACE_LEVEL_DEBUG_HIGH);
		  return RFS_EOK;
		}
	}
  RFS_SEND_TRACE_PARAM ("rfs_delete_fd: cannot find fd",
						doomed_fd, RV_TRACE_LEVEL_ERROR);
  return RFS_EBADFD;
}


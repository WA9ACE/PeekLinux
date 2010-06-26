/**
 * @file	rfsfat_api.c
 *
 * API for RFSFAT SWE.
 *
 * @author	Anton van Breemen (abreemen@ict.nl)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/29/2004	Anton van Breemen (abreemen@ict.nl)		Create.
 *
 * (C) Copyright 2004 by ICT Embedded, All Rights Reserved
 */

#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "gbi/gbi_api.h"
#include "rfs/rfs_api.h"
#include "rfs/fscore_types.h"

#include "rfsfat/rfsfat_cfg.h"
#include "rfsfat/rfsfat_i.h"
#include "rfsfat/rfsfat_api.h"
#include "rfsfat/rfsfat_message.h"


#define INVALID_RFSFAT_ATTRIB(attrib)  ((attrib==FFS_ATTRIB_SUBDIR) ? TRUE: FALSE)

#define VALID_ATTR_SET (FFS_ATTRIB_READWRITE|FFS_ATTRIB_READONLY|FFS_ATTRIB_HIDDEN|FFS_ATTRIB_SYSTEM|FFS_ATTRIB_ARCHIVE)

#define SET_VALID_ATTR(attrib)	((T_FFS_ATTRIB_TYPE) (attrib&VALID_ATTR_SET))

//Forward declarations
static T_FSCORE_RET rfsfat_search_fd_table_entry (T_FSCORE_FD fd,
												  UINT8 * entry);
static T_FSCORE_RET rfsfat_get_free_pair_id (void);
static T_FSCORE_RET rfsfat_mount_unmount_nb(int op,T_RV_RETURN_PATH return_path);

static UINT8 a_str[300]={0,0};
extern void print_unicode_string(T_WCHAR *uni_str);



/**
 * function:  rfsfat_set_partition_table
 */
T_FSCORE_RET
rfsfat_set_partition_table (T_FSCORE_PARTITION_TABLE * partition_table,
							T_RV_RETURN return_path)
{
  T_FSCORE_SET_PARTITION_TABLE_REQ_MSG *msg_p;

  RFSFAT_SEND_TRACE ("rfsfat_set_partition_table", RV_TRACE_LEVEL_WARNING);

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_FSCORE_SET_PARTITION_TABLE_REQ_MSG),
					   RFSFAT_SET_PARTITION_TABLE_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_set_partition_table: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_SET_PARTITION_TABLE_REQ_MSG;
  msg_p->partition_table = partition_table;
  msg_p->return_path = return_path;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return FSCORE_EOK;
}

/**
 * function:  rfsfat_get_func_table
 */
T_FSCORE_RET
rfsfat_get_func_table (T_FSCORE_FUNC_TABLE * func_table_p)
{

  func_table_p->fscore_close = rfsfat_close;
  func_table_p->fscore_fchmod = rfsfat_fchmod;
  func_table_p->fscore_fcntl = rfsfat_fcntl;
  func_table_p->fscore_write = rfsfat_write;
  func_table_p->fscore_trunc = rfsfat_trunc;
  func_table_p->fscore_fstat = rfsfat_fstat;
  func_table_p->fscore_fsync = rfsfat_fsync;
  func_table_p->fscore_lseek = rfsfat_lseek;
  func_table_p->fscore_set_partition_table = rfsfat_set_partition_table;
  func_table_p->fscore_read = rfsfat_read;
  func_table_p->fscore_mount_nb = rfsfat_mount_nb;
  func_table_p->fscore_unmount_nb = rfsfat_unmount_nb;

/* Unicode functions */
  func_table_p->fscore_format_uc_nb = rfsfat_format_uc_nb;
  func_table_p->fscore_mkdir_uc_nb = rfsfat_mkdir_uc_nb;
  func_table_p->fscore_opendir_uc_nb = rfsfat_opendir_uc_nb;
  func_table_p->fscore_open_uc_nb = rfsfat_open_uc_nb;
  func_table_p->fscore_preformat_uc_nb = rfsfat_preformat_uc_nb;
  func_table_p->fscore_readdir_uc_nb = rfsfat_readdir_uc_nb;
  func_table_p->fscore_remove_uc_nb = rfsfat_remove_uc_nb;
  func_table_p->fscore_rename_uc_nb = rfsfat_rename_uc_nb;
  func_table_p->fscore_rmdir_uc_nb = rfsfat_rmdir_uc_nb;
  func_table_p->fscore_stat_uc_nb = rfsfat_stat_uc_nb;
  func_table_p->fscore_chmod_uc_nb = rfsfat_chmod_uc_nb;
  func_table_p->fscore_getattrib_uc_nb = rfsfat_getattribute_uc_nb;
  func_table_p->fscore_setattrib_uc_nb = rfsfat_setattribute_uc_nb;
  func_table_p->fscore_getdatetime_uc_nb = rfsfat_getdatetime_uc_nb;

/* Ascii functions */
  func_table_p->fscore_format_nb = rfsfat_format_nb;
  func_table_p->fscore_mkdir_nb = rfsfat_mkdir_nb;
  func_table_p->fscore_opendir_nb = rfsfat_opendir_nb;
  func_table_p->fscore_open_nb = rfsfat_open_nb;
  func_table_p->fscore_preformat_nb = rfsfat_preformat_nb;
  func_table_p->fscore_readdir_nb = rfsfat_readdir_nb;
  func_table_p->fscore_remove_nb = rfsfat_remove_nb;
  func_table_p->fscore_rename = rfsfat_rename_nb;
  func_table_p->fscore_rmdir_nb = rfsfat_rmdir_nb;
  func_table_p->fscore_stat_nb = rfsfat_stat_nb;
  func_table_p->fscore_chmod_nb = rfsfat_chmod_nb;
  func_table_p->fscore_getattrib_nb = rfsfat_getattribute_nb;
  func_table_p->fscore_setattrib_nb = rfsfat_setattribute_nb;
  func_table_p->fscore_getdatetime_nb = rfsfat_getdatetime_nb;
  func_table_p->fscore_closedir_uc_nb = rfsfat_closedir_nb;
  func_table_p->fscore_set_label_uc_nb = rfsfat_set_label_nb;
  func_table_p->fscore_get_label_uc_nb = rfsfat_get_label_nb;
  return FSCORE_EOK;
}

/**
 * function:  fsfat_open_nb
 */
T_FSCORE_RET
rfsfat_open_nb (const char *pathname,
				T_FSCORE_FLAGS flags,
				T_FSCORE_MODE mode, T_RV_RETURN return_path)
{
  T_RFSFAT_OPEN_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("fsfat_open_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

 if ((return_path.callback_func == NULL) && (return_path.addr_id == 0))
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_OPEN_REQ_MSG),
					   RFSFAT_OPEN_REQ_MSG, (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT fsfat_open_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_OPEN_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->flags = flags;
  msg_p->mode = mode;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}


 /**
 *function:  rfsfat_read
 */
T_FSCORE_SIZE
rfsfat_read (T_FSCORE_FD fd, void *buf, T_FSCORE_SIZE size)
{

  T_RV_RETURN return_path;
  T_RFSFAT_READ_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;
  T_FSCORE_RET rfsfat_retval = FSCORE_EOK;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_read", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EACCES;
	}

  /* Search entry in file descriptor table */
  rfsfat_retval = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (rfsfat_retval < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) rfsfat_retval;
	}

  op_mode =	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].op_mode;
  if (op_mode == O_SIO)
	{
	  /* Synchronous mode, NOT SUPPORTED  */
	  RFSFAT_SEND_TRACE ("RFSFAT API: Synchronious mode not supported",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EBADOP;
	}

  /* Asynchronous mode, check return path */
  return_path =rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].ret_path;
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EBADOP;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_READ_REQ_MSG),
					   RFSFAT_READ_REQ_MSG, (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_READ_REQ_MSG;
  msg_p->fd = fd;
  msg_p->buf = buf;
  msg_p->size = size;
  msg_p->return_path = return_path;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return (T_FSCORE_SIZE) FSCORE_EMSGSEND;
	}
  return (T_FSCORE_SIZE) 0;		// Can not give size because I'm asynchronious
}

/**
 * function:  rfsfat_read_nb
 */
T_FSCORE_SIZE
rfsfat_read_nb (T_FSCORE_FD fd, void *buf, T_FSCORE_SIZE size,  T_RV_RETURN_PATH return_path)
{
  T_FSCORE_RET rfsfat_retval = FSCORE_EOK;
  T_RFSFAT_READ_REQ_MSG *msg_p;
  UINT8 fd_table_entry;
  T_FSCORE_SIZE  pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_read_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EACCES;
	}

  /* check the return path */
   if (return_path.callback_func == NULL && return_path.addr_id== 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* Search entry in file descriptor table */
  rfsfat_retval = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (rfsfat_retval < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) rfsfat_retval;
	}

  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_READ_REQ_MSG),
					   RFSFAT_READ_REQ_MSG, (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EMEMORY;

	}

  /* compose message */
  msg_p->fd = fd;
  msg_p->buf = buf;
  msg_p->size = size;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return (T_FSCORE_SIZE) FSCORE_EMSGSEND;
	}
  return (T_FSCORE_SIZE) 0;		// Can not give size because I'm asynchronious
}
/* * function:  rfsfat_fcntl
 */
T_FSCORE_RET
rfsfat_fcntl (T_FSCORE_FD fd, INT8 cmd, void *arg_p)
{
  UINT8 op_mode;
  T_RV_RETURN *ret_path_p;
  T_FSCORE_RET ret_val = FSCORE_EOK;
  UINT8 fd_table_entry;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_fcntl", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return ret_val;
	}

  //evaluate command
  if (cmd == F_SETFLAG)
	{
	  op_mode = *(UINT8 *) arg_p;

	  if (op_mode != O_AIO)
		{
		  RFSFAT_SEND_TRACE_PARAM ("RFSFAT API: Warning: mode not async",
								   op_mode, RV_TRACE_LEVEL_DEBUG_LOW);
		  return FSCORE_EINVALID;
		}

	  /* set op_mode in filedescriptor table */
	  rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].op_mode =
		op_mode;
	}
  else if (cmd == F_SETRETPATH)
	{
	  /* Check return_path */
	  ret_path_p = (T_RV_RETURN *) arg_p;
	  if (ret_path_p == NULL)
		return FSCORE_EINVALID;
	  /* set return_path in first free FD_TABLE entry */
	  rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].ret_path =
		*ret_path_p;
	}
  else if (cmd == F_GETFLAG)
	{
	  ret_val =
		(T_FSCORE_RET) rfsfat_env_ctrl_blk_p->ffs_globals.
		fd_table[fd_table_entry].op_mode;
	}
  else
	{
	  /*  invalid command */
	  ret_val = FSCORE_EINVALID;
	}
  return ret_val;
}

/**
 * function:  rfsfat_write
 */
T_FSCORE_SIZE
rfsfat_write (T_FSCORE_FD fd, const void *buf, T_FSCORE_SIZE size)
{
  T_FSCORE_RET ret_val = FSCORE_EOK;
  T_RV_RETURN return_path;
  T_RFSFAT_WRITE_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_write", RV_TRACE_LEVEL_WARNING);


  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EACCES;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) ret_val;
	}


  op_mode =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].op_mode;
  if (op_mode == O_SIO)
	{
	  /* Synchronous mode, NOT SUPPORTED  */
	  RFSFAT_SEND_TRACE ("RFSFAT API: Synchronious mode not supported",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EBADOP;
	}

  /* Asynchronous mode, check return path */
  return_path =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].ret_path;
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EBADOP;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_WRITE_REQ_MSG),
					   RFSFAT_WRITE_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_WRITE_REQ_MSG;
  msg_p->fd = fd;
  msg_p->buf = buf;
  msg_p->size = size;
  msg_p->return_path = return_path;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return (T_FSCORE_SIZE) FSCORE_EMSGSEND;
	}
  return (T_FSCORE_SIZE) 0;		// Can not give size because I'm asynchronious
}

/**
 * function:  rfsfat_write_nb
 */
T_FSCORE_SIZE
rfsfat_write_nb (T_FSCORE_FD fd, const void *buf, T_FSCORE_SIZE size,  T_RV_RETURN_PATH return_path)
{
  T_FSCORE_RET ret_val = FSCORE_EOK;
  T_RFSFAT_WRITE_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;
  T_FSCORE_SIZE  pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_write_nb", RV_TRACE_LEVEL_WARNING);


  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EACCES;
	}

   if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) ret_val;
	}

  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_WRITE_REQ_MSG),
					   RFSFAT_WRITE_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_SIZE) FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->fd = fd;
  msg_p->buf = buf;
  msg_p->size = size;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return (T_FSCORE_SIZE) FSCORE_EMSGSEND;
	}
  return (T_FSCORE_SIZE) 0;		// Can not give size because I'm asynchronious
}
/**
 * function:  rfsfat_close
 */
T_FSCORE_RET
rfsfat_close (T_FSCORE_FD fd)
{
  T_FSCORE_RET rfsfat_retval = FSCORE_EOK;
  T_RV_RETURN return_path;
  T_RFSFAT_CLOSE_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_close", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  /* Search entry in file descriptor table */
  rfsfat_retval = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (rfsfat_retval < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return rfsfat_retval;
	}

  op_mode =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].op_mode;
  if (op_mode == O_SIO)
	{
	  /* Synchronous mode, NOT SUPPORTED  */
	  RFSFAT_SEND_TRACE ("RFSFAT API: Synchronious mode not supported",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EBADOP;
	}

  /* Asynchronous mode, check return path */
  return_path =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].ret_path;
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EBADOP;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_CLOSE_REQ_MSG),
					   RFSFAT_CLOSE_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_CLOSE_REQ_MSG;
  msg_p->fd = fd;
  msg_p->return_path = return_path;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return FSCORE_EOK;
}


/**
 * function:  rfsfat_close_nb
 */
T_FSCORE_RET
rfsfat_close_nb (T_FSCORE_FD fd,T_RV_RETURN_PATH return_path)
{
  T_FSCORE_RET rfsfat_retval = FSCORE_EOK;
  T_RFSFAT_CLOSE_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;
  T_FSCORE_RET pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_close_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

   if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* Search entry in file descriptor table */
  rfsfat_retval = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (rfsfat_retval < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return rfsfat_retval;
	}

  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_CLOSE_REQ_MSG),
					   RFSFAT_CLOSE_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->fd = fd;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
return FSCORE_EOK;
	}


/**
 * function:  fsfat_remove_nb
 */
T_FSCORE_RET
rfsfat_remove_nb (const char *pathname, T_RV_RETURN return_path)
{
  T_RFSFAT_REMOVE_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("fsfat_remove_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

   if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_REMOVE_REQ_MSG),
					   RFSFAT_REMOVE_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT fsfat_remove_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_REMOVE_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_lseek
 */
T_FSCORE_OFFSET
rfsfat_lseek (T_FSCORE_FD fd, T_FSCORE_OFFSET offset, T_FSCORE_WHENCE whence)
{
  T_FSCORE_RET ret_val = FSCORE_EOK;
  T_RV_RETURN return_path;
  T_RFSFAT_LSEEK_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_lseek", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_OFFSET) FSCORE_EACCES;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_OFFSET) ret_val;
	}

  op_mode =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].op_mode;
  if (op_mode == O_SIO)
	{
	  /* Synchronous mode, NOT SUPPORTED  */
	  RFSFAT_SEND_TRACE ("RFSFAT API: Synchronious mode not supported",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_OFFSET) FSCORE_EBADOP;
	}

  /* Asynchronous mode, check return path */
  return_path =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].ret_path;
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_OFFSET) FSCORE_EBADOP;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_LSEEK_REQ_MSG),
					   RFSFAT_LSEEK_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_OFFSET) FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_LSEEK_REQ_MSG;
  msg_p->fd = fd;
  msg_p->offset = offset;
  msg_p->whence = whence;
  msg_p->return_path = return_path;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return (T_FSCORE_OFFSET) FSCORE_EMSGSEND;
	}
  return (T_FSCORE_OFFSET) 0;	// Can not give offset because I'm asynchronious
}

/**
 * function:  rfsfat_lseek_nb
 */
T_FSCORE_OFFSET
rfsfat_lseek_nb (T_FSCORE_FD fd, T_FSCORE_OFFSET offset, T_FSCORE_WHENCE whence,  T_RV_RETURN_PATH return_path)
{
  T_FSCORE_RET ret_val = FSCORE_EOK;
  T_RFSFAT_LSEEK_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;
  INT32	pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_lseek_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_OFFSET) FSCORE_EACCES;
	}

   if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_OFFSET) ret_val;
	}

  /* reserve message buffer */

  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_LSEEK_REQ_MSG),
					   RFSFAT_LSEEK_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return (T_FSCORE_OFFSET) FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->fd = fd;
  msg_p->offset = offset;
  msg_p->whence = whence;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id= pair_id;


  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return (T_FSCORE_OFFSET) FSCORE_EMSGSEND;
	}
  return (T_FSCORE_OFFSET) 0;	// Can not give offset because I'm asynchronious
}
/**
 * function:  rfsfat_fchmod
 */
T_FSCORE_RET
rfsfat_fchmod (T_FSCORE_FD fd, T_FSCORE_MODE mode)
{
  T_FSCORE_RET ret_val = FSCORE_EOK;
  T_RV_RETURN return_path;
  T_RFSFAT_FCHMOD_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_fchmod", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return ret_val;
	}

  op_mode =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].op_mode;
  if (op_mode == O_SIO)
	{
	  /* Synchronous mode, NOT SUPPORTED  */
	  RFSFAT_SEND_TRACE ("RFSFAT API: Synchronious mode not supported",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EBADOP;
	}

  /* Asynchronous mode, check return path */
  return_path =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].ret_path;
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EBADOP;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_FCHMOD_REQ_MSG),
					   RFSFAT_FCHMOD_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_FCHMOD_REQ_MSG;
  msg_p->fd = fd;
  msg_p->mode = mode;
  msg_p->return_path = return_path;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return FSCORE_EOK;
}


/**
 * function:  rfsfat_fstat
 */
T_FSCORE_RET
rfsfat_fstat (T_FSCORE_FD fd, T_FSCORE_STAT * stat)
{
  T_FSCORE_RET ret_val = FSCORE_EOK;
  T_RV_RETURN return_path;
  T_RFSFAT_FSTAT_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_fstat", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return ret_val;
	}

  op_mode =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].op_mode;
  if (op_mode == O_SIO)
	{
	  /* Synchronous mode, NOT SUPPORTED  */
	  RFSFAT_SEND_TRACE ("RFSFAT API: Synchronious mode not supported",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EBADOP;
	}

  /* Asynchronous mode, check return path */
  return_path =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].ret_path;
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EBADOP;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_FSTAT_REQ_MSG),
					   RFSFAT_FSTAT_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_FSTAT_REQ_MSG;
  msg_p->fd = fd;
  msg_p->stat = stat;
  msg_p->return_path = return_path;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return FSCORE_EOK;
}
/**
 * function:  rfsfat_fstat_nb
 */
T_FSCORE_RET
rfsfat_fstat_nb (T_FSCORE_FD fd, T_FSCORE_STAT * stat, T_RV_RETURN_PATH return_path)
{
  T_FSCORE_RET ret_val = FSCORE_EOK;
  T_RFSFAT_FSTAT_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;
  INT32	pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_fstat", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id== 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return ret_val;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_FSTAT_REQ_MSG),
					   RFSFAT_FSTAT_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->fd = fd;
  msg_p->stat = stat;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return FSCORE_EOK;
}


/**
 * function:  rfsfat_fsync
 */
T_FSCORE_RET
rfsfat_fsync (T_FSCORE_FD fd)
{
  T_FSCORE_RET ret_val = FSCORE_EOK;
  T_RV_RETURN return_path;
  T_RFSFAT_FSYNC_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_fsync", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return ret_val;
	}

  op_mode =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].op_mode;
  if (op_mode == O_SIO)
	{
	  /* Synchronous mode, NOT SUPPORTED  */
	  RFSFAT_SEND_TRACE ("RFSFAT API: Synchronious mode not supported",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EBADOP;
	}

  /* Asynchronous mode, check return path */
  return_path =
	rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].ret_path;
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EBADOP;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_FSYNC_REQ_MSG),
					   RFSFAT_FSYNC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_FSYNC_REQ_MSG;
  msg_p->fd = fd;
  msg_p->return_path = return_path;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return FSCORE_EOK;
}


/**
 * function:  rfsfat_fsync_nb
 */
T_FSCORE_RET
rfsfat_fsync_nb (T_FSCORE_FD fd,   T_RV_RETURN_PATH return_path)
{
  T_FSCORE_RET ret_val = FSCORE_EOK;
  T_RFSFAT_FSYNC_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_fsync", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EBADOP;
	}

  /* Search entry in file descriptor table */
  ret_val = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (ret_val < FSCORE_EOK)
	{
	  RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",
						 RV_TRACE_LEVEL_WARNING);
	  return ret_val;
	}


  /* reserve message buffer */
   if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_OPEN_REQ_MSG),
					   RFSFAT_OPEN_REQ_MSG, (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT API: out of memory", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->fd = fd;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}
/**
 * function:  rfsfat_chmod_nb
 */
T_FSCORE_RET
rfsfat_chmod_nb (const char *pathname,
				 T_FSCORE_MODE mode, T_RV_RETURN return_path)
{
  T_RFSFAT_CHMOD_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_chmod_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_CHMOD_REQ_MSG),
					   RFSFAT_CHMOD_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_chmod_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_CHMOD_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->mode = mode;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_stat_nb
 */
T_FSCORE_RET
rfsfat_stat_nb (const char *pathname,
				T_FSCORE_STAT * stat, T_RV_RETURN return_path)
{
  T_RFSFAT_STAT_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_stat_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_STAT_REQ_MSG),
					   RFSFAT_STAT_REQ_MSG, (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_stat_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_STAT_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->stat = stat;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_rename_nb
 */
T_FSCORE_RET
rfsfat_rename_nb (const char *oldname,
				  const char *newname, T_RV_RETURN return_path)
{
  T_RFSFAT_RENAME_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_rename_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_RENAME_REQ_MSG),
					   RFSFAT_RENAME_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_rename_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_RENAME_REQ_MSG;
  msg_p->oldname = oldname;
  msg_p->newname = newname;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_mkdir_nb
 */
T_FSCORE_RET
rfsfat_mkdir_nb (const char *pathname,
				 T_FSCORE_MODE mode, T_RV_RETURN return_path)
{
  T_RFSFAT_MKDIR_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_mkdir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_MKDIR_REQ_MSG),
					   RFSFAT_MKDIR_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_mkdir_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_MKDIR_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->mode = mode;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_rmdir_nb
 */
T_FSCORE_RET
rfsfat_rmdir_nb (const char *pathname, T_RV_RETURN return_path)
{
  T_RFSFAT_RMDIR_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_rmdir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_RMDIR_REQ_MSG),
					   RFSFAT_RMDIR_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_rmdir_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_RMDIR_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_opendir_nb
 */
T_FSCORE_RET
rfsfat_opendir_nb (const char *pathname,
				   T_FSCORE_DIR * dirp, T_RV_RETURN return_path)
{
  T_RFSFAT_OPENDIR_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_opendir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_OPENDIR_REQ_MSG),
					   RFSFAT_OPENDIR_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_opendir_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_OPENDIR_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->dirp = dirp;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_readdir_nb
 */
T_FSCORE_RET
rfsfat_readdir_nb (T_FSCORE_DIR * dirp,
				   char *buf, T_FSCORE_SIZE size, T_RV_RETURN return_path)
{
  T_RFSFAT_READDIR_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_readdir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_READDIR_REQ_MSG),
					   RFSFAT_READDIR_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_readdir_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_READDIR_REQ_MSG;
  msg_p->dirp = dirp;
  msg_p->buf = buf;
  msg_p->size = size;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();

  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}



/**
 * function:  rfsfat_closedir_nb
 */
T_FSCORE_RET rfsfat_closedir_nb (T_FSCORE_DIR * dirp,T_RV_RETURN return_path)
{
T_FSCORE_PAIR_VALUE pair_value;

  RFSFAT_SEND_TRACE ("rfsfat_closedir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  pair_value.pair_id= rfsfat_get_free_pair_id ();


  pair_value.fd = rfsfat_send_response_msg( FSCORE_CLOSEDIR_RSP,
                            pair_value,
                            &(return_path),
                            FSCORE_EOK,
                            FSCORE_EOK);

  if(pair_value.fd == FSCORE_EOK)
  	{
  		return  pair_value.pair_id;
  	}

  return FSCORE_ENOTSUPPORTED;

}




/**
 * function:  rfsfat_preformat_nb
 */
T_FSCORE_RET
rfsfat_preformat_nb (const char *pathname,
					 UINT16 magic, T_RV_RETURN return_path)
{
  T_RFSFAT_PREFORMAT_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_preformat_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if ((rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED) &&
	  (rfsfat_env_ctrl_blk_p->state != RFSFAT_IDLE))
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_PREFORMAT_REQ_MSG),
					   RFSFAT_PREFORMAT_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_preformat_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_PREFORMAT_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->magic = magic;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_format_nb
 */
T_FSCORE_RET
rfsfat_format_nb (const char *pathname,
				  const char *name, UINT16 magic, T_RV_RETURN return_path)
{
  T_RFSFAT_FORMAT_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_format_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if ((rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED) &&
	  (rfsfat_env_ctrl_blk_p->state != RFSFAT_IDLE))
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}


  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_FORMAT_REQ_MSG),
					   RFSFAT_FORMAT_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_format_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_FORMAT_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->name = name;
  msg_p->magic = magic;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_trunc
 */
T_FSCORE_RET
rfsfat_trunc (T_FSCORE_FD fd, T_FSCORE_SIZE size)
{
  T_FSCORE_RET rfsfat_retval = FSCORE_EOK;
  T_RV_RETURN_PATH return_path;
  T_RFSFAT_TRUNC_REQ_MSG *msg_p;
  UINT8 op_mode;
  UINT8 fd_table_entry;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_trunc", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  /* Search entry in file descriptor table */
  rfsfat_retval = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (rfsfat_retval < FSCORE_EOK)
 {
	RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",  RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) rfsfat_retval;
 }

  op_mode = rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].op_mode;
  if (op_mode == O_SIO)
 {
	/* Synchronous mode, NOT SUPPORTED  */
	RFSFAT_SEND_TRACE ("RFSFAT API: Synchronious mode not supported", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* Asynchronous mode, check return path */
  return_path =rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[fd_table_entry].ret_path;
  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_TRUNC_REQ_MSG),
					   RFSFAT_TRUNC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_trunc: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->fd = fd;
  msg_p->size = size;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * function:  rfsfat_trunc_nb
 */
T_FSCORE_RET
rfsfat_trunc_nb (T_FSCORE_FD fd, T_FSCORE_SIZE size,  T_RV_RETURN_PATH return_path)
{
  T_FSCORE_RET rfsfat_retval = FSCORE_EOK;
  T_RFSFAT_TRUNC_REQ_MSG *msg_p;
  UINT8 fd_table_entry;
  INT32	pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_trunc_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }
  /* Search entry in file descriptor table */
  rfsfat_retval = rfsfat_search_fd_table_entry (fd, &fd_table_entry);
  if (rfsfat_retval < FSCORE_EOK)
 {
	RFSFAT_SEND_TRACE ("RFSFAT API: file descriptor error",  RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) rfsfat_retval;
 }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_TRUNC_REQ_MSG),
					   RFSFAT_TRUNC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_trunc_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}
  /* compose message */
  msg_p->fd = fd;
  msg_p->size = size;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;

/* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}


/**
 * function:  rfsfat_truncate_nb
 */
T_FSCORE_RET
rfsfat_truncate_nb (const char* pathname, T_FSCORE_SIZE size,  T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_TRUNC_NAME_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_truncate_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_TRUNC_NAME_REQ_MSG),
					   RFSFAT_TRUNC_NAME_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_truncate_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->path_name = pathname;
  msg_p->size = size;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}



T_FSCORE_RET rfsfat_file_read_nb(const char *pathname,void *buf,T_FSCORE_SIZE size,T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_FILE_READ_REQ_MSG *msg_p;
  INT32	pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_file_read_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_FILE_READ_REQ_MSG),
					   RFSFAT_FILE_READ_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_file_read_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->path_name 		= pathname;
  msg_p->buf			= (char*)buf;
  msg_p->size 			= size;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

T_FSCORE_RET rfsfat_file_write_nb(const char *pathname,void *buf,T_FSCORE_SIZE size,T_FSCORE_FLAGS openflags,T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_FILE_WRITE_REQ_MSG *msg_p;
  INT32	pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_file_write_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_FILE_WRITE_REQ_MSG),
					   RFSFAT_FILE_WRITE_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_file write nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->path_name 		= pathname;
  msg_p->buf			= (char*)buf;
  msg_p->size 			= size;
  msg_p->flags			= openflags;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}


T_FSCORE_RET rfsfat_set_label_nb(const T_WCHAR *pathname, const T_WCHAR *label, T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_SET_VOL_LABEL_REQ_MSG *msg_p;
  INT32	pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_set_vol_label", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_SET_VOL_LABEL_REQ_MSG),
					   RFSFAT_SET_VOL_LABEL_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_file_read_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->path_name = pathname;
  msg_p->vol_label = label;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

T_FSCORE_RET rfsfat_get_label_nb(const T_WCHAR *pathname, T_WCHAR *label, T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_GET_VOL_LABEL_REQ_MSG *msg_p;
  INT32	pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_get_vol_label", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_GET_VOL_LABEL_REQ_MSG),
					   RFSFAT_GET_VOL_LABEL_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_file_read_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->path_name = pathname;
  msg_p->vol_label = label;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}


#if 0
T_FSCORE_RET rfsfat_query_partition_nb(T_GBI_MEDIA_TYPE media_type,char *buf,T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_QUERY_PARTITION_REQ_MSG   *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_query_partition_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_QUERY_PARTITION_REQ_MSG),
					   RFSFAT_QUERY_PARTITION_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_query_partition_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}
  /* compose message */
  msg_p->media_type		= media_type;
  msg_p->buf			= (char*)buf;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

#endif

T_FSCORE_RET rfsfat_getattribute_nb(const char *pathname,T_RV_RETURN_PATH return_path)
{

  T_RFSFAT_GETATTRIB_REQ_MSG   *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_getattribute_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_GETATTRIB_REQ_MSG),
					   RFSFAT_GETATTRIB_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_getattribute: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_GETATTRIB_REQ_MSG;
  msg_p->path_name 		= pathname;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}





T_FSCORE_RET rfsfat_setattribute_nb(const char *pathname,T_FFS_ATTRIB_TYPE attrib,T_RV_RETURN_PATH return_path)
{

  T_RFSFAT_SETATTRIB_REQ_MSG   *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_getattribute_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

 /* check the valid attrib set */
 if(!INVALID_RFSFAT_ATTRIB(attrib))
 	{
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: Attrib parameter is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EINVALID;
 	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_SETATTRIB_REQ_MSG),
					   RFSFAT_SETATTRIB_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_setattribute: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
   msg_p->hdr.msg_id = RFSFAT_SETATTRIB_REQ_MSG;
  msg_p->path_name 		= pathname;
  msg_p->attrib			= attrib;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}



T_FSCORE_RET rfsfat_getdatetime_nb(const char *pathname,T_RTC_DATE_TIME *date_time_p, T_RTC_DATE_TIME * crdate_time_p,T_RV_RETURN_PATH return_path)
{

  T_RFSFAT_GETDATETIME_REQ_MSG   *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_getdatetime_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

 /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_GETDATETIME_REQ_MSG),
					   RFSFAT_GETDATETIME_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_getdateandtime: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
    msg_p->hdr.msg_id = RFSFAT_GETDATETIME_REQ_MSG;
  msg_p->path_name 		= pathname;
  msg_p->date_time_p	= date_time_p;
  msg_p->crdate_time_p  = crdate_time_p;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;


}




#if (FFS_UNICODE ==1)

/**************************************************/
//
// Unicode API
//
/**************************************************/

/**
 * rfsfat_format_uc_nb
 */
T_FSCORE_RET rfsfat_format_uc_nb(
							const T_WCHAR*			pathname,
                    		const T_WCHAR        *name,
                          	UINT16                magic,
                          	T_RV_RETURN_PATH          return_path)
{
  T_RFSFAT_FORMAT_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_format_uc_nb", RV_TRACE_LEVEL_WARNING);


  /* Check rfsfat SWE state whether it is operational */
  if ((rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED) &&
    (rfsfat_env_ctrl_blk_p->state != RFSFAT_IDLE))
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }

 if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }


 /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_FORMAT_UC_REQ_MSG),
					   RFSFAT_FORMAT_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_FORMAT_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_FORMAT_UC_REQ_MSG;
  msg_p->pathname 		= pathname;
  msg_p->name = name;
  msg_p->magic = magic;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;


  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}

  return pair_id;
}


/**
 * rfsfat_mkdir_uc_nb
 */
T_FSCORE_RET rfsfat_mkdir_uc_nb(const T_WCHAR   *pathname,
                                T_FSCORE_MODE   mode,
                                T_RV_RETURN_PATH  return_path)
{
  T_RFSFAT_MKDIR_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_mkdir_uc_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }

 if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }



/* reserve message buffer */
 if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_MKDIR_UC_REQ_MSG),
					   RFSFAT_MKDIR_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_MKDIR_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_MKDIR_UC_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->mode = mode;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}

  return pair_id;
}



/**
 * rfsfat_open_uc_nb
 */
T_FSCORE_RET rfsfat_open_uc_nb(
						  const T_WCHAR*		pathname,
                          T_FSCORE_FLAGS        flags,
                          T_FSCORE_MODE         mode,
                          T_RV_RETURN_PATH           return_path)
{
  T_RFSFAT_OPEN_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_open_uc_nb", RV_TRACE_LEVEL_WARNING);


  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }

 if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }



/* reserve message buffer */
 if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_OPEN_UC_REQ_MSG),
					   RFSFAT_OPEN_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_OPEN_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}


  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_OPEN_UC_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->flags = flags;
  msg_p->mode = mode;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
 msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}


/**
 * rfsfat_preformat_uc_nb
 */
T_FSCORE_RET rfsfat_preformat_uc_nb(
								  const T_WCHAR  *pathname,
                                  UINT16  magic,
                                  T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_PREFORMAT_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_preformat_uc_nb", RV_TRACE_LEVEL_WARNING);


  /* Check rfsfat SWE state whether it is operational */
  if ((rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED) &&
    (rfsfat_env_ctrl_blk_p->state != RFSFAT_IDLE))
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }

  if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
 if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_PREFORMAT_UC_REQ_MSG),
					   RFSFAT_PREFORMAT_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_PREFORMAT_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_PREFORMAT_UC_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->magic = magic;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;

 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}


/**
 * rfsfat_remove_nb_uc
 */
T_FSCORE_RET rfsfat_remove_uc_nb( const T_WCHAR  *pathname,
                               T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_REMOVE_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("fsfat_remove_uc_nb", RV_TRACE_LEVEL_WARNING);


  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }

  if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }


  /* reserve message buffer */
if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_REMOVE_UC_REQ_MSG),
					   RFSFAT_REMOVE_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_REMOVE_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}


  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_REMOVE_UC_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
 msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;

 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}




/**
 * rfsfat_rename_uc_nb
 */
T_FSCORE_RET rfsfat_rename_uc_nb(		const T_WCHAR *oldname,
 										const T_WCHAR *newname,
										T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_RENAME_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_rename_uc_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }


  if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }



  /* reserve message buffer */
 if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_RENAME_UC_REQ_MSG),
					   RFSFAT_RENAME_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_REMOVE_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}


  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_RENAME_UC_REQ_MSG;
  msg_p->oldname = oldname;
  msg_p->newname = newname;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
 msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;

 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}




/**
 * rfsfat_rmdir_uc_nb
 */
T_FSCORE_RET rfsfat_rmdir_uc_nb(const T_WCHAR *pathname,
                              T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_RMDIR_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_rmdir_uc_nb", RV_TRACE_LEVEL_WARNING);


  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }

 if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }


  /* reserve message buffer */
 if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_RMDIR_UC_REQ_MSG),
					   RFSFAT_RMDIR_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_REMOVE_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}



  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_RMDIR_UC_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;

 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}

/**
 * rfsfat_stat_uc_nb
 */
T_FSCORE_RET rfsfat_stat_uc_nb( const T_WCHAR  *pathname,
                             T_FSCORE_STAT  *stat,
                             T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_STAT_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_stat_uc_nb", RV_TRACE_LEVEL_WARNING);



  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }

 if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }


  /* reserve message buffer */
 if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_STAT_UC_REQ_MSG),
					   RFSFAT_STAT_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_STAT_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_STAT_UC_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->stat = stat;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;

 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}


/**
 * rfsfat_opendir_uc_nb
 */
T_FSCORE_RET rfsfat_opendir_uc_nb (const T_WCHAR *pathname,
										 T_FSCORE_DIR * dirp,
										 T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_OPENDIR_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_stat_uc_nb", RV_TRACE_LEVEL_WARNING);


  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }

 if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_OPENDIR_UC_REQ_MSG),
					   RFSFAT_OPENDIR_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_OPENDIR_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}


  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_OPENDIR_UC_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->dirp = dirp;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}



/**
 * function:  rfsfat_readdir_uc_nb
 */
T_FSCORE_RET
rfsfat_readdir_uc_nb (T_FSCORE_DIR * dirp,
				   T_WCHAR *buf, T_FSCORE_SIZE size, T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_READDIR_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_readdir_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EACCES;
	}

  if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }

  /* reserve message buffer */
if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_READDIR_UC_REQ_MSG),
					   RFSFAT_READDIR_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_READDIR_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_READDIR_UC_REQ_MSG;
  msg_p->dirp = dirp;
  msg_p->buf = buf;
  msg_p->size = size;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
 msg_p->pair_value.fd = -1; /* Negetive value */
  msg_p->pair_value.pair_id = pair_id;

 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}




T_FSCORE_RET rfsfat_truncate_uc_nb (const T_WCHAR* pathname, T_FSCORE_SIZE size,T_RV_RETURN_PATH return_path)
{
 T_RFSFAT_TRUNC_NAME_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_stat_uc_nb", RV_TRACE_LEVEL_WARNING);


  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
    return FSCORE_EACCES;
  }

 if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }


  /* reserve message buffer */
if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_TRUNC_NAME_UC_REQ_MSG),
					   RFSFAT_TRUNC_NAME_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_TRUNC_NAME_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_TRUNC_NAME_UC_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->size= size;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;


}


T_FSCORE_RET rfsfat_getattribute_uc_nb(const T_WCHAR *pathname,T_RV_RETURN_PATH return_path)
{
  T_RFSFAT_GETATTRIB_UC_REQ_MSG   *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_getattribute_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

   if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }


  /* reserve message buffer */
if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_GETATTRIB_UC_REQ_MSG),
					   RFSFAT_GETATTRIB_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_GETATTRIB_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}


  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_GETATTRIB_UC_REQ_MSG;
  msg_p->path_name 		= pathname;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}

  return pair_id;



}


T_FSCORE_RET rfsfat_setattribute_uc_nb(const T_WCHAR *pathname,T_FFS_ATTRIB_TYPE attrib,T_RV_RETURN_PATH return_path)
{

  T_RFSFAT_SETATTRIB_UC_REQ_MSG   *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_getattribute_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

  if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }


 /* check the valid attrib set */
 if(INVALID_RFSFAT_ATTRIB(attrib))
 	{
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: Attrib parameter is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EINVALID;
 	}

  /* reserve message buffer */
if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_SETATTRIB_UC_REQ_MSG),
					   RFSFAT_SETATTRIB_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_SETATTRIB_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}


  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_SETATTRIB_UC_REQ_MSG;
  msg_p->path_name 		= pathname;
  msg_p->attrib			= SET_VALID_ATTR(attrib);
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}

  return pair_id;

}



T_FSCORE_RET rfsfat_getdatetime_uc_nb(const T_WCHAR *pathname,T_RTC_DATE_TIME *date_time_p,T_RTC_DATE_TIME * crdate_time_p,T_RV_RETURN_PATH return_path)
{

  T_RFSFAT_GETDATETIME_UC_REQ_MSG   *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_getdatetime_uc_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

 if ((return_path.callback_func == NULL && return_path.addr_id == 0) )
 {
	/* return_path is invalid */
	RFSFAT_SEND_TRACE ("RFSFAT API: return_path is invalid", RV_TRACE_LEVEL_WARNING);
	return FSCORE_EBADOP;
 }


  /* reserve message buffer */
if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_GETDATETIME_UC_REQ_MSG),
					   RFSFAT_GETDATETIME_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT T_RFSFAT_GETDATETIME_UC_REQ_MSG: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}


  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_GETDATETIME_UC_REQ_MSG;
  msg_p->path_name 		= pathname;
  msg_p->date_time_p    = date_time_p;
  msg_p->crdate_time_p = crdate_time_p;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;


 /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}

  return pair_id;



}



/**
 * function:  rfsfat_chmod_uc_nb
 */
T_FSCORE_RET
rfsfat_chmod_uc_nb (const T_WCHAR *pathname,
				 T_FSCORE_MODE mode, T_RV_RETURN return_path)
{
  T_RFSFAT_CHMOD_UC_REQ_MSG *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("rfsfat_chmod_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
	{
	  /* The RFSFAT is not able to handle this request at this moment */
	  RFSFAT_SEND_TRACE ("RFSFAT API rfsfat_chmod_nb: not ready", RV_TRACE_LEVEL_WARNING);

	  return FSCORE_EACCES;
	}

  if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API rfsfat_chmod_nb: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}

  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_CHMOD_UC_REQ_MSG),
					   RFSFAT_CHMOD_UC_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_chmod_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}

  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_CHMOD_UC_REQ_MSG;
  msg_p->pathname = pathname;
  msg_p->mode = mode;
  msg_p->return_path = return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_value.pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;
}



T_FSCORE_RET rfsfat_mount_nb(T_RV_RETURN_PATH return_path)
{
  return rfsfat_mount_unmount_nb(RFSFAT_MOUNT, return_path);
}


T_FSCORE_RET rfsfat_unmount_nb(T_RV_RETURN_PATH return_path)
{
 return rfsfat_mount_unmount_nb(RFSFAT_UNMOUNT, return_path);
}




static T_FSCORE_RET rfsfat_mount_unmount_nb(int op,T_RV_RETURN_PATH return_path)
{

  T_RFSFAT_MOUNT_UNMOUNT_REQ_MSG   *msg_p;
  INT32 pair_id;

  RFSFAT_SEND_TRACE ("RFSFAT API: rfsfat_mount_unmount_nb", RV_TRACE_LEVEL_WARNING);

  /* Check rfs SWE state whether it is operational */
  if ((rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)&& (rfsfat_env_ctrl_blk_p->state != RFSFAT_STOPPED))
 {
	/* The RFSFAT is not able to handle this request at this moment */
	RFSFAT_SEND_TRACE ("RFSFAT API: not ready", RV_TRACE_LEVEL_WARNING);
	return (T_FSCORE_SIZE) FSCORE_EACCES;
 }

 if (return_path.callback_func == NULL && return_path.addr_id == 0)
	{
	  /* return_path is invalid */
	  RFSFAT_SEND_TRACE ("RFSFAT API rfsfat_mount_unmount_nb: return_path is invalid",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EINVALID;
	}


  /* reserve message buffer */
  if (rvf_get_msg_buf (rfsfat_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFSFAT_MOUNT_UNMOUNT_REQ_MSG),
					   RFSFAT_MOUNT_UNMOUNT_REQ_MSG,
					   (T_RV_HDR **) & msg_p) == RVF_RED)
	{
	  /* Insufficient memory to create message request */
	  RFSFAT_SEND_TRACE ("RFSFAT rfsfat_mount_unmount_nb: out of memory",
						 RV_TRACE_LEVEL_WARNING);
	  return FSCORE_EMEMORY;
	}



  /* compose message */
  msg_p->hdr.msg_id = RFSFAT_MOUNT_UNMOUNT_REQ_MSG;
  msg_p->op 		= op;
  msg_p->return_path 	= return_path;
  pair_id = rfsfat_get_free_pair_id ();
  msg_p->pair_id = pair_id;

  /* Send message to own mailbox */
  if (RV_OK != rvf_send_msg (rfsfat_env_ctrl_blk_p->addr_id, msg_p))
	{
	  return FSCORE_EMSGSEND;
	}
  return pair_id;

}








void rfsfat_connect_unicode_to_u8(
                            void  (*unicode_to_u8)(const T_WCHAR  *name_uc,
                                                   UINT8               *name_u8))
{


  rfsfat_env_ctrl_blk_p->uc_conv.convert_unicode_to_u8 = unicode_to_u8;

}


void rfsfat_connect_u8_to_unicode(
                            void  (*u8_to_unicode)(const UINT8         *name_u8,
                                                   T_WCHAR        *name_uc))
{


  rfsfat_env_ctrl_blk_p->uc_conv.convert_u8_to_unicode = u8_to_unicode;
}


void  rfsfat_set_unicode_functions(T_FFS_UNICODE_FUNC_TABLE  uc_conv)
{
  /* Check rfsfat SWE state whether it is operational */
  if (rfsfat_env_ctrl_blk_p->state != RFSFAT_CONFIGURED)
  {
    /* The RFSFAT is not able to handle this request at this moment */
    RFSFAT_SEND_TRACE_ERROR ("RFSFAT API: not ready");
    return;
  }

  //RFSFAT_SEND_TRACE_ERROR ("RFSFAT API: rfsfat_set_unicode_functions called");
  rfsfat_env_ctrl_blk_p->uc_conv = uc_conv;
}


/*static void fatfs_convert_unicode_to_u8(const T_WCHAR *mp_uc, UINT8* mp)
{
  UINT16  i = 0;

  while (mp_uc[i] != '\0')
  {
    mp[i] = (char)mp_uc[i]; //just copy lsb
    i++;
  }
  mp[i] = '\0';  // add end of string
}*/


static void fatfs_convert_unicode_to_u8_length(const T_WCHAR *mp_uc, UINT8* mp, UINT16 length)
{
  UINT16  i = 0;

  while ((mp_uc[i] != '\0') && (i < length))
  {
    mp[i] = (char)mp_uc[i]; //just copy lsb
    i++;
  }
  mp[i] = '\0';  // add end of string
}



void print_unicode_string(T_WCHAR *uni_str)
{

 RFSFAT_SEND_TRACE_DEBUG_HIGH("printing Unicode string....start ");
 fatfs_convert_unicode_to_u8_length(uni_str,a_str,300);

  RFSFAT_SEND_TRACE_ERROR((char*)a_str);
 RFSFAT_SEND_TRACE_DEBUG_HIGH(" printing Unicode string....end ");
}

/**************************************************/
//
// end of Unicode API
//
/**************************************************/

#endif






/**
 * Function: rfsfat_get_sw_version
 */
UINT32
rfsfat_get_sw_version (void)
{
  return BUILD_VERSION_NUMBER (RFSFAT_MAJOR, RFSFAT_MINOR, RFSFAT_BUILD);
}


//******************************************************************************
// Local Support Functions
//******************************************************************************

/**
 * static function:  rfs_search_fd_table_entry
 */
static T_FSCORE_RET
rfsfat_search_fd_table_entry (T_FSCORE_FD fd, UINT8 * entry)
{
  UINT8 tmp_entry;
  BOOL fd_found = FALSE;

  /* Retrieve file descriptor information */
  for (tmp_entry = 0;
	   ((tmp_entry < FSCORE_MAX_NR_OPEN_FSCORE_FILES) && (fd_found == FALSE));
	   tmp_entry++)
	{
	  if (rfsfat_env_ctrl_blk_p->ffs_globals.fd_table[tmp_entry].fd == fd)
		{
		  /* File descriptor is openend and information is available */
		  fd_found = TRUE;
		  *entry = tmp_entry;
		}
	}

  if (fd_found == FALSE)
	{
	  /* File descriptor is not openend */
	  return FSCORE_EBADFD;
	}
  else
	{
	  return FSCORE_EOK;
	}
}


static T_FSCORE_RET
rfsfat_get_free_pair_id (void)
{
  T_FSCORE_RET pair_id;

  rfsfat_lock_mutex_pair_id ();
  pair_id = rfsfat_env_ctrl_blk_p->pair_id++;
  rfsfat_unlock_mutex_pair_id ();

  return pair_id;
}


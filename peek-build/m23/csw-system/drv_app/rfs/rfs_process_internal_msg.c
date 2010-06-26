/**
 * @file	rfs_process_internal_msg.c
 *
 * Internal functions of RFS
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/30/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#include "rfs/rfs_process_internal_msg.h"
#include "rfs/rfs_api.h"
#include "rfs/rfs_i.h"
#include "rfs/rfs_fm.h"
#include "rfsfat_api.h"
/*lint -esym(794, mount_entry_p)*/

static const T_RFS_RETURN_PATH rfs_default_return_path =
  { {RVF_INVALID_ADDR_ID, NULL}, NULL, NULL };
#if 0
static void        rfs_i_fill_file_system_stat(T_RFS_STAT  *stat);
static T_RFS_RET  rfs_i_fill_mountpoint_system_stat( T_FSCORE_PARTITION_TABLE *mount_entry_p, T_WCHAR* mpt_name, T_RFS_STAT  *stat);
static void fill_file_system_name(char *str,T_GBI_FS_TYPE file_system_type);

#endif

static UINT8 RFS_get_fs_type(int fs_type);

/**
 * static function definitions
 */

/**
 * function: rfs_send_response_to_client
 */
static T_RV_RET
rfs_send_response_to_client (T_RV_HDR * rsp_p,
							 const T_RV_RETURN * return_path)
{
  T_RV_RET result;

  

  RFS_SEND_TRACE ("rfs_send_response_to_client", RV_TRACE_LEVEL_DEBUG_LOW);


  if((return_path->callback_func == NULL) && (return_path->addr_id == RVF_INVALID_ADDR_ID))
  	{
  	  rvf_free_msg ((T_RV_HDR *) rsp_p); /* free the response message */ 
  	   return RV_OK;   /* No return path, dont know where to respponse */
  	}

  

  /* follow return path: callback or mailbox */
  if (return_path->callback_func != NULL)
	{
	  return_path->callback_func (rsp_p);

	}
  else
	{
	  result = rvf_send_msg (return_path->addr_id, rsp_p);

	  if (result != RVF_OK)
		{
		  RFS_SEND_TRACE
			("RFS rfs_send_response_to_client: could not send response msg",
			 RV_TRACE_LEVEL_WARNING);
		  return RV_MEMORY_ERR;
		}
	}

  return RV_OK;
}

/**
 * rfs_send_result_to_client
 *
 * Sends a result to the client
 * IF mutex THEN use mutex
 * ELSE IF callback THEN use callback
 * ELSE send message
 *
 * @param result              status to be sent
 * @param *path_to_client_p   receiver of the message
 *
 * @return RV_OK if OK
 *
 */

static T_RV_RET
rfs_send_result_to_client (T_RFS_CMD_ID command_id, T_RFS_RET result,
						   T_RFS_PAIR_VALUE pair_value,
						   T_RFS_RETURN_PATH * path_to_client_p)
{
  T_RFS_READY_RSP_MSG *rsp_p;
  T_RV_RETURN *return_path;	

  return_path= (T_RV_RETURN *)(&path_to_client_p->return_path);

  if ((path_to_client_p->mutex != NULL) && (path_to_client_p->result != NULL))
	{
	  RFS_SEND_TRACE ("RFS rfs_send_result_to_client: using mutex",
					  RV_TRACE_LEVEL_DEBUG_HIGH);
	  *path_to_client_p->result = result;
	  rvf_unlock_mutex (path_to_client_p->mutex);
	  return RV_OK;
	}

 if(return_path == NULL)
 	{
 	   return RV_OK;    /* No return path. dont know where to send the message */  
 	}

  if((return_path->callback_func == NULL) && (return_path->addr_id == RVF_INVALID_ADDR_ID))
  	{
  	   return RV_OK;   /* No return path, dont know where to respponse */
  	}

 
  if (rvf_get_msg_buf (rfs_env_ctrl_blk_p->prim_mb_id,
					   sizeof (T_RFS_READY_RSP_MSG),
					   RFS_READY_RSP_MSG, (T_RV_HDR **) & rsp_p) == RVF_RED)
	{
	  RFS_SEND_TRACE ("RFS cannot send response to client: out of memory",
					  RV_TRACE_LEVEL_ERROR);
	  return RV_MEMORY_ERR;
	}

  //RFS_SEND_TRACE_PARAM ("RFS *** get mem (response) *** ", (UINT32) rsp_p, RV_TRACE_LEVEL_ERROR);
  rsp_p->hdr.msg_id = RFS_READY_RSP_MSG;
  rsp_p->command_id = command_id;
  rsp_p->pair_value = pair_value;
  rsp_p->result = result;
  rsp_p->app_data_p=return_path->app_data_p;

  RFS_SEND_TRACE ("RFS rfs_send_result_to_client: sending message",
				  RV_TRACE_LEVEL_DEBUG_HIGH);
  return rfs_send_response_to_client ((T_RV_HDR *) rsp_p,
									  return_path);
}

/**
 * function: rfs_close_resp
 */
static T_RFS_RETURN
rfs_close_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_close_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core fd --> rfs client fd */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_FD |
						/* keep mapping if close failed */
						(core_ready_rsp_msg_p->result >=
						 FSCORE_EOK ? RFS_PAIR_MAP_DELETE : 0));

  /* IF close succeeded THEN delete fd */
  if (core_ready_rsp_msg_p->result >= FSCORE_EOK)
	{
	  rfs_delete_fd (client_pair_value.fd);
	}

  return rfs_send_result_to_client (RFS_CLOSE_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_close_req
 */
T_RFS_RETURN
rfs_close_req (T_RV_HDR * msg_p)
{
  T_RFS_CLOSE_REQ_MSG *req_msg_p = (T_RFS_CLOSE_REQ_MSG *) msg_p;
  T_FSCORE_RET core_func_result;
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  UINT8 fscore_nmb;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;

  /* get core fd, update client return path */
  client_pair_value.fd = req_msg_p->fd;
  client_pair_value.pair_id = 0;
  core_pair_value =
	rfs_map_pair_value (client_pair_value, client_pair_value /*unused */ ,
						&client_return_path,
						&fscore_nmb,
						RFS_PAIR_MAP_FD | RFS_PAIR_MAP_TO_CORE |
						RFS_PAIR_MAP_UPDATE_CLIENT_PATH);
  
 

  /* IF mapping found THEN call core ELSE pass error */
  if (core_pair_value.fd >= 0)
	{
	  /* call the appropriate 'close' function for this mount point */
	  rfs_core_dispatch (&core_func_result, fscore_nmb,
						 fscore_close (core_pair_value.fd));
	}
  else
	{
	  core_func_result = core_pair_value.fd;
	}
  /* IF error THEN complain to client immediately */
  if (core_func_result < 0)
	{
	  rfs_send_result_to_client (RFS_CLOSE_RSP, core_func_result,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_write_resp
 */
static T_RFS_RETURN
rfs_write_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_write_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core fd --> rfs client fd */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_FD);

  return rfs_send_result_to_client (RFS_WRITE_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_write_req
 */
T_RFS_RETURN
rfs_write_req (T_RV_HDR * msg_p)
{
  T_RFS_WRITE_REQ_MSG *req_msg_p = (T_RFS_WRITE_REQ_MSG *) msg_p;
  T_FSCORE_RET core_func_result;
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  UINT8 fscore_nmb;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;

  /* get core fd, update client return path */
  client_pair_value.fd = req_msg_p->fd;
  client_pair_value.pair_id = 0;
  core_pair_value =
	rfs_map_pair_value (client_pair_value, client_pair_value /*unused */ ,
						&client_return_path,
						&fscore_nmb,
						RFS_PAIR_MAP_FD | RFS_PAIR_MAP_TO_CORE |
						RFS_PAIR_MAP_UPDATE_CLIENT_PATH);
  
  /* IF mapping found THEN call core ELSE pass error */
  if (core_pair_value.fd >= 0)
	{
	  /* call the appropriate 'write' function for this mount point */
	  rfs_core_dispatch (&core_func_result, fscore_nmb,
						 fscore_write (core_pair_value.fd, req_msg_p->buf,
									   req_msg_p->size));
	}
  else
	{
	  core_func_result = core_pair_value.fd;
	}
  /* IF error THEN complain to client immediately */
  if (core_func_result < 0)
	{
	  rfs_send_result_to_client (RFS_WRITE_RSP, core_func_result,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_read_resp
 */
static T_RFS_RETURN
rfs_read_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_read_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core fd --> rfs client fd */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_FD);

  return rfs_send_result_to_client (RFS_READ_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_read_req
 */
T_RFS_RETURN
rfs_read_req (T_RV_HDR * msg_p)
{
  T_RFS_READ_REQ_MSG *req_msg_p = (T_RFS_READ_REQ_MSG *) msg_p;
  T_FSCORE_RET core_func_result; 
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  UINT8 fscore_nmb;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;

  /* get core fd, update client return path */
  client_pair_value.fd = req_msg_p->fd;
  client_pair_value.pair_id = 0;
  core_pair_value =
	rfs_map_pair_value (client_pair_value, client_pair_value /*unused */ ,
						&client_return_path,
						&fscore_nmb,
						RFS_PAIR_MAP_FD | RFS_PAIR_MAP_TO_CORE |
						RFS_PAIR_MAP_UPDATE_CLIENT_PATH);

 
  /* IF mapping found THEN call core ELSE pass error */
  if (core_pair_value.fd >= 0)
	{
	  /* call the appropriate 'read' function for this mount point */
	  rfs_core_dispatch (&core_func_result, fscore_nmb,
						 fscore_read (core_pair_value.fd, req_msg_p->buf,
									  req_msg_p->size));
	}
  else
	{
	  core_func_result = core_pair_value.fd;
	}
  /* IF error THEN complain to client immediately */
  if (core_func_result < 0)
	{
	  rfs_send_result_to_client (RFS_READ_RSP, core_func_result,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_lseek_resp
 */
static T_RFS_RETURN
rfs_lseek_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_lseek_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core fd --> rfs client fd */
  client_pair_value =rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_FD);

  return rfs_send_result_to_client (RFS_LSEEK_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_lseek_req
 */
T_RFS_RETURN
rfs_lseek_req (T_RV_HDR * msg_p)
{
  T_RFS_LSEEK_REQ_MSG *req_msg_p = (T_RFS_LSEEK_REQ_MSG *) msg_p;
  T_FSCORE_RET core_func_result;
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  UINT8 fscore_nmb;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;

  /* get core fd, update client return path */
  client_pair_value.fd = req_msg_p->fd;
  client_pair_value.pair_id = 0;
  core_pair_value =
	rfs_map_pair_value (client_pair_value, client_pair_value /*unused */ ,
						&client_return_path,
						&fscore_nmb,
						RFS_PAIR_MAP_FD | RFS_PAIR_MAP_TO_CORE |
						RFS_PAIR_MAP_UPDATE_CLIENT_PATH);

  
  /* IF mapping found THEN call core ELSE pass error */
  if (core_pair_value.fd >= 0)
	{
	  /* call the appropriate 'lseek_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result, fscore_nmb,
						 fscore_lseek (core_pair_value.fd, req_msg_p->offset,
									   req_msg_p->whence));
	}
  else
	{
	  core_func_result = core_pair_value.fd;
	}
  /* IF error THEN complain to client immediately */
  if (core_func_result < 0)
	{
	  rfs_send_result_to_client (RFS_LSEEK_RSP, core_func_result,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_fchmod_resp
 */
static T_RFS_RETURN
rfs_fchmod_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_fchmod_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core fd --> rfs client fd */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_FD);

  return rfs_send_result_to_client (RFS_FCHMOD_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_fchmod_req
 */
T_RFS_RETURN
rfs_fchmod_req (T_RV_HDR * msg_p)
{
  T_RFS_FCHMOD_REQ_MSG *req_msg_p = (T_RFS_FCHMOD_REQ_MSG *) msg_p;
  T_FSCORE_RET core_func_result;
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  UINT8 fscore_nmb;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;

  /* get core fd, update client return path */
  client_pair_value.fd = req_msg_p->fd;
  client_pair_value.pair_id = 0;
  core_pair_value =
	rfs_map_pair_value (client_pair_value, client_pair_value /*unused */ ,
						&client_return_path,
						&fscore_nmb,
						RFS_PAIR_MAP_FD | RFS_PAIR_MAP_TO_CORE |
						RFS_PAIR_MAP_UPDATE_CLIENT_PATH);

 
  /* IF mapping found THEN call core ELSE pass error */
  if (core_pair_value.fd >= 0)
	{
	  /* call the appropriate 'fchmod_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result, fscore_nmb,
						 fscore_fchmod (core_pair_value.fd, req_msg_p->mode));
	}
  else
	{
	  core_func_result = core_pair_value.fd;
	}
  /* IF error THEN complain to client immediately */
  if (core_func_result < 0)
	{
	  rfs_send_result_to_client (RFS_FCHMOD_RSP, core_func_result,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_fstat_resp
 */
static T_RFS_RETURN
rfs_fstat_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_fstat_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core fd --> rfs client fd */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_FD);

  return rfs_send_result_to_client (RFS_FSTAT_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_fstat_req
 */
T_RFS_RETURN
rfs_fstat_req (T_RV_HDR * msg_p)
{
  T_RFS_FSTAT_REQ_MSG *req_msg_p = (T_RFS_FSTAT_REQ_MSG *) msg_p;
  T_FSCORE_RET core_func_result;
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  UINT8 fscore_nmb;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;

  /* get core fd, update client return path */
  client_pair_value.fd = req_msg_p->fd;
  client_pair_value.pair_id = 0;
  core_pair_value =
	rfs_map_pair_value (client_pair_value, client_pair_value /*unused */ ,
						&client_return_path,
						&fscore_nmb,
						RFS_PAIR_MAP_FD | RFS_PAIR_MAP_TO_CORE |
						RFS_PAIR_MAP_UPDATE_CLIENT_PATH);
  
  
  /* IF mapping found THEN call core ELSE pass error */
  if (core_pair_value.fd >= 0)
	{
	  /* call the appropriate 'fstat_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result, fscore_nmb,
						 fscore_fstat (core_pair_value.fd,
									   (T_FSCORE_STAT *) req_msg_p->stat));
	}
  else
	{
	  core_func_result = core_pair_value.fd;
	}
  /* IF error THEN complain to client immediately */
  if (core_func_result < 0)
	{
	  rfs_send_result_to_client (RFS_FSTAT_RSP, core_func_result,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_fsync_resp
 */
static T_RFS_RETURN
rfs_fsync_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_fsync_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core fd --> rfs client fd */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_FD);

  return rfs_send_result_to_client (RFS_FSYNC_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_fsync_req
 */
T_RFS_RETURN
rfs_fsync_req (T_RV_HDR * msg_p)
{
  T_RFS_FSYNC_REQ_MSG *req_msg_p = (T_RFS_FSYNC_REQ_MSG *) msg_p;
  T_FSCORE_RET core_func_result;
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  UINT8 fscore_nmb;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;

  /* get core fd, update client return path */
  client_pair_value.fd = req_msg_p->fd;
  client_pair_value.pair_id = 0;
  core_pair_value =
	rfs_map_pair_value (client_pair_value, client_pair_value /*unused */ ,
						&client_return_path,
						&fscore_nmb,
						RFS_PAIR_MAP_FD | RFS_PAIR_MAP_TO_CORE |
						RFS_PAIR_MAP_UPDATE_CLIENT_PATH);
  
 
  /* IF mapping found THEN call core ELSE pass error */
  if (core_pair_value.fd >= 0)
	{
	  /* call the appropriate 'fsync_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result, fscore_nmb,
						 fscore_fsync (core_pair_value.fd));
	}
  else
	{
	  core_func_result = core_pair_value.fd;
	}
  /* IF error THEN complain to client immediately */
  if (core_func_result < 0)
	{
	  rfs_send_result_to_client (RFS_FSYNC_RSP, core_func_result,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_open_resp
 */
static T_RFS_RETURN
rfs_open_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE client_pair_value_org;
  RFS_SEND_TRACE ("rfs_open_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path, &core_ready_rsp_msg_p->fscore_nmb, RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_DELETE);	// |/*PK pair id should be deleted, fd is now generated*/
  //RFS_PAIR_MAP_DELETE);
  client_pair_value_org = client_pair_value;
  /* core fd */
  core_pair_value.fd = (T_RFS_FD) core_ready_rsp_msg_p->result;
  core_pair_value.pair_id = 0;
  /* IF all ok THEN create fd mapping ELSE just pass error */
  if (core_pair_value.fd >= 0)
	{
	  T_FSCORE_RET dummy;
	  UINT8 op_mode = O_AIO;
	  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;

	  /* rfs client fd */
	  client_pair_value.fd = rfs_new_fd ();
	  /* store mapping */
	  rfs_map_pair_value (client_pair_value, core_pair_value,
						  &client_return_path,
						  &core_ready_rsp_msg_p->fscore_nmb,
						  RFS_PAIR_MAP_FD | RFS_PAIR_MAP_NEW);
	  /* set asynchronous IO */
	  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
	  rfs_core_dispatch (&dummy, core_ready_rsp_msg_p->fscore_nmb,
						 fscore_fcntl (core_pair_value.fd, F_SETRETPATH,
									   &rfs_return_path.return_path));
	  rfs_core_dispatch (&dummy, core_ready_rsp_msg_p->fscore_nmb,
						 fscore_fcntl (core_pair_value.fd, F_SETFLAG,
									   &op_mode));
	}
  else
	{
	  rfs_unreserve_fd ();
	  client_pair_value.fd = core_pair_value.fd;
	}

  return rfs_send_result_to_client (RFS_OPEN_RSP, client_pair_value.fd,
									client_pair_value_org,
									&client_return_path);
}

/**
 * function:  rfs_open_req
 */
T_RFS_RETURN
rfs_open_req (T_RV_HDR * msg_p)
{
  T_RFS_OPEN_REQ_MSG *req_msg_p = (T_RFS_OPEN_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;


  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;
  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc(req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	  if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  if (rfs_reserve_fd () != RFS_EOK)
		{
		  core_func_result.pair_id = RFS_ENUMFD;
		  break;
		}
	 
	  /* todo: parameter check: return path */
	  /* call the appropriate 'open_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_open_uc_nb (req_msg_p->pathname,
										 req_msg_p->flags, req_msg_p->mode,
										 rfs_return_path.return_path));

	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_OPEN_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_chmod_resp
 */
static T_RFS_RETURN
rfs_chmod_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_chmod_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_CHMOD_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_chmod_req
 */
T_RFS_RETURN
rfs_chmod_req (T_RV_HDR * msg_p)
{
  T_RFS_CHMOD_REQ_MSG *req_msg_p = (T_RFS_CHMOD_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;

 

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

 do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc(req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	 // convert_unicode_to_u8(req_msg_p->pathname, dest_name_p);
	  /* todo: parameter check: return path */
	  /* call the appropriate 'chmod_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_chmod_uc_nb (req_msg_p->pathname,
										  req_msg_p->mode,
										  rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_CHMOD_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_stat_resp
 */
static T_RFS_RETURN
rfs_stat_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_stat_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_STAT_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}



#if 0
/**
 * Function: rfs_i_fill_file_system_stat
 *
 * This function determines and fills the RFS specific stat data members in 
 * case a file_system root (e.g. NULL or '/') was given as path. 
 *
 * @param   pathname  Terminated string containing NULL, the name of the mountpount, 
 *                    file or directory.
 * @param   stat      Contains information (meta-data) about the specified object.
 */
static void  rfs_i_fill_file_system_stat(T_RFS_STAT  *stat)
{                                                
    stat->file_system.oname_length  = RFS_FILENAME_MAX;
    stat->file_system.pname_length  = RFS_PATHNAME_MAX;
    stat->file_system.max_openfiles = RFS_MAX_NR_OPEN_FILES;
    stat->file_system.max_opendirs  = 0xFF;
    stat->file_system.cur_openfiles = rfs_env_ctrl_blk_p->open_file_cnt;
    stat->file_system.cur_opendirs  = rfs_env_ctrl_blk_p->open_dir_cnt;
 
} 





static void fill_file_system_name(char *str,T_GBI_FS_TYPE file_system_type)
{

  switch (file_system_type)
  	{

      case GBI_FAT12:
			strncpy(str,RFS_FAT12_FS_NAME,MAX_MP_STAT_NAME);		
			break;
	  	
      case GBI_FAT16_A:
      case GBI_FAT16_B: 
	  case GBI_FAT16_LBA:
			strncpy(str,RFS_FAT16_FS_NAME,MAX_MP_STAT_NAME);		
			break;

      case GBI_FAT32:
	  case GBI_FAT32_LBA:
			strncpy(str,RFS_FAT32_FS_NAME,MAX_MP_STAT_NAME);		
			break;

	  case GBI_RELIANCE:
			strncpy(str,RFS_RELIANCE_FS_NAME,MAX_MP_STAT_NAME);
			break;
			
	  default: 	
             strncpy(str,RFS_UNKNOWN_FS_NAME,MAX_MP_STAT_NAME);
			break;

  	}
  	
}



/**
 * Function: rfs_i_fill_file_system_stat
 *
 * This function determines and fills the RFS specific stat data members in 
 * case a file_system root (e.g. NULL or '/') was given as path. 
 *
 * @param   pathname  Terminated string containing NULL, the name of the mountpount, 
 *                    file or directory.
 * @param   stat      Contains information (meta-data) about the specified object.
 */
static T_RFS_RET  rfs_i_fill_mountpoint_system_stat( T_FSCORE_PARTITION_TABLE *mount_entry_p, T_WCHAR* mpt_name, T_RFS_STAT  *stat)
{                                                
  UINT16 part_index;
  UINT8  partition_name[GBI_MAX_PARTITION_NAME+1];

  memset(stat, 0, sizeof(T_RFS_STAT));
  convert_unicode_to_u8_length(mpt_name, (char*)partition_name, GBI_MAX_PARTITION_NAME);
  for(part_index=0;part_index < mount_entry_p->nbr_partitions; part_index++)
  	{

	   if( (mount_entry_p->partition_info_table[part_index].is_mounted) && (strcmp((const char*)partition_name, (const char*)mount_entry_p->partition_info_table[part_index].partition_name) == 0))
	   	{

		  /* partition found */
          fill_file_system_name(stat->mount_point.fs_type,mount_entry_p->partition_info_table[part_index].filesys_type);
		  
		  strncpy(stat->mount_point.media_type,(const char*)partition_name, MAX_MP_STAT_NAME); 
		  stat->mount_point.mpt_size = mount_entry_p->partition_info_table[part_index].partition_size *;
		  return RFS_OK;
		  
	   	}
	  
  	}

  return RFS_EMOUNT;
  	
} 


#endif

/**
 * function:  rfs_stat_req
 */
T_RFS_RETURN
rfs_stat_req (T_RV_HDR * msg_p)
{
  T_RFS_STAT_REQ_MSG *req_msg_p = (T_RFS_STAT_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  const  T_WCHAR root_dir[3]={0x002F,0x0000,0x0000};
  T_WCHAR * mpt_end_ptr=NULL, *mpt_start_ptr= NULL;
  UINT16 part_index=0;

  
  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

   if ((wstrcmp (req_msg_p->pathname, root_dir)) == 0) 
   	{

#if 0   
	  rfs_i_fill_file_system_stat(req_msg_p->stat);
	  
	  rfs_send_result_to_client (RFS_STAT_RSP, RFS_OK,client_pair_value, &client_return_path);
	  return RFS_OK;  
#else
      rfs_send_result_to_client (RFS_STAT_RSP, RFS_EINVALID,client_pair_value, &client_return_path);
      return RFS_OK; 
#endif

	  
   	}

   

  if (req_msg_p->pathname != NULL)
	{
	  do
		{						/* while (0) */
		  /* find mount point for requested path */
		  mount_entry_p = rfs_get_mount_entry_by_pathname_uc (req_msg_p->pathname, &part_index);
		  if (mount_entry_p == NULL)
			{
			  core_func_result.pair_id = RFS_EINVALID;
			  break;
			}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}


#if 0
          /* App may be asking for Mount point statitics */
		   mpt_start_ptr = (T_WCHAR *)req_msg_p->pathname;
           while (mpt_start_ptr[0] == ((T_WCHAR)'/'))
			{
	  			mpt_start_ptr++;
			}
  			mpt_end_ptr = (T_WCHAR *) wstrchr(mpt_start_ptr, '/');

		   if((mpt_end_ptr == NULL) || (mpt_end_ptr[1] == '\0'))
		   	{
               /* asking for mount point statics */
			    rfs_i_fill_mountpoint_system_stat(mount_entry_p,mpt_start_ptr,req_msg_p->stat);

				rfs_send_result_to_client (RFS_STAT_RSP, RFS_OK,client_pair_value, &client_return_path);
	  			return RFS_OK;  
			    
		   	}
#endif		   

		   		  
		  /* todo: parameter check: return path */
		  /* call the appropriate 'stat_nb' function for this mount point */
		  rfs_core_dispatch (&core_func_result.pair_id,
							 mount_entry_p->fscore_numb,
							 fscore_stat_uc_nb (req_msg_p->pathname,
											 (T_FSCORE_STAT *) req_msg_p->stat,
											 rfs_return_path.return_path));
		}
	  while (0);				//lint !e717
	}
  else
	{
	  /*pathname == NULL */
	  /*file system stat is requested and has */
	  /*already been filled at api level */
	  core_func_result.pair_id = client_pair_value.pair_id;
	  rfs_send_result_to_client (RFS_STAT_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	  return RFS_OK;

	}

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_STAT_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_remove_resp
 */
static T_RFS_RETURN
rfs_remove_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_remove_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_REMOVE_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_remove_req
 */
T_RFS_RETURN
rfs_remove_req (T_RV_HDR * msg_p)
{
  T_RFS_REMOVE_REQ_MSG *req_msg_p = (T_RFS_REMOVE_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;


  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc(req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  //convert_unicode_to_u8(req_msg_p->pathname, dest_name_p);
	  /* todo: parameter check: return path */
	  /* call the appropriate 'remove_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_remove_uc_nb (req_msg_p->pathname,
										   rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_REMOVE_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}


/**
 * function: rfs_rename_resp
 */
static T_RFS_RETURN
rfs_rename_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_rename_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_RENAME_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_rename_req
 */
T_RFS_RETURN
rfs_rename_req (T_RV_HDR * msg_p)
{
  T_RFS_RENAME_REQ_MSG *req_msg_p = (T_RFS_RENAME_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;  

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc (req_msg_p->oldname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	
	  /* todo: parameter check: return path */
	  /* call the appropriate 'rename_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_rename_uc_nb (req_msg_p->oldname,
												req_msg_p->newname,
										rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_RENAME_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_mkdir_resp
 */
static T_RFS_RETURN
rfs_mkdir_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_mkdir_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_MKDIR_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_mkdir_req
 */
T_RFS_RETURN
rfs_mkdir_req (T_RV_HDR * msg_p)
{
  T_RFS_MKDIR_REQ_MSG *req_msg_p = (T_RFS_MKDIR_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc ( req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  //convert_unicode_to_u8(req_msg_p->pathname, dest_name_p);
	  /* todo: parameter check: return path */
	  /* call the appropriate 'mkdir_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_mkdir_uc_nb (req_msg_p->pathname,
										  req_msg_p->mode,
										  rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_MKDIR_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_rmdir_resp
 */
static T_RFS_RETURN
rfs_rmdir_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_rmdir_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_RMDIR_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_rmdir_req
 */
T_RFS_RETURN
rfs_rmdir_req (T_RV_HDR * msg_p)
{
  T_RFS_RMDIR_REQ_MSG *req_msg_p = (T_RFS_RMDIR_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;


  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc(req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  //convert_unicode_to_u8(req_msg_p->pathname, dest_name_p);
	  /* todo: parameter check: return path */
	  /* call the appropriate 'rmdir_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_rmdir_uc_nb (req_msg_p->pathname,
										  rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_RMDIR_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_opendir_resp
 */
static T_RFS_RETURN
rfs_opendir_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_opendir_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  if(core_ready_rsp_msg_p->result >= 0)
  	{
  	  rfs_env_ctrl_blk_p->open_dir_cnt++;
  	}

  return rfs_send_result_to_client (RFS_OPENDIR_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_opendir_req
 */
T_RFS_RETURN
rfs_opendir_req (T_RV_HDR * msg_p)
{
  T_RFS_OPENDIR_REQ_MSG *req_msg_p = (T_RFS_OPENDIR_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  const  T_WCHAR root_dir[3]={0x002F,0x0000,0x0000};
  UINT16 valid_partitions_count=0, fs_index=0;
  UINT16 part_index=0;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

   if ((wstrcmp (req_msg_p->pathname, root_dir)) == 0) 
   	{

      /* open the root directory */
	  req_msg_p->dirp->mpt_idx = 0xFF;    /* Indicate that root directory is opened */
	  req_msg_p->dirp->fscore_nmb = 0XFF;
      req_msg_p->dirp->opendir_ino = 0;   /* starting file system core */
	  req_msg_p->dirp->lastread_ino = 0;   /* current mount point index */


	  /* find the number of active partitions */
	  for(fs_index=0, valid_partitions_count=0;fs_index < rfs_env_ctrl_blk_p->nmb_of_fs; fs_index++)
	  	{

		  for(part_index=0;part_index < rfs_env_ctrl_blk_p->fscore_part_table[fs_index].nbr_partitions; part_index++)
		  	{
		  	    if(rfs_env_ctrl_blk_p->fscore_part_table[fs_index].partition_info_table[part_index].is_mounted)
		  	    	{
		  	    	   valid_partitions_count++;
		  	    	}
		  	}
	  	}
	  
	  
	  rfs_send_result_to_client (RFS_OPENDIR_RSP, valid_partitions_count,client_pair_value, &client_return_path);
	  return RFS_OK;  
   	}


 
  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc (req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
      /* Fill the file system type */ 
      req_msg_p->dirp->fscore_nmb = mount_entry_p->fscore_numb;

	  
	  /* todo: parameter check: return path */
	  /* call the appropriate 'opendir_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_opendir_uc_nb (req_msg_p->pathname,
											(T_FSCORE_DIR *) req_msg_p->dirp,
											rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   *   and map this dirp to a core for later readdir() calls
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);

	}
  else
	{
	  rfs_send_result_to_client (RFS_OPENDIR_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_readdir_resp
 */
static T_RFS_RETURN
rfs_readdir_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;
  UINT16 i;

  RFS_SEND_TRACE ("rfs_readdir_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_READDIR_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_readdir_req
 */
T_RFS_RETURN
rfs_readdir_req (T_RV_HDR * msg_p)
{
  T_RFS_READDIR_REQ_MSG *req_msg_p = (T_RFS_READDIR_REQ_MSG *) msg_p;
  T_FSCORE_RET core_func_result;
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  UINT8 fscore_nmb;
  T_RFS_RET ret; 
  T_RFS_SIZE  size_to_read;


  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;

  client_pair_value.pair_id = req_msg_p->pair_value.pair_id;
  client_pair_value.fd = 0;

  /* IF invalid dirp THEN complain to client immediately */
  fscore_nmb = req_msg_p->dirp->fscore_nmb;

  if(fscore_nmb == 0xFF)   /* its root directory */
  	{

      if(req_msg_p->dirp->opendir_ino >= RFS_NMB_FSCORE)
      	{
      	  ret = 0;   /* Nothing to read */
      	}
	  else
	  	{
	  	   size_to_read = (req_msg_p->size > GBI_MAX_PARTITION_NAME)?GBI_MAX_PARTITION_NAME:req_msg_p->size;

          if(req_msg_p->dirp->lastread_ino >= rfs_env_ctrl_blk_p->fscore_part_table[req_msg_p->dirp->opendir_ino].nbr_partitions)
          	{
          	   req_msg_p->dirp->lastread_ino = 0;
			   req_msg_p->dirp->opendir_ino++;
          	}

		   /* read only mounted partition name */
		   ret = 0;
           while ((req_msg_p->dirp->opendir_ino < RFS_NMB_FSCORE)&& (req_msg_p->dirp->lastread_ino < rfs_env_ctrl_blk_p->fscore_part_table[req_msg_p->dirp->opendir_ino].nbr_partitions))
           	{

				 if(rfs_env_ctrl_blk_p->fscore_part_table[req_msg_p->dirp->opendir_ino].partition_info_table[req_msg_p->dirp->lastread_ino].is_mounted)
				 	{

					   /* copy the paritition name */
					   convert_u8_to_unicode_length((const char*)rfs_env_ctrl_blk_p->fscore_part_table[req_msg_p->dirp->opendir_ino].partition_info_table[req_msg_p->dirp->lastread_ino].partition_name, req_msg_p->buf, size_to_read);
					   
					   req_msg_p->dirp->lastread_ino++;
					   ret = size_to_read;
					   break;
				 	}
				   else
				   	{
				   	   req_msg_p->dirp->lastread_ino++;
				   	}
				 	
           	}   /* end of while */
		 
	  	}   /* end of else */

	  rfs_send_result_to_client (RFS_READDIR_RSP, ret, client_pair_value, &client_return_path);
		 
	  return RFS_OK;
  
  	}


  
  if (fscore_nmb >= RFS_NMB_FSCORE)
	{
	  return rfs_send_result_to_client (RFS_READDIR_RSP, RFS_EINVALID,
										client_pair_value,
										&client_return_path);
	}


  /* call the appropriate 'readdir_nb' function for this mount point */
  core_pair_value.pair_id = -1;
  core_pair_value.fd = 0;
  rfs_core_dispatch (&core_pair_value.pair_id, fscore_nmb,
					 fscore_readdir_uc_nb ((T_FSCORE_DIR *) req_msg_p->dirp,
										req_msg_p->buf, req_msg_p->size,
										rfs_return_path.return_path));

  if (core_pair_value.pair_id >= 0)
	{
	  /* client pair id --> core pair id */
	  core_pair_value =
		rfs_map_pair_value (client_pair_value,
							core_pair_value /*unused */ ,
							&client_return_path, &fscore_nmb,
							RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
							RFS_PAIR_MAP_NEW);
	}
  else
	{
	  core_func_result = core_pair_value.pair_id;	/* pass mapping error */
	  rfs_send_result_to_client (RFS_READDIR_RSP, core_func_result,
								 client_pair_value, &client_return_path);

	}
  return RFS_OK;
}

/**
 * function: rfs_preformat_resp
 */
static T_RFS_RETURN
rfs_preformat_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_preformat_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_PREFORMAT_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_preformat_req
 */
T_RFS_RETURN
rfs_preformat_req (T_RV_HDR * msg_p)
{
  T_RFS_PREFORMAT_REQ_MSG *req_msg_p = (T_RFS_PREFORMAT_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;
 

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;
  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc (req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
	 	{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  //convert_unicode_to_u8(req_msg_p->pathname, dest_name_p);
	  /* todo: parameter check: return path */
	  /* call the appropriate 'preformat_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_preformat_uc_nb (req_msg_p->pathname,
											  req_msg_p->magic,
											  rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_PREFORMAT_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_format_resp
 */
static T_RFS_RETURN
rfs_format_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_format_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_FORMAT_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_format_req
 */
T_RFS_RETURN
rfs_format_req (T_RV_HDR * msg_p)
{
  T_RFS_FORMAT_REQ_MSG *req_msg_p = (T_RFS_FORMAT_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc (req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  /* todo: parameter check: return path */
	  /* call the appropriate 'format_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_format_uc_nb (req_msg_p->pathname,
										   req_msg_p->name, req_msg_p->magic,
										   rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_FORMAT_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_getattrib_resp
 */
static T_RFS_RETURN
rfs_getattrib_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_getattrib_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  if(core_ready_rsp_msg_p->result >= 0)
  	{
  	   core_ready_rsp_msg_p->result &=(FFS_ATTRIB_READWRITE|FFS_ATTRIB_READONLY|FFS_ATTRIB_HIDDEN|FFS_ATTRIB_SYSTEM|FFS_ATTRIB_PART|FFS_ATTRIB_SUBDIR|FFS_ATTRIB_ARCHIVE); 
  	}

  return rfs_send_result_to_client (RFS_GETATTRIB_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_getattib_req
 */
T_RFS_RETURN
rfs_getattrib_req (T_RV_HDR * msg_p)
{
  T_RFS_GETATTRIB_REQ_MSG *req_msg_p = (T_RFS_GETATTRIB_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;
	
  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

 
  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc (req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  //convert_unicode_to_u8(req_msg_p->pathname, dest_name_p);
	  /* todo: parameter check: return path */
	  /* call the appropriate 'format_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_getattrib_uc_nb (req_msg_p->pathname,
										   rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_GETATTRIB_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}
/**
 * function: rfs_setattrib_resp
 */
static T_RFS_RETURN
rfs_setattrib_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_setattrib_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_SETATTRIB_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_getattib_req
 */
T_RFS_RETURN
rfs_setattrib_req (T_RV_HDR * msg_p)
{
  T_RFS_SETATTRIB_REQ_MSG *req_msg_p = (T_RFS_SETATTRIB_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

  
  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc(req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
	 	{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  //convert_unicode_to_u8(req_msg_p->pathname, dest_name_p);
	  /* todo: parameter check: return path */
	  /* call the appropriate 'format_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_setattrib_uc_nb (req_msg_p->pathname,
										   req_msg_p->attr, 
										   rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_SETATTRIB_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

/**
 * function: rfs_getdatetime_resp
 */
static T_RFS_RETURN
rfs_getdatetime_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_getdatetime_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (    RFS_GETDATETIME_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

/**
 * function:  rfs_getdatetime_req
 */
T_RFS_RETURN
rfs_getdatetime_req (T_RV_HDR * msg_p)
{
  T_RFS_GETDATETIME_REQ_MSG *req_msg_p = (T_RFS_GETDATETIME_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;
 

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  core_func_result.fd = 0;

  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc(req_msg_p->pathname, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	 if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  //convert_unicode_to_u8(req_msg_p->pathname, dest_name_p);
	  /* todo: parameter check: return path */
	  /* call the appropriate 'format_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_getdatetime_uc_nb (req_msg_p->pathname,
										   req_msg_p->date_time_p,req_msg_p->crdate_time_p,  
										   rfs_return_path.return_path));
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_GETDATETIME_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}



/**
 * function: rfs_close_resp
 */
static T_RFS_RETURN rfs_mount_unmount_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;
  UINT16 i;
  T_RFS_CMD_ID rsp_id;

  RFS_SEND_TRACE ("rfs_mount_unmount_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  if(core_ready_rsp_msg_p->command_id == FSCORE_UNMOUNT_RSP)
  	{
  	  rsp_id = RFS_UNMOUNT_RSP;
  	}
  else
  	{
  	  rsp_id = RFS_MOUNT_RSP;
  	}
  return rfs_send_result_to_client (rsp_id,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}


static UINT8 RFS_get_fs_type(int fs_type)
{

 switch (fs_type)
 	{
      
	  case RFS_FAT:
   		  return RFS_FAT_NUM;

	  case RFS_RELIANCE:
	  	
#ifdef RFS_RELIANCE_NUM 	  	
            return RFS_RELIANCE_NUM;
#else
       return RFS_UNKNOWN_FS;
#endif         

      default: 
             return RFS_UNKNOWN_FS;
	  	  
 	}

 
}



/**
 * function:  rfs_getdatetime_req
 */
T_RFS_RETURN rfs_mount_unmount_req (T_RV_HDR * msg_p)
{
  T_RFS_MOUNT_UNMOUNT_REQ_MSG *req_msg_p = (T_RFS_MOUNT_UNMOUNT_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT8 				fscore_numb;
  T_RFS_CMD_ID rsp_id;
 

  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;
  

  do
	{							/* while (0) */

	  fscore_numb =  RFS_get_fs_type(req_msg_p->fs_type);

      if( fscore_numb == RFS_UNKNOWN_FS)
      	{
	     core_func_result.pair_id = RFS_EINVALID;
		 	 	
      	}
	  else
	  	{

 if(req_msg_p->op == RFS_UNMOUNT)
 	{

  /* call the appropriate 'format_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 fscore_numb,
						 fscore_unmount_nb(rfs_return_path.return_path));
	

	
 	}
 else
 	{
  /* call the appropriate 'format_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 fscore_numb,
						 fscore_mount_nb(rfs_return_path.return_path));
	
	
 	}
		
	
	
	  	}	  
	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  if(req_msg_p->op == RFS_UNMOUNT)
	  	{
	  	    rsp_id  = RFS_UNMOUNT_RSP;
	  	}
	  else
	  	{
	  	    rsp_id = RFS_MOUNT_RSP;
	  	}
	   	
	  rfs_send_result_to_client (rsp_id, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;
}

static T_RFS_RETURN
rfs_setlabel_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_setlabel_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_SETLABEL_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

T_RFS_RETURN rfs_setlabel_req (T_RV_HDR * msg_p)
{
  T_RFS_SET_LABEL_REQ_MSG *req_msg_p = (T_RFS_SET_LABEL_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;
  
  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;

  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc(req_msg_p->mpt_name, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	  if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}
	  
	  /* todo: parameter check: return path */
	  /* call the appropriate 'open_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_set_label_uc_nb (req_msg_p->mpt_name,
										 req_msg_p->label,  rfs_return_path.return_path));

	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_SETLABEL_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;

}

static T_RFS_RETURN
rfs_getlabel_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;

  RFS_SEND_TRACE ("rfs_getlabel_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

  return rfs_send_result_to_client (RFS_GETLABEL_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}

T_RFS_RETURN rfs_getlabel_req (T_RV_HDR * msg_p)
{
  T_RFS_GET_LABEL_REQ_MSG *req_msg_p = (T_RFS_GET_LABEL_REQ_MSG *) msg_p;
  T_RFS_PAIR_VALUE core_func_result;
  T_RFS_PAIR_VALUE client_pair_value = req_msg_p->pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  T_FSCORE_PARTITION_TABLE *mount_entry_p;
  UINT16 part_index=0;
  
  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;

  do
	{							/* while (0) */
	  /* find mount point for requested path */
	  mount_entry_p = rfs_get_mount_entry_by_pathname_uc(req_msg_p->mpt_name, &part_index);
	  if (mount_entry_p == NULL)
		{
		  core_func_result.pair_id = RFS_EINVALID;
		  break;
		}
	  if (mount_entry_p->partition_info_table[part_index].is_mounted != TRUE)
		{
		  core_func_result.pair_id = RFS_EMOUNT;
		  break;
		}

	  /* todo: parameter check: return path */
	  /* call the appropriate 'open_nb' function for this mount point */
	  rfs_core_dispatch (&core_func_result.pair_id,
						 mount_entry_p->fscore_numb,
						 fscore_get_label_uc_nb (req_msg_p->mpt_name,
						                                     req_msg_p->label,  rfs_return_path.return_path));

	}
  while (0);					//lint !e717

  /* IF parameters and core API call OK 
   * THEN 
   *   create mapping from client pair id to core pair id (ignore returned core pair id)
   * ELSE 
   *   complain to client immediately
   */

  if (core_func_result.pair_id >= 0)
	{
	  rfs_map_pair_value (req_msg_p->pair_value, core_func_result,
						  &client_return_path, &mount_entry_p->fscore_numb,
						  RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
						  RFS_PAIR_MAP_NEW);
	}
  else
	{
	  rfs_send_result_to_client (RFS_GETLABEL_RSP, core_func_result.pair_id,
								 client_pair_value, &client_return_path);
	}
  return RFS_OK;

}


/**
 * function: rfs_close_resp
 */
static T_RFS_RETURN
rfs_closedir_resp (T_FSCORE_READY_RSP_MSG * core_ready_rsp_msg_p)
{
  T_RFS_RETURN_PATH client_return_path;
  T_RFS_PAIR_VALUE client_pair_value;
  UINT16 i;

  RFS_SEND_TRACE ("rfs_closedir_resp", RV_TRACE_LEVEL_DEBUG_LOW);

  /* core pair id --> rfs client pair id */
  client_pair_value =
	rfs_map_pair_value (core_ready_rsp_msg_p->pair_value,
						core_ready_rsp_msg_p->pair_value /*unused */ ,
						&client_return_path,
						&core_ready_rsp_msg_p->fscore_nmb,
						RFS_PAIR_MAP_TO_CLIENT | RFS_PAIR_MAP_PAIR_ID |
						RFS_PAIR_MAP_DELETE);

   if(core_ready_rsp_msg_p->result >= 0)
  	{
  	  rfs_env_ctrl_blk_p->open_dir_cnt--;
  	}

  return rfs_send_result_to_client (RFS_CLOSEDIR_RSP,
									core_ready_rsp_msg_p->result,
									client_pair_value, &client_return_path);
}



/**
 * function:  rfs_closedir_req
 */
T_RFS_RETURN
rfs_closedir_req (T_RV_HDR * msg_p)
{
  T_RFS_CLOSEDIR_REQ_MSG *req_msg_p = (T_RFS_CLOSEDIR_REQ_MSG *) msg_p;
  T_FSCORE_RET core_func_result;
  T_RFS_PAIR_VALUE client_pair_value;
  T_RFS_PAIR_VALUE core_pair_value;
  T_RFS_RETURN_PATH client_return_path = rfs_default_return_path;
  T_RFS_RETURN_PATH rfs_return_path = rfs_default_return_path;
  UINT8 fscore_nmb;


  client_return_path.mutex = req_msg_p->mutex;
  client_return_path.result = req_msg_p->result;
  client_return_path.return_path = req_msg_p->return_path;
  rfs_return_path.return_path.addr_id = rfs_env_ctrl_blk_p->addr_id;

  client_pair_value.pair_id = req_msg_p->pair_value.pair_id;

  /* find core nmb for this dirp */
  fscore_nmb =  req_msg_p->dirp->fscore_nmb;
  /* IF invalid dirp THEN complain to client immediately */
  if (fscore_nmb >= RFS_NMB_FSCORE)
	{
	  return rfs_send_result_to_client (RFS_READDIR_RSP, RFS_OK,
										client_pair_value,
										&client_return_path);
	}


  /* call the appropriate 'readdir_nb' function for this mount point */
  core_pair_value.pair_id = -1;
  rfs_core_dispatch (&core_pair_value.pair_id, fscore_nmb,
					 fscore_closedir_uc_nb ((T_FSCORE_DIR *) req_msg_p->dirp,rfs_return_path.return_path));

  if (core_pair_value.pair_id >= 0)
	{
	  /* client pair id --> core pair id */
	  core_pair_value =
		rfs_map_pair_value (client_pair_value,
							core_pair_value /*unused */ ,
							&client_return_path, &fscore_nmb,
							RFS_PAIR_MAP_PAIR_ID | RFS_PAIR_MAP_TO_CORE |
							RFS_PAIR_MAP_NEW);
	}
  else
	{
	  core_func_result = core_pair_value.pair_id;	/* pass mapping error */
	  rfs_send_result_to_client (RFS_READDIR_RSP, core_func_result,
								 client_pair_value, &client_return_path);

	}
  return RFS_OK;
}



/* *INDENT-OFF* */
#if 0
/**
 * function:  rfs_set_fd_table_entry
 */	
static T_RFS_RET rfs_set_fd_table_entry(T_RFS_FD  fd)
{
  UINT8       tmp_entry;
  BOOL        fd_found = FALSE;  

  /* Check if file already openend */
  for (tmp_entry = 0; 
       ((tmp_entry < RFS_MAX_FS_CORES * RFS_MAX_NR_OPEN_FILES) && (fd_found == FALSE)); 
       tmp_entry++)
  {
    
    if (rfs_env_ctrl_blk_p->fd_table[tmp_entry].fd == fd)
    {
      /* File descriptor is openend and information is available */
      fd_found    = TRUE;
    }
  }
  
  if (fd_found == TRUE)
  {
    /* File descriptor is already openend */
    return RFS_ELOCKED;
  }

  /* File is not openen find free entry in file descriptor table  */
  for (tmp_entry = 0; 
       ((tmp_entry < RFS_MAX_FS_CORES * RFS_MAX_NR_OPEN_FILES) && (fd_found == FALSE)); 
       tmp_entry++)
  {    
    if (rfs_env_ctrl_blk_p->fd_table[tmp_entry].fd == RFS_FD_DEF_VALUE)
    {
      /* File descriptor is entry is free, set file descriptor in table */
      rfs_env_ctrl_blk_p->fd_table[tmp_entry].fd = fd;
      fd_found    = TRUE;
    }
  }
  
  if (fd_found == FALSE)
  {
    /* File descriptor is not set in table,  max number of used file descriptors 
     * is reached
     */
    return RFS_ENUMFD;
  }
  else
  {
    /* No problems occured, set file descriptor is returned */
    return fd;
  }
}
#endif
/* *INDENT-ON* */

T_RFS_RETURN
rfs_handle_core_ready_response (T_RV_HDR * msg_p)
{
  T_FSCORE_READY_RSP_MSG *fscore_ready_rsp_msg_p =
	(T_FSCORE_READY_RSP_MSG *) msg_p;
  T_RFS_RETURN ret = RFS_OK;

  switch (fscore_ready_rsp_msg_p->command_id)
	{
	case FSCORE_CHMOD_RSP:
	  ret = rfs_chmod_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_CLOSE_RSP:
	  ret = rfs_close_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_FCHMOD_RSP:
	  ret = rfs_fchmod_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_FORMAT_RSP:
	  ret = rfs_format_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_FSTAT_RSP:
	  ret = rfs_fstat_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_FSYNC_RSP:
	  ret = rfs_fsync_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_LSEEK_RSP:
	  ret = rfs_lseek_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_MKDIR_RSP:
	  ret = rfs_mkdir_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_OPENDIR_RSP:
	  ret = rfs_opendir_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_OPEN_RSP:
	  ret = rfs_open_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_PREFORMAT_RSP:
	  ret = rfs_preformat_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_READDIR_RSP:
	  ret = rfs_readdir_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_READ_RSP:
	  ret = rfs_read_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_REMOVE_RSP:
	  ret = rfs_remove_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_RENAME_RSP:
	  ret = rfs_rename_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_RMDIR_RSP:
	  ret = rfs_rmdir_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_STAT_RSP:
	  ret = rfs_stat_resp (fscore_ready_rsp_msg_p);
	  break;
	case FSCORE_WRITE_RSP:
	  ret = rfs_write_resp (fscore_ready_rsp_msg_p);
	  break;
    case FSCORE_GETATTRIB_RSP:
	   ret = rfs_getattrib_resp(fscore_ready_rsp_msg_p);
	   break;
	case FSCORE_SETATTRIB_RSP:
	   ret = rfs_setattrib_resp(fscore_ready_rsp_msg_p);
	   break;
    case FSCORE_GETDATETIME_RSP:
	   ret = rfs_getdatetime_resp(fscore_ready_rsp_msg_p);
	   break;

	case FSCORE_CLOSEDIR_RSP:
		ret = rfs_closedir_resp(fscore_ready_rsp_msg_p);
		break;

	case FSCORE_SET_VOL_LABEL_RSP:
		ret = rfs_setlabel_resp(fscore_ready_rsp_msg_p);
		break;

	case FSCORE_GET_VOL_LABEL_RSP:
		ret = rfs_getlabel_resp(fscore_ready_rsp_msg_p);
		break;

	case FSCORE_MOUNT_RSP:
	case FSCORE_UNMOUNT_RSP:
		ret = rfs_mount_unmount_resp(fscore_ready_rsp_msg_p);
		break;
		   
	  
	case FSCORE_SETPARTTBL_RSP:
	  RFS_SEND_TRACE ("RFS: unexpected FSCORE_SETPARTTBL_RSP",
					  RV_TRACE_LEVEL_ERROR);
	  ret = RFS_INTERNAL_ERR;
	  break;
	default:
	  RFS_SEND_TRACE ("RFS: unknown core response", RV_TRACE_LEVEL_ERROR);
	  ret = RFS_INTERNAL_ERR;
	}
  return ret;
}


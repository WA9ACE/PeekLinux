/**
 * @file   rfs_process_internal_msg.h
 *
 * Declaration of internal function for RFS.
 *
 * @author
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	1/23/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef _RFS_PROCESS_INTERNAL_MSG_H_
#define _RFS_PROCESS_INTERNAL_MSG_H_

#include "rfs/rfs_api.h"

/**
 * rfs_open_req
 *
 * Detailled description.
 * Processes the open request message
 *
 * @param   msg_p       .
 *
 * @return
 * @return
 */

extern T_RFS_RETURN rfs_media_manager_process_msg(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_close_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_write_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_read_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_lseek_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_fchmod_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_fstat_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_fsync_req(T_RV_HDR *msg_p);

extern T_RFS_RETURN rfs_open_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_chmod_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_stat_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_remove_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_rename_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_mkdir_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_rmdir_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_opendir_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_readdir_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_preformat_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_format_req(T_RV_HDR *msg_p);
extern T_RFS_RETURN rfs_getattrib_req (T_RV_HDR * msg_p);
extern T_RFS_RETURN rfs_setattrib_req (T_RV_HDR * msg_p);
extern T_RFS_RETURN rfs_getdatetime_req (T_RV_HDR * msg_p);
extern T_RFS_RETURN rfs_closedir_req (T_RV_HDR * msg_p);
extern T_RFS_RETURN rfs_setlabel_req (T_RV_HDR * msg_p);
extern T_RFS_RETURN rfs_getlabel_req (T_RV_HDR * msg_p);	

extern T_RFS_RETURN rfs_handle_core_ready_response(T_RV_HDR *msg_p);

/*@}*/
#endif /* #ifndef _RFS_PROCESS_INTERNAL_MSG_H_ */

/**
 * @file	gbi_pi_datalight_handle_message.h
 *
 * Plugin DATALIGHT: handle of request and response messages header file.
 *
 * @author	 ()
 * @version 0.1
*/

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *		 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __GBI_PI_DATALIGHT_HANDLE_MESSAGE_H_
#define __GBI_PI_DATALIGHT_HANDLE_MESSAGE_H_

T_GBI_RESULT	gbi_plugin_datalight_reg_multiple_msg(T_RV_HDR *msg_p, void **data_p, void **nmb_items_p,UINT32 req_msg_id);
T_GBI_RESULT	gbi_plugin_datalight_reg_msg(T_RV_HDR *msg_p);
T_GBI_RESULT  	gbi_plugin_datalight_rsp_msg(T_RV_HDR *msg_p);
extern T_GBI_RESULT  init_media_disks();

#endif //__GBI_PI_DATALIGHT_HANDLE_MESSAGE_H_



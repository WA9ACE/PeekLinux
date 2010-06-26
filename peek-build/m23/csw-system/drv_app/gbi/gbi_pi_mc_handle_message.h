/**
 * @file	gbi_pi_mc_handle_message.h
 *
 * Plugin MMC/SD: handle of request and response messages header file.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	03/16/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __GBI_PI_MC_HANDLE_MESSAGE_H_
#define __GBI_PI_MC_HANDLE_MESSAGE_H_

T_GBI_RESULT	gbi_plugin_mc_reg_multiple_msg(T_RV_HDR *msg_p, void **data_p, void **nmb_items_p, UINT32 req_msg_id);
T_GBI_RESULT	gbi_plugin_mc_reg_msg(T_RV_HDR *msg_p);
T_GBI_RESULT  gbi_plugin_mc_rsp_msg(T_RV_HDR *msg_p);

#endif //__GBI_PI_MC_HANDLE_MESSAGE_H_

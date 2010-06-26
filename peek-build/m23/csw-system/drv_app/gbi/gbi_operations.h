/**
 * @file	gbi_operations.h
 *
 * Operations definitions for the GBI instance.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	06/01/2004	 ()		Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#ifndef __GBI_OPERATIONS_H_
#define __GBI_OPERATIONS_H_


/**
 * @name Operations Functions
 */
/*@{*/

T_GBI_RESULT 	gbi_i_subscribe_event(T_RV_HDR *msg_p);

T_GBI_RESULT 	gbi_i_startup_event_req_msg(T_RV_HDR *msg_p);

BOOLEAN  			gbi_i_check_if_media_changed( UINT8 plugin_number);

void					gbi_i_media_change_event(UINT8 				 plugin_number,
																			 T_GBI_EVENTS	 event);
																 
void 					gbi_i_media_info_synchronised(UINT8 plugin_number);

T_GBI_RESULT 	gbi_i_generic_client_response(T_RV_HDR *rspmsg_p, 
																						T_RV_RETURN return_path);

T_RV_RET		 	gbi_i_set_timer(T_GBI_TIMER_REQ_TYPE 	timer_type,
															T_GBI_TIME_T 					time, 
															UINT8 								*id, 
															void 									*callback);
													
T_RV_RET			gbi_i_delete_timer(UINT8 id);

T_GBI_PLUGIN_INFO	gbi_i_get_plugin_status_info(UINT8 plugin_idx);

void 					gbi_i_set_plugin_status_info(UINT8								plugin_idx, 
											 										 T_GBI_PLUGIN_INFO		plugin_info);

void          gbi_i_media_change_event(UINT8         plugin_number,
                                       T_GBI_EVENTS  event);

T_GBI_FS_TYPE gbi_i_determine_fat_type(UINT8 *boot_sector_p);

/*@}*/


#endif /* __GBI_OPERATIONS_H_ */

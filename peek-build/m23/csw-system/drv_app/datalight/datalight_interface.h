/**
 * @file	datalight_interface.h
 *
 * DL handle_msg function, which is called when the SWE
 * receives a new message in its mailbox.
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
 */

#ifndef _DATALIGHT_INTERFACE_H_
#define _DATALIGHT_INTERFACE_H_


#include "datalight_api.h"
#include "datalight.h"

T_RV_RET dl_read_sync (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors,  UINT8 *data_p);
T_RV_RET dl_write_sync (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors,  UINT8 *data_p);
T_RV_RET dl_erase_sync (unsigned short disk_num,UINT32 start_sector, UINT32 number_of_sectors);

T_RV_RET dl_op_read_req(T_DL_READ_REQ_MSG *msg_p);

T_RV_RET dl_op_write_req(T_DL_WRITE_REQ_MSG *msg_p);

T_RV_RET dl_op_erase_req(T_DL_ERASE_REQ_MSG *msg_p);

static T_RV_RET dl_send_write_rsp (unsigned short disk_num,T_RV_RET    result,
                                   UINT8    *data_p,T_RV_RETURN  rp);
static T_RV_RET dl_send_erase_rsp (unsigned short disk_num,T_RV_RET  result,T_RV_RETURN  rp);
static T_RV_RET dl_send_read_rsp (unsigned short disk_num,T_RV_RET    result,
                                   UINT8     *data_p, T_RV_RETURN  rp);
static T_RV_RET dl_send_init_rsp(T_RV_RET    result, T_RV_RETURN  rp);
static T_RV_RET dl_send_response_to_client (T_RV_HDR    *rsp_p,
                                             T_RV_RETURN *return_path);
T_RVM_RETURN datalight_get_info (T_RVM_INFO_SWE * info_swe);

T_RVM_RETURN datalight_set_info (T_RVF_ADDR_ID addr_id,
			   T_RV_RETURN_PATH return_path[],
			   T_RVF_MB_ID bk_id_table[],
			   T_RVM_CB_FUNC call_back_error_ft);

extern unsigned long FlashFx_Disks_Init();



#endif /* _DATALIGHT_INTERFACE_H_ */


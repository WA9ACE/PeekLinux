/**
 * @file  gbi_pi_mc_handle_message.c
 *
 * Plugin MC/SD: handle of request and response messages.
 *
 * @author
 * @version 0.1
 */
/*
 * History:
 *
 *  Date        Author          Modification
 *  -------------------------------------------------------------------
 *  03/16/2004   ()   Create.
 *
 * (C) Copyright 2004 by ICT Embedded B.V., All Rights Reserved
 */

#include <string.h>

#include "gbi/gbi_i.h"
#include "gbi/gbi_pi_mc_i.h"
#include "gbi/gbi_mmc.h"
#include "gbi/gbi_operations.h"
#ifdef _WINDOWS
  #include "gbi/pc/gbi_pc_i.h"
#endif

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_gen.h"
#include "rvm/rvm_use_id_list.h"
#include "rvm/rvm_ext_use_id_list.h"

#include "mc/mc_api.h"
#include "mc/mc_message.h"

/* Message handling local functions */
static T_GBI_RESULT gbi_plugin_mc_handle_single_request(T_RV_HDR *msg_p);

static T_GBI_RESULT gbi_plugin_mc_handle_multiple_request(T_RV_HDR  *msg_p,
                                                          void      **data_p,
                                                          void      **nmb_items_p);

static T_GBI_RESULT gbi_plugin_mc_handle_response(T_RV_HDR *msg_p);


/* Generic plugin (MC and PC-sim) local functions */
static T_GBI_RESULT gbi_read_req(T_GBI_READ_REQ_MSG *msg_p);

static T_GBI_RESULT gbi_write_req(T_GBI_WRITE_REQ_MSG *msg_p);

static T_GBI_RESULT gbi_write_with_spare_req(T_GBI_WRITE_WITH_SPARE_REQ_MSG *msg_p);

static T_GBI_RESULT gbi_erase_req(T_GBI_ERASE_REQ_MSG *msg_p);

static T_GBI_RESULT gbi_flush_req(T_GBI_FLUSH_REQ_MSG *msg_p);

static T_GBI_RESULT gbi_get_media_info_req(T_GBI_MEDIA_INFO_REQ_MSG *msg_p,
                                           void **data_p,
                                           void **nmb_items_p);

static T_GBI_RESULT gbi_get_partition_info_req(T_GBI_PARTITION_INFO_REQ_MSG *msg_p,
                                               void **data_p,
                                               void **nmb_items_p);

static T_GBI_RESULT gbi_read_spare_data_req(T_GBI_READ_SPARE_DATA_REQ_MSG *msg_p);

static T_GBI_RESULT gbi_read_resp(T_GBI_RESULT read_result, T_RV_RETURN  return_path);

static T_GBI_RESULT gbi_write_resp(T_GBI_RESULT write_result, T_RV_RETURN  return_path);

static T_GBI_RESULT gbi_erase_resp(T_GBI_RESULT erase_result, T_RV_RETURN  return_path);

static T_GBI_RESULT gbi_flush_resp(T_GBI_RESULT flush_result, T_RV_RETURN  return_path);

static T_GBI_RESULT gbi_get_media_info_resp(T_GBI_RESULT      get_media_result,
                                            UINT8             nmb_of_media,
                                            T_GBI_MEDIA_INFO  *media_info_p,
                                            T_RV_RETURN       return_path);

static T_GBI_RESULT gbi_get_partition_info_resp(T_GBI_RESULT          get_partition_info_result,
                                                UINT8                 nmb_of_partitions,
                                                T_GBI_PARTITION_INFO  *partition_info_p,
                                                T_RV_RETURN           return_path);

static T_GBI_RESULT gbi_read_spare_data_resp(T_GBI_RESULT read_block_result,
                                             T_RV_RETURN  return_path);

static T_GBI_RESULT gbi_write_with_spare_resp(T_GBI_RESULT  write_block_result,
                                              T_RV_RETURN   return_path);


/* MC/SD related local functions */
static T_GBI_RESULT gbi_mmc_get_media_info_req(T_GBI_MEDIA_INFO_REQ_MSG *msg_p, void **data_p, void **nmb_items_p);
static T_GBI_RESULT gbi_mmc_get_partition_info_req(T_GBI_PARTITION_INFO_REQ_MSG *msg_p, void **data_p, void **nmb_items_p);
static T_RV_RET gbi_mmc_med_wt_for_cid_rsp(T_RV_HDR * msg_p);
static T_RV_RET gbi_mmc_med_wt_for_csd_rsp(T_RV_HDR * msg_p);
static T_RV_RET gbi_mmc_par_wt_for_csd_rsp(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_par_wt_for_data(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_get_media_info_request_buf(UINT8 nr_of_cards);
static void     gbi_mmc_free_media_info_request_buf_error(void);
static T_RV_RET gbi_mmc_read_resp(T_GBI_RESULT read_result, T_RV_RETURN  return_path);
static T_RV_RET gbi_mmc_write_resp(T_GBI_RESULT write_result, T_RV_RETURN  return_path);
static T_RV_RET gbi_mmc_erase_resp(T_GBI_RESULT erase_result, T_RV_RETURN return_path);
static T_RV_RET gbi_mmc_init_wt_for_subscr_rsp(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_init_wt_for_subscr_event_rsp(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_init_wt_for_aq_stck_rsp(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_init_wt_for_crd_stck_size_rsp(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_init_wt_for_retreive_rca(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_wt_for_read_rsp(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_wt_for_read_remainder_rsp(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_wt_for_write_rsp(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_wt_for_write_remainder_rsp(T_RV_HDR *msg_p);
static T_RV_RET gbi_mmc_wt_for_erase_rsp(T_RV_HDR *msg_p);

/* Support functions */
static UINT8 gbi_get_media_index(UINT8 media_nmb);
static UINT8 gbi_get_partition_index(UINT8 media_nmb, UINT8 partition_nmb);


/* Intialising media and partition data: PC simulation */
#ifdef _WINDOWS
static void format_partition_fat16(UINT32 *mem_p);
static void format_partition_fat32(UINT32 *mem_p);
static void init_media_partition_tables(void);

const	UINT8	fat32_block_0[512] =  /* boot sector, also in block 6! */
/*             0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
/* 0x0000:*/  {0xeb, 0x58, 0x90, 0x4d, 0x53, 0x44, 0x4f, 0x53, 0x35, 0x2e, 0x30, 0x00, 0x02, 0x01, 0x20, 0x00,
/* 0x0010:*/   0x02, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x3f, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0020:*/   0x00, 0xe5, 0x01, 0x00, /* Number of sectors: GBI_MEDIA_0_PAR_3_NMB_OF_BLOCKS*/
                                       0xbb, 0x03, 0x00, 0x00, /* Number of sectors in one FAT table */
                                                               0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
/* 0x0030:*/   0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0040:*/   0x00, 0x00, 0x29, 0x16, 0x3e, 0xec, 0x18, 0x4e, 0x4f, 0x20, 0x4e, 0x41, 0x4d, 0x45, 0x20, 0x20,
/* 0x0050:*/   0x20, 0x20, 0x46, 0x41, 0x54, 0x33, 0x32, 0x20, 0x20, 0x20, 0x33, 0xc9, 0x8e, 0xd1, 0xbc, 0xf4,
/* 0x0060:*/   0x7b, 0x8e, 0xc1, 0x8e, 0xd9, 0xbd, 0x00, 0x7c, 0x88, 0x4e, 0x02, 0x8a, 0x56, 0x40, 0xb4, 0x08,
/* 0x0070:*/   0xcd, 0x13, 0x73, 0x05, 0xb9, 0xff, 0xff, 0x8a, 0xf1, 0x66, 0x0f, 0xb6, 0xc6, 0x40, 0x66, 0x0f,
/* 0x0080:*/   0xb6, 0xd1, 0x80, 0xe2, 0x3f, 0xf7, 0xe2, 0x86, 0xcd, 0xc0, 0xed, 0x06, 0x41, 0x66, 0x0f, 0xb7,
/* 0x0090:*/   0xc9, 0x66, 0xf7, 0xe1, 0x66, 0x89, 0x46, 0xf8, 0x83, 0x7e, 0x16, 0x00, 0x75, 0x38, 0x83, 0x7e,
/* 0x00a0:*/   0x2a, 0x00, 0x77, 0x32, 0x66, 0x8b, 0x46, 0x1c, 0x66, 0x83, 0xc0, 0x0c, 0xbb, 0x00, 0x80, 0xb9,
/* 0x00b0:*/   0x01, 0x00, 0xe8, 0x2b, 0x00, 0xe9, 0x48, 0x03, 0xa0, 0xfa, 0x7d, 0xb4, 0x7d, 0x8b, 0xf0, 0xac,
/* 0x00c0:*/   0x84, 0xc0, 0x74, 0x17, 0x3c, 0xff, 0x74, 0x09, 0xb4, 0x0e, 0xbb, 0x07, 0x00, 0xcd, 0x10, 0xeb,
/* 0x00d0:*/   0xee, 0xa0, 0xfb, 0x7d, 0xeb, 0xe5, 0xa0, 0xf9, 0x7d, 0xeb, 0xe0, 0x98, 0xcd, 0x16, 0xcd, 0x19,
/* 0x00e0:*/   0x66, 0x60, 0x66, 0x3b, 0x46, 0xf8, 0x0f, 0x82, 0x4a, 0x00, 0x66, 0x6a, 0x00, 0x66, 0x50, 0x06,
/* 0x00f0:*/   0x53, 0x66, 0x68, 0x10, 0x00, 0x01, 0x00, 0x80, 0x7e, 0x02, 0x00, 0x0f, 0x85, 0x20, 0x00, 0xb4,
/* 0x0100:*/   0x41, 0xbb, 0xaa, 0x55, 0x8a, 0x56, 0x40, 0xcd, 0x13, 0x0f, 0x82, 0x1c, 0x00, 0x81, 0xfb, 0x55,
/* 0x0110:*/   0xaa, 0x0f, 0x85, 0x14, 0x00, 0xf6, 0xc1, 0x01, 0x0f, 0x84, 0x0d, 0x00, 0xfe, 0x46, 0x02, 0xb4,
/* 0x0120:*/   0x42, 0x8a, 0x56, 0x40, 0x8b, 0xf4, 0xcd, 0x13, 0xb0, 0xf9, 0x66, 0x58, 0x66, 0x58, 0x66, 0x58,
/* 0x0130:*/   0x66, 0x58, 0xeb, 0x2a, 0x66, 0x33, 0xd2, 0x66, 0x0f, 0xb7, 0x4e, 0x18, 0x66, 0xf7, 0xf1, 0xfe,
/* 0x0140:*/   0xc2, 0x8a, 0xca, 0x66, 0x8b, 0xd0, 0x66, 0xc1, 0xea, 0x10, 0xf7, 0x76, 0x1a, 0x86, 0xd6, 0x8a,
/* 0x0150:*/   0x56, 0x40, 0x8a, 0xe8, 0xc0, 0xe4, 0x06, 0x0a, 0xcc, 0xb8, 0x01, 0x02, 0xcd, 0x13, 0x66, 0x61,
/* 0x0160:*/   0x0f, 0x82, 0x54, 0xff, 0x81, 0xc3, 0x00, 0x02, 0x66, 0x40, 0x49, 0x0f, 0x85, 0x71, 0xff, 0xc3,
/* 0x0170:*/   0x4e, 0x54, 0x4c, 0x44, 0x52, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0180:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0190:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x01a0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0a, 0x4e, 0x54,
/* 0x01b0:*/   0x4c, 0x44, 0x52, 0x20, 0x69, 0x73, 0x20, 0x6d, 0x69, 0x73, 0x73, 0x69, 0x6e, 0x67, 0xff, 0x0d,
/* 0x01c0:*/   0x0a, 0x44, 0x69, 0x73, 0x6b, 0x20, 0x65, 0x72, 0x72, 0x6f, 0x72, 0xff, 0x0d, 0x0a, 0x50, 0x72,
/* 0x01d0:*/   0x65, 0x73, 0x73, 0x20, 0x61, 0x6e, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x20, 0x74, 0x6f, 0x20, 0x72,
/* 0x01e0:*/   0x65, 0x73, 0x74, 0x61, 0x72, 0x74, 0x0d, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x01f0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xac, 0xbf, 0xcc, 0x00, 0x00, 0x55, 0xaa};


/* block 1: FSinfo */
const	UINT8	fat32_block_1[512] =
/*             0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
/* 0x0000:*/  {0x52, 0x52, 0x61, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0010:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0020:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0030:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0040:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0050:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0060:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0070:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0080:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0090:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x00a0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x00b0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x00c0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x00d0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x00e0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x00f0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0100:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0110:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0120:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0130:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0140:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0150:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0160:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0170:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0180:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x0190:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x01a0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x01b0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x01c0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x01d0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0x01e0:*/   0x00, 0x00, 0x00, 0x00, 0x72, 0x72, 0x41, 0x61, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
/* 0x01f0:*/   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xaa};


/* FAT table 'start values'. REST OF FAT set to 0. */
const	UINT8	fat32_start_of_FAT_table[12] =
/*             0     1     2     3     4     5     6     7     8     9     A     B   */
/* 0x0000:*/  {0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f};


/* ROOT DIR, rest set to 0. */
const	UINT8	fat32_rootdir_table[32] =
/*             0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F   */
/* 0x0000 */  {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x20, 0x20, 0x20, 0x20, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00,
/* 0x0010 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9c, 0x72, 0x94, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};



#endif //_WINDOWS

/* Local definitions, used for remainder lenght handling (read and write) */
static UINT8           *remaining_block_p;
static T_GBI_BYTE_CNT  remaining_bytes_per_blocks;
static T_GBI_BYTE_CNT  remaining_bytes;
static UINT32          mc_addr;
static UINT8           *data_buffer;
static UINT8           media_nmb_in_progress;

/**
 * Component:    Plugin Request Handling
 * Sub-function: gbi_plugin_mc_reg_multiple_msg
 *
 * This function handles multiple plugin request messages concerning gathering
 * data.
 *
 * @param   msg_p   Pointer on the header of the message.
 * @param   void    Pointer for storage of data
 * @param   void    Pointer for storage of data
 * @return  GBI_OK, GBI_BUSY
 */
T_GBI_RESULT  gbi_plugin_mc_reg_multiple_msg(T_RV_HDR *msg_p, void **data_p, void **nmb_items_p, UINT32 req_msg_id)
{
  T_GBI_RESULT        result = GBI_OK;
  T_GBI_PLUGIN_INFO   plugin_info;

#ifndef _WINDOWS
  T_RV_RETURN_PATH    gbi_return_path;
#endif

  if (gbi_mc_env_ctrl_blk_p->plugin_status == TRUE)
  {
    /* Handle request */
//    GBI_SEND_TRACE("plugin_status: started", RV_TRACE_LEVEL_DEBUG_MEDIUM);

    plugin_info = gbi_i_get_plugin_status_info(PLUGIN_NMB_MC);
    if (plugin_info.plugin_state != GBI_STATE_READY)   /* dont send any message , if the plugin is not ready */
    {
      return GBI_BUSY;
    }

    result = gbi_plugin_mc_handle_multiple_request(msg_p, data_p, nmb_items_p);
  }
  else
  {
    GBI_SEND_TRACE("plugin_status: not started", RV_TRACE_LEVEL_DEBUG_MEDIUM);

    if (msg_p->msg_id == GBI_I_STARTUP_EVENT_REQ_MSG)
    {
      /* Handle startup message */

#ifdef _WINDOWS

      /* For PC simulation, skip MMC initialisation */
      init_media_partition_tables();

      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
      gbi_mc_env_ctrl_blk_p->plugin_status = TRUE;
#else

      /** Subscribe to the MC/SD driver*/
      GBI_SEND_TRACE_PARAM("GBI addr_id", gbi_env_ctrl_blk_p->addr_id, RV_TRACE_LEVEL_DEBUG_MEDIUM);

      gbi_return_path.addr_id           = gbi_env_ctrl_blk_p->addr_id;
      gbi_return_path.callback_func     = NULL;
      gbi_mc_env_ctrl_blk_p->subscriber = ~0;

      result = (T_GBI_RESULT)mc_subscribe(&gbi_mc_env_ctrl_blk_p->subscriber, gbi_return_path);
      if(result != GBI_OK)
      {
        return GBI_INTERNAL_ERR;
      }
      else
      {
        gbi_mc_env_ctrl_blk_p->state = GBI_MMC_INIT_WT_FOR_SUBSCR_RSP;
      }

#endif

    }
    else
    {

      if(gbi_mc_env_ctrl_blk_p->state == GBI_MMC_INTERNAL_ERROR)

      	{
        gbi_mc_env_ctrl_blk_p->card_stack_size=0;
    	gbi_mc_env_ctrl_blk_p->card_stack_p=NULL;
		*data_p=&gbi_mc_env_ctrl_blk_p->card_stack_p;
		*nmb_items_p=&gbi_mc_env_ctrl_blk_p->card_stack_size;
		}

      return GBI_OK;

    }
  }

//  GBI_SEND_TRACE_PARAM("result", result, RV_TRACE_LEVEL_DEBUG_MEDIUM);

  if (result == GBI_OK)
  {
	  
    /* Request is not completed, return processing */
    /* The MMC plugin needs to redesigned to handle this condition; This is a dirty fix */	  
    if (gbi_mc_env_ctrl_blk_p->state != GBI_MMC_IDLE)    
    {
      /* Save message data and set state for response handling, plugin busy */
	  
      /* If media and partition info request: save pointers for storing data */
      if ((msg_p->msg_id == GBI_MEDIA_INFO_REQ_MSG) ||
          (msg_p->msg_id == GBI_PARTITION_INFO_REQ_MSG))
      {
        /* These pointers are handed over by the GBI and should be point
         * the data memory when the plugin has processed it's request after
         * receiving all applicable responses of the driver
         */
        gbi_mc_env_ctrl_blk_p->store_data_p       = data_p;
        gbi_mc_env_ctrl_blk_p->store_nmb_items_p  = nmb_items_p;
      }

      plugin_info = gbi_i_get_plugin_status_info(PLUGIN_NMB_MC);
      plugin_info.plugin_state = GBI_STATE_PROCESSING;
      gbi_i_set_plugin_status_info(PLUGIN_NMB_MC, plugin_info);

      return GBI_PROCESSING;
    }
    else
    {
              return GBI_OK;	       
    }	    
      
  
  }
  else
  {
    /* Return error */
    return result;
  }
}

/**
 * Component:    Plugin Request Handling
 * Sub-function: gbi_plugin_mc_reg_msg
 *
 * This function handles all single plugin request messages
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK, GBI_BUSY
 */
T_GBI_RESULT  gbi_plugin_mc_reg_msg(  T_RV_HDR *msg_p)
{
  T_GBI_RESULT      result = GBI_OK;
  T_GBI_PLUGIN_INFO plugin_info;

  //GBI_SEND_TRACE("gbi_plugin_mc_reg_msg", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  if (gbi_mc_env_ctrl_blk_p->plugin_status == TRUE)
  {
    /* Handle single request */
//    GBI_SEND_TRACE("plugin_status: started", RV_TRACE_LEVEL_DEBUG_MEDIUM);

    plugin_info = gbi_i_get_plugin_status_info(PLUGIN_NMB_MC);
    if (plugin_info.plugin_state == GBI_STATE_PROCESSING)
    {
      return GBI_BUSY;
    }

    result = gbi_plugin_mc_handle_single_request(msg_p);
  }
  else
  {
      return GBI_BUSY;
  }

  if (result == GBI_OK)
  {
    /* Request is not completed, return processing */
    if (gbi_mc_env_ctrl_blk_p->state != GBI_MMC_IDLE)
    {
      plugin_info = gbi_i_get_plugin_status_info(PLUGIN_NMB_MC);
      plugin_info.plugin_state = GBI_STATE_PROCESSING;
      gbi_i_set_plugin_status_info(PLUGIN_NMB_MC, plugin_info);

      return GBI_PROCESSING;
    }
    else
    {
      /* Return GBI_OK */
      return GBI_OK;
    }
  }
  else
  {
    /* Return error */
    return result;
  }
}


/**
 * Component:    Plugin Response Handling
 * Sub-function: gbi_plugin_mc_rsp_msg
 *
 * This function handles all plugin response messages
 *
 * @param   msg_p   Pointer on the header of the message.
 * @return  GBI_OK
 */
T_GBI_RESULT gbi_plugin_mc_rsp_msg(  T_RV_HDR *msg_p)
{
  T_GBI_RESULT  retval = GBI_OK;

  //GBI_SEND_TRACE("gbi_plugin_mc_rsp_msg", RV_TRACE_LEVEL_DEBUG_MEDIUM);

  retval = gbi_plugin_mc_handle_response(msg_p);

  return retval;
}


/**
 * Local functions
 */
static T_GBI_RESULT gbi_plugin_mc_handle_single_request(T_RV_HDR *msg_p)
{
  T_GBI_RESULT retval = GBI_OK;

  if (msg_p != NULL)
  {
    switch (msg_p->msg_id)
    {
      case GBI_READ_REQ_MSG:

        retval = gbi_read_req ((T_GBI_READ_REQ_MSG *) msg_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_read_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      case GBI_WRITE_REQ_MSG:

        retval = gbi_write_req((T_GBI_WRITE_REQ_MSG *) msg_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_write_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      case GBI_ERASE_REQ_MSG:

        retval = gbi_erase_req((T_GBI_ERASE_REQ_MSG *) msg_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_erase_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      case GBI_FLUSH_REQ_MSG:

        retval = gbi_flush_req((T_GBI_FLUSH_REQ_MSG *) msg_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_flush_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      case GBI_READ_SPARE_DATA_REQ_MSG:

        retval = gbi_read_spare_data_req((T_GBI_READ_SPARE_DATA_REQ_MSG *) msg_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_read_spare_data_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      case GBI_WRITE_WITH_SPARE_REQ_MSG:

        retval = gbi_write_with_spare_req((T_GBI_WRITE_WITH_SPARE_REQ_MSG *) msg_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_write_spare_data_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      default:

        GBI_SEND_TRACE("GBI: gbi_plugin_mc_handle_single_request: Wrong request message", RV_TRACE_LEVEL_ERROR);
        retval = GBI_INTERNAL_ERR;

        break;
    }
  }

  return retval;
}


/**
 * Local functions
 */
static T_GBI_RESULT gbi_plugin_mc_handle_multiple_request(T_RV_HDR *msg_p, void **data_p, void **nmb_items_p)
{
  T_GBI_RESULT retval = GBI_OK;

  if (msg_p != NULL)
  {
    switch (msg_p->msg_id)
    {
      case GBI_MEDIA_INFO_REQ_MSG:

        retval = gbi_get_media_info_req((T_GBI_MEDIA_INFO_REQ_MSG *) msg_p, data_p, nmb_items_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_get_media_info_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      case GBI_PARTITION_INFO_REQ_MSG:
        retval = gbi_get_partition_info_req((T_GBI_PARTITION_INFO_REQ_MSG *) msg_p, data_p, nmb_items_p);
        if (retval != GBI_OK)
        {
          GBI_SEND_TRACE("GBI: gbi_get_partition_info_req failed", RV_TRACE_LEVEL_ERROR);
        }

        break;

      default:

        GBI_SEND_TRACE("GBI: gbi_plugin_mc_handle_multiple_request: Wrong request message", RV_TRACE_LEVEL_ERROR);
        retval = GBI_INTERNAL_ERR;

        break;
    }
  }

  return retval;
}

static T_GBI_RESULT gbi_plugin_mc_handle_response(T_RV_HDR *msg_p)
{
  T_RVM_RETURN        retval     = RVM_OK;
  T_GBI_RESULT        gbi_status = GBI_OK;
  T_MC_EVENT_IND_MSG *rsp;

  rsp = (T_MC_EVENT_IND_MSG *) msg_p;

/*
  GBI_SEND_TRACE("GBI: gbi_plugin_mc_handle_response", RV_TRACE_LEVEL_DEBUG_MEDIUM);
  GBI_SEND_TRACE_PARAM("msg_p->msg_id", msg_p->msg_id, RV_TRACE_LEVEL_DEBUG_MEDIUM);
  GBI_SEND_TRACE_PARAM("gbi_mc_env_ctrl_blk_p->state", gbi_mc_env_ctrl_blk_p->state, RV_TRACE_LEVEL_DEBUG_MEDIUM);
  GBI_SEND_TRACE_PARAM("rsp->event", rsp->event, RV_TRACE_LEVEL_DEBUG_MEDIUM);
*/

  /* Insertion and removal */
  if (msg_p->msg_id == MC_EVENT_IND_MSG)
  {
    if (rsp->event == MC_EVENT_INSERTION)
    {
      gbi_i_media_change_event(GBI_PLUGIN_NMB_0, GBI_EVENT_MEDIA_INSERT);
    }
    else if (rsp->event == MC_EVENT_REMOVAL)
    {
      gbi_i_media_change_event(GBI_PLUGIN_NMB_0, GBI_EVENT_MEDIA_REMOVEAL);
    }
    else
    {
      GBI_SEND_TRACE("GBI: Unknown MC Event", RV_TRACE_LEVEL_ERROR);
    }

    return GBI_OK;
  }

  switch(gbi_mc_env_ctrl_blk_p->state)
  {
    case GBI_MMC_NOT_INITIALISED:
      break;
    case GBI_MMC_INIT_WT_FOR_SUBSCR_RSP:
      retval = gbi_mmc_init_wt_for_subscr_rsp(msg_p);
      break;
    case GBI_MMC_INIT_WT_FOR_SUBSCR_EVENT_RSP:
    retval = gbi_mmc_init_wt_for_subscr_event_rsp(msg_p);
      break;
    case GBI_MMC_INIT_WT_FOR_AQ_STCK_RSP:
      retval = gbi_mmc_init_wt_for_aq_stck_rsp(msg_p);
      break;
    case GBI_MMC_INIT_WT_FOR_CRD_STCK_SIZE_RSP:
      retval = gbi_mmc_init_wt_for_crd_stck_size_rsp(msg_p);
      break;
    case GBI_MMC_INIT_WT_FOR_RETREIVE_RCA:
      retval = gbi_mmc_init_wt_for_retreive_rca(msg_p);
      break;
    case  GBI_MMC_WT_FOR_READ_RSP:
      retval = gbi_mmc_wt_for_read_rsp(msg_p);
      break;
    case  GBI_MMC_WT_FOR_READ_REMAINDER_RSP:
      retval = gbi_mmc_wt_for_read_remainder_rsp(msg_p);
      break;
    case  GBI_MMC_WT_FOR_WRITE_RSP:
      retval = gbi_mmc_wt_for_write_rsp(msg_p);
      break;
    case  GBI_MMC_WT_FOR_WRITE_REMAINDER_RSP:
      retval = gbi_mmc_wt_for_write_remainder_rsp(msg_p);
      break;
    case  GBI_MMC_WT_FOR_ERASE_RSP:
      retval = gbi_mmc_wt_for_erase_rsp(msg_p);
      break;
    case GBI_MMC_MED_WT_FOR_CID:
      retval = gbi_mmc_med_wt_for_cid_rsp(msg_p);
      break;
    case GBI_MMC_MED_WT_FOR_CSD:
      retval = gbi_mmc_med_wt_for_csd_rsp(msg_p);
      break;
    case GBI_MMC_PAR_WT_FOR_CSD:
      retval = gbi_mmc_par_wt_for_csd_rsp(msg_p);
      break;
    case GBI_MMC_PAR_WT_FOR_DATA:
      retval = gbi_mmc_par_wt_for_data(msg_p);
      break;

      /* Other state, fall through */
    case GBI_MMC_INTERNAL_ERROR:
    case GBI_MMC_IDLE:
    case GBI_MMC_INIT_WT_FOR_SND_NOTIF:
    default:
      break;
  }

  /* Plugin should return gbi status */
  if (retval == RV_OK)
  {
    if (gbi_mc_env_ctrl_blk_p->state != GBI_MMC_IDLE)
    {
      gbi_status = GBI_PROCESSING;
    }
    else
    {
      gbi_status = GBI_OK;
    }
  }
  else
  {
    gbi_status = GBI_INTERNAL_ERR;
  }

//  GBI_SEND_TRACE_PARAM("gbi_status", gbi_status, RV_TRACE_LEVEL_DEBUG_MEDIUM);

  return gbi_status;
}


/****************************************************************/
/* Generic Plugin functions (both MC and PC-sim)                */
/* The MC specific functions can be found in this module, the   */
/* PC-sim specific functions, can be found in gbi_pc_i.c        */
/****************************************************************/

/**
 * function: gbi_read_req
 */
static T_GBI_RESULT gbi_read_req(T_GBI_READ_REQ_MSG *msg_p)
{
  T_GBI_RESULT    retval = GBI_OK;
  T_GBI_BLOCK     media_number_of_blocks;
  T_GBI_BYTE_CNT  media_bytes_per_blocks;
  T_GBI_BLOCK     part_first_block_nmb;
  UINT8           partition_index;
  T_GBI_PARTITION_INFO *part_data_p = NULL;

  part_data_p = gbi_mc_env_ctrl_blk_p->requested_partition_info_p;
  if (part_data_p == NULL)
  {
    /** Invalid part_data_p */
    GBI_SEND_TRACE ("GBI parameter: part_data_p is NULL", RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_resp(GBI_NOT_READY, msg_p->return_path);
    return retval;
  }

  /* Retrieve necessary media and partition data */
  partition_index = gbi_get_partition_index(msg_p->media_nmb, msg_p->partition_nmb);

  if (partition_index == INVALID_INDEX)
  {
    /** Invalid media_nmb or partition_nmb */
    GBI_SEND_TRACE ("GBI parameter: Invalid media_nmb or partition_nmb",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  part_data_p = part_data_p + partition_index;

  media_number_of_blocks = part_data_p->nmb_of_blocks;
  media_bytes_per_blocks = part_data_p->bytes_per_block;
  part_first_block_nmb   = part_data_p->first_block_nmb;

  if (msg_p->buffer_p == NULL)
  {
    /* buffer_p is a null pointer */
    GBI_SEND_TRACE ("GBI parameter: buffer_p not valid", RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if (msg_p->first_block_nmb >= media_number_of_blocks)
  {
    /* first_block out of range */
    GBI_SEND_TRACE ("GBI parameter: first_block_nmb out of range",
      RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if (msg_p->first_block_nmb + msg_p->number_of_blocks > media_number_of_blocks)
  {
    GBI_SEND_TRACE ("GBI parameter: number_of_blocks out of range",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if ((msg_p->first_block_nmb + msg_p->number_of_blocks == media_number_of_blocks) &&
      (msg_p->remainder_length > 0))
  {
      /*  Last block is read and there is still a remainder_length to read  */
      GBI_SEND_TRACE ("GBI parameter: remainder_length out of range ",
          RV_TRACE_LEVEL_WARNING);

      retval = gbi_read_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
      return retval;
  }

  if (msg_p->remainder_length > media_bytes_per_blocks)
  {
    /*  remainder_length out of range */
    GBI_SEND_TRACE ("GBI parameter: remainder_length out of range ",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

#ifdef MMC_PLUGIN_USED
  retval = gbi_mmc_hndl_read_req(msg_p, media_bytes_per_blocks, part_first_block_nmb);
#else

#ifdef _WINDOWS

  retval = gbi_pc_read(msg_p, media_bytes_per_blocks);
  if (retval != GBI_OK)
  {
    /* Response message information */
    retval = gbi_read_resp(GBI_INTERNAL_ERR, msg_p->return_path);
    return retval;
  }

#endif  // _WINDOWS

  retval = gbi_read_resp(GBI_OK, msg_p->return_path);
#endif //MMC_PLUGIN_USED
  return retval;
}

/**
 * function: gbi_write_req
 */
static T_GBI_RESULT gbi_write_req(T_GBI_WRITE_REQ_MSG *msg_p)
{
  T_GBI_RESULT    retval = GBI_OK;
  T_GBI_BLOCK     media_number_of_blocks;
  T_GBI_BYTE_CNT  media_bytes_per_blocks;
  T_GBI_BLOCK     part_first_block_nmb;
  UINT8           partition_index;
  T_GBI_PARTITION_INFO *part_data_p = NULL;

  part_data_p = gbi_mc_env_ctrl_blk_p->requested_partition_info_p;
  if (part_data_p == NULL)
  {
    /** Invalid part_data_p */
    GBI_SEND_TRACE ("GBI parameter: part_data_p is NULL", RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_resp(GBI_NOT_READY, msg_p->return_path);
    return retval;
  }


  /* Retrieve necessary media and partition data */
  partition_index = gbi_get_partition_index(msg_p->media_nmb, msg_p->partition_nmb);

  if (partition_index == INVALID_INDEX)
  {
    /** Invalid media_nmb or partition_nmb */
    GBI_SEND_TRACE ("GBI parameter: Invalid media_nmb or partition_nmb",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  part_data_p = part_data_p + partition_index;

  media_number_of_blocks = part_data_p->nmb_of_blocks;
  media_bytes_per_blocks = part_data_p->bytes_per_block;
  part_first_block_nmb   = part_data_p->first_block_nmb;

  if (msg_p->first_block_nmb >= media_number_of_blocks)
  {
    /* first_block_nmb out of range */
    GBI_SEND_TRACE ("GBI parameter: first_block_nmb out of range",
      RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if (msg_p->first_block_nmb + msg_p->number_of_blocks > media_number_of_blocks)
  {
    GBI_SEND_TRACE ("GBI parameter: number_of_blocks out of range",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if ((msg_p->first_block_nmb + msg_p->number_of_blocks == media_number_of_blocks) &&
      (msg_p->remainder_length > 0))
  {
      /*  Last block is written and there is still a remainder_length to write  */
      GBI_SEND_TRACE ("GBI parameter: remainder_length out of range ",
          RV_TRACE_LEVEL_WARNING);

      retval = gbi_write_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
      return retval;
  }

  if (msg_p->remainder_length > media_bytes_per_blocks)
  {
    /*  remainder_length out of range */
    GBI_SEND_TRACE ("GBI parameter: remainder_length out of range ",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if (msg_p->buffer_p == NULL)
  {
    /* buffer_p is a null pointer */
    GBI_SEND_TRACE ("GBI API: buffer_p not valid", RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

#ifdef MMC_PLUGIN_USED
  retval = gbi_mmc_hndl_write_req(msg_p, media_bytes_per_blocks,part_first_block_nmb);

#else

#ifdef _WINDOWS

  retval = gbi_pc_write(msg_p, media_bytes_per_blocks);
  if (retval != GBI_OK)
  {
    /* Response message information */
    retval = gbi_write_resp(GBI_INTERNAL_ERR, msg_p->return_path);
    return retval;
  }

#endif  // _WINDOWS

  retval = gbi_write_resp(GBI_OK, msg_p->return_path);
#endif //MMC_PLUGIN_USED
  return retval;
}

/**
 * function: gbi_erase_req
 */
static T_GBI_RESULT gbi_erase_req(T_GBI_ERASE_REQ_MSG *msg_p)
{
  T_GBI_RESULT    retval = GBI_OK;
  T_GBI_BLOCK     media_number_of_blocks;
  T_GBI_BYTE_CNT  media_bytes_per_blocks;
  T_GBI_BLOCK     part_first_block_nmb;
  UINT8           partition_index;
  T_GBI_PARTITION_INFO *part_data_p = NULL;

  part_data_p = gbi_mc_env_ctrl_blk_p->requested_partition_info_p;
  if (part_data_p == NULL)
  {
    /** Invalid part_data_p */
    GBI_SEND_TRACE ("GBI parameter: part_data_p is NULL", RV_TRACE_LEVEL_WARNING);

    retval = gbi_erase_resp(GBI_NOT_READY, msg_p->return_path);
    return retval;
  }

  /* Retrieve necessary media and partition data */
  partition_index = gbi_get_partition_index(msg_p->media_nmb, msg_p->partition_nmb);

  if (partition_index == INVALID_INDEX)
  {
    /** Invalid media_nmb or partition_nmb */
    GBI_SEND_TRACE ("GBI parameter: Invalid media_nmb or partition_nmb",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_erase_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  part_data_p = part_data_p + partition_index;

  media_number_of_blocks = part_data_p->nmb_of_blocks;
  media_bytes_per_blocks = part_data_p->bytes_per_block;
  part_first_block_nmb   = part_data_p->first_block_nmb;

  if (msg_p->first_block_nmb >= media_number_of_blocks)
  {
    /* first_block out of range */
    GBI_SEND_TRACE ("GBI parameter: first_block_nmb out of range",
      RV_TRACE_LEVEL_WARNING);

    retval = gbi_erase_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if (msg_p->first_block_nmb + msg_p->number_of_blocks > media_number_of_blocks)
  {
    GBI_SEND_TRACE ("GBI parameter: number_of_blocks out of range",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_erase_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }


#ifdef MMC_PLUGIN_USED
  retval = gbi_mmc_hndl_erase_req(msg_p, media_bytes_per_blocks,part_first_block_nmb);

#else

#ifdef _WINDOWS

  retval = gbi_pc_erase(msg_p, media_bytes_per_blocks);
  if (retval != GBI_OK)
  {
    /* Response message information */
    retval = gbi_erase_resp(GBI_INTERNAL_ERR, msg_p->return_path);
    return retval;
  }

#endif  // _WINDOWS

  retval = gbi_erase_resp(GBI_OK,  msg_p->return_path);
#endif //MMC_PLUGIN_USED
  return retval;
}


/**
 * function: gbi_flush_req
 */
static T_GBI_RESULT gbi_flush_req(T_GBI_FLUSH_REQ_MSG *msg_p)
{
  T_GBI_RESULT    retval = GBI_OK;
  UINT8           partition_index;

  /* Retrieve necessary media and partition data */
  partition_index = gbi_get_partition_index(msg_p->media_nmb, msg_p->partition_nmb);

  if (partition_index == INVALID_INDEX)
  {
    /** Invalid media_nmb or partition_nmb */
    GBI_SEND_TRACE ("GBI parameter: Invalid media_nmb or partition_nmb",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_flush_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

#ifdef _WINDOWS

  retval = gbi_pc_flush(msg_p);
  if (retval != GBI_OK)
  {
    /* Response message information */
    retval = gbi_flush_resp(GBI_INTERNAL_ERR, msg_p->return_path);
    return retval;
  }

#endif  // _WINDOWS

  retval = gbi_flush_resp(GBI_OK,  msg_p->return_path);
  return retval;
}

/**
 * function: gbi_get_media_info_req
 */
static T_GBI_RESULT gbi_get_media_info_req(T_GBI_MEDIA_INFO_REQ_MSG *msg_p,
                                           void **data_p,
                                           void **nmb_items_p)
{
  T_GBI_RESULT        retval = GBI_OK;

#ifdef MMC_PLUGIN_USED
  retval = gbi_mmc_get_media_info_req(msg_p, data_p, nmb_items_p);
#else

#ifdef _WINDOWS

  /* The PC simulation is stored in the GBI control block */
  *data_p       = &gbi_env_ctrl_blk_p->media_table[0];
  *nmb_items_p  = &gbi_env_ctrl_blk_p->number_of_media;

#endif  // _WINDOWS

#endif //MMC_PLUGIN_USED


  return retval;
}

/**
 * function: gbi_get_partition_info_req
 */
static T_GBI_RESULT gbi_get_partition_info_req(T_GBI_PARTITION_INFO_REQ_MSG *msg_p,
                                           void **data_p,
                                           void **nmb_items_p)
{
  T_GBI_RESULT        retval = GBI_OK;

#ifdef MMC_PLUGIN_USED
  retval = gbi_mmc_get_partition_info_req(msg_p, data_p, nmb_items_p);
#else

#ifdef _WINDOWS

  /* The PC simulation is stored in the GBI control block */

  /* Let given pointers point to according data */
  //note: possible optimisation.
  *data_p       = &gbi_env_ctrl_blk_p->partition_table[0];
  data_p++;
  *data_p       = &gbi_env_ctrl_blk_p->partition_table[1];
  data_p++;
  *data_p       = &gbi_env_ctrl_blk_p->partition_table[2];
  data_p++;
  *data_p       = &gbi_env_ctrl_blk_p->partition_table[3];
  *nmb_items_p  = &gbi_env_ctrl_blk_p->number_of_partitions;

#endif  // _WINDOWS

#endif //MMC_PLUGIN_USED

  return retval;
}

/**
 * function: gbi_read_spare_data_req
 */
static T_GBI_RESULT gbi_read_spare_data_req(T_GBI_READ_SPARE_DATA_REQ_MSG *msg_p)
{
  T_GBI_RESULT    retval = GBI_OK;
  T_GBI_BLOCK     media_number_of_blocks;
  UINT8           media_spare_area_size;
  UINT8           partition_index;
  UINT8           media_index;
  T_GBI_MEDIA_INFO      *media_data_p = NULL;
  T_GBI_PARTITION_INFO  *part_data_p = NULL;

#ifdef _WINDOWS
  T_GBI_BYTE_CNT  media_bytes_per_blocks;
#endif


  media_data_p = gbi_mc_env_ctrl_blk_p->requested_media_info_p;
  if (media_data_p == NULL)
  {
    /** Invalid media_data_p */
    GBI_SEND_TRACE ("GBI parameter: media_data_p is NULL", RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_spare_data_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  part_data_p = gbi_mc_env_ctrl_blk_p->requested_partition_info_p;
  if (part_data_p == NULL)
  {
    /** Invalid part_data_p */
    GBI_SEND_TRACE ("GBI parameter: part_data_p is NULL", RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_spare_data_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  /* Retrieve necessary media and partition data */
  media_index     = gbi_get_media_index(msg_p->media_nmb);
  partition_index = gbi_get_partition_index(msg_p->media_nmb, msg_p->partition_nmb);

  if (media_index == INVALID_INDEX || partition_index == INVALID_INDEX)
  {
    /** Invalid media_nmb or partition_nmb */
    GBI_SEND_TRACE ("GBI parameter: Invalid media_nmb or partition_nmb",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_spare_data_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  media_data_p = media_data_p + media_index;
  part_data_p = part_data_p + partition_index;

  media_number_of_blocks = part_data_p->nmb_of_blocks;
  media_spare_area_size  = media_data_p->specific.spare_area_size;

  if (media_spare_area_size == GBI_MEDIA_SPARE_AREA_NONE)
  {
    /* The driver does not support the spare data feature.*/
    GBI_SEND_TRACE ("GBI parameter: The driver does not support the spare data feature.",
      RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_spare_data_resp(RV_NOT_SUPPORTED, msg_p->return_path);
    return retval;
  }

  if (msg_p->first_block >= media_number_of_blocks)
  {
    /* first_block out of range */
    GBI_SEND_TRACE ("GBI parameter: first_block out of range",
      RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_spare_data_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if (msg_p->first_block + msg_p->number_of_blocks > media_number_of_blocks)
  {
    /* number_of_blocks is out of range */
    GBI_SEND_TRACE ("GBI parameter: number_of_blocks out of range",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_spare_data_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if (msg_p->info_data_p == NULL)
  {
    /* info_data_p is a null pointer */
    GBI_SEND_TRACE ("GBI parameter: info_data_p not valid", RV_TRACE_LEVEL_WARNING);

    retval = gbi_read_spare_data_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

#ifdef _WINDOWS
  media_bytes_per_blocks = part_data_p->bytes_per_block;

  retval = gbi_pc_read_spare_data(msg_p, media_bytes_per_blocks, media_spare_area_size);
  if (retval != GBI_OK)
  {
    /* Response message information */
    retval = gbi_read_spare_data_resp(GBI_INTERNAL_ERR, msg_p->return_path);
    return retval;
  }

#endif  // _WINDOWS

  /* Response message information */
  retval = gbi_read_spare_data_resp(GBI_OK, msg_p->return_path);
  return retval;
}

/**
 * function: gbi_write_spare_data_req
 */
static T_GBI_RESULT gbi_write_with_spare_req(T_GBI_WRITE_WITH_SPARE_REQ_MSG *msg_p)
{
  T_GBI_RESULT    retval = GBI_OK;
  T_GBI_BLOCK     media_number_of_blocks;
  T_GBI_BYTE_CNT  media_bytes_per_blocks;
  T_GBI_BLOCK     part_first_block_nmb;
  UINT8           media_spare_area_size;
  UINT8           partition_index;
  T_GBI_MEDIA_INFO     *media_data_p = NULL;
  T_GBI_PARTITION_INFO *part_data_p = NULL;

  media_data_p = gbi_mc_env_ctrl_blk_p->requested_media_info_p;
  if (media_data_p == NULL)
  {
    /** Invalid media_data_p */
    GBI_SEND_TRACE ("GBI parameter: media_data_p is NULL", RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_with_spare_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  part_data_p = gbi_mc_env_ctrl_blk_p->requested_partition_info_p;
  if (part_data_p == NULL)
  {
    /** Invalid part_data_p */
    GBI_SEND_TRACE ("GBI parameter: part_data_p is NULL", RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_with_spare_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }


  /* Retrieve necessary media and partition data */
  partition_index = gbi_get_partition_index(msg_p->media_nmb, msg_p->partition_nmb);

  if (partition_index == INVALID_INDEX)
  {
    /** Invalid media_nmb or partition_nmb */
    GBI_SEND_TRACE ("GBI parameter: Invalid media_nmb or partition_nmb",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_with_spare_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  part_data_p = part_data_p + partition_index;

  media_number_of_blocks = part_data_p->nmb_of_blocks;
  media_bytes_per_blocks = part_data_p->bytes_per_block;
  part_first_block_nmb   = part_data_p->first_block_nmb;
  media_spare_area_size  = media_data_p->specific.spare_area_size;

  if (msg_p->first_block_nmb >= media_number_of_blocks)
  {
    /* first_block_nmb out of range */
    GBI_SEND_TRACE ("GBI parameter: first_block_nmb out of range",
      RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_with_spare_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if (msg_p->first_block_nmb + msg_p->number_of_blocks > media_number_of_blocks)
  {
    GBI_SEND_TRACE ("GBI parameter: number_of_blocks out of range",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_with_spare_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if ((msg_p->first_block_nmb + msg_p->number_of_blocks == media_number_of_blocks) &&
      (msg_p->remainder_length > 0))
  {
      /*  Last block is written and there is still a remainder_length to write  */
      GBI_SEND_TRACE ("GBI parameter: remainder_length out of range ",
          RV_TRACE_LEVEL_WARNING);

      retval = gbi_write_with_spare_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
      return retval;
  }

  if (msg_p->remainder_length > media_bytes_per_blocks)
  {
    /*  remainder_length out of range */
    GBI_SEND_TRACE ("GBI parameter: remainder_length out of range ",
        RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_with_spare_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

  if ((msg_p->data_buffer_p == NULL) || (msg_p->spare_buffer_p == NULL))
  {
    /* buffer_p is a null pointer */
    GBI_SEND_TRACE ("GBI API: buffer_p not valid", RV_TRACE_LEVEL_WARNING);

    retval = gbi_write_with_spare_resp(GBI_INVALID_PARAMETER, msg_p->return_path);
    return retval;
  }

#ifdef MMC_PLUGIN_USED
  retval = gbi_write_with_spare_resp(GBI_NOT_SUPPORTED, msg_p->return_path);
  return retval;
#else

#ifdef _WINDOWS

  retval = gbi_pc_write_with_spare(msg_p, media_bytes_per_blocks, media_spare_area_size);
  if (retval != GBI_OK)
  {
    /* Response message information */
    retval = gbi_write_with_spare_resp(GBI_INTERNAL_ERR, msg_p->return_path);
    return retval;
  }

#endif  // _WINDOWS

  retval = gbi_write_with_spare_resp(GBI_OK, msg_p->return_path);
return retval;

#endif //MMC_PLUGIN_USED

}


/**
 * function: gbi_read_resp
 */
static T_GBI_RESULT gbi_read_resp(T_GBI_RESULT read_result, T_RV_RETURN return_path)
{
  T_GBI_RESULT         retval = GBI_OK;
  T_GBI_READ_RSP_MSG   *rsp_p;

  /**
   * Create read response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_READ_RSP_MSG),
                       GBI_READ_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_read_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return GBI_MEMORY_ERR;
  }


  /* compose message */
  rsp_p->hdr.msg_id     = GBI_READ_RSP_MSG;
  rsp_p->result         = read_result;

  /* Send message mailbox */
  retval = gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_write_resp
 */
static T_GBI_RESULT gbi_write_resp(T_GBI_RESULT write_result, T_RV_RETURN return_path)
{
  T_GBI_RESULT         retval = GBI_OK;
  T_GBI_READ_RSP_MSG   *rsp_p;

  /**
   * Create write response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_WRITE_RSP_MSG),
                       GBI_WRITE_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_write_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return GBI_MEMORY_ERR;
  }

  /* compose message */
  rsp_p->hdr.msg_id     = GBI_WRITE_RSP_MSG;
  rsp_p->result         = write_result;

  /* Send message mailbox */
  retval = gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_erase_resp
 */
static T_GBI_RESULT gbi_erase_resp(T_GBI_RESULT erase_result, T_RV_RETURN return_path)
{
  T_GBI_RESULT             retval = GBI_OK;
  T_GBI_ERASE_RSP_MSG  *rsp_p;

  /**
   * Create erase response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_ERASE_RSP_MSG),
                       GBI_ERASE_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_erase_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return GBI_MEMORY_ERR;
  }

  /* compose message */
  rsp_p->hdr.msg_id     = GBI_ERASE_RSP_MSG;
  rsp_p->result         = erase_result;

  /* Send message mailbox */
  retval = gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_flush_resp
 */
static T_GBI_RESULT gbi_flush_resp(T_GBI_RESULT flush_result, T_RV_RETURN return_path)
{
  T_GBI_RESULT             retval = GBI_OK;
  T_GBI_FLUSH_RSP_MSG  *rsp_p;

  /**
   * Create flush response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_FLUSH_RSP_MSG),
                       GBI_FLUSH_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_flush_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return GBI_MEMORY_ERR;
  }

  /* compose message */
  rsp_p->hdr.msg_id     = GBI_FLUSH_RSP_MSG;
  rsp_p->result         = flush_result;


  /* Send message mailbox */
  retval = gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_get_media_info_resp
 */
static T_GBI_RESULT gbi_get_media_info_resp(T_GBI_RESULT  get_media_result,
                                        UINT8             nmb_of_media,
                                        T_GBI_MEDIA_INFO  *media_info_p,
                                        T_RV_RETURN       return_path)
{
  T_GBI_RESULT                  retval = GBI_OK;
  T_GBI_MEDIA_INFO_RSP_MSG  *rsp_p;

  GBI_SEND_TRACE_PARAM("gbi_get_media_info_resp MMC result", get_media_result, RV_TRACE_LEVEL_ERROR);

  /**
   * Create get media info response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_MEDIA_INFO_RSP_MSG),
                       GBI_MEDIA_INFO_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_get_media_info_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return GBI_MEMORY_ERR;
  }

  /* compose message */
  rsp_p->hdr.msg_id     = GBI_MEDIA_INFO_RSP_MSG;
  rsp_p->result         = get_media_result;
  rsp_p->nmb_of_media   = nmb_of_media;
  rsp_p->info_p         = media_info_p;

  /* Send message mailbox */
  retval = gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_get_partition_info_resp
 */
static T_GBI_RESULT gbi_get_partition_info_resp(
                                  T_GBI_RESULT          get_partition_info_result,
                                  UINT8                 nmb_of_partitions,
                                  T_GBI_PARTITION_INFO  *partition_info_p,
                                  T_RV_RETURN           return_path)
{
  T_GBI_RESULT                      retval = GBI_OK;
  T_GBI_PARTITION_INFO_RSP_MSG  *rsp_p;

  GBI_SEND_TRACE_PARAM("gbi_get_partition_info_resp MMC result", get_partition_info_result, RV_TRACE_LEVEL_ERROR);

  /**
   * Create get partition info response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_PARTITION_INFO_RSP_MSG),
                       GBI_PARTITION_INFO_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_get_partition_info_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return GBI_MEMORY_ERR;
  }

  /* compose message */
  rsp_p->hdr.msg_id         = GBI_PARTITION_INFO_RSP_MSG;
  rsp_p->nmb_of_partitions  = nmb_of_partitions;
  rsp_p->info_p             = partition_info_p;
  rsp_p->result             = get_partition_info_result;

  /* Send message mailbox */
  retval = gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_read_spare_data_resp
 */
static T_GBI_RESULT gbi_read_spare_data_resp(T_GBI_RESULT read_block_result,
                                             T_RV_RETURN  return_path)
{
  T_GBI_RESULT                        retval = GBI_OK;
  T_GBI_READ_SPARE_DATA_RSP_MSG   *rsp_p;

  /**
   * Create read block info response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_READ_SPARE_DATA_RSP_MSG),
                       GBI_READ_SPARE_DATA_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_read_spare_data_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return GBI_MEMORY_ERR;
  }

  /* compose message */
  rsp_p->hdr.msg_id     = GBI_READ_SPARE_DATA_RSP_MSG;
  rsp_p->result         = read_block_result;

  /* Send message mailbox */
  retval = gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_write_spare_data_resp
 */
static T_GBI_RESULT gbi_write_with_spare_resp(T_GBI_RESULT  write_block_result,
                                              T_RV_RETURN   return_path)
{
  T_GBI_RESULT                        retval = GBI_OK;
  T_GBI_WRITE_WITH_SPARE_RSP_MSG  *rsp_p;

  /**
   * Create write spare data response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_WRITE_WITH_SPARE_RSP_MSG),
                       GBI_WRITE_WITH_SPARE_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_write_with_spare_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return GBI_MEMORY_ERR;
  }

  /* compose message */
  rsp_p->hdr.msg_id     = GBI_WRITE_WITH_SPARE_RSP_MSG;
  rsp_p->result         = write_block_result;

  /* Send message mailbox */
  retval = gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}


/****************************************************************/
/*    MC Plugin functions: MC (MMC/SD related) functions        */
/****************************************************************/

/**
 * function: gbi_mmc_hndl_read_req
 */
T_RV_RET gbi_mmc_hndl_read_req(T_GBI_READ_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT   media_bytes_per_blocks,
                                 T_GBI_BLOCK      first_block_nmb)
{
  T_RV_RET      retval = RV_OK;
  UINT32        nr_read_bytes;

  /* Determine number of bytes to read */
  nr_read_bytes = (msg_p->number_of_blocks * media_bytes_per_blocks);

  /**calculate the physical addres */
  mc_addr = (first_block_nmb + msg_p->first_block_nmb)*media_bytes_per_blocks;

  //store current message
  gbi_mc_env_ctrl_blk_p->org_msg_retpath = msg_p->return_path;

  data_buffer = (UINT8*)msg_p->buffer_p;

  retval = mc_read (gbi_mc_env_ctrl_blk_p->card_stack_p[msg_p->media_nmb],
           MC_RW_BLOCK,
           mc_addr,
           data_buffer,
           nr_read_bytes,
           gbi_mc_env_ctrl_blk_p->subscriber);

  if(retval != RV_OK)
  {
    /**action failed so send response*/
    retval = gbi_mmc_read_resp(RV_INTERNAL_ERR, msg_p->return_path);

    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
  }
  else
  {
    if (msg_p->remainder_length > 0)
    {
      /* Prepare data for reading remainder data */
      mc_addr                    = mc_addr + nr_read_bytes;
      data_buffer                = data_buffer + nr_read_bytes;
      remaining_bytes_per_blocks = media_bytes_per_blocks;
      remaining_bytes            = msg_p->remainder_length;
      media_nmb_in_progress      = msg_p->media_nmb;

      /**action started now wait for result*/
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_WT_FOR_READ_RSP;
    }
    else
    {
      /* No remainder length, so skip the action and direclty
       * wait for the last read response
       */
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_WT_FOR_READ_REMAINDER_RSP;

      /* The next state function, is not reached. Set remaining pointer to
       * NULL
       */
      remaining_block_p = NULL;
      remaining_bytes   = 0;
    }
  }

  return retval;

}

/**
 * function: gbi_mmc_hndl_write_req
 */
T_RV_RET gbi_mmc_hndl_write_req(T_GBI_WRITE_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT     media_bytes_per_blocks,
                                 T_GBI_BLOCK      first_block_nmb)
{
  T_RV_RET      retval = RV_OK;
  UINT32        nr_write_bytes;


  /* Determine number of bytes to write */
  nr_write_bytes = (msg_p->number_of_blocks * media_bytes_per_blocks);

  /**calculate the physical addres */
  mc_addr = (first_block_nmb + msg_p->first_block_nmb)*media_bytes_per_blocks;

  gbi_mc_env_ctrl_blk_p->org_msg_retpath = msg_p->return_path;
  data_buffer = (UINT8*)msg_p->buffer_p;

  retval = mc_write (gbi_mc_env_ctrl_blk_p->card_stack_p[msg_p->media_nmb],
            MC_RW_BLOCK,
            mc_addr,
            data_buffer,
            nr_write_bytes,
            gbi_mc_env_ctrl_blk_p->subscriber);

  if(retval != RV_OK)
  {
    /**action failed so send response*/
    gbi_mmc_write_resp(RV_INTERNAL_ERR, msg_p->return_path);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
  }
  else
  {
    if (msg_p->remainder_length > 0)
    {
      /* Prepare data for writing remainder data */
      mc_addr                    = mc_addr + nr_write_bytes;
      data_buffer                = data_buffer + nr_write_bytes;
      remaining_bytes_per_blocks = media_bytes_per_blocks;
      remaining_bytes            = msg_p->remainder_length;
      media_nmb_in_progress      = msg_p->media_nmb;

      /**action started now wait for result*/
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_WT_FOR_WRITE_RSP;
    }
    else
    {
      /* No remainder length, so skip the action and direclty
       * wait for the last write response
       */
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_WT_FOR_WRITE_REMAINDER_RSP;

      /* The next state function, is not reached. Set remaining pointer to
       * NULL
       */
      remaining_block_p = NULL;
      remaining_bytes   = 0;
    }
  }

  return retval;
}

/**
 * function: gbi_mmc_hndl_erase_req
 */
T_RV_RET gbi_mmc_hndl_erase_req(T_GBI_ERASE_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT     media_bytes_per_blocks,
                                 T_GBI_BLOCK        first_block_nmb)
{
  T_RV_RET      retval = RV_OK;

  /* The erase function is not supported for MC plugin, because of
   * MC block/group erase is not usefull within GBI interface.
   */
  GBI_SEND_TRACE("Erase function is not supported for MC plugin", RV_TRACE_LEVEL_DEBUG_LOW);
  retval = gbi_mmc_erase_resp(GBI_NOT_SUPPORTED, msg_p->return_path);
  return retval;
}

/**
 * function: gbi_mmc_hndl_flush_req
 */
T_RV_RET gbi_mmc_hndl_flush_req(T_GBI_FLUSH_REQ_MSG *msg_p,
                                 T_GBI_BYTE_CNT     media_bytes_per_blocks,
                                 T_GBI_BLOCK      first_block_nmb)
{
  T_RV_RET retval =RV_OK;
  /**do nothing*/

  return retval;
}

/**
 * function: gbi_mmc_get_media_info_req
 */
static T_GBI_RESULT gbi_mmc_get_media_info_req(T_GBI_MEDIA_INFO_REQ_MSG *msg_p,
                                               void **data_p,
                                               void **nmb_items_p)
{
  T_GBI_RESULT  retval = GBI_OK;

  /**remove old card information*/
  if(gbi_mc_env_ctrl_blk_p->card_stack_size > 0)
  {
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->card_stack_p);
    gbi_mc_env_ctrl_blk_p->card_stack_p = NULL;
  }
  gbi_mc_env_ctrl_blk_p->card_stack_size = 0;

  /**retreive new nr of attached cards*/
        retval = mc_get_stack_size(&gbi_mc_env_ctrl_blk_p->card_stack_size);

  if(retval != RV_OK)
  {
    GBI_SEND_TRACE ("GBI: could not request stack size", RV_TRACE_LEVEL_WARNING);

    retval = gbi_get_media_info_resp(GBI_MEMORY_ERR, 0, NULL, msg_p->return_path);
    return retval;
  }

  if (gbi_mc_env_ctrl_blk_p->card_stack_size == 0)
  {
    GBI_SEND_TRACE ("GBI: card_stack_size is zero", RV_TRACE_LEVEL_WARNING);

    *data_p      = NULL;
    *nmb_items_p = NULL;

    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;

    return GBI_OK;
  }

  /**retreive memory and buffers for this action*/
  retval = gbi_mmc_get_media_info_request_buf(gbi_mc_env_ctrl_blk_p->card_stack_size);
  if(retval != RV_OK)
  {
    GBI_SEND_TRACE ("GBI: could not allocate memory", RV_TRACE_LEVEL_WARNING);
    retval = gbi_get_media_info_resp(GBI_MEMORY_ERR, 0, NULL, msg_p->return_path);
    return retval;
  }

  retval = mc_read_card_stack(gbi_mc_env_ctrl_blk_p->card_stack_p,   gbi_mc_env_ctrl_blk_p->card_stack_size);
  if(retval != RV_OK)
  {
    GBI_SEND_TRACE ("GBI: could not read card stack size", RV_TRACE_LEVEL_WARNING);
    gbi_mmc_free_media_info_request_buf_error();
    retval = gbi_get_media_info_resp(GBI_MEMORY_ERR, 0, NULL, msg_p->return_path);
    return retval;
  }


  /**all memory is available so start with retreival of the necesary data*/
  /**start with reading the CID from the first card */

  gbi_mc_env_ctrl_blk_p->requested_card = 0;
        retval = mc_read_CID(gbi_mc_env_ctrl_blk_p->card_stack_p[0],
                        gbi_mc_env_ctrl_blk_p->requested_cid_p,
                        gbi_mc_env_ctrl_blk_p->subscriber);

  if(retval != RV_OK)
  {
    GBI_SEND_TRACE ("GBI: could not read cid from mmc", RV_TRACE_LEVEL_WARNING);
    gbi_mmc_free_media_info_request_buf_error();
    retval = gbi_get_media_info_resp(GBI_MEMORY_ERR, 0, NULL, msg_p->return_path);
  }

  gbi_mc_env_ctrl_blk_p->org_msg_retpath = msg_p->return_path;
  gbi_mc_env_ctrl_blk_p->state = GBI_MMC_MED_WT_FOR_CID;

  return retval;
}

/**
 * function: gbi_mmc_get_partition_info_req
 */
static T_GBI_RESULT gbi_mmc_get_partition_info_req(T_GBI_PARTITION_INFO_REQ_MSG *msg_p,
                                                   void **data_p,
                                                   void **nmb_items_p)
{
  T_GBI_RESULT          retval                    = GBI_OK;
  T_RVF_MB_STATUS       mb_status;

  gbi_mc_env_ctrl_blk_p->requested_card    = 0;
  gbi_mc_env_ctrl_blk_p->partition_counter = 0;

  if (gbi_mc_env_ctrl_blk_p->card_stack_size == 0)
  {
    GBI_SEND_TRACE ("GBI: card_stack_size is zero", RV_TRACE_LEVEL_WARNING);

    *data_p      = NULL;
    *nmb_items_p = NULL;

    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;

    return GBI_OK;
  }

 

  /**retreive 128 bits for CSD*/
  mb_status = rvf_get_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                           sizeof(UINT8) * 16,
                           (T_RVF_BUFFER**)&gbi_mc_env_ctrl_blk_p->requested_csd_p);

  if (mb_status == RVF_RED)
  {
    /*
     * The flag returned by rvf_get_buf is red, there is not enough
     * memory to allocate the buffer.
     * The environemnt will cancel the CRY instance creation.
     */
    GBI_SEND_TRACE ("GBI: Error to get memory", RV_TRACE_LEVEL_WARNING);
    retval = gbi_get_partition_info_resp(GBI_MEMORY_ERR, 0, NULL, msg_p->return_path);
    return RV_MEMORY_ERR;
  }

  retval = mc_read_CSD(gbi_mc_env_ctrl_blk_p->card_stack_p[gbi_mc_env_ctrl_blk_p->requested_card],
                        gbi_mc_env_ctrl_blk_p->requested_csd_p,
                        gbi_mc_env_ctrl_blk_p->subscriber);

  if(retval != RV_OK)
  {
    GBI_SEND_TRACE ("GBI: could not read csd from mmc", RV_TRACE_LEVEL_WARNING);
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);
    retval = gbi_get_partition_info_resp(GBI_MEMORY_ERR, 0, NULL, msg_p->return_path);  retval = gbi_get_partition_info_resp(GBI_MEMORY_ERR, 0, NULL, msg_p->return_path);
  }

  gbi_mc_env_ctrl_blk_p->org_msg_retpath = msg_p->return_path;
  gbi_mc_env_ctrl_blk_p->state = GBI_MMC_PAR_WT_FOR_CSD;

  return retval;
}

static T_RV_RET gbi_mmc_get_media_info_request_buf(UINT8 nr_of_cards)
{
  T_RVF_MB_STATUS      mb_status;
  UINT16               nbr_of_media          = 0;
  UINT32               buffer_size;

 /* Create instance gathering all the variable used by GBI instance */
  mb_status = rvf_get_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                            sizeof(T_MC_RCA) * nr_of_cards,
                            (T_RVF_BUFFER**)&gbi_mc_env_ctrl_blk_p->card_stack_p);

  if (mb_status == RVF_RED)
  {
    /*
     * The flag returned by rvf_get_buf is red, there is not enough
     * memory to allocate the buffer.
     * The environemnt will cancel the CRY instance creation.
     */
    GBI_SEND_TRACE ("GBI: Error to get memory", RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  nbr_of_media = nr_of_cards;
  buffer_size  = nbr_of_media * sizeof(T_GBI_MEDIA_INFO);

  if (buffer_size == 0)
  {
    /* It is not possible and it makes no sense to get a buffer of size zero */
    GBI_SEND_TRACE ("GBI: Not possible to get a buffer with size zero", RV_TRACE_LEVEL_WARNING);
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->card_stack_p);
    gbi_mc_env_ctrl_blk_p->card_stack_p = NULL;
    gbi_mc_env_ctrl_blk_p->card_stack_size = 0;
    return RV_MEMORY_ERR;
  }

 

  /**retreive 128 bits fo CID*/
  mb_status = rvf_get_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                            sizeof(UINT8) * 16,
                              (T_RVF_BUFFER**)&gbi_mc_env_ctrl_blk_p->requested_cid_p);

  if (mb_status == RVF_RED)
  {
    /*
     * The flag returned by rvf_get_buf is red, there is not enough
     * memory to allocate the buffer.
     * The environemnt will cancel the CRY instance creation.
     */
    GBI_SEND_TRACE ("GBI: Error to get memory", RV_TRACE_LEVEL_WARNING);

    rvf_free_buf(gbi_mc_env_ctrl_blk_p->card_stack_p);
    gbi_mc_env_ctrl_blk_p->card_stack_p           = NULL;
    gbi_mc_env_ctrl_blk_p->card_stack_size        = 0;

    return RV_MEMORY_ERR;
  }

  /**retreive 128 bits for CSD*/
  mb_status = rvf_get_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                            sizeof(UINT8) * 16,
                              (T_RVF_BUFFER**)&gbi_mc_env_ctrl_blk_p->requested_csd_p);

  if (mb_status == RVF_RED)
  {
    /*
     * The flag returned by rvf_get_buf is red, there is not enough
     * memory to allocate the buffer.
     * The environemnt will cancel the CRY instance creation.
     */
    GBI_SEND_TRACE ("GBI: Error to get memory", RV_TRACE_LEVEL_WARNING);

    rvf_free_buf(gbi_mc_env_ctrl_blk_p->card_stack_p);
    gbi_mc_env_ctrl_blk_p->card_stack_p           = NULL;
    gbi_mc_env_ctrl_blk_p->card_stack_size        = 0;

    return RV_MEMORY_ERR;
  }

  return RV_OK;
}

static void gbi_mmc_free_media_info_request_buf_error(void)
{
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->card_stack_p);
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_cid_p);
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);

    gbi_mc_env_ctrl_blk_p->card_stack_p           = NULL;
    gbi_mc_env_ctrl_blk_p->requested_cid_p        = NULL;
    gbi_mc_env_ctrl_blk_p->requested_csd_p        = NULL;

    gbi_mc_env_ctrl_blk_p->card_stack_size        = 0;
}

/*@}*/
/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_med_wt_for_cid_rsp(T_RV_HDR * msg_p)
{
  T_RV_RET retval;
  T_MC_READ_CID_RSP_MSG *rsp_p;

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != MC_READ_CID_RSP_MSG)
  {
    /**no a not supported message*/
    GBI_SEND_TRACE("gbi_mmc_wt_for_cid_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
    return RV_NOT_SUPPORTED;
  }
  rsp_p = (T_MC_READ_CID_RSP_MSG*) msg_p;

  if(rsp_p->result != RV_OK)
  {
    /**action failed so send response and clear used buffers*/
    gbi_mmc_free_media_info_request_buf_error();

    retval = (T_RV_RET)gbi_get_media_info_resp(GBI_MEMORY_ERR, 0, NULL, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return retval;
  }

  /**start with reading the CSD from card */
  gbi_mc_env_ctrl_blk_p->requested_card = 0;
  retval = mc_read_CSD(gbi_mc_env_ctrl_blk_p->card_stack_p[gbi_mc_env_ctrl_blk_p->requested_card],
                        gbi_mc_env_ctrl_blk_p->requested_csd_p,
                        gbi_mc_env_ctrl_blk_p->subscriber);

  if(retval != RV_OK)
  {
    GBI_SEND_TRACE ("GBI: could not read csd from mmc", RV_TRACE_LEVEL_WARNING);

    gbi_mmc_free_media_info_request_buf_error();
    retval = (T_RV_RET)gbi_get_media_info_resp(GBI_MEMORY_ERR, 0, NULL, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
  }

  gbi_mc_env_ctrl_blk_p->state = GBI_MMC_MED_WT_FOR_CSD;

  return retval;
}
/*@}*/

/*@}*/
/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_med_wt_for_csd_rsp(T_RV_HDR * msg_p)
{
  T_RV_RET retval = RV_OK;
  T_MC_READ_CSD_RSP_MSG *rsp_p;
  UINT8 requested_card;
  UINT8 *rec_cid_p;
  UINT8 *rec_csd_p;

  UINT32 taac;
  UINT32 taac_unit;
  UINT32 taac_value;
  UINT32 taac_calc;

  T_GBI_MEDIA_INFO *media_info_p;

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != MC_READ_CSD_RSP_MSG)
  {
    /**no a not supported message*/
    GBI_SEND_TRACE("gbi_mmc_med_wt_for_csd_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
      return RV_NOT_SUPPORTED;
  }
  rsp_p = (T_MC_READ_CSD_RSP_MSG*) msg_p;

  if(rsp_p->result != RV_OK)
  {
    /**action failed so send response and clear used buffers*/
    gbi_mmc_free_media_info_request_buf_error();
    retval = (T_RV_RET)gbi_get_media_info_resp(GBI_MEMORY_ERR, 0, NULL, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return retval;
  }

  /**analyse the retreived CID and CSD*/
  requested_card = gbi_mc_env_ctrl_blk_p->requested_card;
  rec_cid_p = gbi_mc_env_ctrl_blk_p->requested_cid_p;
  rec_csd_p = gbi_mc_env_ctrl_blk_p->requested_csd_p;
  media_info_p = &(gbi_mc_env_ctrl_blk_p->requested_media_info_p[requested_card]);

  media_info_p->media_nmb      = requested_card + GBI_PLUGIN_0_MEDIA_OFFSET;
  media_info_p->media_pressent = TRUE;
  media_info_p->writeable      = TRUE;
  media_info_p->specific.spare_area_size = 0;


  /** Analyse the CID to retreive the media id (Difference between MC or SD*/

  if(mc_get_card_type(gbi_mc_env_ctrl_blk_p->card_stack_p[requested_card])
      == MMC_CARD)
  {
    /**get CRC (bit 1..7)*/
    media_info_p->media_id = rec_cid_p[0] >> 1;

    /**get PSN (2 LSB bytes)(bit(16..31)*/
    media_info_p->media_id |= rec_cid_p[2] << 7;
    media_info_p->media_id |= rec_cid_p[3] << 15;

    /**get MID (bit 120..127)*/
    media_info_p->media_id |= rec_cid_p[15] << 23;

    media_info_p->media_type  = GBI_MMC;

  }
  else
  {/**SD_CARD*/

    /**get CRC (bit 1..7)*/
    media_info_p->media_id = rec_cid_p[0] >> 1;

    /**get PSN (2 LSB bytes)(bit(24..39)*/
    media_info_p->media_id |= rec_cid_p[3] << 7;
    media_info_p->media_id |= rec_cid_p[4] << 15;
    /**get MID (bit 120..127)*/
    media_info_p->media_id |= rec_cid_p[15] << 23;

    media_info_p->media_type  = GBI_SD;

    /**for sd card retreive mechanical Write protect*/
    if(mc_sd_get_mech_wp(gbi_mc_env_ctrl_blk_p->card_stack_p[requested_card])
       == PROTECTED)
    {
      media_info_p->writeable = FALSE;
    }
  }

  /**retreive media info from csd*/

  /**read tmp write protect (bit 12)*/
  if(rec_csd_p[1] & 0x10)
  {
    media_info_p->writeable = FALSE;
  }
  /**read perm write protect (bit 13)*/
  if(rec_csd_p[1] & 0x20)
  {
    media_info_p->writeable = FALSE;
  }

  /** Analyse the CSD to retreive a speed indication*/
  /**only the TAAC will be retreived so obtain a speed indication*/
  /**This is inidication is used to calculate a speedrange*/


  /**retreive taac register*/
  taac = GBI_CSD_GET_TAAC(rec_csd_p);
  /**obtain the TAAC unit*/
  taac_unit = GBI_TAAC_TIME_UNIT(taac);
  /**obtain the TAAC value*/
  taac_value = GBI_TAAC_TIME_VALUE(taac);
  /**calculate */

  /**make 1 ms equal to 1 to prevent floating point*/
  /**first the unit is calculated into 100 us units*/
  /** lower the 1ms has no point since the speed will go over 25Mhz*/
  /**second the value is multiplied with 10 so the */
  /** end value will be in  ns*/
  switch(taac_unit)
  {
    case GBI_TAAC_TIME_UNIT_1_N_S:
      taac_calc = 1;
      break;

    case GBI_TAAC_TIME_UNIT_10_N_S:
      taac_calc = 1;
      break;

    case GBI_TAAC_TIME_UNIT_100_N_S:
      taac_calc = 1;
      break;

    case GBI_TAAC_TIME_UNIT_1_U_S:
      taac_calc = 1;
      break;

    case GBI_TAAC_TIME_UNIT_10_U_S:
      taac_calc = 1;
      break;

    case GBI_TAAC_TIME_UNIT_100_U_S:
      taac_calc = 1;
      break;

    case GBI_TAAC_TIME_UNIT_1_M_S:
      taac_calc = 10;
      break;

    case GBI_TAAC_TIME_UNIT_10_M_S:
      taac_calc = 100;
     break;
    default:
      taac_calc = 100;
      break;
  }

  switch(taac_value)
  {
    case GBI_TAAC_TIME_VALUE_1_0:
      taac_calc = taac_calc * 10;
      break;
    case GBI_TAAC_TIME_VALUE_1_2:
      taac_calc = taac_calc * 12;
      break;
    case GBI_TAAC_TIME_VALUE_1_3:
      taac_calc = taac_calc * 13;
      break;
    case GBI_TAAC_TIME_VALUE_1_5:
      taac_calc = taac_calc * 15;
      break;
    case GBI_TAAC_TIME_VALUE_2_0:
      taac_calc = taac_calc * 20;
      break;
    case GBI_TAAC_TIME_VALUE_2_5:
      taac_calc = taac_calc * 25;
      break;
    case GBI_TAAC_TIME_VALUE_3_0:
      taac_calc = taac_calc * 30;
      break;
    case GBI_TAAC_TIME_VALUE_3_5:
      taac_calc = taac_calc * 35;
      break;
    case GBI_TAAC_TIME_VALUE_4_0:
      taac_calc = taac_calc * 40;
      break;
    case GBI_TAAC_TIME_VALUE_4_5:
      taac_calc = taac_calc * 45;
      break;
    case GBI_TAAC_TIME_VALUE_5_0:
      taac_calc = taac_calc * 50;
      break;
    case GBI_TAAC_TIME_VALUE_5_5:
      taac_calc = taac_calc * 55;
      break;
    case GBI_TAAC_TIME_VALUE_6_0:
      taac_calc = taac_calc * 60;
      break;

    case GBI_TAAC_TIME_VALUE_7_0:
      taac_calc = taac_calc * 70;
      break;
    case GBI_TAAC_TIME_VALUE_8_0:
      taac_calc = taac_calc * 80;
      break;
    default:
      taac_calc = taac_calc * 80;
      break;
  }

  /**calculating on a blocksize of 512 bytes = 4096 bits*/
  /**result is in Hz*/
  media_info_p->read_speed = (4096* 10000) / taac_calc;

  media_info_p->write_speed = media_info_p->read_speed
                              / GBI_CSD_GET_R2W_FACTOR(rec_csd_p);


  if(media_info_p->read_speed > MC_CLK_SPEED)
  {
    media_info_p->read_speed = MC_CLK_SPEED;
  }

  if(media_info_p->write_speed > MC_CLK_SPEED)
  {
    media_info_p->write_speed = MC_CLK_SPEED;
  }

  /**correct for benchmark results*/
  media_info_p->read_speed = media_info_p->read_speed /GBI_MMC_MEDIA_READ_SPEED_FACTOR_DIV;
  media_info_p->write_speed = media_info_p->write_speed /GBI_MMC_MEDIA_WRITE_SPEED_FACTOR_DIV;

  if(gbi_mc_env_ctrl_blk_p->card_stack_size < (gbi_mc_env_ctrl_blk_p->requested_card + 1))
  {
    /**handle other cards*/
    gbi_mc_env_ctrl_blk_p->requested_card++;
    retval = mc_read_CID(gbi_mc_env_ctrl_blk_p->card_stack_p[0],
                          gbi_mc_env_ctrl_blk_p->requested_cid_p,
                          gbi_mc_env_ctrl_blk_p->subscriber);

    if(retval != RV_OK)
    {
      GBI_SEND_TRACE ("GBI: could not read cid from mmc", RV_TRACE_LEVEL_WARNING);
      gbi_mmc_free_media_info_request_buf_error();
      retval = (T_RV_RET)gbi_get_media_info_resp(GBI_MEMORY_ERR, 0, NULL, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    }
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_MED_WT_FOR_CID;
  }
  else
  {
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_cid_p);
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);

    gbi_mc_env_ctrl_blk_p->requested_cid_p        = NULL;
    gbi_mc_env_ctrl_blk_p->requested_csd_p        = NULL;


    /**Done with retreiving card information send response and go to IDLE*/

    *(gbi_mc_env_ctrl_blk_p->store_data_p)      = (void *)gbi_mc_env_ctrl_blk_p->requested_media_info_p;
    *(gbi_mc_env_ctrl_blk_p->store_nmb_items_p) = &gbi_mc_env_ctrl_blk_p->card_stack_size;

    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;

    retval = RV_OK;
  }

  return retval;
}
/*@}*/


/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_par_wt_for_csd_rsp(T_RV_HDR * msg_p)
{
  T_RV_RET retval = RV_OK;
  T_MC_READ_CSD_RSP_MSG *rsp_p;
  UINT8 *rec_csd_p;
  UINT32 buffer_size;
  T_RVF_MB_STATUS mb_status;

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != MC_READ_CSD_RSP_MSG)
  {
    /**no a not supported message*/
    GBI_SEND_TRACE("gbi_mmc_med_wt_for_csd_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
    return RV_NOT_SUPPORTED;
  }
  rsp_p = (T_MC_READ_CSD_RSP_MSG*) msg_p;

  if(rsp_p->result != RV_OK)
  {
    /**action failed so send response and clear used buffers*/

    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);
    retval = (T_RV_RET)gbi_get_partition_info_resp(GBI_MEMORY_ERR, 0, NULL, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return retval;
  }

  /**use CSD data to start datatransfer*/

  /**allocate minimal 512 bytes (depends on READ_BL_LEN)*/
  rec_csd_p = gbi_mc_env_ctrl_blk_p->requested_csd_p;

  buffer_size = 1 << (GBI_CSD_GET_READ_BL_LEN(rec_csd_p));
  while(buffer_size < 512)
  {
    buffer_size  += (1 << (GBI_CSD_GET_READ_BL_LEN(rec_csd_p)));
  }

  mb_status = rvf_get_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                            buffer_size,
                            (T_RVF_BUFFER**)&gbi_mc_env_ctrl_blk_p->mmc_data);

  if (mb_status == RVF_RED)
  {
    /*
     * The flag returned by rvf_get_buf is red, there is not enough
     * memory to allocate the buffer.
     * The environemnt will cancel the CRY instance creation.
     */
    GBI_SEND_TRACE ("GBI: Error to get memory", RV_TRACE_LEVEL_WARNING);

    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);
    retval = (T_RV_RET)gbi_get_partition_info_resp(GBI_MEMORY_ERR, 0, NULL, gbi_mc_env_ctrl_blk_p->org_msg_retpath);

    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;

    return RV_MEMORY_ERR;
  }

  retval = mc_read(gbi_mc_env_ctrl_blk_p->card_stack_p[gbi_mc_env_ctrl_blk_p->requested_card],
                    MC_RW_BLOCK,
                    0x0000,
                    gbi_mc_env_ctrl_blk_p->mmc_data,
                    buffer_size,
                    gbi_mc_env_ctrl_blk_p->subscriber);

  if(retval != RV_OK)
  {
    GBI_SEND_TRACE ("GBI: could not read data from mmc", RV_TRACE_LEVEL_WARNING);
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->mmc_data);
    retval = (T_RV_RET)gbi_get_partition_info_resp(GBI_MEMORY_ERR, 0, NULL, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
  }
  gbi_mc_env_ctrl_blk_p->state = GBI_MMC_PAR_WT_FOR_DATA;

  return retval;
}
/*@}*/


/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_par_wt_for_data(T_RV_HDR * msg_p)
{
  T_RV_RET retval = RV_OK;
  T_MC_READ_RSP_MSG *rsp_p;
  UINT8 requested_card;
  UINT8 *rec_csd_p;
  UINT8 *rec_data_p;
  UINT32 media_bl_length;
  T_GBI_PARTITION_INFO *par_info_p;
  UINT8 par_table_cnt;
  UINT32 csd_val1;
  UINT32 csd_val2;
  UINT32 csd_val3;
  UINT16 factor=0;

  UINT32 nbr_of_sectors;

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != MC_READ_RSP_MSG)
  {
    /**no a not supported message*/
    GBI_SEND_TRACE("gbi_mmc_par_wt_for_data wrong rsp msg",RV_TRACE_LEVEL_ERROR);
    return RV_NOT_SUPPORTED;
  }
  rsp_p = (T_MC_READ_RSP_MSG*) msg_p;

  if(rsp_p->result != RV_OK)
  {
    /**action failed so send response and clear used buffers*/
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->mmc_data);
    retval = (T_RV_RET)gbi_get_partition_info_resp(GBI_MEMORY_ERR, 0, NULL, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return retval;
  }

  /**analyse the received data**/
  rec_csd_p = gbi_mc_env_ctrl_blk_p->requested_csd_p;
  rec_data_p = gbi_mc_env_ctrl_blk_p->mmc_data;
  requested_card = gbi_mc_env_ctrl_blk_p->requested_card;
  par_info_p = &(gbi_mc_env_ctrl_blk_p->requested_partition_info_p[requested_card]);

  /**determine if this media contains a MBR of onlys has a bootsector*/

  /** Check if first byte contains 0xe9(near jump) or 0xeb (short jump)*/
  /** if so this could be a bootsector*/
  /** Check further if this is realys a bootsector*/
  /** See if there is a media_secriptor (bytes 21), should contain 0xfx */
  if((rec_data_p[0] == 0xE9 || rec_data_p[0] == 0xEB)
     &&
    ((rec_data_p[21] & 0xF0)== 0xF0))
  {
    /**if so the this is a floppy formatted sd/mmc card*/
    /**retreive information on this */
    par_info_p->bytes_per_block = (1<< GBI_CSD_GET_READ_BL_LEN(rec_csd_p));

    par_info_p->filesystem_type = gbi_i_determine_fat_type(rec_data_p);

    /* Initialise partition_name */
    par_info_p->partition_name[0] = '\0';

    par_info_p->first_block_nmb = 0x0000;

    media_bl_length = (
                        ((UINT32)(GBI_CSD_GET_C_SIZE(rec_csd_p) + 1))
                      <<(((UINT32)GBI_CSD_GET_C_SIZE_MULT(rec_csd_p)) + ((UINT32)2))
                     );

	factor=(par_info_p->bytes_per_block)/512;
	media_bl_length=media_bl_length*factor;
	par_info_p->bytes_per_block=512;

	 GBI_SEND_TRACE_PARAM("media_bl_length", media_bl_length, RV_TRACE_LEVEL_WARNING);

    par_info_p->last_block_nmb = media_bl_length - 1;
    par_info_p->media_nmb = requested_card;
    par_info_p->nmb_of_blocks  = media_bl_length;
    par_info_p->partition_nmb = 0;
    gbi_mc_env_ctrl_blk_p->partition_counter++;
  }
  else
  {
    /**This could contains a MBR*/
    /**check for signature on 510 (0x55) and 511(0xAA)*/
    if(   (rec_data_p[510] == 0x55)
        &&(rec_data_p[511] == 0xAA)
      )
    {
      /**retreive the partitiontables from the MBR*/
      for(par_table_cnt = 0; par_table_cnt < 4; par_table_cnt++)
      {
        UINT8 *par_tbl_p = &rec_data_p[446] + (par_table_cnt * 16);

        /**Check if bootable field is present (should contains 0x00 or 0x80)*/
        if(  (par_tbl_p[0] == 0x00)
           ||(par_tbl_p[0] == 0x80))
        {
          /*  if the file system type is not used, this partition table entry is empty,
           *  next could be evaluated
           */
          if (par_tbl_p[4] == 0x00)
          {
            continue;
          }

          par_info_p->bytes_per_block =(1<< GBI_CSD_GET_READ_BL_LEN(rec_csd_p));
          par_info_p->filesystem_type = par_tbl_p[4];

          /* Check on File system type*/
          /* if any kind of FAT16 type then set it to FAT16_LBA */
          if((par_info_p->filesystem_type == GBI_FAT16_A) ||
             (par_info_p->filesystem_type == GBI_FAT16_B)
             )
          {
            par_info_p->filesystem_type = GBI_FAT16_LBA;
          }
          else
          /* if any kind of FAT32 type then set it to FAT32_LBA*/
          if(par_info_p->filesystem_type == GBI_FAT32)
          {
            par_info_p->filesystem_type = GBI_FAT32_LBA;
          }
          else
          /* else only GBI_FAT12 supported */
          if(par_info_p->filesystem_type != GBI_FAT12)
          {
            /*not supported FS type so use unknown*/
            par_info_p->filesystem_type = GBI_UNKNOWN;
          }


          /**First sector**/
          par_info_p->first_block_nmb = par_tbl_p[8];
          par_info_p->first_block_nmb |= ((UINT32) par_tbl_p[9]) << 8 ;
          par_info_p->first_block_nmb |= ((UINT32) par_tbl_p[10])<< 16 ;
          par_info_p->first_block_nmb |= ((UINT32) par_tbl_p[11])<< 24 ;

          /**Number of sectors**/
          nbr_of_sectors = par_tbl_p[12];
          nbr_of_sectors |= ((UINT32) par_tbl_p[13]) << 8  ;
          nbr_of_sectors |= ((UINT32) par_tbl_p[14]) << 16 ;
          nbr_of_sectors |= ((UINT32) par_tbl_p[15]) << 24 ;

          /* Determine last block */
          par_info_p->last_block_nmb = (nbr_of_sectors - 1) + par_info_p->first_block_nmb;

          /* Check whether last_block doesn't exceed CSD block data */
          csd_val1 = GBI_CSD_GET_C_SIZE(rec_csd_p) + 1;
          csd_val2 = GBI_CSD_GET_C_SIZE_MULT(rec_csd_p);
          csd_val3 = 1 << (csd_val2 + 2);

		  factor = (par_info_p->bytes_per_block)/512;
          media_bl_length = csd_val1 * csd_val3 * factor;
          par_info_p->bytes_per_block = 512;


          if((par_info_p->last_block_nmb) > (media_bl_length))
          {
            continue;
          }

          /* Initialise partition_name */
          par_info_p->partition_name[0] = '\0';

          par_info_p->media_nmb = requested_card;
          par_info_p->nmb_of_blocks = (par_info_p->last_block_nmb
                                      - par_info_p->first_block_nmb+1);
          par_info_p->partition_nmb = par_table_cnt;
          /**next partition*/
          par_info_p++;
          gbi_mc_env_ctrl_blk_p->partition_counter++;
        }
      }
    }
    else
    {
  /* if not floppy formatted & no MBR found, then it could be a raw card */
  /* In this case, take only the card size & pass on*/
    /**if so the this is a floppy formatted sd/mmc card*/
    /**retreive information on this */

    /* Initialise partition_name */
    par_info_p->partition_name[0] = '\0';
    par_info_p->bytes_per_block = (1<< GBI_CSD_GET_READ_BL_LEN(rec_csd_p));
    par_info_p->first_block_nmb = 0x0000;

    media_bl_length = (
                        ((UINT32)(GBI_CSD_GET_C_SIZE(rec_csd_p) + 1))
                      <<(((UINT32)GBI_CSD_GET_C_SIZE_MULT(rec_csd_p)) + ((UINT32)2))
                     );

    factor=(par_info_p->bytes_per_block)/512;
    media_bl_length=media_bl_length*factor;
    par_info_p->bytes_per_block=512;

    par_info_p->last_block_nmb = media_bl_length - 1;
    par_info_p->media_nmb = requested_card;
    par_info_p->nmb_of_blocks  = media_bl_length;
    par_info_p->partition_nmb = 0;
    par_info_p->filesystem_type = GBI_UNKNOWN;	
    gbi_mc_env_ctrl_blk_p->partition_counter++;

    GBI_SEND_TRACE_PARAM("media_bl_length", media_bl_length, RV_TRACE_LEVEL_WARNING);
    }
  }

  /**free data buffer*/
  rvf_free_buf(gbi_mc_env_ctrl_blk_p->mmc_data);

  if(gbi_mc_env_ctrl_blk_p->card_stack_size < (gbi_mc_env_ctrl_blk_p->requested_card + 1))
  {
    /**handle other cards*/
    gbi_mc_env_ctrl_blk_p->requested_card++;
    retval = mc_read_CSD(gbi_mc_env_ctrl_blk_p->card_stack_p[0],
                          gbi_mc_env_ctrl_blk_p->requested_csd_p,
                          gbi_mc_env_ctrl_blk_p->subscriber);

    if(retval != RV_OK)
    {
      GBI_SEND_TRACE ("GBI: could not read csd from mmc", RV_TRACE_LEVEL_WARNING);
      rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);
      rvf_free_buf(gbi_mc_env_ctrl_blk_p->mmc_data);
      retval = (T_RV_RET)gbi_get_partition_info_resp(GBI_MEMORY_ERR, 0, NULL, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    }
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_PAR_WT_FOR_CSD;
  }
  else
  {
    /*free csd pointer*/
    rvf_free_buf(gbi_mc_env_ctrl_blk_p->requested_csd_p);
    gbi_mc_env_ctrl_blk_p->requested_csd_p = NULL;

    /**Done with retreiving card information send response and go to IDLE*/
    *(gbi_mc_env_ctrl_blk_p->store_data_p)      = (void *)gbi_mc_env_ctrl_blk_p->requested_partition_info_p;
    *(gbi_mc_env_ctrl_blk_p->store_nmb_items_p) = &gbi_mc_env_ctrl_blk_p->partition_counter;



    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;

    retval = RV_OK;
  }

  return retval;
}
/*@}*/

/**
 * function: gbi_read_resp
 */
static T_RV_RET gbi_mmc_read_resp(T_GBI_RESULT read_result, T_RV_RETURN  return_path)
{
  T_RV_RET             retval = RV_OK;
  T_GBI_READ_RSP_MSG   *rsp_p;

  /**
   * Create read response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_READ_RSP_MSG),
                       GBI_READ_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_read_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }


  /* compose message */
  rsp_p->hdr.msg_id     = GBI_READ_RSP_MSG;
  rsp_p->result         = read_result;

  /* Send message mailbox */
  retval = (T_RV_RET)gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_write_resp
 */
static T_RV_RET gbi_mmc_write_resp(T_GBI_RESULT write_result, T_RV_RETURN  return_path)
{
  T_RV_RET             retval = RV_OK;
  T_GBI_READ_RSP_MSG   *rsp_p;

  /**
   * Create write response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_WRITE_RSP_MSG),
                       GBI_WRITE_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_write_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  rsp_p->hdr.msg_id     = GBI_WRITE_RSP_MSG;
  rsp_p->result         = write_result;

  /* Send message mailbox */
  retval = (T_RV_RET)gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * function: gbi_erase_resp
 */
static T_RV_RET gbi_mmc_erase_resp(T_GBI_RESULT erase_result, T_RV_RETURN return_path)
{
  T_RV_RET             retval = RV_OK;
  T_GBI_ERASE_RSP_MSG  *rsp_p;

  /**
   * Create erase response message
   */
  if (rvf_get_msg_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                       sizeof (T_GBI_ERASE_RSP_MSG),
                       GBI_ERASE_RSP_MSG,
                       (T_RV_HDR **) &rsp_p) == RVF_RED)
  {
    GBI_SEND_TRACE ("GBI gbi_erase_resp: out of memory",
                     RV_TRACE_LEVEL_WARNING);
    return RV_MEMORY_ERR;
  }

  /* compose message */
  rsp_p->hdr.msg_id     = GBI_ERASE_RSP_MSG;
  rsp_p->result         = erase_result;

  /* Send message mailbox */
  retval = (T_RV_RET)gbi_i_generic_client_response ((T_RV_HDR *) rsp_p, return_path);

  return retval;
}

/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_init_wt_for_subscr_rsp(T_RV_HDR *msg_p)
{
  T_RV_RET retval = RV_OK;
  T_MC_EVENTS            event;

  if(msg_p->msg_id != MC_SUBSCRIBE_RSP_MSG)
  {
      GBI_SEND_TRACE("gbi_mmc_init_wt_for_subscr_rsp wrong return msg",RV_TRACE_LEVEL_ERROR);
      return RV_NOT_SUPPORTED ;
  }

  if(gbi_mc_env_ctrl_blk_p->subscriber == 0xFFFF)
  {
    /** WRONG subscriber info*/
    GBI_SEND_TRACE("gbi_mmc_init_wt_for_subscr_rsp no subscriber info",RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_INTERNAL_ERROR;
  }
  else
  {
    /** subscriber was succesfull*/
    /* Subscribe to MMC media insertion event */

    event = MC_EVENT_INSERTION | MC_EVENT_REMOVAL;

          if(mc_send_notification(event,
        gbi_mc_env_ctrl_blk_p->subscriber) != RV_OK)
    {
      GBI_SEND_TRACE("gbi_mmc_init_wt_for_subscr_rsp, mmc_send_notification failed ",RV_TRACE_LEVEL_ERROR);
      retval = RV_INTERNAL_ERR;
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_INTERNAL_ERROR;
    }
    else
    {
      /** set next state*/
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_INIT_WT_FOR_SUBSCR_EVENT_RSP;
    }
  }
  return retval;
}
/*@}*/


/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_init_wt_for_subscr_event_rsp(T_RV_HDR *msg_p)
{
  T_RV_RET retval = RV_OK;

  if(msg_p->msg_id != MC_NOTIFICATION_RSP_MSG)
  {
      GBI_SEND_TRACE("gbi_mmc_init_wt_for_subscr_event_rsp wrong return msg",RV_TRACE_LEVEL_ERROR);
      return RV_NOT_SUPPORTED ;
  }

  if(gbi_mc_env_ctrl_blk_p->subscriber == 0xFFFF)
  {
    /** WRONG subscriber info*/
    GBI_SEND_TRACE("gbi_mmc_init_wt_for_subscr_event_rsp no subscriber info",RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_INTERNAL_ERROR;
  }
  else
  {
    /** Subscribe to media removal was succesfull*/
    /** Aquire new stack */
    retval = mc_update_acq (gbi_mc_env_ctrl_blk_p->subscriber);
    if(retval != RV_OK)
    {
      GBI_SEND_TRACE("gbi_mmc_init_wt_for_subscr_event_rsp, mmc_update_acq failed ",RV_TRACE_LEVEL_ERROR);
      retval = RV_INTERNAL_ERR;
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_INTERNAL_ERROR;
    }
    else
    {
      /** set next state*/
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_INIT_WT_FOR_AQ_STCK_RSP;
    }
  }
  return retval;

}
/*@}*/


/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_init_wt_for_aq_stck_rsp(T_RV_HDR *msg_p)
{
  /** Get Stack Size */
  T_RV_RET retval = RV_OK;
  T_MC_UPDATE_ACQ_RSP_MSG *rsp_p;
  T_RVF_MB_STATUS mb_status;

  /**Check for Msg ID*/
  if(msg_p->msg_id != MC_UPDATE_ACQ_RSP_MSG)
  {
      GBI_SEND_TRACE("gbi_mmc_init_wt_for_aq_stck_rsp wrong return msg",RV_TRACE_LEVEL_ERROR);
      return RV_NOT_SUPPORTED ;
  }
  rsp_p = (T_MC_UPDATE_ACQ_RSP_MSG *) msg_p;

  /**Check Result in response*/
  if(rsp_p->result != RV_OK)
  {
    if(rsp_p->result == RV_INVALID_PARAMETER)
    {
      GBI_SEND_TRACE("gbi_mmc_init_wt_for_aq_stck_rsp, MC_UPDATE_ACQ_RSP_MSG failed, Invalid parameter",RV_TRACE_LEVEL_ERROR);
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_INTERNAL_ERROR;
      gbi_mc_env_ctrl_blk_p->plugin_status = TRUE;
      return RV_OK;
    }
    else
    {
      GBI_SEND_TRACE("gbi_mmc_init_wt_for_aq_stck_rsp, MC_UPDATE_ACQ_RSP_MSG failed, unknown error",RV_TRACE_LEVEL_ERROR);
      gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
      gbi_mc_env_ctrl_blk_p->plugin_status = TRUE;
      return RV_OK;
    }
  }

  /* Create instance gathering all the variable used by GBI instance */
  mb_status = rvf_get_buf (gbi_env_ctrl_blk_p->prim_mb_id,
                              sizeof(T_MC_RCA) * rsp_p->stack_size,
                            (T_RVF_BUFFER**)&gbi_mc_env_ctrl_blk_p->card_stack_p);
  if (mb_status == RVF_RED)
  {
    /*
     * The flag returned by rvf_get_buf is red, there is not enough
     * memory to allocate the card stack.
     */
    GBI_SEND_TRACE("GBI: Error to get memory ",RV_TRACE_LEVEL_ERROR);

    return RVM_MEMORY_ERR;
  }

  gbi_mc_env_ctrl_blk_p->card_stack_size = rsp_p->stack_size;
    retval = mc_read_card_stack (gbi_mc_env_ctrl_blk_p->card_stack_p, rsp_p->stack_size);
  if(retval != RV_OK)
  {
    GBI_SEND_TRACE("gbi_mmc_init_wt_for_aq_stck_rsp, mmc_read_card_stack failed ",RV_TRACE_LEVEL_ERROR);
    retval = RV_INTERNAL_ERR;
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_INTERNAL_ERROR;
    return retval;
  }
  gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
  gbi_mc_env_ctrl_blk_p->plugin_status = TRUE;

  return retval;
}


/*@}*/
/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_init_wt_for_crd_stck_size_rsp(T_RV_HDR *msg_p)
{
  T_RV_RET retval = RV_OK;

  return retval;
}
/*@}*/
/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_init_wt_for_retreive_rca(T_RV_HDR *msg_p)
{
  T_RV_RET retval = RV_OK;

  return retval;
}

/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_wt_for_read_rsp(T_RV_HDR *msg_p)
{
  T_RVF_MB_STATUS mb_status;
  T_RV_RET        retval = RV_OK;
  T_MC_READ_RSP_MSG    *rsp_p;

        GBI_SEND_TRACE("gbi_mmc_wt_for_read_rsp", RV_TRACE_LEVEL_WARNING);

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != MC_READ_RSP_MSG)
  {
    /**no a not supported message*/
    GBI_SEND_TRACE("gbi_mmc_wt_for_read_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
      return RV_NOT_SUPPORTED;
  }

  rsp_p = (T_MC_READ_RSP_MSG *) msg_p;

  if(rsp_p->result != RV_OK)
  {
    GBI_SEND_TRACE_PARAM("read result is NOT_OK, card status:",rsp_p->card_status,RV_TRACE_LEVEL_ERROR);
    /**action failed so send response*/
    gbi_mmc_read_resp(RV_INTERNAL_ERR, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return rsp_p->result;
  }

  /* Reserve temporary buffer */
  mb_status = rvf_get_buf(gbi_env_ctrl_blk_p->prim_mb_id,
            remaining_bytes_per_blocks,
            (T_RVF_BUFFER**) &remaining_block_p);

  if (mb_status == RVF_RED)
  {
    /*
    * The flag returned by rvf_get_buf is red, there is not enough
    * memory to allocate the buffer.
    * The environemnt will cancel the CRY instance creation.
    */
    GBI_SEND_TRACE ("GBI: Error to get memory", RV_TRACE_LEVEL_WARNING);

    /**action failed so send response*/
    retval = gbi_mmc_read_resp(RV_MEMORY_ERR, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return GBI_MEMORY_ERR;
  }

  retval = mc_read (gbi_mc_env_ctrl_blk_p->card_stack_p[media_nmb_in_progress],
           MC_RW_BLOCK,
           mc_addr,
           remaining_block_p,
           remaining_bytes_per_blocks,
           gbi_mc_env_ctrl_blk_p->subscriber);


  if(retval != RV_OK)
  {
    /**action failed so send response*/
    gbi_mmc_read_resp(RV_INTERNAL_ERR, gbi_mc_env_ctrl_blk_p->org_msg_retpath);

    /* Free allocated remaining_length buffer */
    rvf_free_buf(remaining_block_p);

    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
  }
  else
  {
    /**action started now wait for result*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_WT_FOR_READ_REMAINDER_RSP;
  }

  return retval;
}
/*@}*/


/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_wt_for_read_remainder_rsp(T_RV_HDR *msg_p)
{
  T_RV_RET            retval;
  T_MC_READ_RSP_MSG    *rsp_p;

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != MC_READ_RSP_MSG)
  {
    /**no a not supported message*/
    GBI_SEND_TRACE("gbi_mmc_wt_for_read_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
      return RV_NOT_SUPPORTED;
  }

  rsp_p = (T_MC_READ_RSP_MSG *) msg_p;

  if(rsp_p->result != RV_OK)
  {
    GBI_SEND_TRACE_PARAM("read result is NOT_OK, card status:",rsp_p->card_status, RV_TRACE_LEVEL_ERROR);
    /**action failed so send response*/
    gbi_mmc_read_resp(RV_INTERNAL_ERR, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;

    if (remaining_block_p != NULL)
    {
      /* Free allocated remaining_length buffer */
      rvf_free_buf(remaining_block_p);
    }

    return rsp_p->result;
  }

  if (remaining_block_p != NULL)
  {
    /* Copy remaining bytes to the buffer pointer pointing to the
     * client's read data buffer
     */
    memcpy(data_buffer, remaining_block_p, remaining_bytes);

    /* Free allocated remaining_length buffer */
    rvf_free_buf(remaining_block_p);
  }

  /**action was succesfull send response*/
  retval = gbi_mmc_read_resp(RV_OK, gbi_mc_env_ctrl_blk_p->org_msg_retpath);

  /**set state back to idle*/
  gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;

  return retval;
}
/*@}*/



/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_wt_for_write_rsp(T_RV_HDR *msg_p)
{
  UINT8           *remaining_data_p;
  T_RVF_MB_STATUS mb_status;
  T_RV_RET        retval = RV_OK;
  T_MC_WRITE_RSP_MSG *rsp_p;

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != MC_WRITE_RSP_MSG)
  {
    /**no a not supported message*/
    GBI_SEND_TRACE("gbi_mmc_wt_for_write_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
      return RV_NOT_SUPPORTED;
  }

  rsp_p = (T_MC_WRITE_RSP_MSG *) msg_p;

  if(rsp_p->result != RV_OK)
  {
    /**action failed so send response*/
    gbi_mmc_write_resp(RV_INTERNAL_ERR, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return rsp_p->result;
  }

  /* Reserve temporary buffer */
  mb_status = rvf_get_buf(gbi_env_ctrl_blk_p->prim_mb_id,
            remaining_bytes_per_blocks,
            (T_RVF_BUFFER**) &remaining_block_p);

  if (mb_status == RVF_RED)
  {
    /*
    * The flag returned by rvf_get_buf is red, there is not enough
    * memory to allocate the buffer.
    * The environemnt will cancel the CRY instance creation.
    */
    GBI_SEND_TRACE ("GBI: Error to get memory", RV_TRACE_LEVEL_WARNING);

    /**action failed so send response*/
    retval = gbi_mmc_write_resp(RV_MEMORY_ERR, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return GBI_MEMORY_ERR;
  }

  remaining_data_p = remaining_block_p;

  /* Copy actual write data to temporary memroy */
  memcpy(remaining_data_p, data_buffer, remaining_bytes);

  /* Fill remaining block data with default data */
  remaining_data_p = remaining_data_p + remaining_bytes;
  memset(remaining_data_p, 0x00, (remaining_bytes_per_blocks - remaining_bytes));

  retval = mc_write( gbi_mc_env_ctrl_blk_p->card_stack_p[media_nmb_in_progress],
                      MC_RW_BLOCK,
                      mc_addr,
                      remaining_block_p,
                      remaining_bytes_per_blocks,
                      gbi_mc_env_ctrl_blk_p->subscriber);

  if(retval != RV_OK)
  {
    /**action failed so send response*/
    gbi_mmc_write_resp(RV_INTERNAL_ERR, gbi_mc_env_ctrl_blk_p->org_msg_retpath);

    /* Free allocated remaining_length buffer */
    rvf_free_buf(remaining_block_p);

    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
  }
  else
  {
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_WT_FOR_WRITE_REMAINDER_RSP;
  }

  return retval;
}


/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_wt_for_write_remainder_rsp(T_RV_HDR *msg_p)
{
  T_RV_RET retval;
  T_MC_WRITE_RSP_MSG *rsp_p;

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != MC_WRITE_RSP_MSG)
  {
    /**no a not supported message*/
    GBI_SEND_TRACE("gbi_mmc_wt_for_write_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
      return RV_NOT_SUPPORTED;
  }

  rsp_p = (T_MC_WRITE_RSP_MSG *) msg_p;

  if(rsp_p->result != RV_OK)
  {
    /**action failed so send response*/
    gbi_mmc_write_resp(RV_INTERNAL_ERR, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return rsp_p->result;
  }

  /**action was succesfull send response*/
  retval = gbi_mmc_write_resp(RV_OK, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
  /**set state back to idle*/
  gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;

  /* Free allocated remaining_length buffer */
  if (remaining_block_p != NULL)
  {
    rvf_free_buf(remaining_block_p);
  }

  return retval;
}

/*@}*/
/**
 * SHORT DESCRIPTION
 *
 * LONGDESCRIPTION
 *
 * @param UINT8:      PARAM1    DESCRIPTION PARAM1
 *
 * @return  T_RV_RET:   RETURNVALUES
 */

/*@{*/
static T_RV_RET gbi_mmc_wt_for_erase_rsp(T_RV_HDR *msg_p)
{
  T_RV_RET retval;
  T_MC_ERASE_GROUP_RSP_MSG *rsp_p;

  /**is this the response to wait for ?*/
  if(msg_p->msg_id != MC_ERASE_GROUP_RSP_MSG)
  {
    /**no a not supported message*/
    GBI_SEND_TRACE("gbi_mmc_wt_for_erase_rsp wrong rsp msg",RV_TRACE_LEVEL_ERROR);
    return RV_NOT_SUPPORTED;
  }
  rsp_p = (T_MC_ERASE_GROUP_RSP_MSG *) msg_p;

  if(rsp_p->result != RV_OK)
  {
    /**action failed so send response*/
    retval = gbi_mmc_erase_resp(RV_INTERNAL_ERR, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
    /**set state back to idle*/
    gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;
    return retval;
  }

  /**action was succesfull send response*/
  retval = gbi_mmc_erase_resp(RV_OK, gbi_mc_env_ctrl_blk_p->org_msg_retpath);
  /**set state back to idle*/
  gbi_mc_env_ctrl_blk_p->state = GBI_MMC_IDLE;

  return retval;
}
/*@}*/


/****************************************************************/
/*    Support functions                                         */
/****************************************************************/

/**
 * function: gbi_get_media_index
 */
static UINT8 gbi_get_media_index(UINT8 media_nmb)
{
  BOOL  media_nmb_found  = FALSE;
  UINT8 curr_index;
  UINT8 nb_of_media;
  UINT8 media_index = INVALID_INDEX;
  T_GBI_MEDIA_INFO *media_p = NULL;

  media_p     = gbi_mc_env_ctrl_blk_p->requested_media_info_p;
  nb_of_media = (UINT8)gbi_mc_env_ctrl_blk_p->card_stack_size;

  for (curr_index = 0;
       (curr_index < nb_of_media && media_nmb_found == FALSE);
       curr_index++)
  {
    if (media_nmb == media_p->media_nmb)
    {
      media_nmb_found = TRUE;
      media_index     = curr_index;
    }

    media_p++;
  }

  /* Media number not found in media_table */
  if (media_nmb_found == FALSE)
  {
    media_index = INVALID_INDEX;
  }

  return media_index;
}

/**
 * function: gbi_get_partition_index
 */
static UINT8 gbi_get_partition_index(UINT8 media_nmb,
                                     UINT8 partition_nmb)
{
  BOOL  partition_nmb_found  = FALSE;
  UINT8 nb_of_partitions;
  UINT8 curr_index;
  UINT8 partition_index = INVALID_INDEX;
  T_GBI_PARTITION_INFO *part_p = NULL;

  part_p           = gbi_mc_env_ctrl_blk_p->requested_partition_info_p;
  nb_of_partitions = gbi_mc_env_ctrl_blk_p->partition_counter;

  for (curr_index = 0;
       (curr_index < nb_of_partitions && partition_nmb_found == FALSE);
       curr_index++)
  {
    if ((media_nmb == part_p->media_nmb)
        && (partition_nmb == part_p->partition_nmb))
    {
      partition_nmb_found = TRUE;
      partition_index     = curr_index;
    }

    part_p++;
  }

  /* Media and partition number not found in partition information */
  if (partition_nmb_found == FALSE)
  {
    partition_index = INVALID_INDEX;
  }

  return partition_index;
}

/** Initialisation*/

#ifdef _WINDOWS
/**
 * Function: format_partition_fat16
 *
 * @return  RVM_OK
 */
static void format_partition_fat16(UINT32 *mem_p)
{
  UINT8 *partition_p  = NULL;

  partition_p = (UINT8 *) mem_p;

  /* Bootsector value, 3 bytes */
  memset((void *)partition_p, JUMP_BOOTSECTOR_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, JUMP_BOOTSECTOR_VALUE_2, 1);
  partition_p++;
  memset((void *)partition_p, JUMP_BOOTSECTOR_VALUE_3, 1);
  partition_p++;

  /* Copy string to memory, 8 bytes */
  memcpy((void *)partition_p, MANUFACTURER_VALUE, MANUFACTURER_SIZE);
  partition_p = partition_p + MANUFACTURER_SIZE;

  /* Bytes per sector value, 2 bytes */
  memset((void *)partition_p, BYTES_PER_SECTOR_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, BYTES_PER_SECTOR_VALUE_2, 1);
  partition_p++;

  /* Sector per cluster value, 1 byte */
  memset((void *)partition_p, SECTORS_PER_CLUSTER_VALUE, 1);
  partition_p++;

  /* Reserved sector value, 2 bytes */
  memset((void *)partition_p, RESERVED_SECTORS_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, RESERVED_SECTORS_VALUE_2, 1);
  partition_p++;

  /* Nbr of FAT tables value, 1 byte */
  memset((void *)partition_p, NBR_FAT_TABLES_VALUE, 1);
  partition_p++;

  /* Nbr of files and dirs in root value, 1 byte */
  memset((void *)partition_p, MAX_NBR_FILES_DIRS_IN_ROOT_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, MAX_NBR_FILES_DIRS_IN_ROOT_VALUE_2, 1);
  partition_p++;

  /* Nbr of sectors in volume value, 2 bytes */
  memset((void *)partition_p, NBR_SECTORS_IN_VOLUME_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, NBR_SECTORS_IN_VOLUME_VALUE_2, 1);
  partition_p++;

  /* media descriptor value, 2 bytes */
  memset((void *)partition_p, MEDIA_DESCRIPTOR_VALUE, 1);
  partition_p++;

  /* Sectors per FAT tables value, 2 bytes */
  memset((void *)partition_p, SECTORS_PER_FAT_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, SECTORS_PER_FAT_VALUE_2, 1);
  partition_p++;

  /* Sectors per track value, 2 bytes */
  memset((void *)partition_p, SECTORS_PER_TRACK_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, SECTORS_PER_TRACK_VALUE_2, 1);
  partition_p++;

  /* Heads value, 2 bytes */
  memset((void *)partition_p, NBR_HEADS_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, NBR_HEADS_VALUE_2, 1);
  partition_p++;

  /* Hidden sectors value, 4 bytes */
  memset((void *)partition_p, REMOVE_HIDDEN_SECTOR_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, REMOVE_HIDDEN_SECTOR_VALUE_2, 1);
  partition_p++;
  memset((void *)partition_p, REMOVE_HIDDEN_SECTOR_VALUE_3, 1);
  partition_p++;
  memset((void *)partition_p, REMOVE_HIDDEN_SECTOR_VALUE_4, 1);
  partition_p++;

  /* Total sectors value, 4 bytes */
  memset((void *)partition_p, TOT_SECTORS_32_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, TOT_SECTORS_32_VALUE_2, 1);
  partition_p++;
  memset((void *)partition_p, TOT_SECTORS_32_VALUE_3, 1);
  partition_p++;
  memset((void *)partition_p, TOT_SECTORS_32_VALUE_4, 1);
  partition_p++;

  /* Drive nbr value, 1 byte */
  memset((void *)partition_p, DRIVE_NUMBER_VALUE, 1);
  partition_p++;

  /* Reserved value, 1 byte */
  memset((void *)partition_p, RESERVED1_VALUE, 1);
  partition_p++;

  /* Goot signature value, 1 byte */
  memset((void *)partition_p, BOOTSIGN_VALUE, 1);
  partition_p++;

  /* Volume ID value, 4 bytes */
  memset((void *)partition_p, VOLUME_ID_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, VOLUME_ID_VALUE_2, 1);
  partition_p++;
  memset((void *)partition_p, VOLUME_ID_VALUE_3, 1);
  partition_p++;
  memset((void *)partition_p, VOLUME_ID_VALUE_4, 1);
  partition_p++;

  /* Copy string to memory */
  memcpy((void *)partition_p, VOLUME_LABEL_VALUE, VOLUME_LABEL_SIZE);
  partition_p = partition_p + VOLUME_LABEL_SIZE;

  /* Copy string to memory */
  memcpy((void *)partition_p, FILE_SYSTEM_TYPE_VALUE, FILE_SYSTEM_TYPE_SIZE);
  partition_p = partition_p + FILE_SYSTEM_TYPE_SIZE;

  /* Skip boot routine */
  partition_p = partition_p + BOOT_ROUTINE_SIZE;

  /* Signature value, 2 bytes */
  memset((void *)partition_p, SIGNATURE_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, SIGNATURE_VALUE_2, 1);
  partition_p++;

  /* First FAT, reserved */
  memset((void *)partition_p, FAT1_RESERVED_0_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, FAT1_RESERVED_0_VALUE_2, 1);
  partition_p++;

  memset((void *)partition_p, FAT1_RESERVED_1_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, FAT1_RESERVED_1_VALUE_2, 1);

  /* Set pointer back to start of first FAT */
  partition_p = partition_p - 3;

  /* Set pointer to start of second FAT */
  partition_p = partition_p + FAT_TABLE_1_SIZE;

  memset((void *)partition_p, FAT2_RESERVED_0_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, FAT2_RESERVED_0_VALUE_2, 1);
  partition_p++;

  memset((void *)partition_p, FAT2_RESERVED_1_VALUE_1, 1);
  partition_p++;
  memset((void *)partition_p, FAT2_RESERVED_1_VALUE_2, 1);
  partition_p++;
}
#endif /** _WINDOWS*/

#ifdef _WINDOWS
/**
 * Function: format_partition_fat32
 *
 * @return  RVM_OK
 */
static void format_partition_fat32(UINT32 *mem_p)
{
  UINT8 *partition_p  = (UINT8 *) mem_p;
	UINT32 u32Var1;
	UINT32 u32Var2;
	UINT32 nmbSectorsPerFAT;

	//copy Template of boot sector
  memcpy((void*)partition_p, (const void*)&fat32_block_0, 512);

	//change size: number of sectors of whole volume
	u32Var1 = GBI_MEDIA_0_PAR_3_NMB_OF_BLOCKS;
  memcpy((void*)&(partition_p[32]), (const void*)&u32Var1, sizeof(UINT32));

 	//Determine number of sectors in one FAT table.
	u32Var1 = GBI_MEDIA_0_PAR_3_NMB_OF_BLOCKS - 32;
	u32Var2 = (256 * 1) + 2;
	u32Var2 = u32Var2 / 2;
	nmbSectorsPerFAT = (u32Var1 + (u32Var2 - 1)) / u32Var2;
  memcpy((void*)&(partition_p[36]), (const void*)&nmbSectorsPerFAT, sizeof(UINT32));

	//copy Template of FS struct sector
  memcpy((void*)&(partition_p[512]), (const void*)&fat32_block_1, 512);

	//Copy template of first FAT sector (behind reserved sectors)
  memcpy((void*)&(partition_p[32*512]), (const void*)&fat32_start_of_FAT_table,
                                             sizeof(fat32_start_of_FAT_table));

	//Copy template of second FAT sector (behind first FAT)
  memcpy((void*)&(partition_p[32*512+nmbSectorsPerFAT*512]),
                   (const void*)&fat32_start_of_FAT_table,
                          sizeof(fat32_start_of_FAT_table));

	//Copy template of ROOT dir (behind FAT)
  memcpy((void*)&(partition_p[32*512+2*nmbSectorsPerFAT*512]),
                   (const void*)&fat32_rootdir_table,
                          sizeof(fat32_rootdir_table));
}
#endif /** _WINDOWS*/


#ifdef _WINDOWS
/**
 * Function: init_media_partition_tables
 *
 * @return  RVM_OK
 */
static void init_media_partition_tables()
{
  gbi_env_ctrl_blk_p->number_of_media                      = GBI_NR_OF_MEDIA;
  gbi_env_ctrl_blk_p->number_of_partitions[GBI_MEDIA_NR_0] = GBI_MEDIA_0_NR_OF_PARTITIONS;

  /* Media table */
  gbi_env_ctrl_blk_p->media_table[0].media_nmb      = GBI_MEDIA_NR_0;

  gbi_env_ctrl_blk_p->media_table[0].media_type     = GBI_MEDIA_0_MEDIA_TYPE;
  gbi_env_ctrl_blk_p->media_table[0].media_pressent = TRUE;
  gbi_env_ctrl_blk_p->media_table[0].writeable      = TRUE;
  gbi_env_ctrl_blk_p->media_table[0].media_id       = GBI_MEDIA_0_MEDIA_ID;
  gbi_env_ctrl_blk_p->media_table[0].read_speed     = GBI_MEDIA_0_READ_SPEED;
  gbi_env_ctrl_blk_p->media_table[0].write_speed    = GBI_MEDIA_0_WRITE_SPEED;
  gbi_env_ctrl_blk_p->media_table[0].specific.spare_area_size
                                                    = GBI_MEDIA_0_SPARE_AREA_SIZE;

  /* Partition table, partition 0 */
  gbi_env_ctrl_blk_p->partition_table[0].media_nmb        = GBI_MEDIA_NR_0;
  gbi_env_ctrl_blk_p->partition_table[0].partition_nmb    = GBI_MEDIA_0_PARTITION_NR_0;
  gbi_env_ctrl_blk_p->partition_table[0].filesystem_type  = GBI_MEDIA_0_PAR_0_FILESYSTEM;
  gbi_env_ctrl_blk_p->partition_table[0].filesystem_name[0] = '\0';
  gbi_env_ctrl_blk_p->partition_table[0].partition_name[0]  = '\0';
  gbi_env_ctrl_blk_p->partition_table[0].bytes_per_block  = GBI_MEDIA_0_BYTES_PER_BLOCK;
  gbi_env_ctrl_blk_p->partition_table[0].nmb_of_blocks    = GBI_MEDIA_0_PAR_0_NMB_OF_BLOCKS;
  gbi_env_ctrl_blk_p->partition_table[0].first_block_nmb  = GBI_MEDIA_0_PAR_0_FIRST_BLOCK;
  gbi_env_ctrl_blk_p->partition_table[0].last_block_nmb   = GBI_MEDIA_0_PAR_0_LAST_BLOCK;

  /* Partition table, partition 1 */
  gbi_env_ctrl_blk_p->partition_table[1].media_nmb        = GBI_MEDIA_NR_0;
  gbi_env_ctrl_blk_p->partition_table[1].partition_nmb    = GBI_MEDIA_0_PARTITION_NR_1;
  gbi_env_ctrl_blk_p->partition_table[1].filesystem_type  = GBI_MEDIA_0_PAR_1_FILESYSTEM;
  gbi_env_ctrl_blk_p->partition_table[1].filesystem_name[0] = '\0';
  gbi_env_ctrl_blk_p->partition_table[1].partition_name[0]  = '\0';
  gbi_env_ctrl_blk_p->partition_table[1].bytes_per_block  = GBI_MEDIA_0_BYTES_PER_BLOCK;
  gbi_env_ctrl_blk_p->partition_table[1].nmb_of_blocks    = GBI_MEDIA_0_PAR_1_NMB_OF_BLOCKS;
  gbi_env_ctrl_blk_p->partition_table[1].first_block_nmb  = GBI_MEDIA_0_PAR_1_FIRST_BLOCK;
  gbi_env_ctrl_blk_p->partition_table[1].last_block_nmb   = GBI_MEDIA_0_PAR_1_LAST_BLOCK;

  /* Partition table, partition 2*/
  gbi_env_ctrl_blk_p->partition_table[2].media_nmb        = GBI_MEDIA_NR_0;
  gbi_env_ctrl_blk_p->partition_table[2].partition_nmb    = GBI_MEDIA_0_PARTITION_NR_2;
  gbi_env_ctrl_blk_p->partition_table[2].filesystem_type  = GBI_MEDIA_0_PAR_2_FILESYSTEM;
  gbi_env_ctrl_blk_p->partition_table[2].filesystem_name[0] = '\0';
  gbi_env_ctrl_blk_p->partition_table[2].partition_name[0]  = '\0';
  gbi_env_ctrl_blk_p->partition_table[2].bytes_per_block  = GBI_MEDIA_0_BYTES_PER_BLOCK;
  gbi_env_ctrl_blk_p->partition_table[2].nmb_of_blocks    = GBI_MEDIA_0_PAR_2_NMB_OF_BLOCKS;
  gbi_env_ctrl_blk_p->partition_table[2].first_block_nmb  = GBI_MEDIA_0_PAR_2_FIRST_BLOCK;
  gbi_env_ctrl_blk_p->partition_table[2].last_block_nmb   = GBI_MEDIA_0_PAR_2_LAST_BLOCK;

  /* Partition table, partition 3*/
  gbi_env_ctrl_blk_p->partition_table[3].media_nmb        = GBI_MEDIA_NR_0;
  gbi_env_ctrl_blk_p->partition_table[3].partition_nmb    = GBI_MEDIA_0_PARTITION_NR_3;
  gbi_env_ctrl_blk_p->partition_table[3].filesystem_type  = GBI_MEDIA_0_PAR_3_FILESYSTEM;
  gbi_env_ctrl_blk_p->partition_table[3].filesystem_name[0] = '\0';
  gbi_env_ctrl_blk_p->partition_table[3].partition_name[0]  = '\0';
  gbi_env_ctrl_blk_p->partition_table[3].bytes_per_block  = GBI_MEDIA_0_BYTES_PER_BLOCK;
  gbi_env_ctrl_blk_p->partition_table[3].nmb_of_blocks    = GBI_MEDIA_0_PAR_3_NMB_OF_BLOCKS;
  gbi_env_ctrl_blk_p->partition_table[3].first_block_nmb  = GBI_MEDIA_0_PAR_3_FIRST_BLOCK;
  gbi_env_ctrl_blk_p->partition_table[3].last_block_nmb   = GBI_MEDIA_0_PAR_3_LAST_BLOCK;

  /* Make this data also available to GBI MC plugin */
  gbi_mc_env_ctrl_blk_p->requested_media_info_p =
    &gbi_env_ctrl_blk_p->media_table[0];

  gbi_mc_env_ctrl_blk_p->requested_partition_info_p =
    &gbi_env_ctrl_blk_p->partition_table[0];

  gbi_mc_env_ctrl_blk_p->card_stack_size   = gbi_env_ctrl_blk_p->number_of_media;
  gbi_mc_env_ctrl_blk_p->partition_counter = 4;

  /* Initialize fysical partition 0 */
  gbi_env_ctrl_blk_p->media_0_partition_0_mem = (UINT32 *) malloc(
      (GBI_MEDIA_0_BYTES_PER_BLOCK * GBI_MEDIA_0_PAR_0_NMB_OF_BLOCKS) +
      (GBI_MEDIA_0_SPARE_AREA_SIZE * GBI_MEDIA_0_PAR_0_NMB_OF_BLOCKS));

  memset(gbi_env_ctrl_blk_p->media_0_partition_0_mem,
    0x00000000,
    ((GBI_MEDIA_0_BYTES_PER_BLOCK * GBI_MEDIA_0_PAR_0_NMB_OF_BLOCKS) +
     (GBI_MEDIA_0_SPARE_AREA_SIZE * GBI_MEDIA_0_PAR_0_NMB_OF_BLOCKS)));

  format_partition_fat16(gbi_env_ctrl_blk_p->media_0_partition_0_mem);

  /* Initialize fysical partition 1 */
  gbi_env_ctrl_blk_p->media_0_partition_1_mem = (UINT32 *) malloc(
      (GBI_MEDIA_0_BYTES_PER_BLOCK * GBI_MEDIA_0_PAR_1_NMB_OF_BLOCKS) +
      (GBI_MEDIA_0_SPARE_AREA_SIZE * GBI_MEDIA_0_PAR_1_NMB_OF_BLOCKS));

  memset(gbi_env_ctrl_blk_p->media_0_partition_1_mem,
    0x00000000,
    ((GBI_MEDIA_0_BYTES_PER_BLOCK * GBI_MEDIA_0_PAR_1_NMB_OF_BLOCKS) +
     (GBI_MEDIA_0_SPARE_AREA_SIZE * GBI_MEDIA_0_PAR_1_NMB_OF_BLOCKS)));

  format_partition_fat16(gbi_env_ctrl_blk_p->media_0_partition_1_mem);

  /* Initialize fysical partition 2 */
  gbi_env_ctrl_blk_p->media_0_partition_2_mem = (UINT32 *) malloc(
      (GBI_MEDIA_0_BYTES_PER_BLOCK * GBI_MEDIA_0_PAR_2_NMB_OF_BLOCKS) +
      (GBI_MEDIA_0_SPARE_AREA_SIZE * GBI_MEDIA_0_PAR_2_NMB_OF_BLOCKS));

  memset(gbi_env_ctrl_blk_p->media_0_partition_2_mem,
    0xFFFFFFFF,
    ((GBI_MEDIA_0_BYTES_PER_BLOCK * GBI_MEDIA_0_PAR_2_NMB_OF_BLOCKS) +
     (GBI_MEDIA_0_SPARE_AREA_SIZE * GBI_MEDIA_0_PAR_2_NMB_OF_BLOCKS)));

  /* Initialize fysical partition 3 */
  gbi_env_ctrl_blk_p->media_0_partition_3_mem = (UINT32 *) malloc(
      (GBI_MEDIA_0_BYTES_PER_BLOCK * GBI_MEDIA_0_PAR_3_NMB_OF_BLOCKS) +
      (GBI_MEDIA_0_SPARE_AREA_SIZE * GBI_MEDIA_0_PAR_3_NMB_OF_BLOCKS));

  memset(gbi_env_ctrl_blk_p->media_0_partition_3_mem,
    0x00000000,
    ((GBI_MEDIA_0_BYTES_PER_BLOCK * GBI_MEDIA_0_PAR_3_NMB_OF_BLOCKS) +
     (GBI_MEDIA_0_SPARE_AREA_SIZE * GBI_MEDIA_0_PAR_3_NMB_OF_BLOCKS)));

  format_partition_fat32(gbi_env_ctrl_blk_p->media_0_partition_3_mem);

}
#endif /** _WINDOWS*/



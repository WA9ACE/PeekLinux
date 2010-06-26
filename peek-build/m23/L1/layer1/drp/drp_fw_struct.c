/**********************************************************************************
drp_fw_struct.c
-- Copyright (c) 2004, Texas Instruments, Inc.
-- Author:

main project function file and global declarations
***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     22/1/2007      Sumeer B         Initial version.
************************************************************************************/


#include "drp_fw_struct.h"
#include "drp_calib_main.h"
#include "drp_main.h"

/******************************************************************************
  DRP FW version and Tag version are picked
  directly from a particular address in flash.
******************************************************************************/
extern const unsigned char drp_ref_sw[];

/* Not Filling Data Correctly, Please Relook */
#pragma DATA_SECTION(drp_fw_certificate, ".drp_cert")
volatile const T_DRP_CERT_TABLE  drp_fw_certificate={
        (UINT32 *)(((unsigned char *)&drp_ref_sw)+4),
        (UINT32 *)(((unsigned char *)&drp_ref_sw)),
        MAX_DRP_SCRIPT,
        0,                                  //dummy1
        {
          (UINT32)&drp_ref_sw,
          0xFFFF,
        },
        {
          DRP_Write_Uplink_Data,
          drp_afe_gain_calib1_fw,
          drp_afe_gain_calib2_fw,
          drp_copy_ref_sw_to_drpsrm_fw,
          drp_copy_sw_data_to_drpsrm_fw,
          drp_dcxo_calib_fw,
          drp_efuse_init,
          drp_gain_correction,
          drp_generate_dbbif_setting_arfcn,
          drp_iqmc_calib_fw,
          drp_lna_cfreq_calib_fw,
          drp_mixer_pole_calib_fw,
          drp_ref_sw_upload_from_ffs,
          drp_sw_data_calib_upload_to_ffs_fw,
          drp_tx_rx_common_calib_fw,
          drp_scf_pole_calib_fw,
          drp_sw_data_calib_upload_from_ffs_fw,
          drp_get_memory_size,
          drpfw_init
        }
       };
#pragma DATA_SECTION(drp_env_int_blk, ".drp_ptr")
T_DRP_ENV_INT_BLK * drp_env_int_blk;
#pragma DATA_SECTION(extRamPtr, ".drp_ptr")
void *extRamPtr;

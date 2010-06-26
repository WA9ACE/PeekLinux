#ifndef __DRP_L1_IF_H__
#define __DRP_L1_IF_H__

/**********************************************************************************
drp_l1_if.h
-- Copyright (c) 2004, Texas Instruments, Inc.
-- Author:

DRP interfaces used by L1.
***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     22/1/2007      Sumeer B         Initial version.
************************************************************************************/

#include "l1_drp_if.h"
#include "drp_api.h"
#include "drp_drive.h"



/************************************************************************************************/
/* function prototypes */
void  DRP_Write_Uplink_Data(SYS_UWORD16 *TM_ul_data);

SINT16 drp_afe_gain_calib1_fw(UWORD16 BAND_INDEX
                           #if DRP_TEST_SW
                           ,UWORD16 pcb_config,
                           ,UWORD16 deq_ctl_flag
                           #endif
                           );
SINT16 drp_afe_gain_calib2_fw(UWORD16 BAND_INDEX
                           #if DRP_TEST_SW
                           ,UWORD16 pcb_config
                           #endif
                           );
uint32 drp_copy_ref_sw_to_drpsrm_fw(void);

/*
   DRP will need to check if calibration data struture in RAM is populated by checking a version string
   If not, use defualt values and commit the structure to DRP SRM.
   T_DRP_SW_DATA type struture is not exposed to L1.
   */
SINT16 drp_copy_sw_data_to_drpsrm_fw(void);

SINT16 drp_dcxo_calib_fw(uint16 BAND_INDEX
                          #if DRP_TEST_SW
                          ,UWORD16 pcb_config
                          #endif
                          );
void drp_efuse_init(void);
WORD16 drp_gain_correction(UWORD16 arfcn, UWORD8 lna_off, UWORD16 agc);


SINT16 drp_iqmc_calib_fw(UWORD16 BAND_INDEX
                          #if DRP_TEST_SW
                          ,UWORD16 pcb_config
                          #endif
                          );

SINT16 drp_lna_cfreq_calib_fw(UWORD16 BAND_INDEX
                           #if DRP_TEST_SW
                           ,UWORD16 pcb_config
                           #endif
                           );
SINT16 drp_mixer_pole_calib_fw(UWORD16 BAND_INDEX
                            #if DRP_TEST_SW
                            ,UWORD16 pcb_config
                            #endif
                            );
int16 drp_ref_sw_upload_from_ffs(const SYS_UWORD8* pathname );

SINT16 drp_sw_data_calib_upload_to_ffs_fw(void);
SINT16 drp_tx_rx_common_calib_fw(
                              #if DRP_TEST_SW
                              UWORD16 pcb_config
                              #endif
                              );
SINT16 drp_sw_data_calib_upload_from_ffs(T_DRP_SW_DATA * ptr_drp_sw_data_calib);

SINT16 drp_scf_pole_calib_fw(UWORD16 BAND_INDEX
                          #if DRP_TEST_SW
                          ,UWORD16 pcb_config
                          #endif
                          );
SINT16 drp_sw_data_calib_upload_from_ffs_fw(void);
/*
 * Initalisation functions
 */

void drp_get_memory_size(uint32 * size_int, uint32 * size_ext);
/* 0 indicates success non zero value indicates failure */
int drpfw_init(const T_MODEM_FUNC_JUMP_TABLE * modem_func_table,const T_MODEM_VAR_JUMP_TABLE * modem_var_table);

typedef struct T_DRP_FUNC_JUMP_TABLE_TAG
{
    void  (*DRP_Write_Uplink_Data)(uint16 *TM_ul_data);
    SINT16 (*drp_afe_gain_calib1)(UWORD16 BAND_INDEX
                          #if DRP_TEST_SW
                           ,UWORD16 pcb_config
                          #endif
                          );
    SINT16 (*drp_afe_gain_calib2)(UWORD16 BAND_INDEX
                          #if DRP_TEST_SW
                          ,UWORD16 pcb_config
                          #endif
                          );
    uint32 (*drp_copy_ref_sw_to_drpsrm)(void);
    SINT16 (*drp_copy_sw_data_to_drpsrm)(void);
    SINT16 (*drp_dcxo_calib)(uint16 BAND_INDEX
                          #if DRP_TEST_SW
                          ,UWORD16 pcb_config
                          #endif
                          );
    void (*drp_efuse_init)(void);
    WORD16 (*drp_gain_correction)(UWORD16 arfcn, UWORD8 lna_off, UWORD16 agc);
    uint16 (*drp_generate_dbbif_setting_arfcn)(uint16 pcb_config, uint16 arfcn);
    SINT16 (*drp_iqmc_calib)(UWORD16 BAND_INDEX
                          #if DRP_TEST_SW
                          ,UWORD16 pcb_config
                          #endif
                          );
    SINT16 (*drp_lna_cfreq_calib)(UWORD16 BAND_INDEX
                          #if DRP_TEST_SW
                          ,UWORD16 pcb_config
                          #endif
                          );
    SINT16 (*drp_mixer_pole_calib)(UWORD16 BAND_INDEX
                          #if DRP_TEST_SW
                          ,UWORD16 pcb_config
                          #endif
                          );
    int16 (*drp_ref_sw_upload_from_ffs)(const uint8* pathname);
    SINT16 (*drp_sw_data_calib_upload_to_ffs)(void);
    SINT16 (*drp_tx_rx_common_calib)(
                          #if DRP_TEST_SW
                          UWORD16 pcb_config
                          #endif
                          );
    SINT16 (*drp_scf_pole_calib)(UWORD16 BAND_INDEX
                          #if DRP_TEST_SW
                          ,UWORD16 pcb_config,
                          #endif
                          );
    SINT16 (*drp_sw_data_calib_upload_from_ffs)(void);
    void (*drp_get_memory_size)(uint32 * size_int, uint32 * size_ext);
    int (*drpfw_init)(const T_MODEM_FUNC_JUMP_TABLE * modem_func_table, const T_MODEM_VAR_JUMP_TABLE * modem_var_table);

}T_DRP_FUNC_JUMP_TABLE;

#endif /*__DRP_L1_API_H__ */


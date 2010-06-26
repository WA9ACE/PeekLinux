/**********************************************************************************
drp_l1_wrapper.h
-- Copyright (c) 2004, Texas Instruments, Inc.
-- Author:

DRP interfaces wrapper used by L1.
***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     22/1/2007      Sumeer B         Initial version.
************************************************************************************/

#include "l1_drp_if.h"
#include "drp_fw_struct.h"
#include "drp_l1_if.h"
#include "drp_calib_main.h"
#include "drp_main.h"
#include "drp_defines.h"
#include "l1_rf61.h"
#include "drp_fw_struct.h"

extern const unsigned char drp_ref_sw[];
extern const T_DRP_SW_DATA drp_sw_data_init_const;
extern volatile const T_DRP_CERT_TABLE  drp_fw_certificate;

unsigned long convert_nanosec_to_cycles(unsigned long time) {
    return drp_env_int_blk->modem_func_tbl->convert_nanosec_to_cycles(time);
}


void wait_ARM_cycles(unsigned long loop) {
    drp_env_int_blk->modem_func_tbl->wait_ARM_cycles(loop);
}

T_FFS_FD  ffs_open(const char *name, T_FFS_OPEN_FLAGS option) {

    return drp_env_int_blk->modem_func_tbl->ffs_open(name,option);
}

T_FFS_SIZE ffs_write(T_FFS_FD fdi, void *src, T_FFS_SIZE amount) {

    return drp_env_int_blk->modem_func_tbl->ffs_write(fdi,src,amount);
}

T_FFS_RET ffs_close(T_FFS_FD fdi) {
    return drp_env_int_blk->modem_func_tbl->ffs_close(fdi);
}

T_FFS_SIZE ffs_read(T_FFS_FD fdi, void *src, T_FFS_SIZE size) {
    return drp_env_int_blk->modem_func_tbl->ffs_read(fdi,src,size);
}

int ffs_fread(const char *name, void *addr, int size) {
    return drp_env_int_blk->modem_func_tbl->ffs_fread(name, addr, size);
}

void (TP_Enable)(SYS_UWORD16 on) {
    drp_env_int_blk->modem_func_tbl->TP_Enable(on);
    return;
}

/**** DRP wrapper functions *********/
SINT16 drp_afe_gain_calib1_fw(UWORD16 BAND_INDEX
#if DRP_TEST_SW
        ,UWORD16 pcb_config,
        ,UWORD16 deq_ctl_flag
#endif
        ) {
    return drp_afe_gain_calib1(BAND_INDEX
#if DRP_TEST_SW
            ,pcb_config,deq_ctl_flag
#endif
            ,&drp_sw_data_calib);
}
/*****************************************************************************/
SINT16 drp_afe_gain_calib2_fw(UWORD16 BAND_INDEX
#if DRP_TEST_SW
        ,UWORD16 pcb_config
#endif
        ) {
    return drp_afe_gain_calib2(BAND_INDEX
#if DRP_TEST_SW
            ,pcb_config
#endif
            ,&drp_sw_data_calib);
}
/*****************************************************************************/
uint32 drp_copy_ref_sw_to_drpsrm_fw(void) {
    return drp_copy_ref_sw_to_drpsrm((unsigned char *)drp_ref_sw);
}
/*****************************************************************************/

SINT16 drp_copy_sw_data_to_drpsrm_fw(void) {
    return drp_copy_sw_data_to_drpsrm(&drp_sw_data_calib);
}
/*****************************************************************************/
SINT16 drp_dcxo_calib_fw(uint16 BAND_INDEX
#if DRP_TEST_SW
        ,UWORD16 pcb_config
#endif
        ) {
    return drp_dcxo_calib(BAND_INDEX
#if DRP_TEST_SW
            ,pcb_config
#endif
            ,&drp_sw_data_calib);
}
/*****************************************************************************/
SINT16 drp_iqmc_calib_fw(UWORD16 BAND_INDEX
#if DRP_TEST_SW
        ,UWORD16 pcb_config
#endif
        ) {
    return drp_iqmc_calib(BAND_INDEX
#if DRP_TEST_SW
            ,pcb_config
#endif
            ,&drp_sw_data_calib);
}
/*****************************************************************************/
SINT16 drp_lna_cfreq_calib_fw(UWORD16 BAND_INDEX
#if DRP_TEST_SW
        ,UWORD16 pcb_config
#endif
        ) {
    return drp_lna_cfreq_calib(BAND_INDEX
#if DRP_TEST_SW
            ,pcb_config
#endif
            ,&drp_sw_data_calib);
}
/*****************************************************************************/
SINT16 drp_mixer_pole_calib_fw(UWORD16 BAND_INDEX
#if DRP_TEST_SW
        ,UWORD16 pcb_config
#endif
        ) {
    return drp_mixer_pole_calib(BAND_INDEX
#if DRP_TEST_SW
            ,pcb_config
#endif
            ,&drp_sw_data_calib);
}
/*****************************************************************************/
SINT16 drp_sw_data_calib_upload_to_ffs_fw(void){
    return drp_sw_data_calib_upload_to_ffs(&drp_sw_data_calib);
}
/*****************************************************************************/
SINT16 drp_tx_rx_common_calib_fw(
#if DRP_TEST_SW
        UWORD16 pcb_config
#endif
        ) {
    return drp_tx_rx_common_calib(
#if DRP_TEST_SW
            pcb_config,
#endif
            &drp_sw_data_calib);
}
/*****************************************************************************/
SINT16 drp_scf_pole_calib_fw(UWORD16 BAND_INDEX
#if DRP_TEST_SW
        ,UWORD16 pcb_config
#endif
        ) {
    return drp_scf_pole_calib(BAND_INDEX
#if DRP_TEST_SW
            pcb_config,
#endif
            ,&drp_sw_data_calib);
}
/*****************************************************************************/
SINT16 drp_sw_data_calib_upload_from_ffs_fw(void) {
    return drp_sw_data_calib_upload_from_ffs(&drp_sw_data_calib);
}
/*****************************************************************************/
void drp_get_memory_size(uint32 * size_int, uint32 * size_ext)
{
    *size_int=sizeof(T_DRP_ENV_INT_BLK);
    *size_ext=0;
}
/*****************************************************************************/
#if 0
/* Keeping Initializers as Const Will Help make init code simple */
const SINT16 abe_est_table_init[NUM_ABE_GAINS] = {
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000,
  0x0000, 0x0000
};

const SINT16 afe_est_table_init[NUM_AFE_GAINS] = {
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000
};


const SINT16 tar_abe_gain_table_init[DRP_NUM_ABE_GAIN_STEPS] = {
  DRP_GAIN_0_DB,
  DRP_GAIN_2_0_DB,
  DRP_GAIN_5_0_DB,
  DRP_GAIN_8_0_DB,
  DRP_GAIN_11_0_DB,
  DRP_GAIN_14_0_DB,
  DRP_GAIN_17_0_DB,
  DRP_GAIN_20_0_DB,
  DRP_GAIN_23_0_DB,
  DRP_GAIN_26_0_DB,
  DRP_GAIN_29_0_DB
};


const SINT16 tar_afe_gain_table_init[2][DRP_NUM_AFE_GAIN_STEPS] = {
  {
    DRP_GAIN_11_0_DB,  //LB
    DRP_GAIN_36_5_DB,  //LB
  },
  {
    DRP_GAIN_11_0_DB,  //HB
    DRP_GAIN_36_5_DB,  //HB
  }
};
#endif

const UINT16 g_band_cntr_ch[NUM_BANDS] = {
   EGSM_CENTER_CH,
   GSM_CENTER_CH,
   DCS_CENTER_CH,
   PCS_CENTER_CH
};

const UINT16 default_ABE_step_table[DRP_NUM_ABE_GAIN_STEPS] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
const UINT16 default_AFE_step_table[2][DRP_NUM_AFE_GAIN_STEPS] = {17, 0,
                                                            18, 0};


/* Drp_main.c */



const unsigned char drp_dcxo_calib_sw[] = {

/* Tag  */ 0x00, 0x00, 0x00, 0x00,
/* Vars */ 0x00, 0x00, 0x00, 0x00,

/* size */ 0x02, 0x00, 0x00, 0x00,
/* addr */ 0x40, 0x05, 0x00, 0x00,
0x04, 0x2a, 0x00, 0x00,

/* size */ 0x36, 0x00, 0x00, 0x00,
/* addr */ 0x04, 0x2a, 0x00, 0x00,
0x0f, 0x00, 0xac, 0x42, 0x07, 0x00, 0xad, 0x42, 0xe0, 0x00, 0x00, 0x82, 0xe0, 0x42, 0x08, 0x82,
0xe0, 0x84, 0x10, 0x82, 0x00, 0x03, 0x21, 0x85, 0x00, 0x88, 0x29, 0x81, 0x01, 0x00, 0x30, 0x9f,
0x00, 0x40, 0x21, 0x91, 0xa0, 0x04, 0x01, 0x82, 0xc0, 0x40, 0x10, 0xa4, 0xe8, 0x46, 0x08, 0x8e,
0xe0, 0x40, 0x08, 0x81, 0x00, 0x83, 0x31, 0x81, 0x00, 0x49, 0x29, 0x81, 0x00, 0x8e, 0x21, 0x8a,
0xf7, 0x3f, 0x01, 0xa9, 0xc0, 0x55, 0x08, 0x86, 0xc0, 0x95, 0x10, 0x86, 0xe8, 0x46, 0x18, 0x8e,
0xe3, 0xc4, 0x18, 0x8e, 0xe3, 0x42, 0x08, 0x8e, 0xe3, 0x84, 0x10, 0x8e, 0xe0, 0x44, 0x00, 0x81,
0xe0, 0x06, 0x00, 0x82, 0x0E, 0x00, 0x80, 0x22, 0x00, 0x00, 0x00, 0x00,

/* Final Record */
/* size */ 0x00, 0x00, 0x00, 0x00,

};

#if DRP_FLYBACK_CALIB
const unsigned char drp_phe_anayser_dft[] = {

// this is the for test clock 1
/* Tag  */ 0x00, 0x00, 0x00, 0x00,
/* Vars */ 0x00, 0x00, 0x00, 0x00,


/* size */ 0x02, 0x00, 0x00, 0x00,
/* addr */ 0x20, 0x05, 0x00, 0x00,


0x00, 0x23, 0x00, 0x00,

/* size */ 0x3e, 0x00, 0x00, 0x00,
/* addr */ 0x00, 0x23, 0x00, 0x00,


0x00, 0x00, 0x20, 0x9f, 0x00, 0x00, 0x28, 0x9f, 0x00, 0x80, 0x30, 0xa2, 0xff, 0x7f, 0x38, 0xa2,
0x00, 0x0b, 0x42, 0x85, 0x03, 0x00, 0x3a, 0x2f, 0x10, 0x00, 0x48, 0x9f, 0x00, 0x00, 0x58, 0x9f,
0x00, 0x00, 0x60, 0x9f, 0x00, 0x03, 0x42, 0x82, 0x02, 0x03, 0x00, 0xab, 0x6c, 0x5e, 0x53, 0x93,
0x00, 0x8e, 0x02, 0x8b, 0x01, 0x00, 0x00, 0xa9, 0x00, 0x80, 0x3a, 0xa4, 0x00, 0x8c, 0x02, 0x8a,
0x01, 0x00, 0x00, 0xa9, 0x00, 0x80, 0x32, 0xa4, 0x00, 0x91, 0x52, 0x86, 0x00, 0xd4, 0x5a, 0x81,
0x04, 0x94, 0x52, 0x8e, 0x00, 0x14, 0x63, 0x81, 0x00, 0x43, 0x4a, 0x82, 0xf3, 0x7f, 0x02, 0xaa,
0x00, 0x16, 0x21, 0x81, 0x00, 0x0b, 0x63, 0x86, 0x00, 0x58, 0x29, 0x81, 0xea, 0x3f, 0x02, 0xaa,
0x00, 0x13, 0x21, 0x86, 0x00, 0x00, 0x3a, 0x2f, 0x00, 0x00, 0x00, 0x00,

/* Final Record */
/* size */ 0x00, 0x00, 0x00, 0x00,
};
#endif
int drpfw_init(const T_MODEM_FUNC_JUMP_TABLE * modem_func_table, const T_MODEM_VAR_JUMP_TABLE * modem_var_table) {
    memset(drp_env_int_blk, 0, sizeof(T_DRP_ENV_INT_BLK));
    drp_env_int_blk->modem_func_tbl = modem_func_table;
    drp_env_int_blk->modem_var_tbl = modem_var_table;
    /* Initalise the global variables */

    // Base pointer to DRP Registers
    drp_regs = (T_DRP_REGS_STR*) DRP_REGS_BASE_ADD;

    // Base pointer to SRM data structure for Calibration data and tables
    drp_srm_data = (T_DRP_SRM_DATA*) DRP_SRM_DATA_ADD;

    // Base pointer to SRM API data structure
    drp_srm_api  = (T_DRP_SRM_API*) DRP_SRM_API_ADD;     // Re-Used in L1/DSP software

    //g_pcb_config = RF_BAND_SYSTEM_INDEX;
    return 0;
}

/*****************************************************************************/
/*****************************************************************************/




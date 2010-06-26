
#include "l1_drp_if.h"
#include "l1_drp_inc.h"
#include "clkm.h"
#include "sys.cfg"
#include "l1sw.cfg"
#include "l1_types.h"
#include "tpudrv.h"
//#include "l1_const.h"
#include "l1_confg.h"

#if TESTMODE
#include "l1tm_defty.h"
#endif
#if (AUDIO_TASK == 1)
#include "l1audio_const.h"
#include "l1audio_cust.h"
#include "l1audio_defty.h"
#endif
#if (L1_GTT == 1)
#include "l1gtt_const.h"
#include "l1gtt_defty.h"
#endif
#if (L1_DYN_DSP_DWNLD == 1)
#include "l1_dyn_dwl_const.h"
#include "l1_dyn_dwl_defty.h"
#endif
#if (L1_MP3 == 1)
#include "l1mp3_defty.h"
#endif
#if (L1_MIDI == 1)
#include "l1midi_defty.h"
#endif
#if (L1_AAC == 1)
#include "l1aac_defty.h"
#endif

#include "l1_defty.h"
#include "l1_varex.h"
#include "drp_fw_struct.h"

extern UWORD16  *TP_Ptr;

// Base pointer to DRP Registers
T_DRP_REGS_STR *drp_regs = (T_DRP_REGS_STR*) DRP_REGS_BASE_ADD;

// Base pointer to SRM data structure for Calibration data and tables
//T_DRP_SRM_DATA *drp_srm_data = (T_DRP_SRM_DATA*) DRP_SRM_DATA_ADD;

// Base pointer to SRM API data structure
T_DRP_SRM_API  *drp_srm_api  = (T_DRP_SRM_API*) DRP_SRM_API_ADD;     // Re-Used in L1/DSP software



/*****************************************************************************/
#pragma DATA_SECTION(drp_cert_var, ".drp_cert")
T_DRP_CERT_TABLE  drp_cert_var;
T_DRP_FUNC_JUMP_TABLE * drp_func_ptr;
SYS_UWORD32 drp_ref_sw_tag; /* Tag     */
SYS_UWORD32 drp_ref_sw_ver; /* Version */
/*****************************************************************************/
void drp_api_addr_init(void){
  // Base pointer to DRP Registers
   drp_regs = (T_DRP_REGS_STR*) DRP_REGS_BASE_ADD;
   // Base pointer to SRM data structure for Calibration data and tables
   //   drp_srm_data = (T_DRP_SRM_DATA*) DRP_SRM_DATA_ADD;
   // Base pointer to SRM API data structure
   drp_srm_api    = (T_DRP_SRM_API*) DRP_SRM_API_ADD;
   /* Tag    */
   drp_ref_sw_tag = *(drp_cert_var.drp_refsw_tag_mem);
   /* Version*/
   drp_ref_sw_ver = *(drp_cert_var.drp_refsw_ver_mem);
   drp_func_ptr   = &(drp_cert_var.functptr);
}


void drp_get_memory_size(uint32 *intSize,uint32 *extSize){
    drp_func_ptr->drp_get_memory_size(intSize,extSize);
    return;
}

int drpfw_init(const T_MODEM_FUNC_JUMP_TABLE * modem_func_table, const T_MODEM_VAR_JUMP_TABLE * modem_var_table){
    return drp_func_ptr->drpfw_init(modem_func_table,modem_var_table);
}
SINT16 drp_lna_cfreq_calib(UWORD16 BAND_INDEX,
                           T_DRP_SW_DATA *sw_data_ptr){
  return drp_func_ptr->drp_lna_cfreq_calib(BAND_INDEX);
}

SINT16 drp_mixer_pole_calib(UWORD16 BAND_INDEX,
                            T_DRP_SW_DATA *sw_data_ptr){
  return drp_func_ptr->drp_mixer_pole_calib(BAND_INDEX);
}

SINT16 drp_iqmc_calib(UWORD16 BAND_INDEX,
                      T_DRP_SW_DATA *sw_data_ptr){
  return drp_func_ptr->drp_iqmc_calib(BAND_INDEX);
}



void drp_efuse_init(void){
  drp_func_ptr->drp_efuse_init();
}

WORD16 drp_gain_correction(UWORD16 arfcn, UWORD8 lna_off, UWORD16 agc){
  return drp_func_ptr->drp_gain_correction(arfcn,lna_off,agc);
}

SINT16 drp_sw_data_calib_upload_to_ffs(T_DRP_SW_DATA * ptr_drp_sw_data_calib) {
  return drp_func_ptr->drp_sw_data_calib_upload_to_ffs();
}

SINT16 drp_tx_rx_common_calib(T_DRP_SW_DATA *sw_data_ptr){
  return drp_func_ptr->drp_tx_rx_common_calib();
}

void  DRP_Write_Uplink_Data(SYS_UWORD16 *TM_ul_data){
  drp_func_ptr->DRP_Write_Uplink_Data(TM_ul_data);
}

SYS_WORD16 drp_ref_sw_upload_from_ffs(const SYS_UWORD8* pathname ){
  return drp_func_ptr->drp_ref_sw_upload_from_ffs(pathname);
}

SINT16 drp_scf_pole_calib(UWORD16 BAND_INDEX, T_DRP_SW_DATA *sw_data_ptr){
  return drp_func_ptr->drp_scf_pole_calib(BAND_INDEX);
}

SINT16 drp_sw_data_calib_upload_from_ffs(T_DRP_SW_DATA * ptr_drp_sw_data_calib){
  return drp_func_ptr->drp_sw_data_calib_upload_from_ffs();
}

uint32  drp_copy_ref_sw_to_drpsrm(unsigned char *ref_sw_ptr){
  return drp_func_ptr->drp_copy_ref_sw_to_drpsrm();
}

SINT16 drp_copy_sw_data_to_drpsrm(T_DRP_SW_DATA *drp_sw_data){
  return drp_func_ptr->drp_copy_sw_data_to_drpsrm();
}

SINT16 drp_dcxo_calib(UINT16 BAND_INDEX, T_DRP_SW_DATA *sw_data_ptr){
  return drp_func_ptr->drp_dcxo_calib(BAND_INDEX);
}

UINT16 drp_generate_dbbif_setting_arfcn(UINT16 pcb_config, UINT16 arfcn){
  return drp_func_ptr->drp_generate_dbbif_setting_arfcn(pcb_config,arfcn);
}


SINT16 drp_afe_gain_calib1(UWORD16 BAND_INDEX,
                           T_DRP_SW_DATA *sw_data_ptr){
  return drp_func_ptr->drp_afe_gain_calib1(BAND_INDEX);
}
SINT16 drp_afe_gain_calib2(UWORD16 BAND_INDEX,
                           T_DRP_SW_DATA *sw_data_ptr){
  return drp_func_ptr->drp_afe_gain_calib2(BAND_INDEX);
}

/* FIXME FIXME These two needs to be removed */
#pragma DATA_SECTION(drp_sw_data_calib, ".drp_dummy_flash")
T_DRP_SW_DATA drp_sw_data_calib={0xDEAD};
#pragma DATA_SECTION(drp_sw_data_init, ".drp_dummy_flash")
T_DRP_SW_DATA drp_sw_data_init={0xBEAF};

const unsigned char drp_ref_sw[1]={0}; /* Script data*/

/* FIXME FIXME to be removed*/
UINT16 g_pcb_config = RF_BAND_SYSTEM_INDEX;


/*C file initalisation*/
const T_MODEM_FUNC_JUMP_TABLE modem_func_jump_table =
{
  convert_nanosec_to_cycles,
  wait_ARM_cycles,
  ffs_open,
  ffs_write,
  ffs_close,
  ffs_read,
  ffs_fread,
  TP_Enable
};

const T_MODEM_VAR_JUMP_TABLE modem_var_jump_table =
{
  (volatile SYS_UWORD16 **)&TP_Ptr,
#if (L1_FF_MULTIBAND == 0)    
  &l1_config.std.id,
#endif  
};



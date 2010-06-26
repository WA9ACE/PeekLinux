/**********************************************************************************
drp_main.h
-- Copyright (c) 2004, Texas Instruments, Inc.
-- Author: Sumeer Bhatara, Shrinivas Gadkari and Pradeep P

header file for drp_main.c
***********************************************************************************
                               Change History
************************************************************************************

Version     Date           Author(s)        Comments
-------------------------------------------------------------------------------------
Ver 0.0     25 Nov 2004    SG, SB and PP    Locosto version based on DRP ver 0.23
Ver 1.01    29 Dec 2004    SG               Based on DRP Ref SW 1.0, with addresses
                                            of srm variables and tables aligned to
                                            locosto scripts version 129.03
************************************************************************************/
/*
#if DRP_STANDALONE_BUILD
void drp_calib_main(void);
#endif // DRP_STANDALONE_BUILD

void drp_copy_sw_data_to_drpsrm(T_DRP_SW_DATA *drp_sw_data);
void drp_copy_ref_sw_to_drpsrm(unsigned char *ref_sw_ptr);
void drp_tx_rx_common_calib(T_DRP_SW_DATA *sw_data_ptr);
void drp_lna_cfreq_calib(UWORD16 BAND_INDEX, T_DRP_SW_DATA *sw_data_ptr);
void drp_iqmc_calib(UWORD16 BAND_INDEX, T_DRP_SW_DATA *sw_data_ptr);
void drp_mixer_pole_calib(UWORD16 BAND_INDEX, T_DRP_SW_DATA *sw_data_ptr);
void drp_scf_pole_calib(UWORD16 BAND_INDEX, T_DRP_SW_DATA *sw_data_ptr);
void drp_afe_gain_calib1(UWORD16 BAND_INDEX, T_DRP_SW_DATA *sw_data_ptr);
void drp_afe_gain_calib2(UWORD16 BAND_INDEX, T_DRP_SW_DATA *sw_data_ptr);
void drp_rx_fcw(UINT16 BAND_INDEX, UINT32 FCW_OFFSET);
*/

#if _DEBUG
extern void drp_tspact_check(void);
#endif
extern void drp_rx_tspact_enable(unsigned short band);

typedef struct T_DRP_ENV_INT_BLK_TAG
{
  SYS_WORD32 drp_ref_sw_tag; /* Tag    */
  SYS_WORD32 drp_ref_sw_ver; /* Version*/
  T_DRP_SW_DATA drp_sw_data_calib;
  T_DRP_SW_DATA * drp_sw_data;
  T_DRP_REGS_STR * drp_regs;
  T_DRP_SRM_DATA * drp_srm_data;
  T_DRP_SRM_API * drp_srm_api;
  UINT16 g_pcb_config;
  SINT16 g_afe_gain;
  const T_MODEM_FUNC_JUMP_TABLE * modem_func_tbl;
  const T_MODEM_VAR_JUMP_TABLE * modem_var_tbl;
} T_DRP_ENV_INT_BLK;


extern T_DRP_ENV_INT_BLK * drp_env_int_blk;

#if (DRP_FW_BUILD==1)
#define TP_Ptr  (*(drp_env_int_blk->modem_var_tbl->TP_ptr))
/* From drv_drive.c */
#define drp_ref_sw_tag (drp_env_int_blk->drp_ref_sw_tag)
#define drp_ref_sw_ver (drp_env_int_blk->drp_ref_sw_ver)


/* From drp_calib_main.c */
#define g_afe_gain              (drp_env_int_blk->g_afe_gain)

/* From drp_main.c */
#define drp_sw_data (drp_env_int_blk->drp_sw_data)
#define drp_regs   (drp_env_int_blk->drp_regs)
#define drp_srm_data (drp_env_int_blk->drp_srm_data)
#define drp_srm_api  (drp_env_int_blk->drp_srm_api)
#define g_pcb_config (drp_env_int_blk->g_pcb_config)
#define drp_sw_data_calib   (drp_env_int_blk->drp_sw_data_calib)
#endif // DRP_FW_BUILD

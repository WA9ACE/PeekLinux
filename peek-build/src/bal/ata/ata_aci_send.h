#ifndef __BTC_ATA_ACI_SEND_H_20080114__
#define __BTC_ATA_ACI_SEND_H_20080114__

#ifdef __cplusplus
extern "C" {
#endif

#include "ata_cfg.h"

#ifdef ATA_CFG_USE_ACI

#include "ata_datatype.h"

#include "typedefs.h"
#include "vsi.h"
#include "gsm.h" 
#include "message.h"
#include "prim.h"
#include "p_em.h"
#include "m_sms.h"
#include "m_fac.h"
#include "p_mncc.h"
#include "p_mnsms.h"
#include "p_sim.h"
#include "p_mmreg.h"
#include "p_mmi.h"
#include "aci_cmh.h"

#include "p_8010_137_nas_include.h"
#include "p_8010_153_cause_include.h"
#include "p_8010_152_ps_include.h"



#include "gaci_cmh.h"

#ifdef ATA_UNIT_TEST
extern T_ATA_return ata_plus_test_int_send(const char* at_cmd);
extern T_ATA_return ata_plus_test_str_send(const char* at_cmd);
#endif /* ATA_UNIT_TEST */

extern T_ATA_return ata_at_d_send(const char* at_cmd);
extern T_ATA_return ata_at_h_send(const char* at_cmd);
extern T_ATA_return ata_at_a_send(const char* at_cmd);
extern T_ATA_return ata_percent_nrg_send(const char* at_cmd);
extern T_ATA_return ata_plus_cfun_send(const char* at_cmd);
extern T_ATA_return ata_plus_cfun_q_send(const char* at_cmd);
extern T_ATA_return ata_percent_csq_send(const char* at_cmd);
extern T_ATA_return ata_plus_cops_send(const char* at_cmd);
extern T_ATA_return ata_percent_creg_send(const char* at_cmd);
extern T_ATA_return ata_plus_clan_send(const char* at_cmd);
extern T_ATA_return ata_plus_cimi_send(const char* at_cmd);
extern T_ATA_return ata_plus_csq_send(const char* at_cmd);
extern T_ATA_return ata_plus_ccwa_send(const char* at_cmd);
extern T_ATA_return ata_percent_chld_send(const char* at_cmd);
extern T_ATA_return ata_plus_cnma_send(const char* at_cmd);
extern T_ATA_return ata_plus_cnmi_send(const char* at_cmd);
extern T_ATA_return ata_plus_csca_send(const char* at_cmd);
extern T_ATA_return ata_plus_cscs_send(const char* at_cmd);
extern T_ATA_return ata_plus_csms_send(const char* at_cmd);
extern T_ATA_return ata_plus_csvm_send(const char* at_cmd);
extern T_ATA_return ata_plus_cscb_send(const char* at_cmd);
extern T_ATA_return ata_plus_clip_send(const char* at_cmd);
extern T_ATA_return ata_plus_clir_send(const char* at_cmd);
extern T_ATA_return ata_plus_cmgd_send(const char* at_cmd);
extern T_ATA_return ata_plus_cmgl_send(const char* at_cmd);
extern T_ATA_return ata_plus_cmgr_send(const char* at_cmd);
extern T_ATA_return ata_plus_cmgs_send(const char* at_cmd);
extern T_ATA_return ata_plus_cmgw_send(const char* at_cmd);
extern T_ATA_return ata_plus_cmss_send(const char* at_cmd);
extern T_ATA_return ata_plus_cacm_send(const char* at_cmd);
extern T_ATA_return ata_plus_caoc_send(const char* at_cmd);
extern T_ATA_return ata_plus_ccfc_send(const char* at_cmd);
extern T_ATA_return ata_plus_cpbr_send(const char* at_cmd);
extern T_ATA_return ata_plus_cpbs_send(const char* at_cmd);
extern T_ATA_return ata_plus_cpuc_send(const char* at_cmd);
extern T_ATA_return ata_plus_cpwd_send(const char* at_cmd);
extern T_ATA_return ata_plus_cpin_send(const char* at_cmd);
extern T_ATA_return ata_plus_cpol_send(const char* at_cmd);
extern T_ATA_return ata_plus_clck_send(const char* at_cmd);
extern T_ATA_return ata_plus_cpbw_send(const char* at_cmd);
extern T_ATA_return ata_plus_cpms_send(const char* at_cmd);
extern T_ATA_return ata_plus_vts_send(const char* at_cmd);
extern T_ATA_return ata_plus_cusd_send(const char* at_cmd);
extern T_ATA_return ata_percent_satc_send(const char* at_cmd);
extern T_ATA_return ata_percent_sate_send(const char* at_cmd);
extern T_ATA_return ata_percent_satr_send(const char* at_cmd);
extern T_ATA_return ata_percent_satt_send(const char* at_cmd);
extern T_ATA_return ata_plus_cgsn_send(const char* at_cmd);    

#ifdef ATA_GPRS
extern T_ATA_return ata_plus_cgatt_send(const char* at_cmd);
#endif

#endif

#ifdef __cplusplus
}
#endif
#endif /*__BTC_ATA_ACI_SEND_H_20080114__*/

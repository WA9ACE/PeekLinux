/**********************************************************************************
Ver 0.0     12 May 2004    SG               Initial creation

************************************************************************************/

#ifndef  L1_DRP_INC_H
#define  L1_DRP_INC_H

#include "l1_types.h"
#include "sys_types.h"  
#include "general.h"
#include "l1_const.h"
#include "l1_rf61.h"

/* FIXME FIXME */
#include "drp_l1_if.h"
#include "drp_fw_struct.h"

void drp_api_addr_init(void);

extern SYS_UWORD32 drp_ref_sw_tag; /* Major version */
extern SYS_UWORD32 drp_ref_sw_ver; /* Minor version */

extern T_DRP_CERT_TABLE drp_cert_var;
extern T_DRP_FUNC_JUMP_TABLE * drp_func_ptr;
extern const T_MODEM_FUNC_JUMP_TABLE modem_func_jump_table;
extern const T_MODEM_VAR_JUMP_TABLE modem_var_jump_table;

extern T_DRP_SRM_API *drp_srm_api;
extern  T_DRP_REGS_STR *drp_regs;
extern T_DRP_SW_DATA drp_sw_data_calib;
extern T_DRP_SW_DATA drp_sw_data_init;
extern UINT16 g_pcb_config;



//extern UWORD16  *TP_Ptr;

#endif /* L1_DRP_INC_H */



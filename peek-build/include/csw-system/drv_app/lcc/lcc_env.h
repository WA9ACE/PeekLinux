/*******************************************************************************
 *
 * PWR_ENV.H
 *
 *
 * (C) Texas Instruments 2001
 *
 ******************************************************************************/

#ifndef __LCC_ENV_H__
#define __LCC_ENV_H__

#include "rvm/rvm_gen.h"
#include "lcc/lcc_api.h"

#include "rvf/rvf_pool_size.h"	/* Stack & Memory Bank sizes definitions */

#define LCC_TASK_VERSION 0x0001 

#define LCC_MAILBOX_USED   RVF_TASK_MBOX_1

/* memory bank size and watermark */
#define LCC_MB_PRIM_SIZE            LCC_MB1_SIZE

#define LCC_MB_PRIM_WATERMARK     (LCC_MB_PRIM_SIZE)

#define LCC_MB_PRIM_INC_SIZE        0
#define LCC_MB_PRIM_INC_WATERMARK   0

extern T_PWR_CTRL_BLOCK *pwr_ctrl;
extern T_PWR_CFG_BLOCK *pwr_cfg;

/* Prototypes */

T_RVM_RETURN lcc_get_info (T_RVM_INFO_SWE  *infoSWE);

T_RVM_RETURN pwr_set_info( T_RVF_ADDR_ID  addr_id,
                           T_RV_RETURN    return_path[],
                           T_RVF_MB_ID    mbId[],
                           T_RVM_RETURN   (*callBackFct) (T_RVM_NAME SWEntName,
                           T_RVM_RETURN errorCause,
                           T_RVM_ERROR_TYPE errorType,
                           T_RVM_STRING errorMsg));

T_RVM_RETURN pwr_init (void);
T_RVM_RETURN pwr_start (void);
T_RVM_RETURN pwr_stop (T_RV_HDR *msg);
T_RVM_RETURN pwr_kill (void);


#endif /* __LCC_ENV_H__ */

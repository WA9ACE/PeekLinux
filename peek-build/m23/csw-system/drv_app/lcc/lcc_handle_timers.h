/******************************************************************************
 * Power Task (pwr)
 * Design and coding by Svend Kristian Lindholm, skl@ti.com
 *
 * PWR Message Processing
 *
 * $Id: pwr_handle_timers.h 1.1 Wed, 20 Aug 2003 10:22:37 +0200 skl $
 *
 ******************************************************************************/
#ifndef __LCC_HANDLE_TIMERS_H_
#define __LCC_HANDLE_TIMERS_H_

#include "lcc/lcc.h"

T_RVM_RETURN process_pwr_handle_T1_expiration    (T_PWR_REQ *request);
T_RVM_RETURN process_pwr_handle_T2_expiration    (T_PWR_REQ *request);
T_RVM_RETURN process_pwr_handle_T3_expiration    (T_PWR_REQ *request);
T_RVM_RETURN process_pwr_handle_T4_expiration    (T_PWR_REQ *request);
T_RVM_RETURN process_pwr_handle_mmi_info_expiration(T_PWR_REQ *request);

void pwr_check_timers(void);
T_RVM_RETURN pwr_create_timer(UINT32 *timer_begin);
T_RVM_RETURN pwr_start_timer(UINT32 *timer_begin);

#endif

/*
+------------------------------------------------------------------------------
|  File:       remu_timer.h
+------------------------------------------------------------------------------
|  Copyright 2003 Texas Instruments Berlin, AG
|                 All rights reserved.
|
|                 This file is confidential and a trade secret of Texas
|                 Instruments Berlin, AG
|                 The receipt of or possession of this file does not convey
|                 any rights to reproduce or disclose its contents or to
|                 manufacture, use, or sell anything it may describe, in
|                 whole, or in part, without the specific written consent of
|                 Texas Instruments Berlin, AG.
+-----------------------------------------------------------------------------
|  Purpose :  Definitions for the riviera emulator timer managment.
+-----------------------------------------------------------------------------
*/

#ifndef REMU_TIMER_H
#define REMU_TIMER_H

/*==== INCLUDES =============================================================*/

/*==== CONSTANTS ============================================================*/

/*==== TYPES ================================================================*/

/*==== PROTOTYPES ===========================================================*/

/* Disable Interrupts, Enable Interrupts*/
void	 gsp_enable(void);
void	 gsp_disable(void);

/* Timer management*/

T_RVF_TIMER_ID  gsp_create_timer(T_RVF_G_ADDR_ID task_id, UINT32  timerDuration, BOOLEAN isContinuous, SHORT (*timeout_action) (OS_HANDLE,OS_HANDLE,USHORT));
void  gsp_del_timer(T_RV_TM_ID tm_id);

void  gsp_reset_timer	(T_RV_TM_ID tm_id, UINT32 new_duration, BOOLEAN isContinuous);

void   gsp_delay(UINT32 ticks);

void   gsp_start_timer(UINT8 tnum, UINT32 ticks, BOOLEAN is_continuous);

void   gsp_stop_timer(UINT8 tnum);

char*    gsp_get_time_stamp(char* tbuf);

UINT32   gsp_get_tick_count(void);

void   gsp_init_timer_list(T_RVF_TIMER_LIST_Q* p_timer_listq);

void   gsp_init_timer_list_entry(T_RVF_TIMER_LIST_ENT * p_tle);

UINT16   gsp_update_timer_list(T_RVF_TIMER_LIST_Q* p_timer_listq);

void   gsp_add_to_timer_list(T_RVF_TIMER_LIST_Q* p_timer_listq, T_RVF_TIMER_LIST_ENT * p_tle);

void   gsp_remove_from_timer_list(T_RVF_TIMER_LIST_Q* p_timer_listq, T_RVF_TIMER_LIST_ENT * p_tle);

T_RVF_TIMER_LIST_ENT*  gsp_get_expired_entry(T_RVF_TIMER_LIST_Q* p_timer_listq);

/*==== MACROS ===============================================================*/

#endif /* remu_timer_H */


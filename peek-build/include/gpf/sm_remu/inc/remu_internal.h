/*
+------------------------------------------------------------------------------
|  File:       remu_internal.h
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
|  Purpose :  Definitions for the riviera emulator internal.
+-----------------------------------------------------------------------------
*/

#ifndef REMU_INTERNAL_H
#define REMU_INTERNAL_H

/*==== INCLUDES =============================================================*/


#ifndef NU_DEBUG
#define NU_DEBUG
#define NU_DEBUG_TRIGGERD
#endif

#include "nucleus.h"
  
#ifdef  NU_DEBUG_TRIGGERD
#undef NU_DEBUG
#endif


#include "typedefs.h"
#include "os.h"
#include "os_types.h"
#include "os_glob.h"


#include "vsi.h"
#include "pei.h"
#include "frm_defs.h"
#include "frm_types.h"
#include "frm_glob.h"
#include "os_const.h"

#include "string.h"
#include "stdio.h"

#include "remu.h"

/*==== TYPES ================================================================*/
/* Original Riviera layout:
typedef struct _rvf_rt_addr_id_data {
  UINT8       type_code;
  UINT8       priority;
  T_RVF_G_ADDR_ID   host_addr_id;
  UINT8       hosting_count;
  T_RVF_G_ADDR_ID   parasites[MAX_PARASITES];
  UINT8       swe_db_index;
  char*       symbolic_name;
  NU_TASK*      pOSTCB;
  NU_EVENT_GROUP*   pOSEvtGrp;
  UINT8*        p_os_stack;
  UINT16        os_stack_size;
//  T_RVF_TIMER_LIST_Q* p_tm_q;
  UINT8       gdHost;
  T_RVF_INTERNAL_BUF* OSTaskQFirst[RVF_NUM_TASK_MBOX];
  T_RVF_INTERNAL_BUF* OSTaskQLast [RVF_NUM_TASK_MBOX];
  T_RV_RET      (* handle_message)  (T_RV_HDR * msg);  // note: T_RV_RETURN and not T_RVM...
  T_RV_RET      (* handle_timer)  (T_RV_HDR * msg);
} T_RVF_RT_ADDR_ID_DATA;
*/

typedef struct {
  UINT8                  timersInPossession;
  UINT8                  vsiTimerHandle[RVF_NUM_TASK_TIMERS];
  UINT8                  priority;
  T_RVF_G_ADDR_ID        host_addr_id;
  char*                  symbolic_name;
  OS_HANDLE              pOSEvtGrp;
  T_HANDLE               Queue[RVF_NUM_TASK_MBOX];
}T_GSP_RT_ADDR_ID_DATA;

/*==== CONSTANTS ============================================================*/

extern T_GSP_RT_ADDR_ID_DATA*  GSPTaskIdTable[];
extern T_OS_COM_TABLE_ENTRY ComTable[];
extern T_HANDLE ext_data_pool_handle;
//extern T_HANDLE BspGroupHandle;


//#define MAX_ENTITIES 45
extern NU_PROTECT       TCD_System_Protect;
#define NU_System_Protect()        NU_Protect(&TCD_System_Protect)
#define NU_System_Unprotect()      NU_Unprotect()

typedef struct _remu_internal_buf
{	
	struct	_remu_internal_buf * p_next;	/* pointer to the next buffer in the queue */ 
	void *data_ptr;
} T_REMU_INTERNAL_BUF;


/*==== PROTOTYPES ===========================================================*/


/*==== MACROS ===============================================================*/


#endif /* remu_internal_H */


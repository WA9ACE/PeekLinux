/*
+------------------------------------------------------------------------------
|  File:       remu_task.h
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
|  Purpose :  Definitions for the riviera emulator task managment.
+-----------------------------------------------------------------------------
*/

#ifndef REMU_TASK_H
#define REMU_TASK_H

/*==== INCLUDES =============================================================*/

#include "arm7/nucleus.h"
#include "PEI.H"
#include "vsi.h"



/*==== CONSTANTS ============================================================*/

#define newQueue 0


/*==== TYPES ================================================================*/

/*==== PROTOTYPES ===========================================================*/

/* Task management*/

void   gsp_init(void);

#ifdef JUNK_CODE_INCLUDED

T_RVF_RET  gsp_original_create_legacy_task(TASKPTR task_entry, UINT8 task_id, char* taskname, SHORT (*pei_create)(T_PEI_INFO const **),
                                                      UINT8* stack, UINT16 stacksize, UINT8 priority, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend);


T_RVF_RET  gsp_rvf_original_create_task(TASKPTR task_entry, T_RVF_G_ADDR_ID task_id,
                                                             char* taskname,  UINT8* stack, UINT16 stacksize,
                                                             UINT8 priority, UINT8 tcode, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend);
/* we can't resamble the interface of rvf_create_task() because we need an additional
     "SHORT (*pei_create)(T_PEI_INFO const **" parameter triggering the pei_create function!
     Please explicitly use gsp_create_pei_task() instead, with the address of pei_create as new parameter 4 */

#endif

T_RVF_RET  gsp_create_legacy_pei_task(TASKPTR task_entry, T_RVF_G_ADDR_ID task_id, char* taskname,
                                                SHORT (*pei_create)(T_PEI_INFO const **), UINT8* stack, UINT16 stacksize,
                                                UINT8 priority, UINT8 tcode, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend);

T_RVF_RET  gsp_create_pei_task(TASKPTR task_entry, T_RVF_G_ADDR_ID task_id, char* taskname,
                                                SHORT (*pei_create)(T_PEI_INFO const **), UINT8* stack, UINT16 stacksize,
                                                UINT8 priority, UINT8 tcode, UINT8 time_slicing, T_RVF_TASK_STATE is_suspend);


T_RVF_RET  gsp_init_remu_task(T_RVF_G_ADDR_ID task_id, char* taskname);

T_RVF_RET  gsp_init_remu_dyn_task(T_RVF_G_ADDR_ID task_id, char* taskname, T_HANDLE qhandle,  UINT8 priority);

T_RVF_RET  gsp_resume_task( T_RVF_G_ADDR_ID taskid);

void   gsp_exit_task(T_RVF_G_ADDR_ID vtask);

T_RVF_RET  gsp_suspend_task(T_RVF_G_ADDR_ID vtask);

T_RVF_G_ADDR_ID gsp_get_taskid(void);

char*    gsp_get_taskname(void);

T_RVF_G_ADDR_ID gsp_get_context();


/*==== MACROS ===============================================================*/


#endif /* remu_task_H */

/* 
+------------------------------------------------------------------------------
|  File:       remu_trace.h
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
|  Purpose :  Definitions for the riviera emulator tracing.
+----------------------------------------------------------------------------- 
*/ 

#ifndef REMU_TRACE_H
#define REMU_TRACE_H

/*==== INCLUDES =============================================================*/

/*==== CONSTANTS ============================================================*/

/*==== TYPES ================================================================*/

/*==== PROTOTYPES ===========================================================*/
void gsp_send_trace1(char*  msg, UINT8 msg_length, UINT32 val, UINT8 trace_level, UINT32 swe_use_id);

/* Trace for debug purposes*/
void gsp_dump_mem();

void gsp_dump_tasks();

void gsp_dump_pool();

/*==== MACROS ===============================================================*/

#endif /* remu_trace_H */        

/* 
+------------------------------------------------------------------------------
|  File:       remu_mem.h
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
|  Purpose :  Definitions for the riviera emulator api.
+----------------------------------------------------------------------------- 
*/ 

#ifndef REMU_MEM_H
#define REMU_MEM_H

/*==== INCLUDES =============================================================*/

/*==== CONSTANTS ============================================================*/

/*==== TYPES ================================================================*/

/*==== PROTOTYPES ===========================================================*/


/* To manage memory */
T_RVF_RET  gsp_create_mb(T_RVF_MB_NAME mb_name, T_RVF_MB_PARAM mb_param, T_RVF_MB_ID* mb_id);

T_RVF_RET  gsp_delete_mb(T_RVF_MB_NAME mb_name);

T_RVF_RET  gsp_get_mb_id(T_RVF_MB_NAME mb_name, T_RVF_MB_ID* mb_id);

T_RVF_MB_STATUS gsp_get_mb_status(T_RVF_MB_ID mb_id);

T_RVF_RET  gsp_mb_is_used(T_RVF_MB_NAME mb_name, UINT8* isUsed);

T_RVF_MB_STATUS gsp_get_buf(T_RVF_MB_ID mb_id, UINT32 buffer_size, T_RVF_BUFFER** p_buffer);

T_RVF_MB_STATUS gsp_count_buf(T_RVF_MB_ID mb_id, T_RVF_BUFFER*  p_buffer);

T_RVF_RET  gsp_free_buf( T_RVF_BUFFER*  p_buffer);

T_RVF_RET  gsp_set_callback_func(T_RVF_MB_ID mb_id, MB_CALLBACK_FUNC func);

T_RVF_RET  gsp_change_callback_func(T_RVF_MB_ID mb_id, MB_CALLBACK_FUNC func);

void gsp_get_protected_buf(T_RVF_MB_ID mb_id, UINT32 buffer_size, T_RVF_BUFFER** p_buffer);

UINT32   gsp_get_buf_size(T_RVF_BUFFER* bptr);

    /* get the parameters of a specific memory bank */
T_RVF_RET  gsp_get_mb_param( T_RVF_MB_NAME mb_name, T_RVF_MB_PARAM*  param);

    /* change the parameters of a specific memory bank */
T_RVF_RET  gsp_set_mb_param( T_RVF_MB_NAME mb_name, T_RVF_MB_PARAM*  param);

T_RVF_MB_STATUS gsp_get_msg_buf  (T_RVF_MB_ID mb_id,
           UINT32 message_size,
           T_RVF_MSG_ID msg_id,
           T_RVF_MSG* * pp_msg);

T_RVF_RET  gsp_free_msg  (T_RVF_MSG*  p_msg);

T_RVF_RET  gsp_free_timer_msg (T_RVF_MSG* p_msg);


/*==== MACROS ===============================================================*/


#endif /* remu_mem_H */        

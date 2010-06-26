/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  SM
+-----------------------------------------------------------------------------
|             Copyright 2003 Texas Instruments.
|             All rights reserved. 
| 
|             This file is confidential and a trade secret of Texas 
|             Instruments .
|             The receipt of or possession of this file does not convey 
|             any rights to reproduce or disclose its contents or to 
|             manufacture, use, or sell anything it may describe, in 
|             whole, or in part, without the specific written consent of 
|             Texas Instruments. 
+-----------------------------------------------------------------------------
| Purpose:    3G QoS utility function definitions in the SM entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_QOS_H
#define SM_QOS_H

/*==== INCLUDES =============================================================*/

#include "sm.h"
#include "message.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

extern void sm_qos_copy_from_sm     (/*@out@*/T_PS_qos      *dst,
				     /*@in@*/ T_SM_qos      *src,
				     /*@out@*/T_PS_ctrl_qos *dctrl)
     /*@modifies dst, *dctrl@*/;

extern void sm_qos_copy_to_sm       (/*@out@*/T_SM_qos      *dst,
				     /*@in@*/ T_PS_qos      *src,
				     T_PS_ctrl_qos           ctrl)
     /*@modifies dst@*/;

extern void sm_qos_convert_to_aim   (/*@in@*/ T_SM_qos       *src_qos,
				     /*@out@*/T_M_SM_qos     *dst_qos,
				     T_PS_sgsn_rel            release)
     /*@modifies dst_qos@*/
     /*@globals internalState@*/;


extern void sm_qos_assign_from_aim  (/*@out@*/T_SM_qos       *dst_qos,
				     /*@in@*/T_M_SM_qos      *src_qos)
     /*@modifies dst_qos@*/
     /*@globals internalState@*/;

extern BOOL sm_qos_is_minimum_satisfied_by_aim
                                    (struct T_SM_CONTEXT_DATA *context,
				     T_M_SM_qos               *aim_qos);

extern BOOL sm_qos_is_minimum_satisfied_by_sm
                                    (struct T_SM_CONTEXT_DATA *context,
				     T_SM_qos                 *sm_qos);

extern BOOL sm_qos_is_requested_qos_present(struct T_SM_CONTEXT_DATA *context);

extern U8   sm_qos_get_traffic_class(struct T_SM_CONTEXT_DATA *context); 

#ifdef TI_PS_OP_SM_RETAIN_BEST_CONTEXT
extern U16 sm_rank_del_contexts_based_on_tc();
extern U16 sm_retain_cntxt_wth_best_bitrate();
extern U16 sm_retain_cntxt_with_least_nsapi(); 
extern U8 sm_qos_rank_context(U16 ti);
#endif

#endif /* SM_QOS_H */
/*==== END OF FILE ==========================================================*/

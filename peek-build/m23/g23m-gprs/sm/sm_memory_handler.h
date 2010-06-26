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
| Purpose:    Memory allocation function definition for the SM Entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef SM_MEMORY_HANDLER_H
#define SM_MEMORY_HANDLER_H

/*==== INCLUDES =============================================================*/

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== EXPORTS ==============================================================*/

/* Memory allocation/free functions */
void sm_pfree(/*@only@*/ /*@out@*/ /*@null@*/void *data);

/*************************************************************/
/* Functions for managing context data structures and arrays */
/*************************************************************/

/*@only@*/ /*@null@*/
struct T_SM_CONTEXT_DATA *
        sm_allocate_context_data(void);

void    sm_assign_context_data_to_nsapi (/*@only@*/
					 struct T_SM_CONTEXT_DATA *context,
					 int /*@alt U8@*/ nsapi);

/*@observer@*/
struct T_SM_CONTEXT_DATA *
        sm_get_context_data_from_nsapi  (int /*@alt U8@*/ nsapi);

/*@observer@*/ /*@null@*/
struct T_SM_CONTEXT_DATA *
        sm_get_context_data_from_ti     (int /*@alt U8@*/ ti);

void    sm_free_context_data            (/*@only@*/
					 struct T_SM_CONTEXT_DATA *context);

void    sm_free_context_data_by_nsapi   (int /*@alt U8@*/ nsapi);

/*@observer@*/ /*@null@*/
struct T_SM_CONTEXT_DATA *
        sm_insert_mt_context_data       (int /*@alt U8@*/ ti);

/*@null@*/ /*@only@*/
struct T_SM_CONTEXT_DATA *
        sm_extract_mt_context_data      (int /*@alt U8@*/ ti);

void    sm_free_pending_mt_context_by_index(U16 index);

U16     sm_linked_nsapis                (U8 ti);

void    sm_allocate_and_copy_coded_msg  (struct T_SM_CONTEXT_DATA *context,
					 U8                        est_cause,
					 /*@in@*/ T_sdu           *msg);

void    sm_free_coded_msg               (struct T_SM_CONTEXT_DATA *context);

/***********************************************************************/
/*  Network Control Managed Memory Handler Functions                   */
/***********************************************************************/

void    sm_nw_allocate_and_copy_requested_pco(/*@special@*/
					 struct T_SM_CONTEXT_DATA *context,
					 size_t /*@alt U8,U16@*/   pco_len,
					 /*@unique@*/ U8          *pco);

void    sm_nw_allocate_and_copy_negotiated_pco(/*@special@*/
					 struct T_SM_CONTEXT_DATA *context,
					 size_t /*@alt U8@*/       pco_len,
					 /*@unique@*/ U8          *pco);

void    sm_nw_free_requested_pco        (/*@special@*/
					 struct T_SM_CONTEXT_DATA *context);

void    sm_nw_free_negotiated_pco       (/*@special@*/
					 struct T_SM_CONTEXT_DATA *context);

void    sm_nw_allocate_and_copy_apn     (/*@special@*/
					 struct T_SM_CONTEXT_DATA  *context,
					 U8 c_apn, /*@unique@*/ U8 *apn);

void    sm_nw_free_apn                  (/*@special@*/
					 struct T_SM_CONTEXT_DATA *context);

BOOL    sm_nw_is_address_and_apn_equal  (struct T_SM_CONTEXT_DATA *context,
                                         /*@observer@*/T_NAS_ip   *context_address,
				         /*@observer@*/T_M_SM_address *msg_address,
                                         U8 v_apn, T_M_SM_apn *apn);

void    sm_nw_allocate_and_copy_requested_tft(/*@special@*/
					 struct T_SM_CONTEXT_DATA *context,
					 /*@in@*/ T_NAS_tft       *tft);

void    sm_nw_allocate_active_tft       (/*@special@*/
					 struct T_SM_CONTEXT_DATA *context);

void    sm_nw_free_requested_tft        (/*@special@*/
					 struct T_SM_CONTEXT_DATA *context);

void    sm_nw_free_active_tft           (/*@special@*/
					 struct T_SM_CONTEXT_DATA *context);

void    sm_nw_store_requested_address   (struct T_SM_CONTEXT_DATA *context,
					 U8 pdp_type,
					 T_NAS_ctrl_ip_address ctrl_ip_address,
					 T_NAS_ip_address *ip_address);

void    sm_nw_store_negotiated_address  (struct T_SM_CONTEXT_DATA *context,
					 T_M_SM_address           *address,
					 U8                        v_address);

void    sm_nw_copy_negotiated_address_to_requested(struct T_SM_CONTEXT_DATA *);

BOOL    sm_is_address_changed_with_reactivation(struct T_SM_CONTEXT_DATA *context,
                                                   T_M_SM_address       *address,
                                                   U8                 v_address);
#endif /* SM_MEMORY_HANDLER_H */

/*==== END OF FILE ==========================================================*/

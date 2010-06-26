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

/*==== INCLUDES =============================================================*/

#include "sm.h"
#include "sm_tft.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

/*==== LOCALS ===============================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_pfree
+------------------------------------------------------------------------------
| Description : PFREE macro replacement
|
| Parameters  : data                - memory to free
+------------------------------------------------------------------------------
*/
void sm_pfree(/*@only@*/ /*@null@*/ /*@out@*/ void *data)
{
  if (data != NULL)
  {
    vsi_c_pfree((T_VOID_STRUCT **)&data FILE_LINE_MACRO);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_mfree
+------------------------------------------------------------------------------
| Description : Wrapper for MFREE macro
|
| Parameters  : data                - memory to free
+------------------------------------------------------------------------------
*/
static void sm_mfree(/*@only@*/ /*@out@*/ /*@null@*/ void *data)
{
  if (data != NULL) {
    MFREE(data);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_allocate_context_data
+------------------------------------------------------------------------------
| Description : Allocate an instance of SM_CONTEXT_DATA
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
struct T_SM_CONTEXT_DATA *sm_allocate_context_data(void)
{
  struct T_SM_CONTEXT_DATA *context;

  context           = (struct T_SM_CONTEXT_DATA *)
                       M_ALLOC((U32)sizeof(struct T_SM_CONTEXT_DATA));

  return context;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_assign_context_data_to_nsapi
+------------------------------------------------------------------------------
| Description : Assign a non-zero context data structure to an NSAPI index in
|               sm_context_array.
|
| Parameters  : context             - Context data
|               nsapi               - Index at which to insert data
+------------------------------------------------------------------------------
*/
void
sm_assign_context_data_to_nsapi(/*@only@*/struct T_SM_CONTEXT_DATA *context,
                                int /*@alt U8@*/ nsapi)
{
  TRACE_ASSERT(context != NULL);
  TRACE_ASSERT(sm_data.sm_context_array[nsapi - (int)NAS_NSAPI_5] == NULL);

  sm_data.sm_context_array[nsapi - (int)NAS_NSAPI_5] = context;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_get_context_data_from_nsapi
+------------------------------------------------------------------------------
| Description : Returns a context data structure for the given NSAPI index.
|               Returns NULL, if the context is inactive.
|
| Parameters  : nsapi               - Index at which to fetch data
+------------------------------------------------------------------------------
*/
struct T_SM_CONTEXT_DATA *
sm_get_context_data_from_nsapi(int /*@alt U8@*/ nsapi)
{
  TRACE_ASSERT((T_NAS_nsapi)nsapi >= NAS_NSAPI_5 && nsapi <= NAS_NSAPI_15);

  /*lint -e{661} sm_context_array causes out of bounds access, it does not! */
  return sm_data.sm_context_array[nsapi - (int)NAS_NSAPI_5];
}

/*
+------------------------------------------------------------------------------
| Function    : sm_assign_context_data_to_nsapi
+------------------------------------------------------------------------------
| Description : Returns a context data structure for the given TI value.
|               Returns NULL, if the context is inactive.
|
| Parameters  : ti                  - TI value identifying the context
+------------------------------------------------------------------------------
*/
/*@null@*/struct T_SM_CONTEXT_DATA *
sm_get_context_data_from_ti(int /*@alt U8@*/ ti)
{
  int                       index;

  /* First, search sm_context_array: */
  for (index = 0; index < SM_MAX_NSAPI_OFFSET; index++) {
    if (sm_data.sm_context_array[index] != NULL &&
        (sm_data.sm_context_array[index]->ti & SM_TI_MASK) == (ti & SM_TI_MASK))
    {
      return sm_data.sm_context_array[index];
    }
  }

  /* Next, try in sm_pending_mt_array: */
  for (index = 0; index < SM_MAX_NSAPI_OFFSET; index++) {
    if (sm_data.sm_pending_mt_array[index] != NULL &&
        sm_data.sm_pending_mt_array[index]->ti == ti)
    {
      return sm_data.sm_pending_mt_array[index];
    }
  }

  /* None found: return NULL */
  return NULL;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_free_context_data
+------------------------------------------------------------------------------
| Description : Free context data memory.  Calls de-initialization functions
|               in state machines.
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_free_context_data(/*@only@*/struct T_SM_CONTEXT_DATA *context)
{
  if (context != NULL) {
    sm_user_plane_control_exit        (context);
    sm_network_control_exit           (context);
    sm_context_deactivate_control_exit(context);
    sm_context_control_exit           (context);

    TRACE_ASSERT(context->apn == NULL);
    TRACE_ASSERT(context->requested_pco == NULL);
    TRACE_ASSERT(context->negotiated_pco == NULL);
    TRACE_ASSERT(context->requested_tft.ptr_tft_pf == NULL);
    TRACE_ASSERT(context->active_tft.ptr_tft_pf == NULL);
    TRACE_ASSERT(context->coded_msg == NULL);

    sm_mfree(context);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_free_context_data_by_nsapi
+------------------------------------------------------------------------------
| Description : Frees context identified by NSAPI. Uses sm_free_context_data()
|
| Parameters  : nsapi               - NSAPI
+------------------------------------------------------------------------------
*/
void sm_free_context_data_by_nsapi(int /*@alt U8@*/ nsapi)
{
  U16 index;
  TRACE_ASSERT((T_NAS_nsapi)nsapi >= NAS_NSAPI_5 && nsapi <= NAS_NSAPI_15);

  index = sm_nsapi_to_index((U16)nsapi);
  sm_free_context_data(sm_data.sm_context_array[index]);
  sm_data.sm_context_array[index] = NULL;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_insert_mt_context_data
+------------------------------------------------------------------------------
| Description : Allocates and inserts context data structure in
|               sm_pending_mt_array.
|
| Parameters  : ti                  - TI value identifying context
+------------------------------------------------------------------------------
*/
/*@observer@*/ /*@null@*/ struct T_SM_CONTEXT_DATA *
sm_insert_mt_context_data(int /*@alt U8@*/ ti)
{
  int                       index, insert_index = -1;
  struct T_SM_CONTEXT_DATA *context;

  for (index = 0; index < SM_MAX_NSAPI_OFFSET; index++) {
    if (insert_index < 0 && sm_data.sm_pending_mt_array[index] == NULL) {
      insert_index = index;
    }
    if (sm_data.sm_pending_mt_array[index] != NULL &&
        sm_data.sm_pending_mt_array[index]->ti == ti) {
      (void)TRACE_EVENT_P1("ERROR: Tried to overwrite MT context data for TI=%d!", ti);
      return sm_data.sm_pending_mt_array[index];
    }
  }

  context = sm_allocate_context_data();

  if (context != NULL && insert_index >= 0) {
    memset(context, 0, sizeof(struct T_SM_CONTEXT_DATA));

    context->ti = (U8)ti;

    sm_data.sm_pending_mt_array[insert_index] = context;
  } else if (insert_index < 0) {
    (void)TRACE_ERROR("ERROR: No free entries in MT context data array!");
  } else {
    (void)TRACE_ERROR("ERROR: Unable to allocate memory for context data array!");
  }

  return context;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_extract_mt_context_data
+------------------------------------------------------------------------------
| Description : Searches sm_pending_mt_array for the input TI.  Returns the
|               address of the data structure found, if any; otherwise NULL.
|               If found, the entry in sm_pending_mt_array is reset to NULL.
|
| Parameters  : ti                  - TI value identifying context
+------------------------------------------------------------------------------
*/
/*@null@*/ /*@only@*/struct T_SM_CONTEXT_DATA *
sm_extract_mt_context_data(int /*@alt U8@*/ ti)
{
  int                       index;

  for (index = 0; index < SM_MAX_NSAPI_OFFSET; index++)
  {
    if (sm_data.sm_pending_mt_array[index] != NULL &&
        sm_data.sm_pending_mt_array[index]->ti == ti)
    {
      struct T_SM_CONTEXT_DATA *context;
      context = sm_data.sm_pending_mt_array[index];
      sm_data.sm_pending_mt_array[index] = NULL;

      return context;
    } /* if */
  } /* for */
  return NULL;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_free_pending_mt_context_by_index
+------------------------------------------------------------------------------
| Description : Frees context in sm_pending_mt_array indentified by index
|
| Parameters  : index               - index in sm_pending_mt_array[]
+------------------------------------------------------------------------------
*/
void sm_free_pending_mt_context_by_index(U16 index)
{
  TRACE_ASSERT(index < (U16)SM_MAX_NSAPI_OFFSET);

  /*lint -e{661} sm_context_array causes out of bounds access, it does not! */
  sm_free_context_data(sm_data.sm_pending_mt_array[index]);
  /*lint -e{661} sm_context_array causes out of bounds access, it does not! */
  sm_data.sm_pending_mt_array[index] = NULL;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_linked_nsapis
+------------------------------------------------------------------------------
| Description : Returns the nsapi_set of secondary contexts linked to this
|               context.
|
| Parameters  : context             - context data
+------------------------------------------------------------------------------
*/
U16 sm_linked_nsapis(U8 ti)
{
  struct T_SM_CONTEXT_DATA *context;
  int                       nsapi;
  U16                       linked_nsapis = 0;
  U8                        linked_ti;

  context = sm_get_context_data_from_ti(ti);

  TRACE_ASSERT(context != NULL);

  if (context EQ NULL)
   return linked_nsapis;
	   
  if (sm_is_secondary(context))
  {
    linked_ti = context->linked_ti;
  } else {
    linked_ti = ti;
  }

  for (nsapi = (int)NAS_NSAPI_5; nsapi < NAS_SIZE_NSAPI; nsapi++)
  {
    context = sm_get_context_data_from_nsapi(nsapi);
    if (context != NULL && context->ti != (U8)ti && context->ti != linked_ti &&
        sm_is_secondary(context) && context->linked_ti == (U8)ti)
    {
      linked_nsapis = sm_add_nsapi_to_nsapi_set(nsapi, linked_nsapis);
    } /* if */
  } /* for */

  return linked_nsapis;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_free_coded_msg
+------------------------------------------------------------------------------
| Description : Stores a coded message in context data.
|               Used for retransmissions (i.e. in case of time-outs, resume
|               after suspension).
|
| Parameters  : context             - context data
|               msg                 - coded message
+------------------------------------------------------------------------------
*/
void sm_allocate_and_copy_coded_msg(struct T_SM_CONTEXT_DATA *context,
                                    U8                        est_cause,
                                    /*@in@*/ T_sdu           *msg)
{
  U32  msg_len;
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_allocate_and_copy_coded_msg");
#endif

  msg_len = (U32)(msg->l_buf >> 3);

  if (context->coded_msg != NULL)
  {
    sm_free_coded_msg(context);
  }

  TRACE_ASSERT(context->coded_msg == NULL);
  /* Allocate space for T_sdu (l_buf and o_buf) + air interface message octets */
  context->coded_msg = (T_sdu *)M_ALLOC(msg_len + (U32)offsetof(T_sdu, buf));

  if (context->coded_msg == NULL)
  {
    return;
  }

  /* coded_msg->o_buf is always 0.  So we store the only external parameter
   * in there (est_cause). */
  context->coded_msg->l_buf = msg->l_buf;
  context->coded_msg->o_buf = (U16)est_cause;
  memcpy(context->coded_msg->buf, &msg->buf[(msg->o_buf >> 3)], (size_t)msg_len);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_free_coded_msg
+------------------------------------------------------------------------------
| Description : Frees the coded message stored for this NSAPI, if any.
|
| Parameters  : context             - context data
+------------------------------------------------------------------------------
*/
void sm_free_coded_msg(struct T_SM_CONTEXT_DATA *context)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_free_coded_msg");
#endif

  if (context->coded_msg != NULL) {
    sm_mfree(context->coded_msg);
  }
  context->coded_msg      = NULL;
}


/*======================================================================
 * Network Control Memory Handler Functions
 *======================================================================*/

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_allocate_and_copy_*_pco
+------------------------------------------------------------------------------
| Description : Allocate and copy requested or negotiated PCO data structure.
|
| Parameters  : context             - context data
|               pco_len             - PCO data length
|               pco                 - PCO data
+------------------------------------------------------------------------------
*/
void sm_nw_allocate_and_copy_requested_pco(/*@special@*/
                                           struct T_SM_CONTEXT_DATA *context,
                                           size_t /*@alt U8,U16@*/   pco_len,
                                           /*@unique@*/ U8          *pco)
{
  TRACE_ASSERT(context->requested_pco == NULL);
  context->requested_pco        = (T_SM_pco *)M_ALLOC((U32)(pco_len + 1));
  TRACE_ASSERT(context->requested_pco != NULL);

  context->requested_pco->c_pco_value = (U8)pco_len;
  memcpy(context->requested_pco->pco_value, pco, (size_t)pco_len);
}

void sm_nw_allocate_and_copy_negotiated_pco(/*@special@*/
                                            struct T_SM_CONTEXT_DATA *context,
                                            size_t /*@alt U8@*/       pco_len,
                                            /*@unique@*/ U8          *pco)
{
  TRACE_ASSERT(context->negotiated_pco == NULL);
  context->negotiated_pco       = (T_SM_pco *)M_ALLOC((U32)(pco_len + 1));
  TRACE_ASSERT(context->negotiated_pco != NULL);

  context->negotiated_pco->c_pco_value = (U8)pco_len;
  memcpy(context->negotiated_pco->pco_value, pco, (size_t)pco_len);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_free_*_pco
+------------------------------------------------------------------------------
| Description : Free requested or negotiated PCO data structure
|
| Parameters  : context             - context data
+------------------------------------------------------------------------------
*/
void
sm_nw_free_requested_pco(/*@special@*/struct T_SM_CONTEXT_DATA *context)
{
  if (context->requested_pco != NULL) {
    sm_mfree(context->requested_pco);
    context->requested_pco      = NULL;
  }
}

void
sm_nw_free_negotiated_pco(/*@special@*/struct T_SM_CONTEXT_DATA *context)
{
  if (context->negotiated_pco != NULL) {
    sm_mfree(context->negotiated_pco);
    context->negotiated_pco     = NULL;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_allocate_and_copy_apn
+------------------------------------------------------------------------------
| Description : Allocate and copy APN data structure.  Inserted into context
|               data structure.
|
| Parameters  : context             - Context data
|               apn                 - APN data structure
+------------------------------------------------------------------------------
*/
void sm_nw_allocate_and_copy_apn(/*@special@*/
                                 struct T_SM_CONTEXT_DATA  *context,
                                 U8 c_apn, /*@unique@*/ U8 *apn)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_nw_allocate_and_copy_apn");
#endif

  TRACE_ASSERT(context->apn == NULL);
  context->apn                  = (T_SMREG_apn *)M_ALLOC((U32)sizeof(T_SMREG_apn));
  TRACE_ASSERT(context->apn != NULL);
  context->apn->c_apn_buf       = c_apn;
  memcpy(context->apn->apn_buf, apn, (size_t)c_apn);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_free_apn
+------------------------------------------------------------------------------
| Description : Free APN data structure
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_nw_free_apn(/*@special@*/ struct T_SM_CONTEXT_DATA *context)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_nw_free_apn");
#endif

  if (context->apn != NULL) {
    sm_mfree(context->apn);
    context->apn                = NULL;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_is_address_and_apn_equal
+------------------------------------------------------------------------------
| Description : Compare requested IP address and APN with incoming ditto
|
| Parameters  : context             - Context data
|               address             - incoming IP address
|               v_apn               - valid flag for APN
|               apn                 - APN value (void if v_apn == FALSE)
+------------------------------------------------------------------------------
*/
BOOL sm_nw_is_address_and_apn_equal(struct T_SM_CONTEXT_DATA *context,
                                    T_NAS_ip                 *context_address,
                                    T_M_SM_address           *msg_address,
                                    U8 v_apn, T_M_SM_apn     *apn)
{
  /* Compare IP address */
  if (context_address->ctrl_ip_address == NAS_is_ip_not_present) {
    return FALSE;
  } else if (msg_address->pdp_type_org == (U8)M_SM_IETF_ORG &&
             context->pdp_type == msg_address->pdp_type_no)
  {
    if (context->pdp_type == (U8)SMREG_PDP_IPV4)
    {
      if (msg_address->c_add_info == (U8)NAS_SIZE_IPv4_ADDR &&
          memcmp(context_address->ip_address.ipv4_addr.a4,
                 msg_address->add_info, (size_t)NAS_SIZE_IPv4_ADDR) == 0)
      {
        /* IP address equal: FALLTHROUGH */
      } else {
        return FALSE;
      }
    } else if (context->pdp_type == (U8)SMREG_PDP_IPV6)
    {
      if (msg_address->c_add_info == (U8)NAS_SIZE_IPv6_ADDR &&
          memcmp(context_address->ip_address.ipv6_addr.a6,
                 msg_address->add_info, (size_t)NAS_SIZE_IPv6_ADDR) == 0)
      {
        /* IP address equal: FALLTHROUGH */
      } else {
        return FALSE;
      }
    } else { /* Bogus pdp_type */
      return FALSE;
    }
  } else {
    return FALSE;
  }

  /* Compare APN */
  if (v_apn == (U8)TRUE) {
    /* Incoming APN present: Requested APN must also be present, and have same length and values */
    if (context->apn != NULL) {
      if (context->apn->c_apn_buf == apn->c_apn_value) {
        if (memcmp(context->apn->apn_buf, apn->apn_value,
                   (size_t)apn->c_apn_value) == 0) {
          return TRUE;
        }
      }
    }
    return FALSE;
  } else {
    /* If incoming APN is absent, equality depends on the presence of a requested APN */
    return (context->apn == NULL);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_allocate_and_copy_requested_tft
+------------------------------------------------------------------------------
| Description : Allocate and copy TFT data structure.  Inserted into context
|               data structure.
|
| Parameters  : context             - Context data
|               tft                 - TFT data structure
+------------------------------------------------------------------------------
*/
void sm_nw_allocate_and_copy_requested_tft(/*@special@*/
                                           struct T_SM_CONTEXT_DATA *context,
                                           /*@in@*/
                                           T_NAS_tft *tft)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_nw_allocate_and_copy_requested_tft");
#endif

  /* Free TFT memory if a TFT was already present but with a different
   * number of elements. */
  if (context->requested_tft.ptr_tft_pf != NULL &&
      context->requested_tft.c_tft_pf != tft->c_tft_pf)
  {
    sm_nw_free_requested_tft(context);
  }

  /* Allocate memory if TFT was not present before, or if it was freed above.*/
  if (context->requested_tft.ptr_tft_pf == NULL)
  {
    context->requested_tft.c_tft_pf   = tft->c_tft_pf;
    context->requested_tft.ptr_tft_pf = (T_NAS_tft_pf *)
      M_ALLOC((U32)sizeof(T_NAS_tft_pf) * (U32)context->requested_tft.c_tft_pf);
    TRACE_ASSERT(context->requested_tft.ptr_tft_pf != NULL);
  }

  memcpy(context->requested_tft.ptr_tft_pf, tft->ptr_tft_pf,
         sizeof(T_NAS_tft_pf) * (size_t)context->requested_tft.c_tft_pf);
  context->requested_tft.tft_precence_mask = sm_tft_precence_mask(tft->ptr_tft_pf, tft->c_tft_pf);
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_allocate_active_tft
+------------------------------------------------------------------------------
| Description : Allocate active TFT data structure.  Inserted into context
|               data structure.
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_nw_allocate_active_tft(/*@special@*/struct T_SM_CONTEXT_DATA *context)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_nw_allocate_active_tft");
#endif

  TRACE_ASSERT (context->active_tft.ptr_tft_pf == NULL);
  context->active_tft.c_tft_pf   = (U8)0;
  context->active_tft.ptr_tft_pf = (T_NAS_tft_pf *)
    M_ALLOC((U32)sizeof(T_NAS_tft_pf) * (U32)NAS_SIZE_TFT_FILTER);
  TRACE_ASSERT(context->active_tft.ptr_tft_pf != NULL);
  context->active_tft.tft_precence_mask = (U8)0;
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_free_requested_tft
+------------------------------------------------------------------------------
| Description : Free TFT data structure
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_nw_free_requested_tft(/*@special@*/struct T_SM_CONTEXT_DATA *context)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_nw_free_requested_tft");
#endif

  if (context->requested_tft.ptr_tft_pf != NULL) {
    sm_mfree(context->requested_tft.ptr_tft_pf);
    context->requested_tft.ptr_tft_pf = NULL;
    context->requested_tft.c_tft_pf   = (U8)0;
    context->requested_tft.tft_precence_mask = (U8)0;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_free_active_tft
+------------------------------------------------------------------------------
| Description : Free TFT data structure
|
| Parameters  : context             - Context data
+------------------------------------------------------------------------------
*/
void sm_nw_free_active_tft(/*@special@*/struct T_SM_CONTEXT_DATA *context)
{
#ifdef DEBUG_VERBOSE
  (void)TRACE_FUNCTION("sm_nw_free_active_tft");
#endif

  if (context->active_tft.ptr_tft_pf != NULL) {
    sm_mfree(context->active_tft.ptr_tft_pf);
    context->active_tft.ptr_tft_pf    = NULL;
    context->active_tft.c_tft_pf      = (U8)0;
    context->active_tft.tft_precence_mask = (U8)0;
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_store_requested_address
+------------------------------------------------------------------------------
| Description : Utility function which stores the IP address requested from
|               the network.
|
| Parameters  : context             - context data
|               pdp_type            - PDP type number
|               ctrl_ip_address     - IP address type
|               ip_address          - IP address (void if ctrl_ip_address == NAS_is_ip_not_present)
+------------------------------------------------------------------------------
*/
void sm_nw_store_requested_address(struct T_SM_CONTEXT_DATA *context,
                                   U8 pdp_type,
                                   T_NAS_ctrl_ip_address ctrl_ip_address,
                                   T_NAS_ip_address *ip_address)
{
  context->pdp_type               = pdp_type;
  context->requested_address.ctrl_ip_address = ctrl_ip_address;
  if (ctrl_ip_address != NAS_is_ip_not_present) {
    if (pdp_type == (U8)SMREG_PDP_PPP ||
        (pdp_type == (U8)SMREG_PDP_IPV4 && ctrl_ip_address != NAS_is_ipv4) ||
        (pdp_type == (U8)SMREG_PDP_IPV6 && ctrl_ip_address != NAS_is_ipv6)) {
      (void)TRACE_ERROR("Mismatching pdp_type and ip_address controller field in SMREG_PDP_ACTIVATE_REQ - using union controller!");
    }
    switch (ctrl_ip_address)
    {
    case NAS_is_ipv4:
      context->pdp_type           = (U8)SMREG_PDP_IPV4;
      memcpy(context->requested_address.ip_address.ipv4_addr.a4,
             ip_address->ipv4_addr.a4, (size_t)NAS_SIZE_IPv4_ADDR);
      break;
    case NAS_is_ipv6:
      context->pdp_type           = (U8)SMREG_PDP_IPV6;
      memcpy(context->requested_address.ip_address.ipv6_addr.a6,
             ip_address->ipv6_addr.a6, (size_t)NAS_SIZE_IPv6_ADDR);
      break;
    default:
      break;
    }
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_store_negotiated_address
+------------------------------------------------------------------------------
| Description : Utility function which stores the address received from the
|               network.
|
| Parameters  : context             - context data
|               v_address           - valid flag for address
|               address             - IP address (void if v_address == FALSE)
+------------------------------------------------------------------------------
*/
void sm_nw_store_negotiated_address(struct T_SM_CONTEXT_DATA *context,
                                    T_M_SM_address           *address,
                                    U8                        v_address)
{
  if (v_address == (U8)TRUE && address->pdp_type_org == (U8)M_SM_IETF_ORG) {
    /* Update PDP type from message */
    context->pdp_type = address->pdp_type_no;
    if (address->pdp_type_no == (U8)M_SM_IP4_TYPE) {
      if (address->c_add_info != (U8)NAS_SIZE_IPv4_ADDR) {
        (void)TRACE_EVENT_P1("Warning: IPv4 address has length %d != 4!",
                             (int)address->c_add_info);
      }
      context->negotiated_address.ctrl_ip_address = NAS_is_ipv4;
      memcpy(&context->negotiated_address.ip_address, &address->add_info, (size_t)NAS_SIZE_IPv4_ADDR);
    } else if (address->pdp_type_no == (U8)M_SM_IP6_TYPE) {
      if (address->c_add_info != (U8)NAS_SIZE_IPv6_ADDR) {
        (void)TRACE_EVENT_P1("Warning: IPv6 address has length %d != 16!",
                             (int)address->c_add_info);
      }
      context->negotiated_address.ctrl_ip_address = NAS_is_ipv6;
      memcpy(&context->negotiated_address.ip_address, &address->add_info, (size_t)NAS_SIZE_IPv6_ADDR);
    } else {
      (void)TRACE_EVENT_P1("ERROR: Invalid PDP type %d; address discarded!",
                           address->pdp_type_no);
    }
  } else {
    /* Check for static IP address allocation/request;
     * PDP addr IE is not included in this case [3G 24.008, 9.5.2.1] */
    switch (context->requested_address.ctrl_ip_address)
    {
    case NAS_is_ipv4:
      context->pdp_type           = (U8)SMREG_PDP_IPV4;
      context->negotiated_address.ctrl_ip_address = NAS_is_ipv4;
      memcpy(context->negotiated_address.ip_address.ipv4_addr.a4,
             context->requested_address .ip_address.ipv4_addr.a4,
             (size_t)NAS_SIZE_IPv4_ADDR);
      break;
    case NAS_is_ipv6:
      context->pdp_type           = (U8)SMREG_PDP_IPV6;
      context->negotiated_address.ctrl_ip_address = NAS_is_ipv6;
      memcpy(context->negotiated_address.ip_address.ipv6_addr.a6,
             context->requested_address .ip_address.ipv6_addr.a6,
             (size_t)NAS_SIZE_IPv6_ADDR);
      break;
    default:
    /* PPP or other non-IP address context; i.e. no static address alloc. */
    context->negotiated_address.ctrl_ip_address = NAS_is_ip_not_present;
    }
  }
}

/*
+------------------------------------------------------------------------------
| Function    : sm_nw_copy_negotiated_address_to_requested
+------------------------------------------------------------------------------
| Description : Utility function which copies the address received from the
|               network to be the requested address.  Used for context
|               reactivations, which must use the negotiated IP address.
|
| Parameters  : context             - context data
+------------------------------------------------------------------------------
*/
void sm_nw_copy_negotiated_address_to_requested(struct T_SM_CONTEXT_DATA *context)
{
  sm_nw_store_requested_address(context, context->pdp_type,
                                context->negotiated_address.ctrl_ip_address,
                                &context->negotiated_address.ip_address);
}
 
/*
+------------------------------------------------------------------------------
| Function    : sm_is_address_changed_with_reactivation
+------------------------------------------------------------------------------
| Description : Utility function which compares the address of the air message
|               and the address already stored in the context. This function 
|               is useful only in PDP REACTIVATION cases. If the addresses 
|               are not equal the function returns false and the pdp context
|               gets deactivated.
|
| Parameters  : context             - context data, air message address
+------------------------------------------------------------------------------
*/
BOOL sm_is_address_changed_with_reactivation(struct T_SM_CONTEXT_DATA *context,
                                             T_M_SM_address           *address,
                                             U8                      v_address)
{
  BOOL result = FALSE; 
  if (sm_is_context_pending_reactivation(context))
  {
    if (v_address == (U8)TRUE && address->pdp_type_org == (U8)M_SM_IETF_ORG) 
    {
      if ( (address->pdp_type_no == (U8)M_SM_IP4_TYPE) && 
           (context->negotiated_address.ctrl_ip_address == NAS_is_ipv4) )
      {
        if (memcmp(&context->negotiated_address.ip_address, 
           &address->add_info, (size_t)NAS_SIZE_IPv4_ADDR) != 0)
        {
          result = TRUE;
        }
      } else if ( (address->pdp_type_no == (U8)M_SM_IP6_TYPE) && 
           (context->negotiated_address.ctrl_ip_address == NAS_is_ipv6) )
       {
         if (memcmp(&context->negotiated_address.ip_address, 
           &address->add_info, (size_t)NAS_SIZE_IPv6_ADDR) != 0)
         {
           result = TRUE;
         }
       }
    }
    /* Clear the pending reactivation flag. We don't need this anymore. */
    sm_set_context_pending_reactivation(context, FALSE);
  } 
  if (result == TRUE) {
    (void)TRACE_EVENT_P1( "PDP address changed with reactivation. Deactivate nsapi %d",
                                                                     context->nsapi );
  }
  return result;
}
/*==== END OF FILE ==========================================================*/

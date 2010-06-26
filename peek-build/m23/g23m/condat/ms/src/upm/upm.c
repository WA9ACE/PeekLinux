/*----------------------------------------------------------------------------
|  Project :  3G PS
|  Module  :  UPM
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
| Purpose:    Utility functions for the UPM entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

/*==== INCLUDES ============================================================*/

#include "upm.h"

/*==== CONSTS ===============================================================*/

/*==== TYPES ================================================================*/

T_UPM_DATA upm_data;

/*==== LOCALS ===============================================================*/

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : upm_pfree
+------------------------------------------------------------------------------
| Description : PFREE macro replacement
|
| Parameters  : data                - memory to free
+------------------------------------------------------------------------------
*/
void upm_pfree(/*@only@*/ /*@null@*/ /*@out@*/ void *data)
{
  if (data != NULL)
  {
    vsi_c_pfree((T_VOID_STRUCT **)&data FILE_LINE_MACRO);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_mfree
+------------------------------------------------------------------------------
| Description : Wrapper for MFREE macro.
|
| Parameters  : data                   - memory to free (NULL allowed)
+------------------------------------------------------------------------------
*/
void upm_mfree(/*@only@*/ /*@out@*/ /*@null@*/ void *data)
{
  if (data != NULL) {
    MFREE(data);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_nsapi2nsapi_set
+------------------------------------------------------------------------------
| Description : Return nsapi_set corresponding to NSAPI
|
| Parameters  : nsapi                  - NSAPI
+------------------------------------------------------------------------------
*/
U16 upm_nsapi2nsapi_set(int /*@alt U8@*/ nsapi) /*@*/
{
  TRACE_ASSERT ((T_NAS_nsapi)nsapi >= NAS_NSAPI_5 &&  nsapi <= NAS_NSAPI_15);

  return (U16)(1UL << (U16)nsapi);
}

/*
+------------------------------------------------------------------------------
| Function    : upm_get_context_data_from_nsapi
+------------------------------------------------------------------------------
| Description : Returns context data structure associated with indicated NSAPI.
|
| Parameters  : nsapi                  - NSAPI index
+------------------------------------------------------------------------------
*/
/*@null@*/ /*@exposed@*/
struct T_CONTEXT_DATA *upm_get_context_data_from_nsapi(int /*@alt U8@*/ nsapi)
{
  TRACE_ASSERT (((T_NAS_nsapi)nsapi >= NAS_NSAPI_5 && nsapi <= NAS_NSAPI_15));

  /*lint -e{661} upm_context_array causes out of bounds access, it does not! */
  return (upm_data.upm_context_array[nsapi - (int)NAS_NSAPI_5]);
}

/*
+------------------------------------------------------------------------------
| Function    : upm_assign_context_data_to_nsapi
+------------------------------------------------------------------------------
| Description : Assign context data structure to global context array.
|
| Parameters  : ptr_context_data       - Pointer to context data
|               nsapi                  - NSAPI index
+------------------------------------------------------------------------------
*/
void upm_assign_context_data_to_nsapi(/*@keep@*/
				      struct T_CONTEXT_DATA *ptr_context_data,
				      int /*@alt U8@*/ nsapi)
{
  (void)TRACE_EVENT_P2("upm_assign_context_data_to_nsapi: NSAPI%3d = %p", nsapi, ptr_context_data);

  TRACE_ASSERT ((T_NAS_nsapi)nsapi >= NAS_NSAPI_5 && nsapi <= NAS_NSAPI_15);

  /*lint -e{661} upm_context_array causes out of bounds access, it does not! */
  upm_data.upm_context_array[nsapi - (int)NAS_NSAPI_5] = ptr_context_data;
  ptr_context_data->nsapi = nsapi;
}

/*
+------------------------------------------------------------------------------
| Function    : upm_free_context_data
+------------------------------------------------------------------------------
| Description : Free context data structure associated with indicated NSAPI.
|
| Parameters  : nsapi                  - NSAPI index
+------------------------------------------------------------------------------
*/
void upm_free_context_data(int /*@alt U8@*/ nsapi)
{
  TRACE_ASSERT ((T_NAS_nsapi)nsapi >= NAS_NSAPI_5 && nsapi <= NAS_NSAPI_15);

  /*lint -e661 (upm_context_array causes out of bounds access, it does not!) */
  if ( upm_data.upm_context_array[nsapi - (int)NAS_NSAPI_5] != NULL ) {
    upm_mfree(upm_data.upm_context_array[nsapi - (int)NAS_NSAPI_5]);
    upm_data.upm_context_array[nsapi - (int)NAS_NSAPI_5] = NULL;
  /*lint +e661 (upm_context_array causes out of bounds access, it does not!) */	
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_check_for_released_context_and_release
+------------------------------------------------------------------------------
| Description : Check whether a context is released (both link and DTI
|               connections released), and if so, freee context data structure
|               associated with indicated NSAPI.
|
| Parameters  : nsapi                  - NSAPI index
+------------------------------------------------------------------------------
*/
void upm_check_for_released_context_and_release(struct T_CONTEXT_DATA *context)
{
  if (upm_is_link_released(context)
#ifndef UPM_WITHOUT_USER_PLANE
      && upm_is_dti_disconnected(context)
#endif
      )
  {
    U8 nsapi = context->nsapi;

    upm_free_context_data(nsapi);
    (void)TRACE_EVENT_P1("upm_check_for_released_context_and_release on NSAPI %d: Context data freed...", nsapi);
  }
}

/*
+------------------------------------------------------------------------------
| Function    : upm_get_current_rat
+------------------------------------------------------------------------------
| Description : Returns the RAT in which UPM is currently active
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
#ifdef TI_DUAL_MODE
T_PS_rat upm_get_current_rat(void)
{
  return upm_data.upm_rat;
}
#endif /* TI_DUAL_MODE */

/*
+------------------------------------------------------------------------------
| Function    : upm_set_current_rat
+------------------------------------------------------------------------------
| Description : Sets currently active RAT
|
| Parameters  : rat                    - RAT received from SM or MM
+------------------------------------------------------------------------------
*/
#ifdef TI_DUAL_MODE
void upm_set_current_rat(T_PS_rat rat)
     /*@modifies upm_data.upm_rat@*/
{
  upm_data.upm_rat = rat;
}
#endif /* TI_DUAL_MODE */

/*
+------------------------------------------------------------------------------
| Function    : upm_ll_entity_test
+------------------------------------------------------------------------------
| Description : Returns TRUE if upm_ll_entity_test is set in UPM data struct
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
BOOL upm_ll_entity_test(void)
{
  return upm_data.upm_ll_entity_test;
}

/*
+------------------------------------------------------------------------------
| Function    : upm_activate_ll_entity_test
+------------------------------------------------------------------------------
| Description : Enables lower layer stubs for entity test
|
| Parameters  : None
+------------------------------------------------------------------------------
*/
void upm_activate_ll_entity_test(void)
{
  upm_data.upm_ll_entity_test = TRUE;
}

/*
+------------------------------------------------------------------------------
| Function    : upm_set_sequence_number
+------------------------------------------------------------------------------
| Description : Stores the receive sequence number from MM for later retrieval
|
| Parameters  : ptr_context_data    - Context data
|               seq_no              - Receive sequence number
+------------------------------------------------------------------------------
*/
void upm_set_sequence_number(struct T_CONTEXT_DATA *ptr_context_data, U8 seq_no)
{
  (void)TRACE_FUNCTION("upm_set_sequence_number");

  ptr_context_data->seq_nos_valid = (U8)TRUE;
  ptr_context_data->recv_seq_no   = seq_no;
}

/*
+------------------------------------------------------------------------------
| Function    : upm_get_sequence_number
+------------------------------------------------------------------------------
| Description : Retrieves the receive sequence number previously gotten from MM
|
| Parameters  : ptr_context_data    - Context data
+------------------------------------------------------------------------------
*/
U8 upm_get_sequence_number(struct T_CONTEXT_DATA *ptr_context_data)
{
  (void)TRACE_FUNCTION("upm_get_sequence_number");

  if (ptr_context_data->seq_nos_valid != (U8)0)
  {
    ptr_context_data->seq_nos_valid = (U8)0;
    return ptr_context_data->recv_seq_no;
  } else {
    return (U8)0;
  }
}

/*==== END OF FILE ==========================================================*/

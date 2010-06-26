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
| Purpose:    Global definitions for the Session Management (SM) Entity.
|             For design details, see:
|             8010.908 SM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef _SM_H
#define _SM_H

//TISH modified for MSIM
// #ifdef WIN32
// #define DEBUG
// #endif

/*#ifdef FF_EGPRS
#ifndef SM_EDGE
#define SM_EDGE
#endif  
#endif */ /*FF_EGPRS*/ 

/*#ifdef SM_EDGE */
#define DEBUG_VERBOSE
/*#endif */ /*#ifdef SM_EDGE*/
/*==== INCLUDES =============================================================*/

#include <string.h>
#include <typedefs.h>
#include <vsi.h>
#include <gsm.h>
#include <gprs.h>
#include <prim.h>

/*#ifndef SM_EDGE
#include <trace.h>
#endif */

#ifdef DEBUG
#include "sm_debug.h"
#endif

struct T_SM_CONTEXT_DATA;

#include "sm_context_control.h"
#include "sm_context_deactivate_control.h"
#include "sm_network_control.h"
#include "sm_user_plane_control.h" 
#include "sm_mm_output_handler.h"

/*==== CONSTS ===============================================================*/

/* CCD constant: Bit-offset from which to place coded data in buffer. */
#define ENCODE_OFFSET         16

/* Session Management Protocol Disciminator; Global (PS-wide) define? */
#define PD_SM               0x0a

/* Transaction Indentifier-related definitions */
#define SM_TI_MASK          0x7f
#define SM_TI_FLAG          0x80
#define SM_TI_NON_EXT_MASK  0x07
#define SM_TI_EXTEND_VALUE  0x07
#define SM_MAX_NON_EXT_TI      6
#define SM_MAX_EXT_TI        127
#define SM_TI_NONE          0xff
#define SM_HIGH_VALUE       255

/* Often-used constant: max NSAPI index after NSAPI 5 offset is subtracted */
#define SM_MAX_NSAPI_OFFSET  ((int)NAS_SIZE_NSAPI - (int)NAS_NSAPI_5)

/* Frame parameter: Maximum timers used = number of concurrently active NSAPIs*/
#define SM_TIMER_MAX        SM_MAX_NSAPI_OFFSET

enum SM_CONTEXT_FLAGS {
  SM_CONTEXT_FLAG_COMP_PARAMS                = 0x01,
  SM_CONTEXT_FLAG_STARTED_DURING_SUSPEND     = 0x02,
  SM_CONTEXT_FLAG_SECONDARY_CONTEXT          = 0x04,
  SM_CONTEXT_FLAG_PENDING_DEALLOCATION       = 0x08,
  SM_CONTEXT_FLAG_PENDING_REACTIVATION       = 0x10,
  SM_CONTEXT_FLAG_PFI_PRESENT                = 0x20
};

typedef enum {
  SM_UPDATE_QOS                              = 0x01,
  SM_UPDATE_QOS_DOWNGRADE                    = 0x02,
  SM_UPDATE_ADDRESS                          = 0x04,
  SM_UPDATE_COMP_PARAMS                      = 0x08,
  SM_UPDATE_SAPI_RADIO_PRIO_PFI              = 0x10
} T_SM_UPDATE_FLAGS;
/*==== TYPES ================================================================*/

typedef struct {
  U8              c_pco_value;
  U8              pco_value[1];
} T_SM_pco;

typedef struct {
  U8              tft_precence_mask;
  U8              c_tft_pf;
  /*@only@*/ /*@null@*/
  T_NAS_tft_pf   *ptr_tft_pf;
} T_SM_tft;

typedef struct {
  T_PS_ctrl_qos   ctrl_qos;
  T_PS_qos        qos;
} T_SM_qos;

/*@abstract@*/ struct T_SM_CONTEXT_DATA {
  U8                                     ti;
  U8                                     nsapi;
  U8                                     linked_ti;

  U8                                     active_timer; /* T3380, T3381, or T3390 */
  U8                                     timeouts;

  U8                                     sapi;         /* Managed by Network Control */
  U8                                     radio_prio;   /* Managed by Network Control */
  U8                                     pdp_type;     /* Managed by Network Control */
  U8                                     pfi;          /* Managed by Network Control */
  U8                                     flags; 

  U8                                     qos_rank;     

  T_NAS_comp_params                      comp_params;

  T_CAUSE_ps_cause                       aci_cause;
  T_CAUSE_ps_cause                       nw_cause;

  T_SM_CONTEXT_CONTROL_STATE             context_control_state;
  T_SM_CONTEXT_DEACTIVATE_CONTROL_STATE  context_deactivate_control_state;
  T_SM_NETWORK_CONTROL_STATE             network_control_state;
  T_SM_USER_PLANE_CONTROL_STATE          user_plane_control_state;

  /* Fields below allocated, managed, and deallocated by Network Control */
  T_SM_qos                               minimum_qos;
  T_SM_qos                               requested_qos;
  T_SM_qos                               accepted_qos;

  T_NAS_ip                               requested_address;
  T_NAS_ip                               negotiated_address;

  /*@null@*/ /*@only@*/ /*@reldef@*/
  T_SMREG_apn                           *apn;
  /*@null@*/ /*@only@*/ /*@reldef@*/
  T_SM_pco                              *requested_pco;
  /*@null@*/ /*@only@*/ /*@reldef@*/
  T_SM_pco                              *negotiated_pco;
  T_SM_tft                               requested_tft;
  T_SM_tft                               active_tft;

  /*@null@*/ /*@only@*/
  T_sdu                                 *coded_msg;
};

typedef struct {
  T_PS_rat                               sm_current_rat;
  T_PS_sgsn_rel                          sm_current_nw_release;
  BOOL                                   sm_suspended; 
  U16                                    sm_suspend_cause; 
                                         /*sm_suspend_cause stores the cause of SM suspension.
                                          the value has meaning only when sm_suspended=true
                                         at other places the value must be set to CAUSE_MM_SUCCESS*/
  BOOL                                   sm_attached;
  U16                                    sm_context_activation_status;
  U16                                    sm_nsapis_requested_deactivated;
  U16                                    sm_nsapis_being_deactivated;
  /*@only@*/ /*@reldef@*/
  struct T_SM_CONTEXT_DATA              *sm_context_array[SM_MAX_NSAPI_OFFSET];
  /*@only@*/ /*@reldef@*/
  struct T_SM_CONTEXT_DATA              *sm_pending_mt_array[SM_MAX_NSAPI_OFFSET];
} T_SM_DATA;

#ifdef DEBUG
#define M_TRANSITION(_EVENT, _FUNC) {_EVENT, _FUNC}
#else
#define M_TRANSITION(_EVENT, _FUNC) {_FUNC}
#endif /* DEBUG */

/*==== EXPORTS ==============================================================*/

extern T_SM_DATA sm_data;

#define hCommACI    sm_hCommACI    /* Communication handle to ACI   */
#define hCommMM     sm_hCommMM     /* Communication handle to MM    */
#define hCommUPM    sm_hCommUPM    /* Communication handle to UPM   */

/* Communication handles */
extern T_HANDLE     hCommACI;
extern T_HANDLE     hCommMM;
extern T_HANDLE     hCommUPM;
extern T_HANDLE     sm_handle;

#define VSI_CALLER        sm_handle,
#define VSI_CALLER_SINGLE sm_handle

/* Uniquify pei_create() */
#define pei_create sm_pei_create
short sm_pei_create (T_PEI_INFO **info);

/*
 * If all entities are linked into one module this definitions
 * prefixes the global data with the enity name
 */
#define _decodedMsg   _sm_decodedMsg

/* CCD coding/decoding buffer */
extern U8 _decodedMsg [MAX_MSTRUCT_LEN_SM];

#include "sm_memory_handler.h"

/* Convenience functions relating to NSAPI handling */
U16              sm_nsapi2nsapi_set(int /*@alt U8@*/ nsapi) /*@*/;
U16              sm_add_nsapi_to_nsapi_set(int /*@alt U8@*/ nsapi, U16 nsapi_set) /*@*/;
U16              sm_remove_nsapi_from_nsapi_set(int /*@alt U8@*/ nsapi, U16 nsapi_set) /*@*/;
BOOL             sm_is_nsapi_in_nsapi_set(int /*@alt U8,U16@*/ nsapi, U16 nsapi_set) /*@*/;

U16 /*@alt int@*/sm_nsapi_to_index(U16 /*@alt U8,int@*/nsapi) /*@*/;
U16 /*@alt int@*/sm_index_to_nsapi(U16 /*@alt U8,int@*/index) /*@*/;

/* Get/set current RAT and core network release */
T_PS_rat
/*@alt U8@*/     sm_get_current_rat(void);
void             sm_set_current_rat(T_PS_rat rat);

T_PS_sgsn_rel
/*@alt U8@*/     sm_get_current_nw_release(void);
void             sm_set_current_nw_release(T_PS_sgsn_rel sgsn_rel);

/* Query functions for global attached/suspended state */
BOOL             sm_is_suspended(void);
BOOL             sm_is_attached(void);

/* Set and query functions for context flags */
BOOL             sm_is_secondary(struct T_SM_CONTEXT_DATA *context);
void             sm_set_secondary(struct T_SM_CONTEXT_DATA *);
U16              sm_get_pdp_context_status(void);
void             sm_set_started_during_suspend(struct T_SM_CONTEXT_DATA *context);
void             sm_clear_started_during_suspend(struct T_SM_CONTEXT_DATA *context);
BOOL             sm_is_started_during_suspend(struct T_SM_CONTEXT_DATA *context) /*@*/;
void             sm_mark_context_for_deallocation(struct T_SM_CONTEXT_DATA *context);
BOOL             sm_is_context_pending_deallocation(struct T_SM_CONTEXT_DATA *context);
void             sm_set_context_pending_reactivation(struct T_SM_CONTEXT_DATA *, BOOL);
BOOL             sm_is_context_pending_reactivation(struct T_SM_CONTEXT_DATA *context);
void             sm_set_pfi_included(struct T_SM_CONTEXT_DATA *, BOOL);
BOOL             sm_is_pfi_included(struct T_SM_CONTEXT_DATA *context);

BOOL             sm_is_llc_sapi_valid(U8 llc_sapi, U8 ti);

BOOL             sm_is_aci_update_required(T_SM_UPDATE_FLAGS update_flags);
BOOL             sm_is_user_plane_update_required(T_SM_UPDATE_FLAGS update_flags);

void             sm_set_nw_cause(struct T_SM_CONTEXT_DATA *context,
				 T_CAUSE_ctrl_value ctrl_cause, U16 cause);
/*@observer@*/
T_CAUSE_ps_cause*sm_get_nw_cause(struct T_SM_CONTEXT_DATA *context);

void             sm_set_aci_cause(struct T_SM_CONTEXT_DATA *context,
				  T_CAUSE_ctrl_value ctrl_cause, U16 cause);
/*@observer@*/
T_CAUSE_ps_cause*sm_get_aci_cause(struct T_SM_CONTEXT_DATA *context);

#endif /* _SM_H */
/*==== END OF FILE ==========================================================*/

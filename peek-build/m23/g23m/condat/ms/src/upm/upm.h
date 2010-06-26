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
| Purpose:    Global definitions for the User Plane Manager (UPM) Entity.
|             For design details, see:
|             8010.939 UPM Detailed Specification
+---------------------------------------------------------------------------*/

/*==== DECLARATION CONTROL =================================================*/

#ifndef _UPM_H
#define _UPM_H

/*#ifdef FF_EGPRS
#ifndef UPM_EDGE
#define UPM_EDGE
#endif */ /*#ifndef UPM_EDGE*/
/*#endif */ /*#ifdef FF_EGPRS*/

/*#ifdef UPM_EDGE */
#define ENTITY_UPM
/*#endif */

#ifdef WIN32
#define DEBUG
#endif

/*==== INCLUDES =============================================================*/

#include <string.h>
#include <typedefs.h>
#include <vsi.h>
#include <gsm.h>
#include <gprs.h>
#include <prim.h>

/*#ifndef UPM_EDGE
#include <trace.h> 
#endif *//*#ifndef UPM_EDGE*/

#ifndef UPM_WITHOUT_USER_PLANE
#include <dti.h>
#endif /* UPM_WITHOUT_USER_PLANE */

struct T_CONTEXT_DATA;
typedef U32 T_MEM_HANDLE;

#include "upm_debug.h"

#ifndef UPM_WITHOUT_USER_PLANE
#include "upm_dti_control.h"
#include "upm_uplink_data_control.h"
#include "upm_uplink_data_suspend.h"
#include "upm_downlink_data_control.h"
#endif /* UPM_WITHOUT_USER_PLANE */
#include "upm_link_control.h"
#ifdef TI_UMTS
#include "upm_rab_control.h"
#endif /* TI_UMTS */
#ifdef TI_GPRS
#include "upm_sndcp_control.h"
#endif  /* TI_GPRS */
#ifdef TI_DUAL_MODE
#include "upm_rat_control.h"
#endif /* TI_DUAL_MODE */

/*==== CONSTS ===============================================================*/

#define UPM_UPLINK_DATA_BUFFER_SIZE    16
#define UPM_DOWNLINK_DATA_BUFFER_SIZE  16
#define UPM_DTI_QUEUE_SIZE             1

#define UPM_MAX_NSAPI_OFFSET (NAS_SIZE_NSAPI - (int)NAS_NSAPI_5)
/*==== TYPES ================================================================*/

#ifndef UPM_WITHOUT_USER_PLANE 
struct T_UPM_desc_list_entry
{
  /*@null@*/ struct T_UPM_desc_list_entry *next;
  T_desc2                                 *desc;
};

/*@abstract@*/ struct T_UPM_USER_PLANE_DATA
{
  T_UPM_DTI_UPPER_CONTROL_STATE     dti_upper_control_state;
#ifdef TI_GPRS
  T_UPM_DTI_LOWER_CONTROL_STATE     dti_lower_control_state;
#endif /* TI_GPRS */
  T_UPM_UPLINK_DATA_CONTROL_STATE   ul_data_state;
  T_UPM_UPLINK_DATA_SUSPEND_STATE   ul_data_suspend_state;
  T_UPM_DOWNLINK_DATA_CONTROL_STATE dl_data_state;

  U8                                dti_flags[UPM_DTI_NUMBER_OF_INTERFACES];

  U16                  	            ul_data_buffer_count;
  U16                  	            dl_data_buffer_count;

  U32                               ul_data_octets_transferred;
  U32                               ul_data_packets_transferred;
  U32                               dl_data_octets_transferred;
  U32                               dl_data_packets_transferred;

  U32                               ul_data_packets_dropped;
  U32                               dl_data_packets_dropped;

  /*@null@*/ /*@only@*/ /*@reldef@*/
  struct T_UPM_desc_list_entry     *ul_data_buffer;
  /*@null@*/ /*@only@*/ /*@reldef@*/
  T_MEM_HANDLE                     *dl_data_buffer;
};
#endif  /* UPM_WITHOUT_USER_PLANE */

#ifdef TI_GPRS
/*@abstract@*/ struct T_UPM_CONTEXT_DATA_GPRS
{
  U8                                sndcp_delay;        /* R97 QoS */
  U8                   	    	    sndcp_relclass;     /* R97 QoS */
  U8                   	    	    sndcp_peak_bitrate; /* R97 QoS */
  U8                   	    	    sndcp_preced;       /* R97 QoS */
  U8                   	    	    sndcp_mean;         /* R97 QoS */

  U8                                pkt_flow_id;        /* EDGE */
  U8                   	    	    sndcp_radio_prio;
  U8                   	    	    sndcp_llc_sapi;
};
#endif /* TI_GPRS */

/*@abstract@*/ struct T_CONTEXT_DATA
{
  U8                                nsapi;

  U8                                seq_nos_valid;
  U8                                recv_seq_no;
  U8                                send_seq_no;

  T_UPM_LINK_CONTROL_STATE          link_control_state;
#ifdef TI_UMTS
  T_UPM_RAB_CONTROL_STATE   	    rab_control_state;
#endif /* TI_UMTS */
#ifdef TI_GPRS
  T_UPM_SNDCP_CONTROL_STATE 	    sndcp_control_state;
#endif /* TI_GPRS */

#ifdef TI_UMTS
  U8                                rab_qos_tc;
  U8                   	    	    mem_user;
  U16                  	    	    mem_space_before;
#endif /* TI_UMTS */
#ifdef TI_GPRS
  struct T_UPM_CONTEXT_DATA_GPRS    gprs_data;
#endif /* TI_GPRS */

#ifndef UPM_WITHOUT_USER_PLANE
  struct T_UPM_USER_PLANE_DATA      up_data;
#endif /* UPM_WITHOUT_USER_PLANE */
};

typedef struct {
#ifdef TI_DUAL_MODE
  T_UPM_RAT_CONTROL_STATE          rat_control_state;
  T_PS_rat                         upm_rat;
#endif /* TI_DUAL_MODE */
  BOOL                             upm_ll_entity_test;
/* Communication handles */
  T_HANDLE                         hCommACI;
  T_HANDLE                         hCommSM;
  T_HANDLE                         hCommMM;
  T_HANDLE                         upm_handle;
#ifdef TI_UMTS
  T_HANDLE                         hCommPDCP;
  T_HANDLE                         hCommRRC;
#endif /* TI_UMTS */
#ifdef TI_GPRS
  T_HANDLE                         hCommSNDCP;
  T_HANDLE                         hCommRR;
#endif /* TI_GPRS */

#ifndef UPM_WITHOUT_USER_PLANE
  DTI_HANDLE                       upm_hDTI;
#endif /* UPM_WITHOUT_USER_PLANE */
  /*@only@*/ /*@reldef@*/
  struct T_CONTEXT_DATA           *upm_context_array[UPM_MAX_NSAPI_OFFSET];
} T_UPM_DATA;

#ifdef DEBUG
#define M_TRANSITION(_EVENT, _FUNC) {_EVENT, _FUNC}
#else /* WIN32 */
#define M_TRANSITION(_EVENT, _FUNC) {_FUNC}
#endif /* WIN32 */

/*==== EXPORTS ==============================================================*/

extern T_UPM_DATA upm_data;

void     upm_pfree(/*@null@*/ /*@only@*/ /*@out@*/void *data);
void     upm_mfree(/*@null@*/ /*@only@*/ /*@out@*/void *data);

U16      upm_nsapi2nsapi_set(int /*@alt U8@*/ nsapi) /*@*/;

/*@null@*/ /*@exposed@*/
struct T_CONTEXT_DATA *upm_get_context_data_from_nsapi(int /*@alt U8@*/ nsapi);
void     upm_assign_context_data_to_nsapi(/*@keep@*/ struct T_CONTEXT_DATA *,
					  int /*@alt U8@*/ nsapi);

void     upm_free_context_data(int /*@alt U8@*/ nsapi);
void     upm_check_for_released_context_and_release(struct T_CONTEXT_DATA *);

BOOL     upm_ll_entity_test(void);
void     upm_activate_ll_entity_test(void) /*@modifies upm_data.upm_ll_entity_test@*/;

#ifdef TI_DUAL_MODE
T_PS_rat upm_get_current_rat(void);
void     upm_set_current_rat(T_PS_rat rat) /*@modifies upm_data.upm_rat@*/;
#endif /* TI_DUAL_MODE */

void     upm_set_sequence_number(struct T_CONTEXT_DATA *, U8);
U8       upm_get_sequence_number(struct T_CONTEXT_DATA *);

#define hCommACI    upm_data.hCommACI    /* Communication handle to ACI   */
#define hCommMM     upm_data.hCommMM     /* Communication handle to MM    */
#define hCommSM     upm_data.hCommSM     /* Communication handle to SM    */

#ifdef TI_UMTS
#define hCommPDCP   upm_data.hCommPDCP   /* Communication handle to PDCP  */
#define hCommRRC    upm_data.hCommRRC    /* Communication handle to RRC   */
#endif /* TI_UMTS */
#ifdef TI_GPRS
#define hCommSNDCP  upm_data.hCommSNDCP  /* Communication handle to SNDCP */
#define hCommRR     upm_data.hCommRR     /* Communication handle to RR    */
#endif /* TI_GPRS */
#ifndef UPM_WITHOUT_USER_PLANE
#define upm_hDTI    upm_data.upm_hDTI
#endif /* UPM_WITHOUT_USER_PLANE */

#define VSI_CALLER        upm_data.upm_handle,
#define VSI_CALLER_SINGLE upm_data.upm_handle

/* Uniquify pei_create() */
#define pei_create upm_pei_create
extern short upm_pei_create (T_PEI_INFO **info)
     /*@modifies *info, internalState@*/ /*@globals internalState@*/;

#endif /* _UPM_H */
/*==== END OF FILE ==========================================================*/


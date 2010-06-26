/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  gmm_pei.c
+----------------------------------------------------------------------------- 
|  Copyright 2002 Texas Instruments Berlin, AG 
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
|  Purpose :  This module implements the process body interface
|             for the entity GPRS Mobility Management (GMM)
|             
|             Exported functions:
|             
|             pei_create    - Create the Protocol Stack Entity
|             pei_init      - Initialize Protocol Stack Entity
|             pei_primitive - Process Primitive
|             pei_timeout   - Process Timeout
|             pei_exit      - Close resources and terminate
|             pei_run       - Process Primitive
|             pei_config    - Dynamic Configuration
|             pei_monitor   - Monitoring of physical Parameters
+----------------------------------------------------------------------------- 
*/ 


#define GMM_PEI_C

#define ENTITY_GMM

/*==== INCLUDES =============================================================*/

#include <stddef.h>     /* to get definition of offsetof(), for MAK_FUNC_S */
#include <stdlib.h>     /* to get atoi for tokenizer in pei_init */
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_gmm.h"    /* to get cnf-definitions */
#include "mon_gmm.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "pei.h"        /* to get PEI interface */
#include "gmm.h"        /* to get the global entity definitions */
#include "gmm_f.h"      /* to get the debug print function */

#include "gmm_kernp.h"  /* to get primitive interface to KERN */
#include "gmm_txp.h"    /* to get primitive interface to TX */
#include "gmm_rxp.h"    /* to get primitive interface to RX */
#include "gmm_rdyp.h"   /* to get primitive interface to RDY */
#include "gmm_syncp.h"  /* to get primitive interface to SYNC */

#include "gmm_kernf.h"  /* to get functions from KERN */
#include "gmm_txf.h"    /* to get functions from TX */
#include "gmm_rxf.h"    /* to get functions from RX */
#include "gmm_rdyf.h"   /* to get functions from RDY */
#include "gmm_syncf.h"  /* to get functions from SYNC */


#include "ccdapi.h"     /* to get ccd stuff */
#include "tok.h"        /* to get tokenizer */
#include  <string.h>    /* to get memcpy */

#include "gmm_em.h"     /*to get definitions of the Engineering Mode*/
/*==== DEFINITIONS ==========================================================*/

/*==== TYPES ================================================================*/

/*==== GLOBAL VARS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

static  BOOL          first_access  = TRUE;
static  T_MONITOR     gmm_mon;

/*
 * Jumptables to primitive handler functions. One table per SAP.
 *
 * Use MAK_FUNC_0 for primitives which contains no SDU.
 * Use MAK_FUNC_S for primitives which contains a SDU.
 */

/*******************************************************
 * SAVE prim
 *******************************************************/

#define PEI_SIGNAL_NOT_SAVED      FALSE
#define PEI_SIGNAL_SAVED          TRUE
#define PEI_END_SAVETAB           0xFF

#define GMM_MAX_SIGNALS_SAVED      5     /* value adjusted to GMM requirements */

typedef struct SAVE_QUEUE
{
  void*              prim;
  T_VOID_FUNC        func;
  struct SAVE_QUEUE* next;
} T_SAVE_QUEUE;

void gmm_pei_delete_queue( T_SAVE_QUEUE** queue );

typedef struct
{
  USHORT  state;
  USHORT  signal[GMM_MAX_SIGNALS_SAVED];
} T_SAVE_TAB;

/*==== LOCAL PROTOTYPES =====================================================*/
LOCAL void gmm_pei_handle_prim( T_VOID_FUNC func, void* prim );
LOCAL BOOL gmm_pei_handle_save( void* prim, T_SAVE_QUEUE** queue,
                                     const T_SAVE_TAB* table, T_VOID_FUNC func);
LOCAL void gmm_pei_handle_queue( T_SAVE_QUEUE** queue,
                                      const T_SAVE_TAB* table);

/*==== VAR LOCAL ============================================================*/

LOCAL T_SAVE_QUEUE*     save_queue;     /* SAVE queue hangers */

LOCAL const T_SAVE_TAB save_tab[] = {

  { KERN_GMM_REG_INITIATED,      
                            {MMGMM_LUP_NEEDED_IND,
                             GMMREG_NET_REQ, 
                             GMMREG_PLMN_RES,
                             0, 0 }
  },

  { KERN_GMM_RAU_INITIATED,      
                            {MMGMM_LUP_NEEDED_IND,
                             GMMREG_DETACH_REQ,
                             GMMREG_NET_REQ, 
                             GMMREG_PLMN_RES,
                             0 }
  },

  {KERN_GMM_RAU_WAIT_FOR_NPDU_LIST ,
                           {GMMRR_CELL_IND, 
                            0, 0, 0, 0 }
  },
  { KERN_GMM_DEREG_SUSPENDING,
                            {GMMREG_NET_REQ, 
                             GMMREG_PLMN_RES,
                             GMMRR_CS_PAGE_IND,
                             MMGMM_CM_ESTABLISH_IND,
                             MMGMM_CM_EMERGENCY_IND     }
  },
  { KERN_GMM_REG_SUSPENDING,
                            {GMMREG_NET_REQ, 
                             GMMREG_PLMN_RES,
                             GMMRR_CS_PAGE_IND,
                             MMGMM_CM_ESTABLISH_IND,
                             MMGMM_CM_EMERGENCY_IND     }
  },
  { KERN_GMM_REG_RESUMING,
                            {GMMREG_DETACH_REQ, 
                             GMMRR_CS_PAGE_IND,
                             MMGMM_CM_ESTABLISH_IND,
                             MMGMM_CM_EMERGENCY_IND,
                             GMMREG_ATTACH_REQ }
  },
  { KERN_GMM_DEREG_RESUMING,
                            {GMMREG_DETACH_REQ, 
                             GMMRR_CS_PAGE_IND,
                             MMGMM_CM_ESTABLISH_IND,
                             MMGMM_CM_EMERGENCY_IND,
                             GMMREG_ATTACH_REQ}
  },
  { PEI_END_SAVETAB,
                           { 0, 0, 0, 0, 0 }
  }
};
/*******************************************************
 * End SAVE prim
 *******************************************************/


/*
 * Function is needed for grr_table[]. This declaration can be removed
 * as soon as this function is no more called (i.e. all primitives are
 * handled).
 */
LOCAL void primitive_not_supported (void *data);

static const T_FUNC gmmreg_table[] =
{
  MAK_FUNC_0(kern_gmmreg_attach_req,        GMMREG_ATTACH_REQ),
  MAK_FUNC_0(kern_gmmreg_detach_req,        GMMREG_DETACH_REQ),
  MAK_FUNC_0(kern_gmmreg_net_req,           GMMREG_NET_REQ),
  MAK_FUNC_0(kern_gmmreg_plmn_res,          GMMREG_PLMN_RES),
  MAK_FUNC_0(kern_gmmreg_plmn_mode_req,     GMMREG_PLMN_MODE_REQ),
  MAK_FUNC_0(kern_gmmreg_config_req,        GMMREG_CONFIG_REQ),
};

#ifdef FF_EM_MODE
static const T_FUNC em_ul_table[] =
{
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x00 */
  /*EM_SC_GPRS_INFO_REQ cannot be sent directly from ACI to GRR, because no SAP is defined.
  So it have to be passed via GMM.*/
  MAK_FUNC_0(em_gmm_sc_gprs_info_req,       EM_SC_GPRS_INFO_REQ     ), /* 0x01 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x02 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x03 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x04 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x05 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x06 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x07 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x08 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x09 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0A */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0B */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0C */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0D */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0E */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0F */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x10 */
  MAK_FUNC_0(em_gmm_pco_trace_req   ,       EM_PCO_TRACE_REQ   ), /* 0x11*/ /*PCO output*/
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x12 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x13 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x14 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x15 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x16 */
  MAK_FUNC_0(em_gmm_info_req,                EM_GMM_INFO_REQ   ), /* 0x17 */
  /*EM_GRLC_INFO_REQ cannot be sent directly from ACI to GRLC, so it is passed via GMM*/
  MAK_FUNC_0(em_gmm_grlc_info_req,          EM_GRLC_INFO_REQ        ),  /* 0x18 */
  MAK_FUNC_N(primitive_not_supported,       0                       ), /* 0x19 */
  MAK_FUNC_0(em_gmm_grr_event_req,          EM_GRR_EVENT_REQ        ), /* 0x1A */
  MAK_FUNC_0(em_gmm_event_req,              EM_GMM_EVENT_REQ        ), /* 0x1B */
  MAK_FUNC_0(em_gmm_grlc_event_req,         EM_GRLC_EVENT_REQ       ), /* 0x1C */
  MAK_FUNC_0(em_gmm_throughput_info_req,    EM_THROUGHPUT_INFO_REQ  ) /* 0x1D */
};

static const T_FUNC em_dl_table[] =
{
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x00 */
/*EM_SC_GPRS_INFO_CNF cannot be sent directly from GRR to ACI, so it is passed via GMM*/
  MAK_FUNC_0(em_gmm_sc_gprs_info_cnf,       EM_SC_GPRS_INFO_CNF      ), /* 0x01 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x02 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x03 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x04 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x05 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x06 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x07 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x08 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x09 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0A */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0B */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0C */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0D */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0E */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x0F */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x10 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x11 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x12 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x13 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x14 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x15 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x16 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* 0x17 */
/*EM_GRLC_INFO_CNF cannot be sent directly from ACI to GRLC, so it is passed via GMM*/
  MAK_FUNC_0(em_gmm_grlc_info_cnf,          EM_GRLC_INFO_CNF         ), /* 0x18 */
  MAK_FUNC_N(primitive_not_supported,       0                        ), /* 0x19 */
  MAK_FUNC_N(primitive_not_supported,       0                        ), /* 0x1A */
  MAK_FUNC_N(primitive_not_supported,       0                        ), /* 0x1B */
  MAK_FUNC_N(primitive_not_supported,       0                        ), /* 0x1C */
  MAK_FUNC_0(em_gmm_throughput_info_cnf,    EM_THROUGHPUT_INFO_CNF   ) /* 0x1D */
};
#endif /* FF_EM_MODE */  


static const T_FUNC gmmrr_table[] =
{
  MAK_FUNC_0(sync_gmmrr_cell_ind,           GMMRR_CELL_IND),
  MAK_FUNC_N(primitive_not_supported,       0                  ),
  MAK_FUNC_0(kern_gmmrr_page_ind,           GMMRR_PAGE_IND),
  MAK_FUNC_0(kern_gmmrr_cs_page_ind,        GMMRR_CS_PAGE_IND),
  MAK_FUNC_0(kern_gmmrr_suspend_cnf,        GMMRR_SUSPEND_CNF),
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ),/* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_0(kern_gmmrr_cr_ind,             GMMRR_CR_IND),
};


static const T_FUNC cgrlc_table[] = /* TCS 2.1 */
{ /* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_N(primitive_not_supported,       0                  ), /* TCS 2.1 */
  MAK_FUNC_0(kern_cgrlc_status_ind,         CGRLC_STATUS_IND ), /* TCS 2.1 */
  MAK_FUNC_0(kern_cgrlc_test_mode_cnf,      CGRLC_TEST_MODE_CNF ), /* TCS 2.1 */
  MAK_FUNC_0(rdy_cgrlc_trigger_ind,         CGRLC_TRIGGER_IND ), /* TCS 2.1 */
  MAK_FUNC_0(rdy_cgrlc_standby_state_ind,   CGRLC_STANDBY_STATE_IND ), /* TCS 2.1 */
  MAK_FUNC_0(rdy_cgrlc_ready_state_ind,     CGRLC_READY_STATE_IND ), /* TCS 2.1 */
}; /* TCS 2.1 */


#ifndef GMM_TCS4
static const T_FUNC gmmsm_table[] =
{
  MAK_FUNC_0(kern_gmmsm_establish_req,      GMMSM_ESTABLISH_REQ),
  MAK_FUNC_S(tx_gmmsm_unitdata_req,         GMMSM_UNITDATA_REQ),
  MAK_FUNC_0(kern_gmmsm_sequence_res,       GMMSM_SEQUENCE_RES),
};
#else   
static const T_FUNC mmpm_table[] =
{
#ifdef REL99
  MAK_FUNC_0(kern_gmmsm_pdp_status_req,    MMPM_PDP_CONTEXT_STATUS_REQ),
#else
  MAK_FUNC_0(primitive_not_supported,      MMPM_PDP_CONTEXT_STATUS_REQ),
#endif
  MAK_FUNC_N(primitive_not_supported,      0),
  MAK_FUNC_N(primitive_not_supported,      0),
  MAK_FUNC_0(kern_gmmsm_sequence_res,      MMPM_SEQUENCE_RES),
  MAK_FUNC_S(tx_gmmsm_unitdata_req,        MMPM_UNITDATA_REQ),
};

#endif /* #ifndef GMM_TCS4 */

static const T_FUNC gmmsms_table[] =
{
  MAK_FUNC_0(kern_gmmsms_reg_state_req,     GMMSMS_REG_STATE_REQ)
};

LOCAL const T_FUNC sim_table[] = {
  MAK_FUNC_0 (primitive_not_supported      , SIM_READ_CNF           ), /* 0x00 */
  MAK_FUNC_0 (primitive_not_supported      , SIM_UPDATE_CNF         ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_READ_RECORD_CNF    ),
  MAK_FUNC_N (primitive_not_supported      , 0                      ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_UPDATE_RECORD_CNF  ),
  MAK_FUNC_N (primitive_not_supported      , 0                      ),
  MAK_FUNC_N (primitive_not_supported      , 0                      ),
  MAK_FUNC_N (primitive_not_supported      , 0                      ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_INCREMENT_CNF      ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_VERIFY_PIN_CNF     ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_CHANGE_PIN_CNF     ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_DISABLE_PIN_CNF    ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_ENABLE_PIN_CNF     ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_UNBLOCK_CNF        ),
  MAK_FUNC_0 (kern_sim_authentication_cnf       , SIM_AUTHENTICATION_CNF ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_MMI_INSERT_IND     ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_MM_INSERT_IND   ),  
  MAK_FUNC_0 (kern_sim_remove_ind    , SIM_REMOVE_IND         ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_SYNC_CNF           ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_ACTIVATE_CNF       ),
  MAK_FUNC_0 (primitive_not_supported      , SIM_SMS_INSERT_IND     ), /* 0x14 */
  MAK_FUNC_0 (primitive_not_supported      , SIM_TOOLKIT_IND        ), /* 0x15 */
  MAK_FUNC_0 (primitive_not_supported      , SIM_TOOLKIT_CNF        ), /* 0x16 */
  MAK_FUNC_0 (primitive_not_supported      , SIM_ACTIVATE_IND       ), /* 0x17 */
  MAK_FUNC_0 (primitive_not_supported      , SIM_MM_INFO_IND        ), /* 0x18 GMM GlumPs*/
  MAK_FUNC_0 (primitive_not_supported      , SIM_ACCESS_CNF         ), /* 0x19 */
  MAK_FUNC_0 (primitive_not_supported   , SIM_FILE_UPDATE_IND    ),  /* 0x1a */
  MAK_FUNC_0 (kern_sim_gmm_insert_ind  , SIM_GMM_INSERT_IND   ),  /* 0x1b */
};

static const T_FUNC ll_table[] =
{
  MAK_FUNC_0(primitive_not_supported,       LL_RESET_IND),
  MAK_FUNC_S(primitive_not_supported,       LL_ESTABLISH_CNF),
  MAK_FUNC_S(primitive_not_supported,       LL_ESTABLISH_IND),
  MAK_FUNC_0(primitive_not_supported,       LL_RELEASE_CNF),
  MAK_FUNC_0(primitive_not_supported,       LL_RELEASE_IND),
  MAK_FUNC_S(primitive_not_supported,       LL_XID_CNF),
  MAK_FUNC_S(primitive_not_supported,       LL_XID_IND),
  MAK_FUNC_0(primitive_not_supported,       LL_READY_IND),
  MAK_FUNC_0(primitive_not_supported,       LL_UNITREADY_IND),
  MAK_FUNC_0(primitive_not_supported,       LL_DATA_CNF),
  MAK_FUNC_S(primitive_not_supported,       LL_DATA_IND),
  MAK_FUNC_S(rx_ll_unitdata_ind,            LL_UNITDATA_IND)
};

static const T_FUNC llgmm_table[] =
{
  MAK_FUNC_0(kern_llgmm_status_ind,         LLGMM_STATUS_IND),
  MAK_FUNC_0(kern_llgmm_tlli_ind,         LLGMM_TLLI_IND)
};

static const T_FUNC mmgmm_table[] =
{
  MAK_FUNC_0(sync_mmgmm_reg_cnf,            MMGMM_REG_CNF),
  MAK_FUNC_0(sync_mmgmm_reg_rej,            MMGMM_REG_REJ),
  MAK_FUNC_0(sync_mmgmm_nreg_ind,           MMGMM_NREG_IND),
  MAK_FUNC_0(kern_mmgmm_nreg_cnf,           MMGMM_NREG_CNF),
  MAK_FUNC_0(kern_mmgmm_plmn_ind,           MMGMM_PLMN_IND),
  MAK_FUNC_0(kern_mmgmm_auth_rej_ind,       MMGMM_AUTH_REJ_IND),
  MAK_FUNC_0(kern_mmgmm_cm_establish_ind,   MMGMM_CM_ESTABLISH_IND),
  MAK_FUNC_0(kern_mmgmm_cm_release_ind,     MMGMM_CM_RELEASE_IND),
  MAK_FUNC_0(sync_mmgmm_activate_ind,       MMGMM_ACTIVATE_IND),
  MAK_FUNC_0(kern_mmgmm_t3212_val_ind,      MMGMM_T3212_VAL_IND),
  MAK_FUNC_0(kern_mmgmm_info_ind,           MMGMM_INFO_IND),
  MAK_FUNC_0(kern_mmgmm_cm_emergency_ind,   MMGMM_CM_EMERGENCY_IND),
  MAK_FUNC_0(kern_mmgmm_lup_accept_ind,     MMGMM_LUP_ACCEPT_IND),
  MAK_FUNC_0(kern_mmgmm_lup_needed_ind,     MMGMM_LUP_NEEDED_IND),
  MAK_FUNC_0(kern_mmgmm_ciphering_ind,      MMGMM_CIPHERING_IND),
  MAK_FUNC_0(kern_mmgmm_tmsi_ind,           MMGMM_TMSI_IND),
  MAK_FUNC_0(kern_mmgmm_ahplmn_ind,         MMGMM_AHPLMN_IND)
};


/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : primitive_not_supported
+------------------------------------------------------------------------------
|  Description  :  This function handles unsupported primitives.
|
|  Parameters  :  -
|
|  Return      :  -
|
+------------------------------------------------------------------------------
*/
LOCAL void primitive_not_supported (void *data)
{
  TRACE_FUNCTION ("primitive_not_supported");

  PFREE (data);
}


/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : pei_primitive
+------------------------------------------------------------------------------
|  Description  :  This function is called by the frame when a primitive is
|                received and needs to be processed.
|
|                             |
|                          GMMREG
|                       |     |     |     |
|                     GMMSM   |   GMMSM GMMAA             UPLINK
|                       |     |     |     |
|                   +---v-----v-----v-----v----+
|                   |                          |
|         MMGMM ---->           GMM            |
|                   |                          |
|                   +---^--------^--------^----+
|                       |        |        |
|                       |        |      LLGMM            DOWNLINK
|                       |      GMMRR      |
|                       |        |
|                     SIM
|                       |
|
|
|  Parameters  :  prim      - Pointer to the received primitive
|
|  Return      :  PEI_OK    - function succeeded
|               PEI_ERROR - function failed
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_primitive (void * primptr)
{
  TRACE_FUNCTION ("pei_primitive");
#ifdef TRACE_FUNC
#ifdef IDENTATION
  gmm_data->deep=0;
#endif
#endif

  if (primptr NEQ NULL)
  {
    T_PRIM *prim  = (T_PRIM *)primptr;
    ULONG           opc = prim->custom.opc;
    USHORT           n;
    const T_FUNC    *table;

    /*
     * This must be called for Partition Pool supervision. Will be replaced
     * by another macro some time.
     */
    VSI_PPM_REC (&prim->custom, __FILE__, __LINE__);

    GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_STATE);
    PTRACE_IN (opc);

    switch (SAP_NR(opc))
    {
      case GMMREG_UL:
        table = gmmreg_table;
        n = TAB_SIZE (gmmreg_table);
        break;
      case GMMRR_DL:
        table = gmmrr_table;
        n = TAB_SIZE (gmmrr_table);
        break;      
      case SAP_NR(CGRLC_DL):  /* TCS 2.1 */
        table = cgrlc_table;  /* TCS 2.1 */
        n = TAB_SIZE (cgrlc_table); /* TCS 2.1 */
        break; /* TCS 2.1 */
#ifndef GMM_TCS4
      case GMMSM_UL:
        table = gmmsm_table;
        n = TAB_SIZE (gmmsm_table);
        break;
#else
      case SAP_NR(MMPM_UL):
        table = mmpm_table;
        n = TAB_SIZE (mmpm_table);
        break;
#endif
      case GMMSMS_UL:
        table = gmmsms_table;
        n = TAB_SIZE (gmmsms_table);
        break;
      case SAP_NR(SIM_UL):/*lint !e778 (Info -- Constant expression evaluates to 0 in operation '&') */
        table = sim_table;
        n = TAB_SIZE (sim_table);
        break;
      case LL_DL:
        table = ll_table;
        n = TAB_SIZE (ll_table);
        break;
      case LLGMM_DL:
        table = llgmm_table;
        n = TAB_SIZE (llgmm_table);
        break;
      case MMGMM_DL:
        table = mmgmm_table;
        n = TAB_SIZE (mmgmm_table);
        break;
#ifdef FF_EM_MODE
      case EM_Ul:
        table = em_ul_table;
        n = TAB_SIZE (em_ul_table);
        break;
      case EM_Dl:
        table = em_dl_table;
        n = TAB_SIZE (em_dl_table);
        break;
#endif /* FF_EM_MODE */
      default:
        TRACE_ERROR("pei_primitive. Unknown SAP");
        table = NULL;
        n = 0;
        break;
    }

    if (table != NULL)
    {
      if (PRIM_NR(opc) < n)
      {
        table += PRIM_NR(opc);
#ifdef PALLOC_TRANSITION
        P_SDU(prim) = table->soff ? (T_sdu*) (((char*)&prim->data) + table->soff) : 0;
#ifndef NO_COPY_ROUTING
        P_LEN(prim) = table->size + sizeof (T_PRIM_HEADER);
#endif /* NO_COPY_ROUTING */
#endif /* PALLOC_TRANSITION */

        /*******************************************************
         * SAVE prim
         *******************************************************/

        if( gmm_pei_handle_save( prim, &save_queue, save_tab, table->func )
          ==  PEI_SIGNAL_NOT_SAVED )
        {
          UBYTE old_kern_state = gmm_data->kern.state;
          /*******************************************************
           * SAVE prim
           *******************************************************/

          JUMP (table->func) (P2D(prim));
          
          /*******************************************************
           * SAVE prim
           *******************************************************/
          if( gmm_data->kern.state != old_kern_state )
          {
            gmm_pei_handle_queue(&save_queue, save_tab);
          }
        } 
        else
        {
          TRACE_2_INFO ("S:%2d SAVE of Primitive 0x%x", gmm_data->kern.state, opc );
        }



        /*******************************************************
         * End SAVE prim
         *******************************************************/
        
      }
      else
      {
        primitive_not_supported (P2D(prim));
      }
      return PEI_OK;
    }

    /*
     * primitive is not a GSM primitive - forward it to the environment
     */
    if (opc & SYS_MASK)
      vsi_c_primitive (VSI_CALLER prim);
    else
    {
      PFREE (P2D(prim));
      return PEI_ERROR;
    }
  }
  return PEI_OK;
}


/*
+------------------------------------------------------------------------------
|  Function    : pei_init
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame. It is used to initialise
|               the entitiy.
|
|  Parameters  :  handle            - task handle
|
|  Return      :  PEI_OK            - entity initialised
|               PEI_ERROR         - entity not (yet) initialised
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
  TRACE_FUNCTION ("pei_init");

  /*
   * Initialize task handle
   */
  GMM_handle = handle;

  /*
   * Open communication channels
   */
  if (hCommSMS < VSI_OK)
  {
    if ((hCommSMS = vsi_c_open (VSI_CALLER SMS_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommSM < VSI_OK)
  {
    if ((hCommSM = vsi_c_open (VSI_CALLER SM_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommGRLC < VSI_OK) /* TCS 2.1 */
  { /* TCS 2.1 */
    if ((hCommGRLC = vsi_c_open (VSI_CALLER GRLC_NAME)) < VSI_OK) /* TCS 2.1 */
      return PEI_ERROR; /* TCS 2.1 */
  } /* TCS 2.1 */

  if (hCommGRR < VSI_OK)
  {
    if ((hCommGRR = vsi_c_open (VSI_CALLER GRR_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommLLC < VSI_OK)
  {
    if ((hCommLLC = vsi_c_open (VSI_CALLER LLC_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommSIM < VSI_OK)
  {
    if ((hCommSIM = vsi_c_open (VSI_CALLER SIM_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommMM < VSI_OK)
  {
    if ((hCommMM = vsi_c_open (VSI_CALLER MM_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommMMI < VSI_OK)
  {
    if ((hCommMMI = vsi_c_open (VSI_CALLER ACI_NAME)) < VSI_OK) /* TCS 2.1 */
      return PEI_ERROR;
  }
  if (hCommGMM < VSI_OK)
  {
    if ((hCommGMM = vsi_c_open (VSI_CALLER GMM_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#ifdef GMM_TCS4
  if (hCommUPM < VSI_OK)
  {
    if ((hCommUPM = vsi_c_open (VSI_CALLER UPM_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#endif


  /*
   * Initialize global pointer llc_data. This is required to access all
   * entity data.
   */
  gmm_data = &gmm_data_base;

  /*
   * Initialite ccd
   */
  ccd_init ();

#ifdef FF_EM_MODE
  em_init_gmm_event_trace();
#endif /* FF_EM_MODE */


  /*
   * Initialize entity data (call init function of every service)
   */

  kern_init();
  rxgmm_init();
  txgmm_init();
  rdy_init();
  sync_gmm_init();

  GMM_TRACE_GMM_DATA(GMM_DEBUG_PRINT_MASK_FULL);

  return (PEI_OK);
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_timeout
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when a timer has expired.
|
|  Parameters  :  index             - timer index
|
|  Return      :  PEI_OK            - timeout processed
|               PEI_ERROR         - timeout not processed
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_timeout (USHORT index)
{
  UBYTE      old_kern_state;
  TRACE_FUNCTION ("pei_timeout");

  old_kern_state = gmm_data->kern.state;
  /*
   * Process timeout
   */
  switch (index)
  {
    case kern_T3302:
      /*
       * T3302 expired.
       */
      kern_t3302 ();
      break;
    case kern_T3310:
      /*
       * T3310 expired.
       */
      kern_t3310 ();
      break;
    case kern_T3311:
      /*
       * T3311 expired.
       */
      kern_t3311 ();
      break;
    case kern_T3321:
      /*
       * T3321 expired.
       */
      kern_t3321 ();
      break;
    case rdy_T3312:
      /*
       * T3312 expired.
       */
      kern_t3312 ();
      break;
    case kern_TPOWER_OFF:
      kern_tpower_off();
      break;
    case kern_TLOCAL_DETACH:
      kern_tlocal_detach();
      break;
    case sync_TSYNC:
      sync_tsync();
      break;
    default:
      TRACE_ERROR("Unknown Timeout");
      return PEI_ERROR;
  }

  if( gmm_data->kern.state != old_kern_state )
  {
    gmm_pei_handle_queue(  &save_queue, save_tab);
  }
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_signal
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when a signal has been
|               received.
|
|  Parameters  :  opc               - signal operation code
|               *data             - pointer to primitive
|
|  Return      :  PEI_OK            - signal processed
|               PEI_ERROR         - signal not processed
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  TRACE_FUNCTION ("pei_signal");

  /*
   * Process signal
   */
  switch (opc)
  {
    default:
      TRACE_ERROR("Unknown Signal OPC");
      break;
  }/*lint !e764 (Info -- switch statement does not have a case) */

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_exit
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
|  Parameters  :  -
|
|  Return      :  PEI_OK            - exit sucessful
|               PEI_ERROR         - exit not sueccessful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit");

  /*
   * Close communication channels
   */
  vsi_c_close (VSI_CALLER hCommSMS);
  hCommSMS = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommSM);
  hCommSM = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommGRLC); /* TCS 2.1 */
  hCommGRLC = VSI_ERROR; /* TCS 2.1 */

  vsi_c_close (VSI_CALLER hCommGRR);
  hCommGRR = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommLLC);
  hCommLLC = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommSIM);
  hCommSIM = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommMM);
  hCommMM = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommMMI);
  hCommMMI = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommGMM);
  hCommGMM = VSI_ERROR;

  /*
   * delete queues
   */
  gmm_pei_delete_queue(&save_queue);
  
  /*
   * close ccd
   */
  ccd_exit ();

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_run
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
|  Parameters  :  handle            - Communication handle
|
|  Return      :  PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle )
{
  return PEI_OK;
}


/*
+------------------------------------------------------------------------------
|  Function    : gmm_tok_key
+------------------------------------------------------------------------------
|  Description  : This function is used by pei_config in windows test
|                 environment for string tokenizing for CHECK STATE command, e.g.
|                 COMMAND("GMM CONFIG CHECK_STATE=<REG_NORMAL_SERVICE>");
|
|
|  Parameters  :  
|
|  Return      :  
|               
|
+------------------------------------------------------------------------------
*/
#ifndef _TARGET_
typedef struct KW_STATE_DATA
{
   char   keyword[60];
   SHORT  code;
} KW_STATE_DATA;

LOCAL SHORT gmm_tok_key (KW_STATE_DATA * keytab, char * keyword)
{
  /*
   * Empty string terminates
   */
  while (keytab->keyword[0])
  {
    if (strcmp (keytab->keyword, keyword ) == 0)
      return (keytab->code);
    keytab++;
  }

  return (TOK_NOT_FOUND);
}
#endif

/*
+------------------------------------------------------------------------------
|  Function    : pei_config
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when a primitive is
|               received indicating dynamic configuration.
|
|               This function is not used in this entity.
|
|  Parameters  :  handle            - Communication handle
|
|  Return      :  PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_config (char *inString)
{
  TRACE_FUNCTION ("pei_config");
  TRACE_FUNCTION (inString);

#ifndef NCONFIG
   /*
   * Parse next keyword and number of variables
   */
    if(!strcmp(inString,"PERIODIC_LAU"))
    {
      PALLOC (mmgmm_lup_needed_ind, MMGMM_LUP_NEEDED_IND);
      mmgmm_lup_needed_ind->reason = MMGMM_T3212; /* TCS 2.1 */
      TRACE_EVENT(  "Info: Periodic LAU requested by MM");
      PSEND (hCommGMM, mmgmm_lup_needed_ind);
    }
    else if(!strcmp(inString,"PERIODIC_RAU"))
    {
      kern_t3312();
    }
    else if(!strcmp(inString,"REATTACH"))
    {
      PALLOC (gmmrr_page_ind,  GMMRR_PAGE_IND);
        gmmrr_page_ind->page_id=GMMRR_IMSI;
      PSEND (hCommGMM, gmmrr_page_ind);
    }
    else if(!strcmp(inString,"CELL"))
    {
      PALLOC (gmmrr_cell_ind,  GMMRR_CELL_IND);
      gmmrr_cell_ind->cell_info.service_state=GMMRR_SERVICE_FULL;
      gmmrr_cell_ind->cell_info.cell_env.rai.lac=0x2506;
      gmmrr_cell_ind->cell_info.cell_env.rai.rac=0x71;
      gmmrr_cell_ind->cell_info.net_mode=gmm_data->config.nmo;
      PSEND (hCommGMM, gmmrr_cell_ind);
    }
    else if(!strcmp(inString,"NMO_I"))
    {
      gmm_data->config.nmo = GMMRR_NET_MODE_I;
    }
    else if(!strcmp(inString,"NMO_II"))
    {
      gmm_data->config.nmo = GMMRR_NET_MODE_II;
    }
    else if(!strcmp(inString,"NMO_III"))
    {
      gmm_data->config.nmo = GMMRR_NET_MODE_III;
    }
    else if(!strcmp(inString,"GMM_INFO"))
    {
      PALLOC (gmmreg_info_ind,  GMMREG_INFO_IND);
      PSEND (hCommMMI, gmmreg_info_ind);
    }

    else if(!strcmp(inString,"RESET_SIM"))
    {
      gmm_data->sim_gprs_invalid = TRUE;
      kern_sim_del_locigprs ();
      kern_sim_gmm_update();
      kern_mm_auth_rej();
      {
        PALLOC (sim_update_req, SIM_UPDATE_REQ);

        sim_update_req->source         = SRC_GMM;
        /* req_id is not filled since response is not handled */

        sim_update_req->v_path_info    = FALSE;

        sim_update_req->datafield      = SIM_FPLMN;
        sim_update_req->length         = 12;
        memset (sim_update_req->trans_data,0xff,12);
        sim_update_req->offset         = 0;

        PSEND (hCommSIM, sim_update_req);
      }
      {
        PALLOC (sim_update_req, SIM_UPDATE_REQ);

        sim_update_req->source         = SRC_GMM;

        /* req_id is not filled since response is not handled */        
        sim_update_req->v_path_info    = FALSE;

        sim_update_req->datafield      = SIM_LOCI;
        sim_update_req->length         = 11;
        memset (sim_update_req->trans_data,0xff,8);
        sim_update_req->trans_data[8]=0xfe;
        sim_update_req->trans_data[9]=0x0;
        sim_update_req->trans_data[10]=GU2_NOT_UPDATED;
        sim_update_req->offset         = 0;

        PSEND (hCommSIM, sim_update_req);
      }
    }
    else if(!strcmp(inString,"ANITE"))
    {
      gmm_data->anite = TRUE;
    }
    else if(!strcmp(inString,"CIPHER_OFF"))
    {
      gmm_data->config.cipher_on =   0x00;
      TRACE_EVENT ("ciphering switched OFF");
    }
    else if(!strcmp(inString,"CIPHER_ON"))
    {
      gmm_data->config.cipher_on |= 0x01;
    }
    else if(!strcmp(inString,"PREUSE_OFF"))
    {
      gmm_data->config.preuse_off =    TRUE;
    }

#ifdef REL99
    else if(!strcmp(inString,"CL_SGSN_REL_98_OR_OLDER")) /* TCS 4.0 */
    { /* TCS 4.0 */
      /*gmm_data->config.sgsnr_flag =    R_98NW; */
      cl_nwrl_set_sgsn_release(PS_SGSN_98_OLDER); /*CL function*/ /* TCS 4.0 */
    } /* TCS 4.0 */
    else if(!strcmp(inString,"CL_SGSN_REL_99_ONWARDS")) /* TCS 4.0 */
    { /* TCS 4.0 */
      /* gmm_data->config.sgsnr_flag =    R_99NW; */
      cl_nwrl_set_sgsn_release(PS_SGSN_99_ONWARDS); /*CL function*/ /* TCS 4.0 */
    } /* TCS 4.0 */
    else if (!strcmp(inString,"CELL_NOTIFY_ON")) /* TCS 4.0 */
    { /* TCS 4.0 */
      gmm_data->config.cell_notification = FIRST_CELL_NOTIFY; /* TCS 4.0 */
    } /* TCS 4.0 */
    else if (!strcmp(inString, "CELL_NOTIFY_OFF")) /* TCS 4.0 */
    { /* TCS 4.0 */
      gmm_data->config.cell_notification = NO_CELL_NOTIFY; /* TCS 4.0 */
    } /* TCS 4.0 */
#endif

#ifdef _SIMULATION_    
    else if(!strcmp(inString, "NEXT_TIMEOUT"))
    {
      T_TIME old_value = 0;
      T_TIME new_value = 0;
      USHORT timer;
      USHORT timeout = TIMER_MAX;
    
      /* Find next timer to timeout */    
      
      for(timer = 0; timer < TIMER_MAX; timer++)
      {
        if ( VSI_OK == vsi_t_status ( GMM_handle ,timer, &new_value ))
        {
          if (new_value >0 )
          {
            if((old_value==0)||
               (new_value < old_value))
            {
              old_value = new_value;
              timeout = timer;        
            }
          }
        }
      }

      
      /* Force timeout */
      if(TIMER_MAX != timeout )
      {
        TRACE_1_INFO("gmm_pei_config - forcing timeout:%d", timeout);
        
        vsi_t_stop ( GMM_handle, timeout);       

        for(timer = 0; timer < TIMER_MAX ; timer++)
        {
          if ( VSI_OK == vsi_t_status ( GMM_handle ,timer, &new_value ))
          {
            if (timer!=timeout )
            {
              if (new_value>0 && new_value-old_value >0 )
              {
                vsi_t_start ( GMM_handle , timer, new_value-old_value ); 
              }
            }
          }
        }

        pei_timeout(timeout);
      }
    }
#endif

#define GMM_CONFIG_DRX               1
#define GMM_CONFIG_GEA            2

#ifndef _TARGET_
#define GMM_CONFIG_CHECK_STATE       3
#endif

#define GMM_DRX               "DRX"
#define GMM_GEA               "GEA"

#ifndef _TARGET_
#define GMM_CHECK_STATE       "CHECK_STATE"

#define GMM_NULL_NO_IMSI                      "NULL_NO_IMSI"
#define GMM_NULL_IMSI                         "NULL_IMSI"
#define GMM_DEREG_INITIATED                   "DEREG_INITIATED"  
#define GMM_DEREG_ATTEMPTING_TO_ATTACH        "DEREG_ATTEMPTING_TO_ATTACH"
#define GMM_DEREG_NO_CELL_AVAILABLE           "DEREG_NO_CELL_AVAILABLE"
#define GMM_DEREG_LIMITED_SERVICE             "DEREG_LIMITED_SERVICE"
#define GMM_DEREG_NO_IMSI                     "DEREG_NO_IMSI"
#define GMM_DEREG_PLMN_SEARCH                 "DEREG_PLMN_SEARCH"
#define GMM_DEREG_SUSPENDED                   "DEREG_SUSPENDED"

#define GMM_REG_INITIATED                     "REG_INITIATED"

#define GMM_REG_NO_CELL_AVAILABLE             "REG_NO_CELL_AVAILABLE"
#define GMM_REG_LIMITED_SERVICE               "REG_LIMITED_SERVICE"
#define GMM_REG_ATTEMPTING_TO_UPDATE_MM       "REG_ATTEMPTING_TO_UPDATE_MM  "
#define GMM_REG_ATTEMPTING_TO_UPDATE          "REG_ATTEMPTING_TO_UPDATE"
#define GMM_REG_RESUMING                      "REG_RESUMING"
#define GMM_REG_SUSPENDED                     "REG_SUSPENDED"
#define GMM_REG_NORMAL_SERVICE                "REG_NORMAL_SERVICE"

#define GMM_RAU_INITIATED                     "RAU_INITIATED"

#define GMM_RAU_WAIT_FOR_NPDU_LIST            "RAU_WAIT_FOR_NPDU_LIST"

#define GMM_REG_IMSI_DETACH_INITIATED         "REG_IMSI_DETACH_INITIATED"

#define GMM_DEREG_SUSPENDING                  "DEREG_SUSPENDING"
#define GMM_DEREG_RESUMING                    "DEREG_RESUMING"

#define GMM_REG_SUSPENDING                    "REG_SUSPENDING"
#define GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ     "NULL_NO_IMSI_LIMITED_SERVICE_REQ"
#define GMM_NULL_IMSI_LIMITED_SERVICE_REQ     "NULL_IMSI_LIMITED_SERVICE_REQ"
#define GMM_REG_TEST_MODE                     "REG_TEST_MODE"
#define GMM_NULL_PLMN_SEARCH                  "NULL_PLMN_SEARCH"
#define GMM_REG_TEST_MODE_NO_IMSI             "REG_TEST_MODE_NO_IMSI "

#endif
    
    {   
      LOCAL KW_DATA kwtab[] = 
      {
        GMM_DRX, GMM_CONFIG_DRX,
        GMM_GEA, GMM_CONFIG_GEA,
#ifndef _TARGET_
        GMM_CHECK_STATE, GMM_CONFIG_CHECK_STATE,
#endif
        "", 0  
      };
#ifndef _TARGET_
      LOCAL KW_STATE_DATA kw_state_tab[] = 
      {
         GMM_NULL_NO_IMSI                      ,KERN_GMM_NULL_NO_IMSI,
         GMM_NULL_IMSI                         ,KERN_GMM_NULL_IMSI,
         GMM_DEREG_INITIATED                   ,KERN_GMM_DEREG_INITIATED,  
         GMM_DEREG_ATTEMPTING_TO_ATTACH        ,KERN_GMM_DEREG_ATTEMPTING_TO_ATTACH,
         GMM_DEREG_NO_CELL_AVAILABLE           ,KERN_GMM_DEREG_NO_CELL_AVAILABLE,
         GMM_DEREG_LIMITED_SERVICE             ,KERN_GMM_DEREG_LIMITED_SERVICE,
         GMM_DEREG_NO_IMSI                     ,KERN_GMM_DEREG_NO_IMSI,
         GMM_DEREG_PLMN_SEARCH                 ,KERN_GMM_DEREG_PLMN_SEARCH,
         GMM_DEREG_SUSPENDED                   ,KERN_GMM_DEREG_SUSPENDED,

         GMM_REG_INITIATED                     ,KERN_GMM_REG_INITIATED,

         GMM_REG_NO_CELL_AVAILABLE             ,KERN_GMM_REG_NO_CELL_AVAILABLE,
         GMM_REG_LIMITED_SERVICE               ,KERN_GMM_REG_LIMITED_SERVICE,
         GMM_REG_ATTEMPTING_TO_UPDATE_MM       ,KERN_GMM_REG_ATTEMPTING_TO_UPDATE_MM  ,
         GMM_REG_ATTEMPTING_TO_UPDATE          ,KERN_GMM_REG_ATTEMPTING_TO_UPDATE,
         GMM_REG_RESUMING                      ,KERN_GMM_REG_RESUMING,
         GMM_REG_SUSPENDED                     ,KERN_GMM_REG_SUSPENDED,
         GMM_REG_NORMAL_SERVICE                ,KERN_GMM_REG_NORMAL_SERVICE,

         GMM_RAU_INITIATED                     ,KERN_GMM_RAU_INITIATED,

         GMM_RAU_WAIT_FOR_NPDU_LIST            ,KERN_GMM_RAU_WAIT_FOR_NPDU_LIST,

         GMM_REG_IMSI_DETACH_INITIATED         ,KERN_GMM_REG_IMSI_DETACH_INITIATED,

         GMM_DEREG_SUSPENDING                  ,KERN_GMM_DEREG_SUSPENDING,
         GMM_DEREG_RESUMING                    ,KERN_GMM_DEREG_RESUMING,
         GMM_REG_SUSPENDING                    ,KERN_GMM_REG_SUSPENDING,
         
         GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ     ,KERN_GMM_NULL_NO_IMSI_LIMITED_SERVICE_REQ,
         GMM_NULL_IMSI_LIMITED_SERVICE_REQ     ,KERN_GMM_NULL_IMSI_LIMITED_SERVICE_REQ,
         GMM_REG_TEST_MODE                     ,KERN_GMM_REG_TEST_MODE,
         GMM_NULL_PLMN_SEARCH                  ,KERN_GMM_NULL_PLMN_SEARCH,
         GMM_REG_TEST_MODE_NO_IMSI             ,KERN_GMM_REG_TEST_MODE_NO_IMSI ,
        "", 0  
      };

#endif
      SHORT valno;
      char *keyw;
      char *val[10];
      
      tok_init (inString);

      while((valno = tok_next(&keyw,val)) NEQ TOK_EOCS)
      {
        switch ((tok_key((KW_DATA *)kwtab, keyw)))
        {
          case GMM_CONFIG_DRX:
            if (3==valno)
            {
              TRACE_EVENT ("use of DRX <split_pg_cycle_code,split_on_ccch,non_drx_timer>");
              gmm_data->drx_parameter.split_pg_cycle_code = atoi (val[0]);
              gmm_data->drx_parameter.split_on_ccch       = atoi (val[1]);
              gmm_data->drx_parameter.non_drx_timer       = atoi (val[2]);
            }
            else
            {
              TRACE_ERROR("[PEI_CONFIG]: use CONFIG DRX <1,2,3>");
            }
            break;
          case GMM_CONFIG_GEA:
            if (1==valno)
            {
              TRACE_EVENT ("GEA=<bit_field of wanted GEA in dec>");
              
              gmm_data->config.cipher_on |= atoi (val[0]);
              TRACE_1_INFO("GEA2=%d", (gmm_data->config.cipher_on & 0x02)>0);
              TRACE_1_INFO("GEA3=%d", (gmm_data->config.cipher_on & 0x04)>0);
              TRACE_1_INFO("GEA4=%d", (gmm_data->config.cipher_on & 0x08)>0);
              TRACE_1_INFO("GEA5=%d", (gmm_data->config.cipher_on & 0x10)>0);
              TRACE_1_INFO("GEA6=%d", (gmm_data->config.cipher_on & 0x20)>0);
              TRACE_1_INFO("GEA7=%d", (gmm_data->config.cipher_on & 0x40)>0);
            }
            else
            {
              TRACE_ERROR("[PEI_CONFIG]: use CONFIG GEA=<0-255>//255 means all GEA wanted");
            }
            break;
#ifndef _TARGET_
          case GMM_CONFIG_CHECK_STATE:
            if(1==valno)
            {
              if (gmm_tok_key ((KW_STATE_DATA *)kw_state_tab, val[0])!=GET_STATE(KERN))
              {
                PALLOC ( cgrlc_status_ind, CGRLC_STATUS_IND ); /* TCS 2.1 */
                PSEND ( hCommGMM, cgrlc_status_ind ); /* TCS 2.1 */
                TRACE_1_OUT_PARA(   "neq %s", val[0]);
                TRACE_ERROR("state wrong");
              }
              else
              {
                TRACE_1_INFO("state %s ok",val[0]);
              }
            }
            else
            {
              TRACE_ERROR("[PEI_CONFIG]: USE CONFIG CHECK_STATE <state>");
            }
            break;
#endif            
          default:
            break;
        }
      }
    }
#endif
    return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_monitor
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame in case sudden entity
|               specific data is requested (e.g. entity Version).
|
|  Parameters  :  out_monitor       - return the address of the data to be
|                                   monitoredCommunication handle
|
|  Return      :  PEI_OK            - sucessful (address in out_monitor is valid)
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  TRACE_FUNCTION ("pei_monitor");

  /*
   * Version = "0.S" (S = Step).
   */
  gmm_mon.version = "GMM 0.1";
  *out_monitor = &gmm_mon;

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_create
+------------------------------------------------------------------------------
|  Description  :  This function is called by the frame when the process is
|                created.
|
|  Parameters  :  out_name          - Pointer to the buffer in which to locate
|                                   the name of this entity
|
|  Return      :  PEI_OK            - entity created successfuly
|               PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{
static T_PEI_INFO pei_info =
              {
               "GMM",         /* name */
               {              /* pei-table */
                 pei_init,
                 pei_exit,
                 pei_primitive,
                 pei_timeout,
                 pei_signal,
                 pei_run,
                 pei_config,
                 pei_monitor
               },
               2048,          /* stack size */
               25,            /* queue entries */
               190,           /* priority (1->low, 255->high) */
               TIMER_MAX,     /* number of timers */
               0x03|PRIM_NO_SUSPEND /* flags: bit 0   active(0) body/passive(1) */
              };              /*        bit 1   com by copy(0)/reference(1) */


  TRACE_FUNCTION ("pei_create");

  /*
   * Close Resources if open
   */
  if (first_access)
    first_access = FALSE;
  else
    pei_exit();

  /*
   * Export startup configuration data
   */
  *info = &pei_info;

  return (PEI_OK);
}

/*
+------------------------------------------------------------------------------
| Function    : gmm_pei_handle_queue
+------------------------------------------------------------------------------
| Description : This function searches for the first stored primitive, which
|               must not be saved further in current state.
|
| Parameters  : state     - current state of the instance
|               queue     - PtrPtr to the hanger of the instance save queue
|               table     - Ptr to the state/signals-to-save table
|               inst_data - Ptr to instance data
+------------------------------------------------------------------------------
*/
LOCAL void gmm_pei_handle_queue( T_SAVE_QUEUE** queue,
                                      const T_SAVE_TAB* table)
{
  /* search for a T_SAVE_TAB entry which fits to current state */
  if( *queue )
    while( table->state != PEI_END_SAVETAB && table->state != gmm_data->kern.state )
      table++;

  while( *queue )
  {
    T_SAVE_QUEUE* current = *queue;
    ULONG  opc            = ((T_PRIM_HEADER*)current->prim)->opc;
    USHORT n              = 0;

    if( table->state != PEI_END_SAVETAB )
    {
      /* search for this signals in savelist of this state */
      while( n < GMM_MAX_SIGNALS_SAVED && table->signal[n] &&
             table->signal[n] != opc                       )
        n++;
    }

    if( table->signal[n] != opc )
    {
      TRACE_1_INFO ("Pimitive 0x%x send from SAVE queue",
                    ((T_PRIM_HEADER*)current->prim)->opc );
      /* this signal must not further be stored -> handle it now */
      /* first remove it from save queue, because of recursion!! */
      *queue = current->next;
      gmm_pei_handle_prim(current->func, current->prim );
      /* and free memory after last use */
      MFREE( current );

      /* recursion! handle at maximum one prim at once -> job done */
      return;
    }

    /* goto next queued signal */
    queue = &((*queue)->next);
  }
}
/*
+------------------------------------------------------------------------------
| Function    : gmm_pei_delete_queue
+------------------------------------------------------------------------------
| Description : This function removes all stored primitives from the queue and
|               frees the memory.
|
| Parameters  : queue    - PtrPtr to the hanger of the instance save queue
+------------------------------------------------------------------------------
*/
GLOBAL void gmm_pei_delete_queue( T_SAVE_QUEUE** queue )
{
  while( *queue )
  {
    T_SAVE_QUEUE* current = *queue;


    TRACE_1_INFO( "Pimitive 0x%x deleted from SAVE queue",
                                         ((T_PRIM_HEADER*)current->prim)->opc );

    *queue = current->next;
    PFREE( P2D(current->prim) );
    MFREE( current );
  }
}
/*
+------------------------------------------------------------------------------
| Function    : gmm_pei_handle_prim
+------------------------------------------------------------------------------
| Description : This function calls the handle function of the signal
|
| Parameters  : func       - Function, which should handle the signal
|               inst_data  - Ptr to instance data
|               *prim      - Ptr to primitive
+------------------------------------------------------------------------------
*/
LOCAL void gmm_pei_handle_prim( T_VOID_FUNC func, void* prim )
{
  JUMP(func)( P2D(prim));

  gmm_pei_handle_queue(   &save_queue, save_tab);
}
/*
+------------------------------------------------------------------------------
| Function    : gmm_pei_handle_save
+------------------------------------------------------------------------------
| Description : This function saves the prim pointer to the save queue, if
|               this is necessary in current state.
|
| Parameters  : *prim    - Ptr to primitive
|               state    - Current state of the instance
|               queue    - PtrPtr to the hanger of the instance save queue
|               table    - Ptr to the state/signals-to-save table
|               func     - Function, which should handle the signal
|
| Returns     : PEI_SIGNAL_SAVED      - if signal is stored to save queue
|               PEI_SIGNAL_NOT_SAVED  - else
+------------------------------------------------------------------------------
*/
LOCAL BOOL gmm_pei_handle_save( void* prim, T_SAVE_QUEUE** queue,
                                     const T_SAVE_TAB* table, T_VOID_FUNC func  )
{
  ULONG opc = ((T_PRIM_HEADER*)prim)->opc;

  while( table->state != PEI_END_SAVETAB )
  {
    if( table->state == gmm_data->kern.state )
    {
      UBYTE n = 0;

      /* check for signals to save */
      while( n < GMM_MAX_SIGNALS_SAVED && table->signal[n] )
      {
        if( table->signal[n] == opc )
        {
          /* store signal to end of save queue */
          while( *queue )
            queue = &((*queue)->next);

          MALLOC( *queue, sizeof(T_SAVE_QUEUE) );

          if( *queue )
          {
            (*queue)->prim = prim;
            (*queue)->func = func;
            (*queue)->next = NULL;
          }
          else
          {
            TRACE_ERROR( "Out of memory in mm_pei_handle_save()" );
            return PEI_SIGNAL_NOT_SAVED;
          }

          return PEI_SIGNAL_SAVED;
        }

        /* goto next signal */
        n++;
      }

      break;
    }

    /* gote next entry */
    table++;
  }

  return PEI_SIGNAL_NOT_SAVED;
}

/*==== END OF FILE ==========================================================*/

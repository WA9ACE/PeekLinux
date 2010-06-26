/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRR
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
|             for the entity GPRS Radio Resource Management (GRR)
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
|+----------------------------------------------------------------------------- 
*/ 

#define GRR_PEI_C

#define ENTITY_GRR

/*==== INCLUDES =============================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>     /* to get definition of offsetof(), for MAK_FUNC_S */
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"

#include "custom.h" /*FMM*/ /*set FF_EM_MODE compile switch for SIMULATION and define WAP_NAME*/

#include "gsm.h"        /* to get a lot of macros */
#include "ccdapi.h"     /* to get CCD API */
#include "cnf_grr.h"    /* to get cnf-definitions */
#include "mon_grr.h"    /* to get mon-definitions */

#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "tok.h"
#include "pcm.h"

#include "grr.h"        /* to get the global entity definitions */
#include "grr_f.h"
#include "grr_gfpp.h"
#include "grr_measp.h"
#include "grr_csp.h"
#include "grr_pgp.h"
#include "grr_psip.h"
#include "grr_tcp.h"

#ifdef _SIMULATION_
  #include "grr_meass.h"  /* to get definition of meas_init() */
#endif /* #ifdef _SIMULATION_ */

#include "grr_cpapp.h"
#include "grr_css.h"
#include "grr_ctrlp.h"
#include "grr_em.h"     /*for Engineering mode*/



/*==== DEFINITIONS ==========================================================*/

#define GRR_TSTR_ILLEGAL_KEYW 0
#define GRR_TSTR_OK           1

/*==== TYPES ================================================================*/

/*==== GLOBAL VARS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

/*
 * Function is needed for grr_table[].
 */

LOCAL SHORT pei_signal (ULONG opc, void *data);

static  BOOL          first_access  = TRUE;
static  T_MONITOR     grr_mon;


/*
 * Jumptables to primitive handler functions. One table per SAP.
 *
 * Use MAK_FUNC_0 for primitives which contains no SDU.
 * Use MAK_FUNC_S for primitives which contains a SDU.
 */


/*
 * Function is needed for grr_table[]. This declaration can be removed
 * as soon as this function is no more called (i.e. all primitives are
 * handled).
 */
LOCAL void primitive_not_supported (void *data);



static const T_FUNC gmmrr_table[] =
{
  MAK_FUNC_0(ctrl_gmmrr_assign_req,     GMMRR_ASSIGN_REQ),
  MAK_FUNC_0(ctrl_gmmrr_enable_req,     GMMRR_ENABLE_REQ),
  MAK_FUNC_0(ctrl_gmmrr_disable_req,    GMMRR_DISABLE_REQ),
  MAK_FUNC_0(ctrl_gmmrr_ready_req,      GMMRR_READY_REQ),
  MAK_FUNC_0(ctrl_gmmrr_standby_req,    GMMRR_STANDBY_REQ),
  MAK_FUNC_0(ctrl_gmmrr_cs_page_res,    GMMRR_CS_PAGE_RES),
  MAK_FUNC_0(ctrl_gmmrr_suspend_req,    GMMRR_SUSPEND_REQ),
  MAK_FUNC_0(ctrl_gmmrr_resume_req,     GMMRR_RESUME_REQ),
  MAK_FUNC_N(primitive_not_supported,   0),
  MAK_FUNC_N(primitive_not_supported,   0),
  MAK_FUNC_0(ctrl_gmmrr_attach_started_req,  GMMRR_ATTACH_STARTED_REQ),
  MAK_FUNC_0(ctrl_gmmrr_attach_finished_req, GMMRR_ATTACH_FINISHED_REQ),
  MAK_FUNC_0(ctrl_gmmrr_cell_res,             GMMRR_CELL_RES)
};


static const T_FUNC cgrlc_table[] =
{
  MAK_FUNC_0(tc_cgrlc_tbf_rel_ind,         CGRLC_TBF_REL_IND),
  MAK_FUNC_0(tc_cgrlc_ul_tbf_ind,          CGRLC_UL_TBF_IND),
  MAK_FUNC_0(gfp_cgrlc_data_ind,           CGRLC_DATA_IND),
  MAK_FUNC_0(tc_cgrlc_ctrl_msg_sent_ind,   CGRLC_CTRL_MSG_SENT_IND),
  MAK_FUNC_0(tc_cgrlc_starting_time_ind,   CGRLC_STARTING_TIME_IND),
  MAK_FUNC_0(tc_cgrlc_t3192_started_ind,   CGRLC_T3192_STARTED_IND),
  MAK_FUNC_0(tc_cgrlc_cont_res_done_ind,   CGRLC_CONT_RES_DONE_IND),
  MAK_FUNC_0(tc_cgrlc_ta_value_ind,        CGRLC_TA_VALUE_IND),
  MAK_FUNC_N(primitive_not_supported,   0),
  MAK_FUNC_N(primitive_not_supported,   0),
  MAK_FUNC_N(primitive_not_supported,   0),
  MAK_FUNC_0(ctrl_cgrlc_standby_state_ind, CGRLC_STANDBY_STATE_IND),
  MAK_FUNC_0(ctrl_cgrlc_ready_state_ind,   CGRLC_READY_STATE_IND),
  MAK_FUNC_0(grr_cgrlc_pwr_ctrl_cnf,       CGRLC_PWR_CTRL_CNF),
  MAK_FUNC_0(tc_cgrlc_test_mode_ind,       CGRLC_TEST_MODE_IND)
};


/* according to l1p_sign.h */
static const T_FUNC mphp_table[] =
{
  MAK_FUNC_N(primitive_not_supported,             0),                                /*0*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*1*/
  MAK_FUNC_0(gfp_mphp_single_block_con,           MPHP_SINGLE_BLOCK_CON),            /*2*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*3*/
  MAK_FUNC_0(gfp_mphp_stop_single_block_con,      MPHP_STOP_SINGLE_BLOCK_CON),       /*4*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*5*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*6*/
  MAK_FUNC_0(gfp_mphp_assignment_con,             MPHP_ASSIGNMENT_CON),              /*7*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*8*/
  MAK_FUNC_0(gfp_mphp_tbf_release_con,            MPHP_TBF_RELEASE_CON),             /*9*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*10*/
  MAK_FUNC_0(gfp_mphp_repeat_ul_fixed_alloc_con,  MPHP_REPEAT_UL_FIXED_ALLOC_CON),   /*11*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*12*/
  MAK_FUNC_0(gfp_mphp_pdch_release_con,           MPHP_PDCH_RELEASE_CON),            /*13*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*14*/
  MAK_FUNC_0(gfp_mphp_timing_advance_con,         MPHP_TIMING_ADVANCE_CON),          /*15*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*16*/
  MAK_FUNC_0(gfp_mphp_update_psi_param_con,       MPHP_UPDATE_PSI_PARAM_CON),        /*17*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*18*/
  MAK_FUNC_0(gfp_mphp_ra_con,                     MPHP_RA_CON),                      /*19*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*20*/
  MAK_FUNC_0(gfp_mphp_ra_stop_con,                MPHP_RA_STOP_CON),                 /*21*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*22*/
  MAK_FUNC_0(gfp_mphp_polling_ind,                MPHP_POLLING_IND),                 /*23*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*24*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*25*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*26*/
  MAK_FUNC_0(gfp_mphp_stop_pccch_con,             MPHP_STOP_PCCCH_CON),              /*27*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*28*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*29*/
  MAK_FUNC_0(gfp_mphp_scell_pbcch_stop_con,       MPHP_SCELL_PBCCH_STOP_CON),        /*30*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*31*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*32*/
  MAK_FUNC_0(gfp_mphp_cr_meas_stop_con,           MPHP_CR_MEAS_STOP_CON),            /*33*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*34*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*35*/
  MAK_FUNC_0(gfp_mphp_int_meas_stop_con,          MPHP_INT_MEAS_STOP_CON),           /*36*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*37*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*38*/
  MAK_FUNC_0(gfp_mphp_ncell_pbcch_stop_con,       MPHP_NCELL_PBCCH_STOP_CON),        /*39*/
  MAK_FUNC_0(gfp_mphp_data_ind,                   MPHP_DATA_IND),                    /*40*/
  MAK_FUNC_0(gfp_mphp_cr_meas_ind,                MPHP_CR_MEAS_IND),                 /*41*/
  MAK_FUNC_0(gfp_mphp_int_meas_ind,               MPHP_INT_MEAS_IND),                /*42*/
  MAK_FUNC_0(gfp_mphp_tint_meas_ind,              MPHP_TINT_MEAS_IND),               /*43*/
  MAK_FUNC_0(gfp_mphp_ncell_pbcch_ind,            MPHP_NCELL_PBCCH_IND),             /*44*/
  MAK_FUNC_N(primitive_not_supported,             0),                                /*45*/
  MAK_FUNC_0(gfp_mphp_tcr_meas_ind,               MPHP_TCR_MEAS_IND)                 /*46*/
};



static const T_FUNC rrgrr_table[] =
{
  MAK_FUNC_0(ctrl_rrgrr_gprs_si13_ind,                RRGRR_GPRS_SI13_IND),               /* 0 */
  MAK_FUNC_N(primitive_not_supported,                 0),                                 /* 1:  NOT USED */
  MAK_FUNC_0(ctrl_rrgrr_packet_paging_ind,            RRGRR_PACKET_PAGING_IND),           /* 2 */
  MAK_FUNC_0(ctrl_rrgrr_ia_ind,                       RRGRR_IA_IND),                      /* 3 */
  MAK_FUNC_0(ctrl_rrgrr_iaext_ind,                    RRGRR_IAEXT_IND),                   /* 4 */
  MAK_FUNC_0(ctrl_rrgrr_assignment_rej_ind,           RRGRR_ASSIGNMENT_REJ_IND),          /* 5 */
  MAK_FUNC_0(ctrl_rrgrr_data_ind,                     RRGRR_DATA_IND),                    /* 6 */
  MAK_FUNC_N(primitive_not_supported,                 RRGRR_RESUMED_TBF_CNF),             /* 7 */
  MAK_FUNC_0(ctrl_rrgrr_ia_downlink_ind,              RRGRR_IA_DOWNLINK_IND),             /* 8 */
  MAK_FUNC_0(ctrl_rrgrr_stop_task_cnf,                RRGRR_STOP_TASK_CNF),               /* 9 */
  MAK_FUNC_0(ctrl_rrgrr_ext_meas_cnf,                 RRGRR_EXT_MEAS_CNF),                /* A */
  MAK_FUNC_N(primitive_not_supported,                 0),                                 /* B */
  MAK_FUNC_N(primitive_not_supported,                 0),                                 /* C */
  MAK_FUNC_0(ctrl_rrgrr_rr_est_ind,                   RRGRR_RR_EST_IND),                  /* D */
  MAK_FUNC_N(primitive_not_supported,                 0),                                 /* E */
  MAK_FUNC_N(primitive_not_supported,                 0),                                 /* F:  NOT USED */
  MAK_FUNC_0(ctrl_rrgrr_suspend_dcch_cnf,             RRGRR_SUSPEND_DCCH_CNF),            /* 10 */
  MAK_FUNC_0(ctrl_rrgrr_reconnect_dcch_cnf,           RRGRR_RECONNECT_DCCH_CNF),          /* 11 */
  MAK_FUNC_0(ctrl_rrgrr_stop_dcch_ind,                RRGRR_STOP_DCCH_IND),               /* 12 */
  MAK_FUNC_0(ctrl_rrgrr_cr_ind,                       RRGRR_CR_IND),                      /* 13 */
  MAK_FUNC_N(primitive_not_supported,                 0),                                 /* 14: NOT USED */
  MAK_FUNC_0(ctrl_rrgrr_check_bsic_ind,               RRGRR_NCELL_SYNC_IND),              /* 15 */
  MAK_FUNC_0(ctrl_rrgrr_sync_ind,                     RRGRR_SYNC_IND),                    /* 16 */
  MAK_FUNC_0(ctrl_rrgrr_meas_rep_cnf,                 RRGRR_MEAS_REP_CNF),                /* 17 */
  MAK_FUNC_N(primitive_not_supported,                 0),                                 /* 18: NOT USED */
  MAK_FUNC_0(ctrl_rrgrr_ms_id_ind,                    RRGRR_MS_ID_IND),                   /* 19 */
  MAK_FUNC_0(primitive_not_supported,                 RRGRR_START_TASK_CNF)               /* 1A */

#if defined (REL99) AND defined (TI_PS_FF_EMR)
  , 
  MAK_FUNC_0(ctrl_rrgrr_si2quater_ind,                RRGRR_SI2QUATER_IND)                /* 1B */
#endif

};

static const T_FUNC tb_table[] =
{
  MAK_FUNC_0(meas_tb_rxlev_sc_req,                    TB_RXLEV_SC_REQ)    /*0*/
};

#ifdef FF_EM_MODE
static const T_FUNC em_ul_table[] =
{
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x00 */
  MAK_FUNC_0(grr_em_sc_gprs_info_req,       EM_SC_GPRS_INFO_REQ    ), /* 0x01 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x02 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x03 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x04 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x05 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x06 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x07 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x08 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x09 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x0A */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x0B */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x0C */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x0D */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x0E */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x0F */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x10 */
  MAK_FUNC_0(grr_em_pco_trace_req,          EM_PCO_TRACE_REQ       ), /* 0x11*/ /*PCO output*/
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x12 */
  MAK_FUNC_0(grr_em_fmm_sc_gprs_info_req,   EM_FMM_SC_GPRS_INFO_REQ), /* 0x13 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x14 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x15 */
  MAK_FUNC_0(grr_em_fmm_tbf_info_req,       EM_FMM_TBF_INFO_REQ    )  /* 0x16 */
};
#endif /* FF_EM_MODE */  




/*==== END DIAGNOSTICS ======================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : primitive_not_supported
+------------------------------------------------------------------------------
| Description :  This function handles unsupported primitives.
|
| Parameters  : -
|
| Return      : -
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
| Function    : pei_primitive
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when a primitive is
|                received and needs to be processed.
|
|                          |           |
|                         GRR         GMMRR               UPLINK
|                          |           |
|                   +------v-----------v-------+
|                   |                          |
|                   |            GRR           <--RRGRR--
|                   |                          |
|                   +--------------^------- ---+
|                                  |
|                                 L1                     DOWNLINK
|                                  |
|
|
| Parameters  : prim      - Pointer to the received primitive
|
| Return      : PEI_OK    - function succeeded
|               PEI_ERROR - function failed
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_primitive (void * primptr)
{
  TRACE_FUNCTION ("pei_primitive");

  if (primptr NEQ NULL)
  {
    T_PRIM *prim	= (T_PRIM *)primptr;
    ULONG            opc = prim->custom.opc;
    USHORT           n;
    const T_FUNC    *table;

    PTRACE_IN (opc);

    /*
     * This must be called for Partition Pool supervision. Will be replaced
     * by another macro some time.
     */
    VSI_PPM_REC (&prim->custom, __FILE__, __LINE__);

    switch (SAP_NR(opc))
    {
      case MPHP_DL:
        table = mphp_table;
        n = TAB_SIZE (mphp_table);
        break;
      case TB_DL:
        table = tb_table;
        n = TAB_SIZE (tb_table);
        break;
      case SAP_NR(CGRLC_DL):
        table = cgrlc_table;
        n = TAB_SIZE (cgrlc_table);
        break;
      case GMMRR_UL:
        table = gmmrr_table;
        n = TAB_SIZE (gmmrr_table);
        break;
      case RRGRR_DL:
        table = rrgrr_table;
        n = TAB_SIZE (rrgrr_table);
        break;
#ifdef FF_EM_MODE
      case EM_Ul:
        table = em_ul_table;
        n = TAB_SIZE (em_ul_table);
        break;
#endif /* FF_EM_MODE */
      default:
        table = NULL;
        n = 0;
        break;
    }

    if (table != NULL)
    {
      if ((PRIM_NR(opc)) < n)
      {
        table += PRIM_NR(opc);
#ifdef PALLOC_TRANSITION
        P_SDU(prim) = table->soff ? (T_sdu*) (((char*)&prim->data) + table->soff) : 0;
#ifndef NO_COPY_ROUTING
        P_LEN(prim) = table->size + sizeof (T_PRIM_HEADER);
#endif /* NO_COPY_ROUTING */
#endif /* PALLOC_TRANSITION */
        JUMP (table->func) (P2D(prim));
      }
      else
      {
        primitive_not_supported (P2D(prim));
      }
      return PEI_OK;
    }
  }
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : pei_init
+------------------------------------------------------------------------------
| Description : This function is called by the frame. It is used to initialise
|               the entitiy.
|
| Parameters  : handle            - task handle
|
| Return      : PEI_OK            - entity initialised
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
  GRR_handle = handle;


  /*
   * Open communication channels
   */
  if (hCommGRR < VSI_OK)
  {
    if ((hCommGRR = vsi_c_open (VSI_CALLER GRR_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommGMM < VSI_OK)
  {
    if ((hCommGMM = vsi_c_open (VSI_CALLER GMM_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommGRLC < VSI_OK)
  {
    if ((hCommGRLC = vsi_c_open (VSI_CALLER GRLC_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommRR < VSI_OK)
  {
    if ((hCommRR = vsi_c_open (VSI_CALLER RR_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  if (hCommL1 < VSI_OK)
  {
    if ((hCommL1 = vsi_c_open (VSI_CALLER L1_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

  if (hCommPL < VSI_OK)
  {
    if ((hCommPL = vsi_c_open (VSI_CALLER PL_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#ifdef FF_WAP /*FFM*/
    if (hCommWAP < VSI_OK)
  {
    if ((hCommWAP = vsi_c_open (VSI_CALLER WAP_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#endif
  /*
   * call function pcm_Init to be sure that the global variable std
   * is set before the initialisation of the GRR services takes place
   */
  pcm_Init();

  /*
   * Initialize entity data (call init function of every service)
   */
  grr_init();

/*  {
    PALLOC (tst_test_hw_req, TST_TEST_HW_REQ);
    PSEND (hCommL1, tst_test_hw_req);
  }*/
  return (PEI_OK);
}

/*
+------------------------------------------------------------------------------
| Function    : pei_timeout
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a timer has expired.
|
| Parameters  : index             - timer index
|
| Return      : PEI_OK            - timeout processed
|               PEI_ERROR         - timeout not processed
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_timeout (USHORT index)
{
  TRACE_FUNCTION ("pei_timeout");

  /*
   * Process timeout
   */
  switch (index)  /* SZML-GLBL/007 */
  {
    case T3134:           ctrl_t3134();                                      break;
    case T3158:           cs_t3158();                                        break;
    case T3162:           tc_t3162();                                        break;
    case T3168:           tc_t3168();                                        break;
    case T3170:           tc_t3170();                                        break;
    case T3172_1:         tc_t3172(T3172_1);                                 break;
/*    case T3172_2:         tc_t3172(T3172_2);                                 break; */
/*    case T3172_3:         tc_t3172(T3172_3);                                 break; */
/*    case T3172_4:         tc_t3172(T3172_4);                                 break; */
/*    case T3172_5:         tc_t3172(T3172_5);                                 break; */
/*    case T3172_6:         tc_t3172(T3172_6);                                 break; */
/*    case T3172_7:         tc_t3172(T3172_7);                                 break; */
    case T3174:           ctrl_t3174();                                      break;
    case T3176:           ctrl_t3176();                                      break;
    case T3178:           meas_t3178();                                      break;
    case T3186:           tc_t3186();                                        break;
    case T_60_SEC:        psi_t_60_sec();                                    break;
    case T_30_SEC:        psi_t_30_sec();                                    break;
    case T_10_SEC:        psi_t_10_sec();                                    break;
    case T_IM_SYNC:       meas_t_im_sync();                                  break;
    case T15_SEC_CC:      /* no action required when timer expires!! */      break;
    case T_TRANS_NON_DRX: grr_handle_non_drx_period( TRANSFER_NDRX, FALSE ); break;
    case T_NC2_NON_DRX:   grr_handle_non_drx_period( NC2_NDRX, FALSE );      break;
    case T_COMP_PSI:      psi_complete_psi_read_failed();                    break;
    case T_RESELECT:      cs_t_reselect();                                   break;
#ifdef REL99
    case T_POLL_TIMER:    ctrl_t_poll_timer();                               break;
#endif
    default:              TRACE_EVENT_P1( "Unknown Timeout: %d", index );    break;
  }

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : pei_signal
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a signal has been
|               received.
|
| Parameters  : opc               - signal operation code
|               *data             - pointer to primitive
|
| Return      : PEI_OK            - signal processed
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
  
  TRACE_EVENT_P1( "Unknown Signal: %08X", opc );



  return(PEI_OK);
}

/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - exit sucessful
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
  vsi_c_close (VSI_CALLER hCommGRR);
  hCommGRR = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommGMM);
  hCommGMM = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommRR);
  hCommRR = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommGRLC);
  hCommGRLC = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommL1);
  hCommL1 = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommPL);
  hCommPL = VSI_ERROR;

  ccd_exit();

#ifdef FF_WAP /*FFM*/
  vsi_c_close (VSI_CALLER hCommWAP);
  hCommWAP = VSI_ERROR;
#endif

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
| Parameters  : handle            - Communication handle
|
| Return      : PEI_OK            - sucessful
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
| Function    : pei_config
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a primitive is
|               received indicating dynamic configuration.
|
|               This function is not used in this entity.
|
| Parameters  : handle            - Communication handle
|
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
#if !defined (NCONFIG)
LOCAL const KW_DATA kwtab[] =
                        {
                          GRR_STR_IM_INIT,     GRR_NUM_IM_INIT,
                          GRR_STR_TBF_CCCH,    GRR_NUM_TBF_CCCH,
                          GRR_STR_NO_TBF_CCCH, GRR_NUM_NO_TBF_CCCH,
#if !defined (NTRACE)
                          GRR_STR_CRP_TRACE,   GRR_NUM_CRP_TRACE,
                          GRR_STR_IM_TRACE,    GRR_NUM_IM_TRACE,
#endif /* #if !defined (NTRACE) */
#ifdef _SIMULATION_
                          GRR_STR_RES_RANDOM,  GRR_NUM_RES_RANDOM,
                          GRR_STR_STD,         GRR_NUM_STD,
                          GRR_STR_NC2,         GRR_NUM_NC2,

#endif /* #ifdef _SIMULATION_ */


                          "",                  0
                        };
#endif /* #if !defined (NCONFIG) */


LOCAL SHORT pei_config (char *inString)
{
#if !defined (NCONFIG)
  char    * s = inString;
  char    * keyw;
  char    * val [10];

  TRACE_FUNCTION( "pei_config" );

  TRACE_EVENT_P1( "[PEI_CONFIG]: %s", inString );

  tok_init(s);

  /*
   * Parse next keyword and number of variables
   */
  while ((tok_next(&keyw,val)) != TOK_EOCS)
  {
    UBYTE ncomment = GRR_TSTR_OK;
    switch ((tok_key((KW_DATA *)kwtab,keyw)))
    {
      case GRR_NUM_IM_INIT:
        meas_init( ( UBYTE )atoi( val[0] ) );
        break;

      case GRR_NUM_TBF_CCCH:
        grr_data->ms.tbf_mon_ccch = TRUE;  /* Allowed */
        break;

      case GRR_NUM_NO_TBF_CCCH:
        grr_data->ms.tbf_mon_ccch = FALSE; /* Not allowed */
        break;

#if !defined (NTRACE)

      case GRR_NUM_CRP_TRACE:
        grr_data->cs.v_crp_trace  = atoi( val[0] );
        break;

      case GRR_NUM_IM_TRACE:
        grr_data->meas_im.n_im_trace = atoi( val[0] );
        break;

#endif /* #if !defined (NTRACE) */

#ifdef _SIMULATION_
      case GRR_NUM_RES_RANDOM:
        grr_data->tc.res_random = atoi( val[0] );
        TRACE_EVENT_P1("grr_data->tc.res_random is %d",atoi( val[0]));
        break;
      case GRR_NUM_STD:
        std = atoi( val[0] );
#ifdef TI_PS_FF_QUAD_BAND_SUPPORT
        if ( std EQ 33)
        {
          pcm_Init ();
          {
            rr_csf_check_rfcap (TRUE);
          }
        }
#endif
        TRACE_EVENT_P1("std is %d",atoi( val[0]));
        break;
      case GRR_NUM_NC2:
        grr_data->nc2_on = atoi( val[0] );
        break;

#endif

      default:
        ncomment = GRR_TSTR_ILLEGAL_KEYW;
        break;
    }

    TRACE_EVENT_P2( "[PEI_CONFIG]: %s -> %s", 
                    keyw,
                    ncomment EQ GRR_TSTR_OK ? "OK" : "Illegal Keyword" );
  }

#endif /* #if !defined (NCONFIG) */

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : pei_config
+------------------------------------------------------------------------------
| Description : This function is called by the frame in case sudden entity
|               specific data is requested (e.g. entity Version).
|
| Parameters  : out_monitor       - return the address of the data to be
|                                   monitoredCommunication handle
|
| Return      : PEI_OK            - sucessful (address in out_monitor is valid)
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  TRACE_FUNCTION ("pei_monitor");

  grr_mon.version = "GRR 1.0";
  *out_monitor = &grr_mon;

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : pei_create
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when the process is
|                created.
|
| Parameters  : out_name          - Pointer to the buffer in which to locate
|                                   the name of this entity
|
| Return      : PEI_OK            - entity created successfuly
|               PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{

static T_PEI_INFO pei_info =
              {
               "GRR",         /* name */
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
               2560,          /* stack size increased for omaps00149330 */
               PEI_PRIM_QUEUE_SIZE,
                              /* queue entries */
               204,           /* priority (1->low, 255->high) */
               TIMER_COUNT, 
                              /* number of timers */
#ifdef _TARGET_
               PASSIVE_BODY|COPY_BY_REF|TRC_NO_SUSPEND|INT_DATA_TASK|PRIM_NO_SUSPEND
#else
               PASSIVE_BODY|COPY_BY_REF
#endif
              };


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



  return PEI_OK;
}

/*==== END OF FILE ==========================================================*/

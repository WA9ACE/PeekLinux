/* 
+----------------------------------------------------------------------------- 
|  Project :  GPRS (8441)
|  Modul   :  GRLC
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
|             for the entity GRLC
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

#define GRLC_PEI_C

#define ENTITY_GRLC

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
#include "cnf_grlc.h"    /* to get cnf-definitions */
#include "mon_grlc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "message.h"
#include "tok.h"
#include "pcm.h"
#include "grlc.h"        /* to get the global entity definitions */
#include "grlc_f.h"
#include "grlc_gffp.h"
#include "grlc_tmp.h"
#include "grlc_rup.h"
#include "grlc_rdp.h"
#include "grlc_tpcs.h"
#include "grlc_measp.h"

#include "grlc_em.h"

/*==== DEFINITIONS ==========================================================*/

#define GRLC_TSTR_ILLEGAL_KEYW 0
#define GRLC_TSTR_OK           1

/*==== TYPES ================================================================*/

/*==== GLOBAL VARS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/


/*
 * Function is needed for grlc_table[].
 */

LOCAL SHORT pei_signal (ULONG opc, void *data);

static  BOOL          first_access  = TRUE;
static  T_MONITOR     grlc_mon;


/*
 * Jumptables to primitive handler functions. One table per SAP.
 *
 * Use MAK_FUNC_0 for primitives which contains no SDU.
 * Use MAK_FUNC_S for primitives which contains a SDU.
 */


/*
 * Function is needed for grlc_table[]. This declaration can be removed
 * as soon as this function is no more called (i.e. all primitives are
 * handled).
 */
LOCAL void primitive_not_supported (void *data);

static const T_FUNC grlc_table[] =
{
  MAK_FUNC_S(tm_grlc_data_req,                    GRLC_DATA_REQ),
  MAK_FUNC_S(tm_grlc_unitdata_req,                GRLC_UNITDATA_REQ),
  MAK_FUNC_0(tm_grlc_activate_gmm_queue_req,      GRLC_ACTIVATE_GMM_QUEUE_REQ),
  MAK_FUNC_0(tm_grlc_flush_data_req,              GRLC_FLUSH_DATA_REQ)
};

static const T_FUNC cgrlc_table[] =
{
  MAK_FUNC_0(tm_cgrlc_enable_req,                 CGRLC_ENABLE_REQ),             /* 0x00 */
  MAK_FUNC_0(tm_cgrlc_disable_req,                CGRLC_DISABLE_REQ),            /* 0x01 */
  MAK_FUNC_0(tm_cgrlc_ul_tbf_res,                 CGRLC_UL_TBF_RES),             /* 0x02 */
  MAK_FUNC_0(tm_cgrlc_dl_tbf_req,                 CGRLC_DL_TBF_REQ),             /* 0x03 */
  MAK_FUNC_0(tm_cgrlc_tbf_rel_req,                CGRLC_TBF_REL_REQ),            /* 0x04 */
  MAK_FUNC_0(tm_cgrlc_tbf_rel_res,                CGRLC_TBF_REL_RES),            /* 0x05 */
  MAK_FUNC_0(tm_cgrlc_data_req,                   CGRLC_DATA_REQ),               /* 0x06 */
  MAK_FUNC_0(tm_cgrlc_poll_req,                   CGRLC_POLL_REQ),               /* 0x07 */
  MAK_FUNC_0(tm_cgrlc_access_status_req,          CGRLC_ACCESS_STATUS_REQ),      /* 0x08 */
  MAK_FUNC_0(tm_cgrlc_test_mode_req,              CGRLC_TEST_MODE_REQ),          /* 0x09 */
  MAK_FUNC_0(tm_cgrlc_test_end_req,               CGRLC_TEST_END_REQ),           /* 0x0A */
  MAK_FUNC_0(tm_cgrlc_ta_value_req,               CGRLC_TA_VALUE_REQ),           /* 0x0B */
  MAK_FUNC_0(meas_int_level_req,                  CGRLC_INT_LEVEL_REQ),          /* 0x0C */
  MAK_FUNC_0(tm_cgrlc_pwr_ctrl_req,               CGRLC_PWR_CTRL_REQ),           /* 0x0D */
  MAK_FUNC_0(tm_cgrlc_ready_timer_config_req,     CGRLC_READY_TIMER_CONFIG_REQ), /* 0x0E */
  MAK_FUNC_0(tm_cgrlc_force_to_standby_req,       CGRLC_FORCE_TO_STANDBY_REQ)    /* 0x0F */
};


#ifdef _SIMULATION_

static const T_FUNC mac_table[] =
{
  MAK_FUNC_0(gff_mac_data_ind,                    MAC_DATA_IND),
  MAK_FUNC_0(gff_mac_ready_ind,                   MAC_READY_IND),
  MAK_FUNC_0(gff_mac_pwr_ctrl_ind,                MAC_PWR_CTRL_IND)
};

static const T_FUNC l1test_table[] =
{
  MAK_FUNC_N(primitive_not_supported,             0),
  MAK_FUNC_N(primitive_not_supported,             0),
  MAK_FUNC_N(primitive_not_supported,             0),
  MAK_FUNC_N(primitive_not_supported,             0),
  MAK_FUNC_0(gff_l1test_call_mphp_power_control,  L1TEST_CALL_MPHP_POWER_CONTROL),
  MAK_FUNC_N(primitive_not_supported,             0),
  MAK_FUNC_N(primitive_not_supported,             0),
};

#endif /* #ifdef _SIMULATION_ */


#ifdef FF_EM_MODE
static const T_FUNC em_ul_table[] =
{
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x00 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x01 */
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
  MAK_FUNC_0(grlc_em_pco_trace_req,         EM_PCO_TRACE_REQ       ), /* 0x11*/ /*PCO output*/
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x12 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x13 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x14 */
  MAK_FUNC_0(grlc_em_fmm_rlc_trans_info_req, EM_FMM_RLC_TRANS_INFO_REQ),/* 0x15 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x16 */
  MAK_FUNC_N(primitive_not_supported,       0                      ), /* 0x17 */
  MAK_FUNC_0(grlc_em_grlc_info_req,         EM_GRLC_INFO_REQ       )  /* 0x18 */

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
|                         GRLC        CGRLC               UPLINK
|                          |           |
|                   +------v-----------v-------+
|                   |                          |
|                   |            GRLC          |
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
      case SAP_NR(CGRLC_UL):
        table = cgrlc_table;
        n = TAB_SIZE (cgrlc_table);
        break;
#ifdef _SIMULATION_
      case MAC_DL:
        table = mac_table;
        n = TAB_SIZE (mac_table);
        break;
      case SAP_NR(L1TEST_DL):
        table = l1test_table;
        n = TAB_SIZE (l1test_table);
        break;
#endif
      case SAP_NR(GRLC_UL):
        table = grlc_table;
        n = TAB_SIZE (grlc_table);
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
  GRLC_handle = handle;


  /*
   * Open communication channels
   */
  if (hCommGRLC < VSI_OK)
  {
    if ((hCommGRLC = vsi_c_open (VSI_CALLER GRLC_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
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
  if (hCommLLC < VSI_OK)
  {
    if ((hCommLLC = vsi_c_open (VSI_CALLER LLC_NAME)) < VSI_OK)
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
   * is set before the initialisation of the GRLC services takes place
   */
  pcm_Init();

  /*
   * Initialize entity data (call init function of every service)
   */
  grlc_init();

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
    case T3164: ru_t3164();                                     break;
    case T3166: ru_t3166();                                     break;
    case T3168: tm_t3168();                                     break;
    case T3180: ru_t3180();                                     break;
    case T3182: ru_t3182();                                     break;
    case T3184: ru_t3184();                                     break;
    case T3188: tm_t3188();                                     break;
    case T3190: rd_t3190();                                     break;
    case T3192: rd_t3192();                                     break;
    case T3314: ru_t3314();                                     break;
    default:    TRACE_EVENT_P1( "Unknown Timeout: %d", index ); break;
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
  switch (opc)
  {
    case MAC_DATA_IND:
      gff_mac_data_ind(data);
      break;

    case MAC_READY_IND:
      gff_mac_ready_ind(data);
      break;
    case MAC_PWR_CTRL_IND:
      gff_mac_pwr_ctrl_ind(data);
      break;
    default:
      TRACE_EVENT_P1( "Unknown Signal: %08X", opc );
      break;
  }


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
  vsi_c_close (VSI_CALLER hCommGRLC);
  hCommGRLC = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommGRR);
  hCommGRR = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommGMM);
  hCommGMM = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommLLC);
  hCommLLC = VSI_ERROR;

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
                          GRLC_STR_TPC_PWR_PAR, GRLC_NUM_TPC_PWR_PAR,
                          GRLC_STR_TPC_FIX_PCL, GRLC_NUM_TPC_FIX_PCL,
#if !defined (NTRACE)
                          GRLC_STR_TPC_TRACE,   GRLC_NUM_TPC_TRACE,
                          GRLC_STR_IM_TRACE,    GRLC_NUM_IM_TRACE,
#endif /* #if !defined (NTRACE) */
                          GRLC_STR_CCD_USAGE,   GRLC_NUM_CCD_USAGE,
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
  while ((tok_next(&keyw,val)) NEQ TOK_EOCS)
  {
    UBYTE ncomment = GRLC_TSTR_OK;

    switch ((tok_key((KW_DATA *)kwtab,keyw)))
    {
      case GRLC_NUM_TPC_PWR_PAR:
        sig_pei_config_tpc_set_pwr_par( ( UBYTE )atoi( val[0] ),
                                        ( UBYTE )atoi( val[1] ) );
        break;

      case GRLC_NUM_TPC_FIX_PCL:
        sig_pei_config_tpc_fix_pcl( ( UBYTE )atoi( val[0] ) );
        break;

#if !defined (NTRACE)

      case GRLC_NUM_TPC_TRACE:
        grlc_data->tpc.n_tpc_trace = atoi( val[0] );
        break;

      case GRLC_NUM_IM_TRACE:
        grlc_data->meas.v_im_trace = atoi( val[0] );
        break;

#endif /* #if !defined (NTRACE) */

      case GRLC_NUM_CCD_USAGE:
        grlc_data->grlc_wo_ccd = atoi( val[0] );
        break;

      default:
        ncomment = GRLC_TSTR_ILLEGAL_KEYW;
        break;
    }

    TRACE_EVENT_P2( "[PEI_CONFIG]: %s -> %s", 
                    keyw,
                    ncomment EQ GRLC_TSTR_OK ? "OK" : "Illegal Keyword" );
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

  grlc_mon.version = "GRLC 1.0";
  *out_monitor = &grlc_mon;

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
               "GRLC",         /* name */
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
               2048,                           /* stack size */
               PEI_PRIM_QUEUE_SIZE,            /* queue entries */
               226,                            /* priority (1->low, 255->high): GRR 204 RR 205  ALR 225 */  
               TIMER_COUNT,                    /* number of timers */
#ifdef _TARGET_
               PASSIVE_BODY|COPY_BY_REF|TRC_NO_SUSPEND|PRIM_NO_SUSPEND
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

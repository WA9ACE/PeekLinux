/*
+-----------------------------------------------------------------------------
|  Project :  GPRS (8441)
|  Modul   :  sndcp_pei.c
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
|             for the entity SNDCP
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

#define SNDCP_PEI_C

#define ENTITY_SNDCP

/*==== INCLUDES =============================================================*/

#include <stddef.h>     /* to get definition of offsetof(), for MAK_FUNC_S */
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_sndcp.h"    /* to get cnf-definitions */
#include "mon_sndcp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */

#include "dti.h"       /* To get dti lib */

#include "sndcp.h"         /* to get the global entity definitions */

#include "ccdapi.h"     /* to get ccd stuff */

#include "sndcp_ciaf.h"     /* to get functions for service mg */
#include "sndcp_ciap.h"     /* to get primitive handling for service mg */
#include "sndcp_cias.h"     /* to get signal handling for service  mg*/

#include "sndcp_mgf.h"     /* to get functions for service mg */
#include "sndcp_mgp.h"     /* to get primitive handling for service mg */
#include "sndcp_mgs.h"     /* to get signal handling for service  mg*/

#include "sndcp_ndf.h"     /* to get functions for service nd */
#include "sndcp_ndp.h"     /* to get primitive handling for service nd */
#include "sndcp_nds.h"     /* to get signal handling for service nd */

#include "sndcp_nuf.h"     /* to get functions for service nu */
#include "sndcp_nup.h"     /* to get primitive handling for service nu */
#include "sndcp_nus.h"     /* to get signal handling for service nu */

#include "sndcp_pdf.h"     /* to get functions for service pd */
#include "sndcp_pdaf.h"     /* to get functions for service pda */

#include "sndcp_suf.h"     /* to get functions for service su */
#include "sndcp_sup.h"     /* to get primitive handling for service su */
#include "sndcp_sus.h"     /* to get signal handling for service su */

#include "sndcp_suaf.h"     /* to get functions for service sua */
#include "sndcp_suap.h"     /* to get primitive handling for service sua */
#include "sndcp_suas.h"     /* to get signal handling for service sua */

#include "sndcp_sdf.h"     /* to get functions for service sd */
#include "sndcp_sdp.h"     /* to get primitive handling for service sd */
#include "sndcp_sds.h"     /* to get signal handling for service sd */

#include "sndcp_sdaf.h"     /* to get functions for service sda */
#include "sndcp_sdap.h"     /* to get primitive handling for service sda */
#include "sndcp_sdas.h"     /* to get signal handling for service sda */

#include "sndcp_pdp.h"     /* to get primitive handling for service pd */
#include "sndcp_pds.h"     /* to get signal handling for service pd */

#include "sndcp_pdap.h"    /* to get primitive handling for service pda */
#include "sndcp_pdas.h"    /* to get signal handling for service pda */

#include "sndcp_f.h"

#include <string.h>    /* to get memcpy() */
#include <stdlib.h>    /* to get atoi() */



/*==== DEFINITIONS ==========================================================*/
#define TCPIP_NAME    "TCP"
/*==== TYPES ================================================================*/

/*==== GLOBAL VARS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

#ifdef _SNDCP_DTI_2_

LOCAL void sndcp_dti_dti_connect_cnf (T_DTI2_CONNECT_CNF
                                             *dti2_connect_cnf);
LOCAL void sndcp_dti_dti_connect_ind (T_DTI2_CONNECT_IND
                                             *dti2_connect_ind);
LOCAL void sndcp_dti_dti_disconnect_ind (T_DTI2_DISCONNECT_IND
                                             *dti2_disconnect_ind);
LOCAL void sndcp_dti_dti_ready_ind (T_DTI2_READY_IND
                                             *dti2_ready_ind);
LOCAL void sndcp_dti_dti_data_ind (T_DTI2_DATA_IND
                                             *dti2_data_ind);
LOCAL void sndcp_dti_dti_getdata_req (T_DTI2_GETDATA_REQ
                                            *dti_getdata_req);
LOCAL void sndcp_dti_dti_data_req (T_DTI2_DATA_REQ
                                         *dti_data_req);
LOCAL void sndcp_dti_dti_connect_req (T_DTI2_CONNECT_REQ
                                            *dti2_connect_req);
LOCAL void sndcp_dti_dti_connect_res (T_DTI2_CONNECT_RES
                                            *dti2_connect_res);
LOCAL void sndcp_dti_dti_disconnect_req (T_DTI2_DISCONNECT_REQ
                                            *dti2_disconnect_req);
#ifdef _SIMULATION_
LOCAL void sndcp_dti_dti_data_test_req (T_DTI2_DATA_TEST_REQ
                                            *dti_data_test_req);
LOCAL void sndcp_dti_dti_data_test_ind (T_DTI2_DATA_TEST_IND
                                              *dti_data_test_ind);
#endif /*_SIMULATION_*/

#else /*_SNDCP_DTI_2_*/

LOCAL void sndcp_dti_dti_getdata_req (T_DTI_GETDATA_REQ
                                            *dti_getdata_req);

LOCAL void sndcp_dti_dti_data_req (T_DTI_DATA_REQ
                                         *dti_data_req);
#ifdef _SIMULATION_
LOCAL void sndcp_dti_dti_data_test_req (T_DTI_DATA_TEST_REQ
                                            *dti_data_test_req);
#endif

LOCAL void sndcp_dti_dti_ready_ind (T_DTI_READY_IND
                                          *dti_ready_ind);

LOCAL void sndcp_dti_dti_data_ind (T_DTI_DATA_IND
                                         *dti_data_ind);

#ifdef _SIMULATION_
LOCAL void sndcp_dti_dti_data_test_ind (T_DTI_DATA_TEST_IND
                                              *dti_data_test_ind);
#endif
#endif /* _SNDCP_DTI_2_ */


static  BOOL          first_access  = TRUE;
static  T_MONITOR     sndcp_mon;



#ifdef TI_PS_OP_ICUT_SNDCP

/* PDP_TBR added SNDCP terminal loopback */
U8 SNDCP_LOOPBACK = FALSE;

BOOL bufFull[SNDCP_NUMBER_OF_NSAPIS] = {FALSE,FALSE,FALSE,FALSE, FALSE,FALSE,FALSE,FALSE,
                                       FALSE,FALSE,FALSE,FALSE, FALSE,FALSE,FALSE,FALSE};
T_DTI2_DATA_IND *saveDti2_data_ind[SNDCP_NUMBER_OF_NSAPIS] = {NULL,NULL,NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

#endif /* TI_PS_OP_ICUT_SNDCP */




/*
 * Jumptables to primitive handler functions. One table per SAP.
 *
 * Use MAK_FUNC_0 for primitives which contains no SDU.
 * Use MAK_FUNC_S for primitives which contains a SDU.
 */

/*
 * Function is needed for function_table[]. This declaration can be removed
 * as soon as this function is no more called (i.e. all primitives are
 * handled).
 */
LOCAL void primitive_not_supported (void *data);

#ifndef SNDCP_UPM_INCLUDED
static const T_FUNC snsm_table[] =
{
  MAK_FUNC_0(mg_snsm_activate_ind,    SNSM_ACTIVATE_IND), /* 6700 */
  MAK_FUNC_0(mg_snsm_deactivate_ind,  SNSM_DEACTIVATE_IND), /* 6701 */
  MAK_FUNC_0(mg_snsm_modify_ind,      SNSM_MODIFY_IND), /* 6702 */
  MAK_FUNC_0(mg_snsm_sequence_ind,    SNSM_SEQUENCE_IND) /* 6703 */
};
#endif

#ifndef SNDCP_UPM_INCLUDED
static const T_FUNC sn_table[] =
{
  MAK_FUNC_N(primitive_not_supported, 0), /* 2800 */
  MAK_FUNC_N(primitive_not_supported, 0), /* 2801 */
  MAK_FUNC_N(primitive_not_supported, 0), /* 2802 */
  MAK_FUNC_N(primitive_not_supported, 0), /* 2803 */
  MAK_FUNC_N(primitive_not_supported, 0), /* 2804 */
  MAK_FUNC_N(primitive_not_supported, 0), /* 2805 */
  MAK_FUNC_N(primitive_not_supported, 0), /* 2806 */
  MAK_FUNC_N(primitive_not_supported, 0)  /* 2807 */
#ifdef _SNDCP_MEAN_TRACE_
  ,
  MAK_FUNC_0(sndcp_sn_count_req, SN_COUNT_REQ) /* 2808 */
#endif
  ,
  MAK_FUNC_0(mg_sn_switch_req, SN_SWITCH_REQ)  /* 2809 */
};

#else  /*#ifndef SNDCP_UPM_INCLUDED*/

static const T_FUNC sn_table[] =
{                                                                 /* edge mode */
  MAK_FUNC_0(mg_snsm_activate_ind,      SN_ACTIVATE_REQ),         /*0x8000009E */
  MAK_FUNC_0(mg_snsm_deactivate_ind,    SN_DEACTIVATE_REQ),       /*0x8001009E */
#ifdef TI_DUAL_MODE
  MAK_FUNC_0(mg_get_pending_pdu_req,    SN_GET_PENDING_PDU_REQ),  /*0x8002009E */
#else
  MAK_FUNC_N(primitive_not_supported,   SN_GET_PENDING_PDU_REQ),  /* 0x8002009E */ 
#endif
  MAK_FUNC_0(mg_snsm_modify_ind,        SN_MODIFY_REQ),           /*0x8003009E */
  MAK_FUNC_0(mg_snsm_sequence_ind,      SN_SEQUENCE_REQ),         /*0x8004009E */
  MAK_FUNC_0(sndcp_sn_count_req,        SN_COUNT_REQ),            /*0x8005009E */
  MAK_FUNC_0(mg_sn_dti_req,             SN_DTI_REQ)               /*0x8006009E */
  
};
#endif /*#ifdef SNDCP_UPM_INCLUDED*/

static const T_FUNC ll_table[] =
{
  MAK_FUNC_0(mg_ll_reset_ind,     LL_RESET_IND),     /* 6200 */
  MAK_FUNC_S(mg_ll_establish_cnf, LL_ESTABLISH_CNF), /* 6201 */
  MAK_FUNC_S(mg_ll_establish_ind, LL_ESTABLISH_IND), /* 6202 */
  MAK_FUNC_0(mg_ll_release_cnf, LL_RELEASE_CNF),     /* 6203 */
  MAK_FUNC_0(mg_ll_release_ind, LL_RELEASE_IND),     /* 6204 */
  MAK_FUNC_S(mg_ll_xid_cnf, LL_XID_CNF),             /* 6205 */
  MAK_FUNC_S(mg_ll_xid_ind, LL_XID_IND),             /* 6206 */
  MAK_FUNC_0(sua_ll_ready_ind, LL_READY_IND),        /* 6207 */
  MAK_FUNC_0(su_ll_unitready_ind, LL_UNITREADY_IND), /* 6208 */
  MAK_FUNC_0(sua_ll_data_cnf, LL_DATA_CNF),          /* 6209 */
  MAK_FUNC_S(pda_ll_data_ind, LL_DATA_IND),          /* 620A */
  MAK_FUNC_S(pd_ll_unitdata_ind, LL_UNITDATA_IND),   /* 620B */
  MAK_FUNC_0(mg_ll_status_ind, LL_STATUS_IND)        /* 620C */
};

#ifdef _SNDCP_DTI_2_

/*
 * Jumptable for the DTI service access point for downlink data transmission.
 * Contains the processing-function addresses and opcodes of
 * indication and confirm primitives. Use of DTILIB can be selected.
 *
 */

LOCAL const T_FUNC ddl_table_dti[] = {

  MAK_FUNC_0(sndcp_dti_dti_connect_ind,    DTI2_CONNECT_IND),      /* 7750 */
  MAK_FUNC_0(sndcp_dti_dti_connect_cnf,    DTI2_CONNECT_CNF),      /* 7751 */
  MAK_FUNC_0(sndcp_dti_dti_disconnect_ind, DTI2_DISCONNECT_IND),   /* 7752 */
  MAK_FUNC_0(sndcp_dti_dti_ready_ind,      DTI2_READY_IND),        /* 7753 */
  MAK_FUNC_0(sndcp_dti_dti_data_ind,       DTI2_DATA_IND)          /* 7754 */

#if defined (_SIMULATION_)
  ,
  MAK_FUNC_0(sndcp_dti_dti_data_test_ind, DTI2_DATA_TEST_IND)      /* 7755 */
#endif

};

/*
 * Jumptable for the DTI service access point for uplink data transmission.
 * Contains the processing-function addresses and opcodes of
 * request and response primitives.
 */


LOCAL const T_FUNC dul_table_dti[] = {

  MAK_FUNC_0(sndcp_dti_dti_connect_req, DTI2_CONNECT_REQ),       /*0x3750*/
  MAK_FUNC_0(sndcp_dti_dti_connect_res, DTI2_CONNECT_RES),       /*0x3751*/
  MAK_FUNC_0(sndcp_dti_dti_disconnect_req, DTI2_DISCONNECT_REQ), /*0x3752*/
  MAK_FUNC_0(sndcp_dti_dti_getdata_req, DTI2_GETDATA_REQ),       /*0x3753*/
  MAK_FUNC_0(sndcp_dti_dti_data_req, DTI2_DATA_REQ)              /*0x3754*/

#if defined (_SIMULATION_)
  ,
  MAK_FUNC_0(sndcp_dti_dti_data_test_req, DTI2_DATA_TEST_REQ),   /*0x3755*/
  MAK_FUNC_N(primitive_not_supported, DTI2_DUMMY_REQ)          /*0x3756*/
#endif /* _SIMULATION_ */
};

#else /*_SNDCP_DTI_2_*/

/*
 * Jumptable for the DTI service access point for uplink data transmission.
 * Contains the processing-function addresses and opcodes of
 * request and response primitives.
 */

LOCAL const T_FUNC dul_table_dti[] = {

  MAK_FUNC_0(sndcp_dti_dti_getdata_req, DTI_GETDATA_REQ),
  MAK_FUNC_0(sndcp_dti_dti_data_req, DTI_DATA_REQ)

#if defined (_SIMULATION_)
  ,
  MAK_FUNC_S(sndcp_dti_dti_data_test_req, DTI_DATA_TEST_REQ)
#endif /* _SIMULATION_ */
};


/*
 * Jumptable for the DTI service access point for downlink data transmission.
 * Contains the processing-function addresses and opcodes of
 * indication and confirm primitives. Use of DTILIB can be selected.
 *
 */


LOCAL const T_FUNC ddl_table_dti[] = {

  MAK_FUNC_0(sndcp_dti_dti_ready_ind, DTI_READY_IND),
  MAK_FUNC_0(sndcp_dti_dti_data_ind, DTI_DATA_IND)

#if defined (_SIMULATION_)
  ,
  MAK_FUNC_S(sndcp_dti_dti_data_test_ind, DTI_DATA_TEST_IND)
#endif
};
#endif /* SNDCP_DTI_2_ */

/*==== DIAGNOSTICS ==========================================================*/

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

#ifdef _SNDCP_DTI_2_
/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_connect_cnf
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI2_CONNECT_CNF.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_connect_cnf (T_DTI2_CONNECT_CNF   *dti2_connect_cnf)
{
  TRACE_FUNCTION ("sndcp_dti_dti_connect_cnf");

  dti_dti_connect_cnf (sndcp_data->hDTI, dti2_connect_cnf);
}


/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_connect_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI2_CONNECT_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_connect_ind (T_DTI2_CONNECT_IND   *dti2_connect_ind)
{
  TRACE_FUNCTION ("sndcp_dti_dti_connect_ind");

  dti_dti_connect_ind (sndcp_data->hDTI, dti2_connect_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_disconnect_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI2_DISCONNECT_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_disconnect_ind (T_DTI2_DISCONNECT_IND   *dti2_disconnect_ind)
{
  TRACE_FUNCTION ("sndcp_dti_dti_disconnect_ind");

  dti_dti_disconnect_ind (sndcp_data->hDTI, dti2_disconnect_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_getdata_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_GETDATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/

LOCAL void sndcp_dti_dti_getdata_req (T_DTI2_GETDATA_REQ   *dti2_getdata_req)
{
  TRACE_FUNCTION ("sndcp_dti_dti_getdata_req");

  dti_dti_getdata_req (sndcp_data->hDTI, dti2_getdata_req);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_data_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_data_req (T_DTI2_DATA_REQ   *dti2_data_req)
{
  TRACE_FUNCTION ("sndcp_dti_dti_data_req");

  dti_dti_data_req (sndcp_data->hDTI, dti2_data_req);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_data_test_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_TEST_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
#ifdef _SIMULATION_
LOCAL void sndcp_dti_dti_data_test_req (T_DTI2_DATA_TEST_REQ
                                              *dti2_data_test_req)
{
  TRACE_FUNCTION ("sndcp_dti_dti_data_test_req");

  dti_dti_data_test_req (sndcp_data->hDTI, dti2_data_test_req);
}
#endif

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_data_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_data_ind (T_DTI2_DATA_IND   *dti2_data_ind)
{
  TRACE_FUNCTION ("sndcp_dti_dti_data_ind");

  dti_dti_data_ind (sndcp_data->hDTI, dti2_data_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_ready_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_READY_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_ready_ind (T_DTI2_READY_IND   *dti2_ready_ind)
{
  TRACE_FUNCTION ("sndcp_dti_dti_ready_ind");

  dti_dti_ready_ind (sndcp_data->hDTI, dti2_ready_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_data_test_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_TEST_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
#ifdef _SIMULATION_
LOCAL void sndcp_dti_dti_data_test_ind (T_DTI2_DATA_TEST_IND   *dti2_data_test_ind)
{
  TRACE_FUNCTION ("sndcp_dti_dti_data_test_ind");

  dti_dti_data_test_ind (sndcp_data->hDTI, dti2_data_test_ind);
}
#endif

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_connect_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_connect_req (T_DTI2_CONNECT_REQ   *dti2_connect_req)
{
  TRACE_FUNCTION ("sndcp_dti_dti_connect_req");

  dti_dti_connect_req (sndcp_data->hDTI, dti2_connect_req);
  if ( sndcp_data->nsapi_neighbor_ra[sndcp_data->nsapi] EQ TCPIP_NAME)
  {
     mg_dti_open(sndcp_data->nsapi);
  }
}
/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_connect_res
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_connect_res (T_DTI2_CONNECT_RES   *dti2_connect_res)
{
  TRACE_FUNCTION ("sndcp_dti_dti_connect_res");

  dti_dti_connect_res (sndcp_data->hDTI, dti2_connect_res);
}
/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_disconnect_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_disconnect_req (T_DTI2_DISCONNECT_REQ   *dti2_disconnect_req)
{
  TRACE_FUNCTION ("sndcp_dti_dti_disconnect_req");

  dti_dti_disconnect_req (sndcp_data->hDTI, dti2_disconnect_req);
}


#else /* _SNDCP_DTI_2_ */

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_getdata_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_GETDATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/

LOCAL void sndcp_dti_dti_getdata_req (T_DTI_GETDATA_REQ   *dti_getdata_req)
{
  TRACE_FUNCTION ("sndcp_dti_dti_getdata_req");

  dti_dti_getdata_req (sndcp_data->hDTI, dti_getdata_req);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_data_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_data_req (T_DTI_DATA_REQ   *dti_data_req)
{
  TRACE_FUNCTION ("sndcp_dti_dti_data_req");

  dti_dti_data_req (sndcp_data->hDTI, dti_data_req);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_data_test_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_TEST_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
#ifdef _SIMULATION_
LOCAL void sndcp_dti_dti_data_test_req (T_DTI_DATA_TEST_REQ
                                              *dti_data_test_req)
{
  TRACE_FUNCTION ("sndcp_dti_dti_data_test_req");

  dti_dti_data_test_req (sndcp_data->hDTI, dti_data_test_req);
}
#endif

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_data_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_data_ind (T_DTI_DATA_IND   *dti_data_ind)
{
  TRACE_FUNCTION ("sndcp_dti_dti_data_ind");

  dti_dti_data_ind (sndcp_data->hDTI, dti_data_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_ready_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_READY_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void sndcp_dti_dti_ready_ind (T_DTI_READY_IND   *dti_ready_ind)
{
  TRACE_FUNCTION ("sndcp_dti_dti_ready_ind");

  dti_dti_ready_ind (sndcp_data->hDTI, dti_ready_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : sndcp_dti_dti_data_test_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_TEST_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
#ifdef _SIMULATION_
LOCAL void sndcp_dti_dti_data_test_ind (T_DTI_DATA_TEST_IND   *dti_data_test_ind)
{
  TRACE_FUNCTION ("sndcp_dti_dti_data_test_ind");

  dti_dti_data_test_ind (sndcp_data->hDTI, dti_data_test_ind);
}
#endif

#endif /* _SNDCP_DTI_2_ */


/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : pei_primitive
+------------------------------------------------------------------------------
| Description :  This function is called by the frame when a primitive is
|                received and needs to be processed.
|
|                                 |
|                                MMI               UPLINK
|                                 |
|                   +-------------v------------+
|                   |                          |
|                   |          SNDCP           |
|                   |                          |
|                   +------^------^-----^------+
|                          |      |     |
|                         SM     CCI   LLC         DOWNLINK
|                          |      |     |
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
    T_PRIM *prim  = (T_PRIM *)primptr;
#ifdef SNDCP_UPM_INCLUDED
    U32           opc = (prim->custom.opc);
#else
    USHORT           opc = (USHORT)(prim->custom.opc);
#endif
    USHORT           n;
    const T_FUNC    *table;

    /*
     * This must be called for Partition Pool supervision. Will be replaced
     * by another macro some time.
     */
    VSI_PPM_REC (&prim->custom, __FILE__, __LINE__);

#ifdef SNDCP_UPM_INCLUDED
    PTRACE_IN (opc);

    switch (SAP_NR(opc))
    {
      case SAP_NR(SN_UL):
        table = sn_table;
        n = TAB_SIZE (sn_table);
        break;
      case SAP_NR(LL_DL):
        table = ll_table;
        n = TAB_SIZE (ll_table);
        break;
      case DTI2_DL:
        opc -= 0x50;
        table = ddl_table_dti;
        n = TAB_SIZE (ddl_table_dti);
        break;
      case DTI2_UL:
        opc -= 0x50;
        table = dul_table_dti;
        n = TAB_SIZE (dul_table_dti);
        break;
      default:
        table = NULL;
        n = 0;
        break;
    } 
#else  /*#ifdef SNDCP_UPM_INCLUDED*/

    PTRACE_IN (opc);

    switch (SAP_NR(opc))
    {
      case  SAP_NR(SN_UL):
        table = sn_table;
        n = TAB_SIZE (sn_table);
        break;
      case SNSM_DL:
        table = snsm_table;
        n = TAB_SIZE (snsm_table);
        break;
      case SAP_NR(LL_DL):
        table = ll_table;
        n = TAB_SIZE (ll_table);
        break;
#ifdef _SNDCP_DTI_2_
      case DTI2_DL:
        opc -= 0x50;
        table = ddl_table_dti;
        n = TAB_SIZE (ddl_table_dti);
        break;
      case DTI2_UL:
        opc -= 0x50;
        table = dul_table_dti;
        n = TAB_SIZE (dul_table_dti);
        break;
#else /* _SNDCP_DTI_2_ */
      case DTI_DL:
        table = ddl_table_dti;
        n = TAB_SIZE (ddl_table_dti);
        break;
      case DTI_UL:
        table = dul_table_dti;
        n = TAB_SIZE (dul_table_dti);
        break;
#endif /* _SNDCP_DTI_2_ */
      default:
        table = NULL;
        n = 0;
        break;
    }
#endif
    if (table != NULL)
    {
#ifdef SNDCP_UPM_INCLUDED
      if ((PRIM_NR(opc)) < n)
      {
        table += PRIM_NR(opc);
#else
      if ((opc & PRM_MASK) < n)
      {
        table += opc & PRM_MASK;
#endif
#ifdef PALLOC_TRANSITION
        P_SDU(prim) =
          table->soff ? (T_sdu*)
          (((char*)&prim->data) + table->soff) : 0;
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
#ifdef FLOW_TRACE
  UBYTE i = 0;
  UBYTE j = 0;
  UBYTE k = 0;
#endif

  TRACE_FUNCTION ("pei_init");

  /*
   * Initialize task handle
   */
  SNDCP_handle = handle;

  /*
   * Open communication channels
   */
  if (hCommMMI < VSI_OK)
  {

#ifdef _SNDCP_DTI_2_
  if ((hCommMMI = vsi_c_open (VSI_CALLER ACI_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#else /* _SNDCP_DTI_2_ */
  if ((hCommMMI = vsi_c_open (VSI_CALLER MMI_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#endif /* _SNDCP_DTI_2_ */


#ifdef SNDCP_UPM_INCLUDED
  if (hCommUPM < VSI_OK)
  {
    if ((hCommUPM = vsi_c_open (VSI_CALLER UPM_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#else
  if (hCommSM < VSI_OK)
  {
    if ((hCommSM = vsi_c_open (VSI_CALLER SM_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
  
#endif /*#ifdef SNDCP_UPM_INCLUDED*/
  
  if (hCommLLC < VSI_OK)
  {
    if ((hCommLLC = vsi_c_open (VSI_CALLER LLC_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

  /*
   * Initialize global pointer sndcp_data. This is required to access all
   * entity data.
   */
  sndcp_data = &sndcp_data_base;


#ifdef FLOW_TRACE
  for(i = 0; i < 5; i++) {
    for (j = 0; j < 2; j++) {
      for (k = 0; j < 2; j++) {
        sndcp_trace_flow_control(i, j, k, FALSE);
      }
    }
  }
#endif


  /*
   * Initialize global data.
   *
   * These arrays are not reset here:
   * nsapi_qos_ra,
   * nsapi_sapi_ra,
   * nsapi_prio_ra,
   * nsapi_tlli_ra,
   *
   */
  {
    UBYTE nsapi = 0;
    UBYTE sapi_index = 0;
    for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
      sndcp_data->nsapi_used_ra[nsapi] = FALSE;
      sndcp_data->nsapi_ack_ra[nsapi] = FALSE;
      sndcp_data->nsapi_state_ra[nsapi] = MG_IDLE;

      sndcp_data->nsapi_direction_ra[nsapi] = FALSE;
      sndcp_data->nsapi_interface_ra[nsapi] = SNDCP_INTERFACE_UNACK;
      sndcp_data->nsapi_linkid_ra[nsapi] = 0;
      sndcp_data->nsapi_neighbor_ra[nsapi] = NULL;

      sndcp_data->cur_segment_number[nsapi] = 0;

#ifdef _SNDCP_MEAN_TRACE_

      sndcp_data->start_time_uplink_ack [nsapi] = 0;
      sndcp_data->start_time_uplink_unack [nsapi] = 0;
      sndcp_data->start_time_downlink_ack [nsapi] = 0;
      sndcp_data->start_time_downlink_unack [nsapi] = 0;

      sndcp_data->cur_time_uplink_ack [nsapi] = 0;
      sndcp_data->cur_time_uplink_unack [nsapi] = 0;
      sndcp_data->cur_time_downlink_ack [nsapi] = 0;
      sndcp_data->cur_time_downlink_unack [nsapi] = 0;

      sndcp_data->cur_num_uplink_ack [nsapi] = 0;
      sndcp_data->cur_num_uplink_unack [nsapi] = 0;
      sndcp_data->cur_num_downlink_ack [nsapi] = 0;
      sndcp_data->cur_num_downlink_unack [nsapi] = 0;

      sndcp_data->cur_pac_uplink_ack [nsapi] = 0;
      sndcp_data->cur_pac_uplink_unack [nsapi] = 0;
      sndcp_data->cur_pac_downlink_ack [nsapi] = 0;
      sndcp_data->cur_pac_downlink_unack [nsapi] = 0;
#ifndef NCONFIG
      sndcp_data->data_count[nsapi] = 0;
#endif


#endif /* _SNDCP_MEAN_TRACE_ */
    }
    for (sapi_index = 0; sapi_index < SNDCP_NUMBER_OF_SAPIS; sapi_index++) {
      sndcp_data->sapi_ack_ra[sapi_index] = FALSE;
      sndcp_data->sapi_state_ra[sapi_index] = MG_IDLE;
    }
  }

  sndcp_data->vj_count = 0;
  sndcp_data->always_xid = TRUE;

#ifndef NCONFIG
  sndcp_data->millis = 0;
#endif

  sndcp_data->trace_ip_header = FALSE;
  sndcp_data->trace_ip_datagram = FALSE;
  sndcp_data->trace_ip_bin = FALSE;
  sndcp_data->ip_filter = FALSE;

  sndcp_data->nsapi_rcv_rdy_b4_used=0;


  /*
   * Initialize entity data (call init function of every service)
   */
  cia_init();
  mg_init();
  nd_init();
  nu_init();
  pd_init();
  pda_init();
  sd_init();
  sda_init();
  su_init();
  sua_init();

  sndcp_data->hDTI =
  dti_init(SNDCP_MAX_DTI_LINKS, /* U8 maximum_links */
           SNDCP_handle, /* T_HANDLE handle, */
#ifdef _SNDCP_DTI_2_
           DTI_DEFAULT_OPTIONS, /* U32 */
#endif /* _SNDCP_DTI_2_ */
           sndcp_sig_callback);

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
  if (index < TIMER_MAX) {
    mg_exp_re_est_timer((UBYTE)index);
  } else {
    TRACE_ERROR("Unknown Timeout");
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
  /*
  switch (opc)
  {
    default:
    */
      TRACE_ERROR("Unknown Signal OPC");
      /*
      break;
  } */

  return PEI_OK;
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
  UBYTE nsapi = 0;

  TRACE_FUNCTION ("pei_exit");

  /*
   * Close communication channels
   */
  vsi_c_close (VSI_CALLER hCommMMI);
  hCommMMI = VSI_ERROR;

#ifdef SNDCP_UPM_INCLUDED
  vsi_c_close (VSI_CALLER hCommUPM);
  hCommUPM = VSI_ERROR;
#else
  vsi_c_close (VSI_CALLER hCommSM);
  hCommSM = VSI_ERROR;
#endif

  vsi_c_close (VSI_CALLER hCommLLC);
  hCommLLC = VSI_ERROR;

  for (nsapi = 0; nsapi < SNDCP_NUMBER_OF_NSAPIS; nsapi++) {
    mg_delete_npdus(nsapi);
  }

  dti_deinit(sndcp_data->hDTI);
  sndcp_data->hDTI = NULL;


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
/*
 * !!!!! Does not compile!
 *
  T_QMSG Message;

  TRACE_FUNCTION ("pei_run");

  if (pei_init (TaskHandle) != PEI_OK)
    return PEI_ERROR;

  while (!exit_flag)
  {
    vsi_c_await (VSI_CALLER ComHandle, &Message);
    switch (Message.MsgType)
    {
      case MSG_PRIMITIVE:
        pei_primitive (Message.Msg.Primitive.Prim );
        break;
      case MSG_SIGNAL:
        pei_signal ( (USHORT)Message.Msg.Signal.SigOPC,
          Message.Msg.Signal.SigBuffer );
        break;
      case MSG_TIMEOUT:
        pei_timeout ( (USHORT)Message.Msg.Timer.Index );
        break;
      default:
        TRACE_ERROR("Unknown Message Type");
        break;
    }
  }

 *
 *
 */

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
LOCAL SHORT pei_config (char *inString)
{
  TRACE_FUNCTION ("pei_config");
  TRACE_FUNCTION (inString);

#ifndef NCONFIG
  /*
   * Parse next keyword and number of variables
   */

  /*if(!strcmp(inString,"DISCARD")) {*/
  if (inString[0] == 'D' &&             /* DISCARD NSAPI */
      inString[1] == 'I' &&
      inString[2] == 'S' &&
      inString[3] == 'C' &&
      inString[4] == 'A' &&
      inString[5] == 'R' &&
      inString[6] == 'D') {

    UBYTE nsapi = (UBYTE)atoi(&inString[8]);

    TRACE_EVENT_P1("Each DTI2_DATA Prim on NSAPI %d will be discarded", nsapi);
    nu_config_discard(nsapi);
  } else if (inString[0] == 'R' &&      /* RESUME NSAPI */
             inString[1] == 'E' &&
             inString[2] == 'S' &&
             inString[3] == 'U' &&
             inString[4] == 'M' &&
             inString[5] == 'E') {

    UBYTE nsapi = (UBYTE)atoi(&inString[7]);
    TRACE_EVENT_P1 ("Each DTI2_DATA Prim on NSAPI %d will be forwarded", nsapi);
    nu_config_resume(nsapi);
  } else if (inString[0] == 'S' &&
             inString[1] == 'E' &&
             inString[2] == 'N' &&
             inString[3] == 'D') {

      UBYTE nsapi = (UBYTE)atoi(&inString[4]);

      U32 octets = (U32)atoi(&inString[7]);

      nu_config_send(nsapi, octets);

  } else if (inString[0] == 'P' &&
             inString[1] == 'R' &&
             inString[2] == 'I' &&
             inString[3] == 'O') {

    UBYTE nsapi = (UBYTE)atoi(&inString[4]);
    UBYTE prio = (UBYTE)atoi(&inString[7]);

    sndcp_set_nsapi_prio(nsapi, prio);
  } else if (inString[0] == 'N' &&
             inString[1] == 'O' &&
             inString[2] == ' ' &&
             inString[3] == 'X' &&
             inString[4] == 'I' &&
             inString[5] == 'D') {


    sndcp_no_xid();


  } else if (inString[0] == 'D' &&
             inString[1] == 'E' &&
             inString[2] == 'L' &&
             inString[3] == 'A' &&
             inString[4] == 'Y') {

      USHORT millis = (USHORT)atoi(&inString[5]);

      mg_config_delay(millis);

  } else if (inString[0] == 'T' &&      /* TRACE_HEADER_ON */
             inString[1] == 'R' &&
             inString[2] == 'A' &&
             inString[3] == 'C' &&
             inString[4] == 'E' &&
             inString[5] == '_' &&
             inString[6] == 'H' &&
             inString[7] == 'E' &&
             inString[8] == 'A' &&
             inString[9] == 'D' &&
             inString[10] == 'E' &&
             inString[11] == 'R' &&
             inString[12] == '_' &&
             inString[13] == 'O' &&
             inString[14] == 'N') {
    sndcp_data->trace_ip_header = TRUE;
  } else if (inString[0] == 'T' &&      /* TRACE_HEADER_OFF */
             inString[1] == 'R' &&
             inString[2] == 'A' &&
             inString[3] == 'C' &&
             inString[4] == 'E' &&
             inString[5] == '_' &&
             inString[6] == 'H' &&
             inString[7] == 'E' &&
             inString[8] == 'A' &&
             inString[9] == 'D' &&
             inString[10] == 'E' &&
             inString[11] == 'R' &&
             inString[12] == '_' &&
             inString[13] == 'O' &&
             inString[14] == 'F' &&
             inString[15] == 'F') {
    sndcp_data->trace_ip_header = FALSE;
  }else if (inString[0] == 'T' &&       /* TRACE_PACKET_ON */
             inString[1] == 'R' &&
             inString[2] == 'A' &&
             inString[3] == 'C' &&
             inString[4] == 'E' &&
             inString[5] == '_' &&
             inString[6] == 'P' &&
             inString[7] == 'A' &&
             inString[8] == 'C' &&
             inString[9] == 'K' &&
             inString[10] == 'E' &&
             inString[11] == 'T' &&
             inString[12] == '_' &&
             inString[13] == 'O' &&
             inString[14] == 'N') {
    sndcp_data->trace_ip_datagram = TRUE;
  } else if (inString[0] == 'T' &&      /* TRACE_PACKET_OFF */
             inString[1] == 'R' &&
             inString[2] == 'A' &&
             inString[3] == 'C' &&
             inString[4] == 'E' &&
             inString[5] == '_' &&
             inString[6] == 'P' &&
             inString[7] == 'A' &&
             inString[8] == 'C' &&
             inString[9] == 'K' &&
             inString[10] == 'E' &&
             inString[11] == 'T' &&
             inString[12] == '_' &&
             inString[13] == 'O' &&
             inString[14] == 'F' &&
             inString[15] == 'F') {
    sndcp_data->trace_ip_datagram = FALSE;
  


#ifdef TI_PS_OP_ICUT_SNDCP
  /* PDP_TBR added SNDCP terminal loopback */
  } else  if   ( inString[0] == 'L' &&   
                inString[1] == 'O' &&
                inString[2] == 'O' &&
                inString[3] == 'P' &&
                inString[4] == 'B' &&
                inString[5] == 'A' &&
                inString[6] == 'C' &&
                inString[7] == 'K') 
  {
    SNDCP_LOOPBACK = 1;
    TRACE_EVENT("SNDCP LOOPBACK");
  
#endif /* TI_PS_OP_ICUT_SNDCP */

  } else if (inString[0] == 'T' &&      /* TRACE_IP_BIN */
             inString[1] == 'R' &&
             inString[2] == 'A' &&
             inString[3] == 'C' &&
             inString[4] == 'E' &&
             inString[5] == '_' &&
             inString[6] == 'I' &&
             inString[7] == 'P' &&
             inString[8] == '_' &&
             inString[9] == 'B' &&
             inString[10] == 'I' &&
             inString[11] == 'N') {
    sndcp_data->trace_ip_bin = TRUE;
  } else if (inString[0] == 'I' &&      /* IP_FILTER_OFF */
             inString[1] == 'P' &&
             inString[2] == '_' &&
             inString[3] == 'F' &&
             inString[4] == 'I' &&
             inString[5] == 'L' &&
             inString[6] == 'T' &&
             inString[7] == 'E' &&
             inString[8] == 'R' &&
             inString[9] == '_' &&
             inString[10] == 'O' &&
             inString[11] == 'F' &&
             inString[12] == 'F') {

    TRACE_EVENT("INFO IP FILTER: IP filter is disabled.");
    sndcp_data->ip_filter = FALSE;
  } else if (inString[0] == 'I' &&      /* IP_FILTER_ON */
             inString[1] == 'P' &&
             inString[2] == '_' &&
             inString[3] == 'F' &&
             inString[4] == 'I' &&
             inString[5] == 'L' &&
             inString[6] == 'T' &&
             inString[7] == 'E' &&
             inString[8] == 'R' &&
             inString[9] == '_' &&
             inString[10] == 'O' &&
             inString[11] == 'N') {

    TRACE_EVENT("INFO IP FILTER: IP filter is enabled.");
    sndcp_data->ip_filter = TRUE;
  } else {
    TRACE_EVENT("ERROR PEI SNDCP: unknown Config Primitive!");
  }

#endif

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
| Function    : pei_monitor
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

  /*
   * Version = "0.S" (S = Step).
   */
  sndcp_mon.version = "SNDCP 0.1";
  *out_monitor = &sndcp_mon;

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
               "SND",         /* name */
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
               1536,          /* stack size */
               10,            /* queue entries */
               185,           /* priority (1->low, 255->high) */
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

  return PEI_OK;
}

/*==== END OF FILE ==========================================================*/

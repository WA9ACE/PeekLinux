/*
+-----------------------------------------------------------------------------
|  File     : psi_pei.c
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
|  Purpose  : This module implements the process body interface
|             for the entity Protocol Stack Interface (PSI)
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
|  $Identity:$
+-----------------------------------------------------------------------------
*/

#define PSI_PEI_C

#define ENTITY_PSI

/*==== INCLUDES =============================================================*/

#ifdef _SIMULATION_
#include <stdio.h>
#include "nucleus.h"
#endif /* _SIMULATION_ */
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_psi.h"   /* to get cnf-definitions */
#include "mon_psi.h"   /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "DIO_inline.h" /* to get the function definition of used SAP DIO */
#include "pei.h"        /* to get PEI interface */
#include "dti.h"
#include "psi.h"       /* to get the global entity definitions */

#include "psi_kerf.h"  /* to get ker functions */
#include "psi_kerp.h"  /* to get ker primitives */
#include "psi_kers.h"  /* to get ker signals */

#include "psi_rxf.h"   /* to get rx functions */
#include "psi_rxp.h"   /* to get rx primitives */
#include "psi_rxs.h"   /* to get rx signals */

#include "psi_txf.h"   /* to get tx functions */
#include "psi_txp.h"   /* to get tx primitives */
#include "psi_txs.h"   /* to get tx signals */

#include "psi_drxf.h"  /* to get drx functions */
#include "psi_drxs.h"  /* to get drx signals */

#include "psi_dtxf.h"  /* to get dtx functions */
#include "psi_dtxs.h"  /* to get dtx signals */

#include <string.h>



/*==== DEFINITIONS ==========================================================*/

/*==== TYPES ================================================================*/

/*==== GLOBAL VARS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/
static  BOOL          first_access  = TRUE;
static  T_MONITOR     psi_mon;

#ifndef _SIMULATION_
// ROSHIN_USBF_FIX
extern T_HANDLE		usbf_psi_rx_mutex;
extern int 		usbf_psi_use_rx_mutex;
#endif

LOCAL void psi_dti_dti_connect_cnf (T_DTI2_CONNECT_CNF
                                             *dti2_connect_cnf);
LOCAL void psi_dti_dti_connect_ind (T_DTI2_CONNECT_IND
                                             *dti2_connect_ind);
LOCAL void psi_dti_dti_disconnect_ind (T_DTI2_DISCONNECT_IND
                                             *dti2_disconnect_ind);
LOCAL void psi_dti_dti_ready_ind (T_DTI2_READY_IND
                                             *dti2_ready_ind);
LOCAL void psi_dti_dti_data_ind (T_DTI2_DATA_IND
                                             *dti2_data_ind);
LOCAL void psi_dti_dti_getdata_req (T_DTI2_GETDATA_REQ
                                            *dti_getdata_req);
LOCAL void psi_dti_dti_data_req (T_DTI2_DATA_REQ
                                         *dti_data_req);
LOCAL void psi_dti_dti_connect_req (T_DTI2_CONNECT_REQ   
                                            *dti2_connect_req);
LOCAL void psi_dti_dti_connect_res (T_DTI2_CONNECT_RES   
                                            *dti2_connect_res);
LOCAL void psi_dti_dti_disconnect_req (T_DTI2_DISCONNECT_REQ   
                                            *dti2_disconnect_req);
#ifdef _SIMULATION_
LOCAL const void psi_dti_dti_data_test_req (T_DTI2_DATA_TEST_REQ
                                            *dti_data_test_req);
LOCAL const void psi_dti_dti_data_test_ind (T_DTI2_DATA_TEST_IND
                                              *dti_data_test_ind);
#endif /*_SIMULATION_*/


/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_connect_cnf
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI2_CONNECT_CNF.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_dti_dti_connect_cnf (T_DTI2_CONNECT_CNF   *dti2_connect_cnf)
{
  TRACE_FUNCTION ("psi_dti_dti_connect_cnf");

  dti_dti_connect_cnf (psi_data->hDTI, dti2_connect_cnf);
}


/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_connect_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI2_CONNECT_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_dti_dti_connect_ind (T_DTI2_CONNECT_IND   *dti2_connect_ind)
{
  TRACE_FUNCTION ("psi_dti_dti_connect_ind");

  dti_dti_connect_ind (psi_data->hDTI, dti2_connect_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_disconnect_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI2_DISCONNECT_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_dti_dti_disconnect_ind (T_DTI2_DISCONNECT_IND   *dti2_disconnect_ind)
{
  TRACE_FUNCTION ("psi_dti_dti_disconnect_ind");

  dti_dti_disconnect_ind (psi_data->hDTI, dti2_disconnect_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_getdata_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_GETDATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/

LOCAL void psi_dti_dti_getdata_req (T_DTI2_GETDATA_REQ   *dti2_getdata_req)
{
  TRACE_FUNCTION ("psi_dti_dti_getdata_req");

  dti_dti_getdata_req (psi_data->hDTI, dti2_getdata_req);
}

/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_data_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_dti_dti_data_req (T_DTI2_DATA_REQ   *dti2_data_req)
{
  TRACE_FUNCTION ("psi_dti_dti_data_req");

  dti_dti_data_req (psi_data->hDTI, dti2_data_req);
}

/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_data_test_req
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
LOCAL const void psi_dti_dti_data_test_req (T_DTI2_DATA_TEST_REQ   
                                              *dti2_data_test_req)
{
  TRACE_FUNCTION ("psi_dti_dti_data_test_req");

  dti_dti_data_test_req (psi_data->hDTI, dti2_data_test_req);
}
#endif /*_SIMULATION_*/

/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_data_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_dti_dti_data_ind (T_DTI2_DATA_IND   *dti2_data_ind)
{
  TRACE_FUNCTION ("psi_dti_dti_data_ind");

  dti_dti_data_ind (psi_data->hDTI, dti2_data_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_ready_ind
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_READY_IND.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_dti_dti_ready_ind (T_DTI2_READY_IND   *dti2_ready_ind)
{
  TRACE_FUNCTION ("psi_dti_dti_ready_ind");

  dti_dti_ready_ind (psi_data->hDTI, dti2_ready_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_data_test_ind
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
LOCAL const void psi_dti_dti_data_test_ind (T_DTI2_DATA_TEST_IND   *dti2_data_test_ind)
{
  TRACE_FUNCTION ("psi_dti_dti_data_test_ind");

  dti_dti_data_test_ind (psi_data->hDTI, dti2_data_test_ind);
}
#endif /*_SIMULATION_*/

/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_connect_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_dti_dti_connect_req (T_DTI2_CONNECT_REQ   *dti2_connect_req)
{
  TRACE_FUNCTION ("psi_dti_dti_connect_req");

  dti_dti_connect_req (psi_data->hDTI, dti2_connect_req);
}
/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_connect_res
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_dti_dti_connect_res (T_DTI2_CONNECT_RES   *dti2_connect_res)
{
  TRACE_FUNCTION ("psi_dti_dti_connect_res");

  dti_dti_connect_res (psi_data->hDTI, dti2_connect_res);
}
/*
+------------------------------------------------------------------------------
| Function    : psi_dti_dti_disconnect_req
+------------------------------------------------------------------------------
| Description :  This function handles primitive DTI_DTI_DATA_REQ.
|
| Parameters  : received primitive
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void psi_dti_dti_disconnect_req (T_DTI2_DISCONNECT_REQ   *dti2_disconnect_req)
{
  TRACE_FUNCTION ("psi_dti_dti_disconnect_req");
  
  dti_dti_disconnect_req (psi_data->hDTI, dti2_disconnect_req);
}


/*
 * Jumptables to primitive handler functions. One table per SAP.
 *
 * Use MAK_FUNC_0 for primitives which contains no SDU.
 * Use MAK_FUNC_S for primitives which contains a SDU.
 */

/*
 * Function is needed for developing. This declaration can be removed
 * as soon as this function is no more called (i.e. all primitives are
 * handled).
 */
LOCAL void primitive_not_supported (void *data);

static const T_FUNC psi_table[] =
{
  /* Primitives */
  MAK_FUNC_0(psi_ker_conn_res, PSI_CONN_RES),                  /* 0x00 */
  MAK_FUNC_0(psi_ker_conn_rej, PSI_CONN_REJ),                  /* 0x01 */
  MAK_FUNC_0(psi_ker_close_req, PSI_CLOSE_REQ),                   /* 0x02 */
  MAK_FUNC_0(psi_ker_dti_open_req, PSI_DTI_OPEN_REQ),             /* 0x03 */
  MAK_FUNC_0(psi_ker_dti_close_req, PSI_DTI_CLOSE_REQ),           /* 0x04 */
#ifndef _SIMULATION_
  MAK_FUNC_0(psi_ker_setconf_req, PSI_SETCONF_REQ),               /* 0x05 */
#else
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x05 */
#endif /*_SIMULATION_*/
  MAK_FUNC_0(psi_ker_line_state_req, PSI_LINE_STATE_REQ),         /* 0x06 */

  /* Signals */
  MAK_FUNC_0(primitive_not_supported, PSI_SIG_CLEAR_IND),         /* 0x07 */
  MAK_FUNC_0(primitive_not_supported, PSI_SIG_FLUSH_IND),         /* 0x08 */
  MAK_FUNC_0(primitive_not_supported, PSI_SIG_READ_IND),          /* 0x09 */
  MAK_FUNC_0(primitive_not_supported, PSI_SIG_WRITE_IND),         /* 0x0A */
  MAK_FUNC_0(primitive_not_supported, PSI_SIG_CONNECT_IND),       /* 0x0B */
  MAK_FUNC_0(primitive_not_supported, PSI_SIG_DISCONNECT_IND)     /* 0x0C */
#ifdef _SIMULATION_
  /* Fill up until offset of 0x2B is reached for simulation primives */
  ,
  MAK_FUNC_0(psi_ker_setconf_req_test, PSI_SETCONF_REQ_TEST),     /* 0x0D */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x0E */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x0F */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x10 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x11 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x12 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x13 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x14 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x15 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x16 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x17 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x18 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x19 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x1A */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x1B */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x1C */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x1D */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x1E */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x1F */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x20 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x21 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x22 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x23 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x24 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x25 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x26 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x27 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x28 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x29 */
  MAK_FUNC_N(primitive_not_supported, 0),                         /* 0x2A */
#ifdef BAT_SIMULATION_DIO_IL
  MAK_FUNC_N(primitive_not_supported, 0)                          /* 0x2B */
#else
  MAK_FUNC_0(psi_diosim_sign_ind, PSI_DIOSIM_SIGN_IND)            /* 0x2B */
#endif /* BAT_SIMULATION_DIO_IL */
#endif /*_SIMULATION_*/
};


/*
 * Jumptable for the DTI service access point for downlink data transmission.
 * Contains the processing-function addresses and opcodes of
 * indication and confirm primitives. Use of DTILIB can be selected.
 * 
 */

LOCAL const T_FUNC ddl_table_dti[] = {

  MAK_FUNC_0(psi_dti_dti_connect_ind,    DTI2_CONNECT_IND),      /* 7750 */
  MAK_FUNC_0(psi_dti_dti_connect_cnf,    DTI2_CONNECT_CNF),      /* 7751 */
  MAK_FUNC_0(psi_dti_dti_disconnect_ind, DTI2_DISCONNECT_IND),   /* 7752 */
  MAK_FUNC_0(psi_dti_dti_ready_ind,      DTI2_READY_IND),        /* 7753 */
  MAK_FUNC_0(psi_dti_dti_data_ind,       DTI2_DATA_IND)          /* 7754 */

#if defined (_SIMULATION_)
  ,
  MAK_FUNC_S(psi_dti_dti_data_test_ind, DTI2_DATA_TEST_IND)      /* 7755 */
#endif /*_SIMULATION_*/

};

/*
 * Jumptable for the DTI service access point for uplink data transmission.
 * Contains the processing-function addresses and opcodes of
 * request and response primitives.
 */


LOCAL const T_FUNC dul_table_dti[] = {

  MAK_FUNC_0(psi_dti_dti_connect_req, DTI2_CONNECT_REQ),       /*0x3750*/
  MAK_FUNC_0(psi_dti_dti_connect_res, DTI2_CONNECT_RES),       /*0x3751*/
  MAK_FUNC_0(psi_dti_dti_disconnect_req, DTI2_DISCONNECT_REQ), /*0x3752*/
  MAK_FUNC_0(psi_dti_dti_getdata_req, DTI2_GETDATA_REQ),       /*0x3753*/
  MAK_FUNC_0(psi_dti_dti_data_req, DTI2_DATA_REQ)              /*0x3754*/

#if defined (_SIMULATION_)
  ,
  MAK_FUNC_S(psi_dti_dti_data_test_req, DTI2_DATA_TEST_REQ),   /*0x3755*/
  MAK_FUNC_N(primitive_not_supported, DTI2_DUMMY_REQ)          /*0x3756*/
#endif /* _SIMULATION_ */
};

#if !defined(DTILIB) OR !defined(DTI2)
#error "PSI requires DTILIB and DTI2"
#endif


/*==== DIAGNOSTICS ==========================================================*/
#ifdef _DEBUG
#endif /* _DEBUG */

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


/*
+------------------------------------------------------------------------------
| Function    : psi_dti_sign_callback
+------------------------------------------------------------------------------
| Description : callback function for dti lib.
|
| Parameters  : 
|
+------------------------------------------------------------------------------
*/

LOCAL void psi_dti_sign_callback  (U8 instance, 
                                   U8 interfac, 
                                   U8 channel, 
                                   U8 reason, 
                                   T_DTI2_DATA_IND *dti2_data_ind)
{
  TRACE_FUNCTION("psi_dti_sign_callback");
    
  if(psi_ker_instance_switch(instance) EQ PSI_INST_OK)
  {
    switch(reason)
    {
      case DTI_REASON_CONNECTION_OPENED:
        /*
         * set rx and tx state machine of the given interface to idle state
         */
        /*
         * The nsapi is given here with the parameter 'channel'.
         */
        TRACE_EVENT("DTI_REASON_CONNECTION_OPENED");
        psi_ker_dti_reason_connection_opened();
        break;
      case DTI_REASON_CONNECTION_CLOSED:
        /*
         * set the given interface to closed state
         */
        TRACE_EVENT("DTI_REASON_CONNECTION_CLOSED");
        psi_ker_dti_reason_connection_closed();
        break;
      case DTI_REASON_DATA_RECEIVED:
        /*
         * process received data
         */
        TRACE_EVENT("DTI_REASON_DATA_RECEIVED");
        psi_drx_dti_reason_data_received(dti2_data_ind);
        break;
      case DTI_REASON_TX_BUFFER_FULL:
        /*
         * set tx state machine of the given interface to TX_IDLE state
         */
        TRACE_EVENT("DTI_REASON_TX_BUFFER_FULL");
        psi_dtx_dti_reason_tx_buffer_full();
        break;
      case DTI_REASON_TX_BUFFER_READY:
        /*
         * set tx state machine of the given interface to TX_READY state
         */
        TRACE_EVENT("DTI_REASON_TX_BUFFER_READY");
        psi_dtx_dti_reason_tx_buffer_ready();
        break;
      default:
        TRACE_ERROR("psi_sig_callback called with undefined reason");
    }
  }
  else
  {
    TRACE_ERROR("psi_dti_sign_callback: wrong instance");
  }
}


/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
|       Function        : pei_primitive
+------------------------------------------------------------------------------
|       Description     : This function is called by the frame when a primitive
|                         is received and needs to be processed.
|
|                            |        |
|                           ACI      DTI                 UPLINK
|                            |        |
|                   +--------v--------v--------+
|                   |                          |
|                   |           PSI            |
|                   |                          |
|                   +--------------------------+
|
|
|       Parameters      :       prim      - Pointer to the received primitive
|
|       Return          :       PEI_OK    - function succeeded
|                               PEI_ERROR - function failed
|
+------------------------------------------------------------------------------
*/


LOCAL SHORT pei_primitive (void * primptr)
{
  TRACE_FUNCTION ("pei_primitive");

  if (primptr NEQ NULL)
  {
    T_PRIM* prim = (T_PRIM*)primptr;
    ULONG   opc  = prim->custom.opc;
    USHORT  n;
    const T_FUNC* table;

    /*
     * This must be called for Partition Pool supervision. Will be replaced
     * by another macro some time.
     */
    VSI_PPM_REC (&prim->custom, __FILE__, __LINE__);
    PTRACE_IN (opc);

    switch (SAP_NR(opc))
    {
      case SAP_NR(PSI_UL):
        table = psi_table;
        n = TAB_SIZE (psi_table);
        break;
      case SAP_NR(DTI2_UL):
        table = dul_table_dti;
        n = TAB_SIZE (dul_table_dti);
        /* 
         * to be able to distinguish DTI1/DTI2 opcodes,
         * the ones for DTI2 start at 0x50
         */
        opc -= 0x50;
        break;
      case DTI2_DL:
        table = ddl_table_dti;
        n = TAB_SIZE (ddl_table_dti);
        /* 
         * to be able to distinguish DTI1/DTI2 opcodes,
         * the ones for DTI2 start at 0x50
         */
        opc -= 0x50;
        break;
      default:
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
| Function    : psi_dio_sign_callback
+------------------------------------------------------------------------------
| Description : The function psi_dio_sign_callback() is the callback function
|               of the DIO driver to indicate events of the driver. This 
|               function is called in interrupt context. It converts the given 
|               opcode to a signal.
|
| Parameters  : pointer to signal
|
+------------------------------------------------------------------------------
*/
GLOBAL void psi_dio_sign_callback (T_DRV_SIGNAL *SigPtr)
{
  
  U32 devId = 0;

  TRACE_FUNCTION( "psi_dio_sign_callback" );

  devId = (U32) *SigPtr->UserData;

  /*
   * activate signal
   */
  switch (SigPtr->SignalType)
  {
    case DRV_SIGTYPE_READ:
      PSIGNAL(hCommPSI, PSI_SIG_READ_IND, devId);
      break;

    case DRV_SIGTYPE_WRITE:
      PSIGNAL(hCommPSI, PSI_SIG_WRITE_IND, devId);
      break;

    case DRV_SIGTYPE_CLEAR:
      PSIGNAL(hCommPSI, PSI_SIG_CLEAR_IND, devId);
      break;
    
    case DRV_SIGTYPE_FLUSH:
      PSIGNAL(hCommPSI, PSI_SIG_FLUSH_IND, devId);
      break;

    case DRV_SIGTYPE_CONNECT:
      PSIGNAL(hCommPSI, PSI_SIG_CONNECT_IND, devId);
      break;

    case DRV_SIGTYPE_DISCONNECT:
      PSIGNAL(hCommPSI, PSI_SIG_DISCONNECT_IND, devId);
      break;

    default:
      TRACE_ERROR( "psi_dio_sign_callback: unknown signal" );
      break;
  }
} /* psi_dio_sign_callback() */


/*
+------------------------------------------------------------------------------
|       Function        : pei_init
+------------------------------------------------------------------------------
|       Description     : This function is called by the frame. It is used to
|                         initialise the entitiy.
|
|       Parameters      : handle    - task handle
|
|       Return          : PEI_OK    - entity initialised
|                         PEI_ERROR - entity not (yet) initialised
|
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_init (T_HANDLE handle)
{
  USHORT  i = 0;
  U16     ret = 0;
  DTI_HANDLE hDTI = NULL;

  /*
   * Initialize task handle
   */
   PSI_handle = handle;

   TRACE_FUNCTION ("psi_pei_init");
   
  /*
   * Open communication channel
   */
  if (hCommMMI < VSI_OK)
  {
    if ((hCommMMI = vsi_c_open (VSI_CALLER ACI_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

  if (hCommPSI < VSI_OK)
  {
    if ((hCommPSI = vsi_c_open (VSI_CALLER PSI_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

  /*
   * Init DTI lib.
   */
  hDTI =  dti_init(PSI_INSTANCES, /* U8 maximum_links */
                   PSI_handle, /* T_HANDLE handle, */
                   DTI_DEFAULT_OPTIONS, /* U32 */
                   psi_dti_sign_callback);

  /*
   * Initialize entity data (call init function of every service)
   * the order of function calls is important
   */
  for( i = 0; i < PSI_INSTANCES; i++ )
  {
    psi_data = &(psi_data_base[i]);
    psi_ker_init();
    psi_rx_init();
    psi_tx_init();
    psi_drx_init();
    psi_dtx_init();

    psi_data->used = FALSE;
    psi_data->instance = (U8)i;
    psi_data->device_no = 0;

    psi_data->hDTI = hDTI;
  }
  psi_data = &(psi_data_base[0]);

  /* initialize DIO interface layer */
  dio_init(); 
  
  /* registration of user PSI in DIO */
  ret = dio_user_init((U32)PSI_NAME, (U16)PSI_handle, psi_dio_sign_callback);
  switch (ret)
  {
    case (DRV_NOTCONFIGURED):
      TRACE_ERROR("psi_pei_init(): dio_user_init returned DRV_NOTCONFIGURED");
      break;
    case (DRV_INVALID_PARAMS):
      TRACE_ERROR("psi_pei_init(): returned DRV_INVALID_PARAMS");
      break;
    case (DRV_OK):
      break;
    default:
      break;
  }
  return (PEI_OK);
}


/*
+------------------------------------------------------------------------------
|       Function        : pei_timeout
+------------------------------------------------------------------------------
|       Description     : This function is called by the frame when a timer
|                         has expired.
|
|       Parameters      : index             - timer index
|
|       Return          : PEI_OK            - timeout processed
|                         PEI_ERROR         - timeout not processed
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_timeout (USHORT index)
{
  TRACE_FUNCTION ("pei_timeout");

  /* There are no timers in PSI, so no timers to be handled */
  TRACE_ERROR("pei_timeout: Unknown Timeout");

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|       Function        : pei_signal
+------------------------------------------------------------------------------
|       Description     : This function is called by the frame when a signal
|                         has been received.
|
|       Parameters      : opc               - signal operation code
|                         *data             - pointer to primitive
|
|       Return          : PEI_OK            - signal processed
|                         PEI_ERROR         - signal not processed
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_signal (ULONG opc, void *data)
{
  U32 devId = (U32) data; /*lint !e507 Size incompatibility*/

  TRACE_FUNCTION ("pei_signal");

  /*
   * Process signal
   */
  switch (opc)
  {
    case PSI_SIG_CONNECT_IND:
      TRACE_EVENT ("PSI_SIG_CONNECT_IND");
      psi_ker_sig_connect_ind(devId);
      break;

    case PSI_SIG_DISCONNECT_IND:
      TRACE_EVENT ("PSI_SIG_DISCONNECT_IND");
      psi_ker_sig_disconnect_ind(devId);
      break;

    case PSI_SIG_WRITE_IND:
      TRACE_EVENT ("PSI_SIG_WRITE_IND");    
      psi_tx_sig_write_ind(devId);
      break;

    case PSI_SIG_READ_IND:
      TRACE_EVENT ("PSI_SIG_READ_IND");
      psi_tx_sig_read_ind(devId);

#ifndef _SIMULATION_
	if (usbf_psi_use_rx_mutex) {
		usbf_psi_use_rx_mutex = 0;
		vsi_s_release(0, usbf_psi_rx_mutex);
	}
#endif	
      break;

    case PSI_SIG_FLUSH_IND:
      TRACE_EVENT ("PSI_SIG_FLUSH_IND");
      psi_tx_sig_flush_ind(devId);
      break;
      
    case PSI_SIG_CLEAR_IND:
      TRACE_EVENT("currently SIG_CLEAR_IND is not handled");;
      break;

    default:
      TRACE_ERROR("Unknown Signal OPC");
      return PEI_ERROR;
  }

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|       Function        : pei_exit
+------------------------------------------------------------------------------
|       Description     : This function is called by the frame when the entity
|                         is terminated. All open resources are freed.
|
|       Parameters      : -
|
|       Return          : PEI_OK            - exit sucessful
|                         PEI_ERROR         - exit not sueccessful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit");
  
  /*Close communication channel to MMI */  
  vsi_c_close (VSI_CALLER hCommMMI);
  hCommMMI = VSI_ERROR;
  
  /* close the communication between user PSI and DIO interface */
  dio_user_exit((U32)PSI_NAME);

   /* deallocate the resources of the  DIO interface 
   dio_exit();*/ 

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|       Function        : pei_run
+------------------------------------------------------------------------------
|       Description     : This function is called by the frame when entering
|                         the main loop. This function is only required in the
|                         active variant.
|
|               This function is not used.
|
|       Parameters      : handle            - Communication handle
|
|       Return          : PEI_OK            - sucessful
|                         PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle )
{

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|       Function        : pei_config
+------------------------------------------------------------------------------
|       Description     : This function is called by the frame when a primitive
|                         is received indicating dynamic configuration.
|
|               This function is not used in this entity.
|
|       Parameters      : handle            - Communication handle
|
|       Return          : PEI_OK            - sucessful
|                         PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_config (char *inString)
{
  TRACE_FUNCTION ("pei_config");
/*  TRACE_EVENT_P1 ("%s", inString);*/

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|       Function        : pei_monitor
+------------------------------------------------------------------------------
|       Description     : This function is called by the frame in case sudden
|                         entity specific data is requested
|                         (e.g. entity Version).
|
|       Parameters      : out_monitor - return the address of the data to be
|                                       monitoredCommunication handle
|
|       Return          : PEI_OK      - sucessful 
|                                       (address in out_monitor is valid)
|                         PEI_ERROR   - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  TRACE_FUNCTION ("pei_monitor");

  /*
   * Version = "0.S" (S = Step).
   */
  psi_mon.version = VERSION_PSI;
  *out_monitor = &psi_mon;

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|       Function        : pei_create
+------------------------------------------------------------------------------
|       Description     : This function is called by the frame when the process
|                         is created.
|
|       Parameters      : out_name          - Pointer to the buffer in which to
|                                             locate the name of this entity
|
|       Return          : PEI_OK            - entity created successfuly
|                         PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{
static T_PEI_INFO pei_info =
              {
               PSI_NAME,      /* name */
               {                /* pei-table */
                 pei_init,
                 pei_exit,
                 pei_primitive,
                 pei_timeout,
                 pei_signal,
                 pei_run,
                 pei_config,
                 pei_monitor
               },
               2048,            /* stack size */
               10,              /* queue entries */
               200,             /* priority (1->low, 255->high) */
               0, /* number of timers */
               0x03 | PRIM_NO_SUSPEND | TRC_NO_SUSPEND /* flags: bit 0   active(0) body/passive(1) */
              };                /*        bit 1   com by copy(0)/reference(1) */


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


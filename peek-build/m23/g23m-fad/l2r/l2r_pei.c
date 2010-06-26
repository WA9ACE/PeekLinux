/* 
+----------------------------------------------------------------------------- 
|  Project :  CSD (8411)
|  Modul   :  L2r_pei.c
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
|  Purpose :  This Modul defines the process body interface
|             for the component L2R of the base station
+----------------------------------------------------------------------------- 
*/ 

#ifndef L2R_PEI_C
#define L2R_PEI_C
#endif

#define ENTITY_L2R
#define ENTITY_TRA

/*==== INCLUDES ===================================================*/

#include <string.h>

#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "tools.h"
#include "cus_l2r.h"
#include "cnf_l2r.h"
#include "mon_l2r.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"      /* functionality of the dti library */

#include "cl_ribu.h"
#include "l2r.h"
#include "tra_pei.h"

/*==== CONST ======================================================*/
/*==== VAR EXPORT =================================================*/

GLOBAL T_MODE l2r_data_mode = NONTRANSP;     /* = default */

GLOBAL T_TRA_DATA *tra_data;
GLOBAL T_TRA_DATA *tra_data_first_elem;
GLOBAL DTI_HANDLE l2r_hDTI;             /* DTI connection for DTI library */

GLOBAL T_HANDLE l2r_handle;
GLOBAL T_HANDLE hCommCTRL = VSI_ERROR;  /* Controller Communication */
GLOBAL T_HANDLE hCommRLP  = VSI_ERROR;  /* RLP Communication */

#ifdef _SIMULATION_
GLOBAL T_HANDLE hCommRA   = VSI_ERROR;  /* RA Communication */
#endif

LOCAL T_HANDLE hCommL2R  = VSI_ERROR;   /* L2R Communication (used by relay entity) */

/*
 * instance data base
 */
GLOBAL T_L2R_DATA l2r_data_base [L2R_INSTANCES], *l2r_data;
GLOBAL ULONG      l2r_data_magic_num;

/*==== VAR LOCAL ==================================================*/

LOCAL BOOL              first_access = TRUE;
LOCAL T_MONITOR         l2r_mon;

/*
+------------------------------------------------------------------------------
|  Function    : pei_not_supported
+------------------------------------------------------------------------------
|  Description : An unsupported primitive is received.
|
|  Parameters  : data -
|
|
|  Return      : -
+------------------------------------------------------------------------------
*/


LOCAL void pei_not_supported (void *data)
{
  TRACE_FUNCTION ("pei_not_supported()");
  PFREE (data);
}

/*
 *
 * Use MAK_FUNC_0 for primitives which contains no SDU.
 *
 * Use MAK_FUNC_S for primitives which contains a SDU.
 */

/*
 * jumptable for the entity service access point. Contains
 * the processing-function addresses and opcodes of
 * request and response primitives.
 *
 */

LOCAL const T_FUNC l2r_table[] = {
  MAK_FUNC_0( mgt_l2r_activate_req   ,     L2R_ACTIVATE_REQ   ),
  MAK_FUNC_0( mgt_l2r_deactivate_req ,     L2R_DEACTIVATE_REQ ),
  MAK_FUNC_0( mgt_l2r_connect_req    ,     L2R_CONNECT_REQ    ),
  MAK_FUNC_0( mgt_l2r_disc_req       ,     L2R_DISC_REQ       ),
  MAK_FUNC_N( pei_not_supported      ,     0                  ),
  MAK_FUNC_N( pei_not_supported      ,     0                  ),
  MAK_FUNC_N( pei_not_supported      ,     0                  ),
  MAK_FUNC_0( mgt_l2r_dti_req        ,     L2R_DTI_REQ        ) 
};

LOCAL const T_FUNC tra_table[] = {
  MAK_FUNC_0( mgt_tra_activate_req   ,     TRA_ACTIVATE_REQ   ),
  MAK_FUNC_0( mgt_tra_deactivate_req ,     TRA_DEACTIVATE_REQ ),
  MAK_FUNC_0( mgt_tra_dti_req        ,     TRA_DTI_REQ        ) 
};

LOCAL void pei_dti_connect_req(T_DTI2_CONNECT_REQ *dti_connect_req);
LOCAL void pei_dti_connect_cnf(T_DTI2_CONNECT_CNF *dti_connect_cnf);
LOCAL void pei_dti_connect_ind(T_DTI2_CONNECT_IND *dti_connect_ind);
LOCAL void pei_dti_connect_res(T_DTI2_CONNECT_RES *dti_connect_res);
LOCAL void pei_dti_disconnect_req(T_DTI2_DISCONNECT_REQ *dti_disconnect_req);
LOCAL void pei_dti_disconnect_ind(T_DTI2_DISCONNECT_IND *dti_disconnect_ind);
LOCAL void pei_dti_data_req(T_DTI2_DATA_REQ *dti_data_req);
LOCAL void pei_dti_getdata_req(T_DTI2_GETDATA_REQ *dti_getdata_req);
LOCAL void pei_dti_data_ind(T_DTI2_DATA_IND *dti_data_ind);
LOCAL void pei_dti_ready_ind(T_DTI2_READY_IND *dti_ready_ind);
LOCAL void pei_dti_data_test_req(T_DTI2_DATA_TEST_REQ *dti_data_test_req);
LOCAL void pei_dti_data_test_ind(T_DTI2_DATA_TEST_IND *dti_data_test_ind);

LOCAL const T_FUNC dti_ul_table[] = {
  MAK_FUNC_0( pei_dti_connect_req     ,    DTI2_CONNECT_REQ    ),
  MAK_FUNC_0( pei_dti_connect_res     ,    DTI2_CONNECT_RES    ),
  MAK_FUNC_0( pei_dti_disconnect_req  ,    DTI2_DISCONNECT_REQ ),
  MAK_FUNC_0( pei_dti_getdata_req     ,    DTI2_GETDATA_REQ    ),
  MAK_FUNC_0( pei_dti_data_req        ,    DTI2_DATA_REQ       ) 
#ifdef _SIMULATION_
  ,
  MAK_FUNC_S( pei_dti_data_test_req     ,   DTI2_DATA_TEST_REQ )
#endif  /* _SIMULATION_ */
};

LOCAL const T_FUNC dti_dl_table[] = {
  MAK_FUNC_0( pei_dti_connect_ind     ,    DTI2_CONNECT_IND    ),
  MAK_FUNC_0( pei_dti_connect_cnf     ,    DTI2_CONNECT_CNF    ),
  MAK_FUNC_0( pei_dti_disconnect_ind  ,    DTI2_DISCONNECT_IND ),
  MAK_FUNC_0( pei_dti_ready_ind       ,    DTI2_READY_IND      ),
  MAK_FUNC_0( pei_dti_data_ind        ,    DTI2_DATA_IND       ) 
#ifdef _SIMULATION_
  ,
  MAK_FUNC_S( pei_dti_data_test_ind   ,    DTI2_DATA_TEST_IND )
#endif  /* _SIMULATION_ */
};

LOCAL const T_FUNC ra_table[] = {
  MAK_FUNC_0( rcv_ra_ready_ind    ,        RA_READY_IND     ),
  MAK_FUNC_S( rcv_ra_data_ind     ,        RA_DATA_IND      ),
};

LOCAL const T_FUNC rlp_table[] = {
  MAK_FUNC_0( mgt_rlp_detach_cnf     ,     RLP_DETACH_CNF     ),
  MAK_FUNC_0( mgt_rlp_connect_ind    ,     RLP_CONNECT_IND    ),
  MAK_FUNC_0( mgt_rlp_connect_cnf    ,     RLP_CONNECT_CNF    ),
  MAK_FUNC_0( mgt_rlp_disc_ind       ,     RLP_DISC_IND       ),
  MAK_FUNC_0( mgt_rlp_disc_cnf       ,     RLP_DISC_CNF       ),
  MAK_FUNC_0( mgt_rlp_reset_ind      ,     RLP_RESET_IND      ),
  MAK_FUNC_0( mgt_rlp_reset_cnf      ,     RLP_RESET_CNF      ),
  MAK_FUNC_0( up_rlp_ready_ind       ,     RLP_READY_IND      ),
  MAK_FUNC_S( dn_rlp_data_ind        ,     RLP_DATA_IND       ),
  MAK_FUNC_S( mgt_rlp_ui_ind         ,     RLP_UI_IND         ),
  MAK_FUNC_0( mgt_rlp_xid_ind        ,     RLP_XID_IND        ),
  MAK_FUNC_0( mgt_rlp_error_ind      ,     RLP_ERROR_IND      ),
  MAK_FUNC_0( mgt_rlp_statistic_ind  ,     RLP_STATISTIC_IND  ) 
};

/*
+------------------------------------------------------------------------------
|  Function    : pei_primitive
+------------------------------------------------------------------------------
|  Description : Process protocol specific primitive.
|
|  Parameters  : ptr -
|
|
|  Return      : PEI_ERROR -
|                PEI_OK    -
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_primitive (void * ptr)
{
  T_PRIM * prim = ptr;
  /*
   * @ADAPT@
   *                    |          |
   *                    Controller/UL       UPPER LAYER
   *                    |          |
   *                    v          v
   *      +-----------(l2r)------(tra)----+
   *      |                   |           |
   *      |            L2R    |   TRA     |
   *      |                   |           |
   *      +-------------^----------^------+
   *                    |          |
   *                   RLP        RA        LOWER LAYER
   *                    |          |
   *
   */

  TRACE_FUNCTION ("pei_primitive()");

  if (prim NEQ NULL)
  {
    ULONG            opc = prim->custom.opc;
    USHORT           n;
    const T_FUNC    *table;

    VSI_PPM_REC ((T_PRIM_HEADER *)prim, __FILE__, __LINE__);

    PTRACE_IN (opc);
    l2r_data = GET_INSTANCE (*primData);

    switch (SAP_NR(opc))
    {
      case SAP_NR(L2R_UL): table =  l2r_table;      n = TAB_SIZE (l2r_table); break;
      case SAP_NR(TRA_UL): table =  tra_table;      n = TAB_SIZE (tra_table); break;
      case SAP_NR(RLP_DL): table =  rlp_table;      n = TAB_SIZE (rlp_table); break;
      case DTI2_UL:
        {
          table = dti_ul_table;			n = TAB_SIZE (dti_ul_table);
          /*
           * to be able to distinguish DTI1/DTI2 opcodes,
           * the ones for DTI2 start at 0x50
           */
          opc -= 0x50;
        }
        break;
      case DTI2_DL:
        {
          table = dti_dl_table;     n = TAB_SIZE (dti_dl_table);
          /*
           * to be able to distinguish DTI1/DTI2 opcodes,
           * the ones for DTI2 start at 0x50
           */
          opc -= 0x50;
        }
        break;
      case SAP_NR(RA_DL ): table =  ra_table;       n = TAB_SIZE (ra_table);  break;
      default    : table =  NULL;           n = 0;               break;
    }

    if (table != NULL )
    {
      if (PRIM_NR(opc) < n)
      {
        table += PRIM_NR(opc);
#ifdef PALLOC_TRANSITION
        P_SDU(prim) = table->soff ? (T_sdu*) (((char*)&prim->data) + table->soff) : 0;
        P_LEN(prim) = table->size + sizeof (T_PRIM_HEADER);
#endif /* PALLOC_TRANSITION */
        JUMP (table->func) (P2D(prim));
      }
      else
      {
        pei_not_supported (P2D(prim));
      }
      return PEI_OK;
    }
    /*
     * Primitive is no GSM Primitive
     * then forward to the environment
     */

#ifdef GSM_ONLY
    PFREE (P2D(prim))

    return PEI_ERROR;
#else
    if (opc & SYS_MASK)
      vsi_c_primitive (VSI_CALLER prim);
    else
    {
      PFREE (P2D(prim));
      return PEI_ERROR;
    }
#endif /* GSM_ONLY */
  }
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|    Function: pei_sig_callback
+------------------------------------------------------------------------------
|    PURPOSE : Callback function for DTILIB
+------------------------------------------------------------------------------
*/

LOCAL void pei_sig_callback(U8 instance, U8 interfac, U8 channel,
                             U8 reason, T_DTI2_DATA_IND *dti_data_ind)
{
  TRACE_FUNCTION("pei_sig_callback");

  if (l2r_data_mode EQ NONTRANSP)
  {

#ifdef _SIMULATION_
    if (interfac NEQ L2R_DTI_UP_INTERFACE || channel NEQ L2R_DTI_UP_CHANNEL)
    {
      TRACE_ERROR("[PEI_SIG_CALLBACK] channel or interface not valid!");
      return; /* error, not found */
    }
#endif /* _SIMULATION_ */

    if (l2r_hDTI NEQ D_NO_DATA_BASE)
    {
      switch (reason)
      {
        case DTI_REASON_CONNECTION_OPENED:
          sig_dti_mgt_connection_opened_ind();
          break;

        case DTI_REASON_CONNECTION_CLOSED:
          sig_dti_mgt_connection_closed_ind();
          break;

        case DTI_REASON_DATA_RECEIVED:
          PACCESS (dti_data_ind);
          {
          PPASS (dti_data_ind, dti_data_req, DTI2_DATA_REQ);
          sig_dti_up_data_received_ind(dti_data_req);
          }
          break;

        case DTI_REASON_TX_BUFFER_FULL:
          sig_dti_dn_tx_buffer_full_ind();
          break;

        case DTI_REASON_TX_BUFFER_READY:
          sig_dti_dn_tx_buffer_ready_ind();
          break;

        default:
          TRACE_ERROR("unknown DTILIB reason parameter");
          break;
      }
    }
    else
    {
      TRACE_ERROR("Pointer to DTILIB database not existing");
    }
  }
  else if (l2r_data_mode EQ TRANSP)
  {

#ifdef _SIMULATION_
    if (interfac NEQ TRA_DTI_UP_INTERFACE || channel NEQ TRA_DTI_UP_CHANNEL)
    {
      TRACE_ERROR("[PEI_SIG_CALLBACK] channel or interface not valid!");
      return; /* error, not found */
    }
#endif /* _SIMULATION_ */

    if (l2r_hDTI NEQ D_NO_DATA_BASE)
    {
      switch (reason)
      {
        case DTI_REASON_CONNECTION_OPENED:
          sig_dti_tra_mgt_connection_opened_ind();
          break;

        case DTI_REASON_CONNECTION_CLOSED:
          sig_dti_tra_mgt_connection_closed_ind();
          break;

        case DTI_REASON_DATA_RECEIVED:
          PACCESS (dti_data_ind);
          {
          PPASS (dti_data_ind, dti_data_req, DTI2_DATA_REQ);
          sig_dti_tra_up_data_received_ind(dti_data_req);
          }
          break;

        case DTI_REASON_TX_BUFFER_FULL:
          sig_dti_tra_dn_tx_buffer_full_ind();
          break;

        case DTI_REASON_TX_BUFFER_READY:
          sig_dti_tra_dn_tx_buffer_ready_ind();
          break;

        default:
          TRACE_ERROR("unknown DTILIB reason parameter");
          break;
      }
    }
    else
    {
      TRACE_ERROR("Pointer to DTILIB database not existing");
    }
  }
} /*lint !e715 instance, interfac, channel not referenced */

/*
+------------------------------------------------------------------------------
|  Function    : pei_init
+------------------------------------------------------------------------------
|  Description : Initialize Protocol Stack Entity
|
|  Parameters  : handle -
|
|
|  Return      : PEI_OK    -
|                PEI_ERROR -
+------------------------------------------------------------------------------
*/

  #define CTRL_NAME ACI_NAME

LOCAL SHORT pei_init (T_HANDLE handle)
{
  BOOL ret = PEI_OK;

  tra_data = NULL;

  l2r_handle = handle;

  TRACE_FUNCTION ("pei_init()");

  if (hCommCTRL < VSI_OK)
   {
     if ((hCommCTRL = vsi_c_open (VSI_CALLER CTRL_NAME)) < VSI_OK)
       return PEI_ERROR;
  }

  if (hCommRLP < VSI_OK)
  {
    if ((hCommRLP = vsi_c_open (VSI_CALLER RLP_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

  if (hCommL2R < VSI_OK)
  {
    if ((hCommL2R = vsi_c_open (VSI_CALLER L2R_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

#ifdef _SIMULATION_
  if (hCommRA < VSI_OK)
  {
    if ((hCommRA = vsi_c_open (VSI_CALLER RA_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#endif

  /*
   * initialize dtilib for this entity
   */
  l2r_hDTI = dti_init(L2R_INSTANCES, handle, DTI_DEFAULT_OPTIONS, pei_sig_callback);
  
  if(!l2r_hDTI)
    return PEI_ERROR;

  l2r_data = &l2r_data_base[0];
  l2r_data_magic_num = 0;                     /* memory is not yet initialized */
  INIT_STATE (DTI, DTI_CLOSED);
  mgt_init(&l2r_data->mgt);
  dn_init(&l2r_data->dn);
  up_init(&l2r_data->up);
  tra_data_first_elem = (T_TRA_DATA *) &l2r_data_base[0];
  return (ret);
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_timeout
+------------------------------------------------------------------------------
|  Description : Process timeout
|
|  Parameters  : index -
|
|
|  Return      : PEI_OK    -
|
+------------------------------------------------------------------------------
*/
EXTERN void tim_tup_snd (void);

LOCAL SHORT pei_timeout (USHORT index)
{
  if (index EQ TIMER_TUP_SND)
  {
    tim_tup_snd ();
  }
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_signal
+------------------------------------------------------------------------------
|  Description : Functional interface to signal a primitive.
|
|  Parameters  : opc      -
|                primData -
|
|
|  Return      : PEI_OK
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_signal (ULONG opc, void *primData)
{
#ifdef OPTION_SIGNAL

  l2r_data = GET_INSTANCE (prim);
  switch (opc)
  {
      case RA_READY_IND:
        rcv_ra_ready_ind
        (
          (T_RA_READY_IND *)primData
        );
        break;

      case RA_DATA_IND:
        rcv_ra_data_ind
        (
          (T_RA_DATA_IND *)primData
        );
        break;

      case RA_BREAK_IND:
        rcv_ra_break_ind
        (
          (T_RA_BREAK_IND *)primData
        );
        break;
  }
#endif      /* OPTION_SIGNAL */
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_exit
+------------------------------------------------------------------------------
|  Description : Close Resources and terminate
|
|  Parameters  : -
|
|
|  Return      : PEI_OK -
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit()");

  /*
   * clean up vsi communication
   */
  vsi_c_close (VSI_CALLER hCommCTRL);
  hCommCTRL = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommRLP);
  hCommRLP = VSI_ERROR;

#ifdef _SIMULATION_
  vsi_c_close (VSI_CALLER hCommRA);
  hCommRA  = VSI_ERROR;
#endif

  if (GET_STATE (DTI) NEQ DTI_CLOSED)
  {
    if (l2r_data_mode EQ NONTRANSP)
    {
      dti_close(l2r_hDTI, L2R_DTI_UP_DEF_INSTANCE, L2R_DTI_UP_INTERFACE, L2R_DTI_UP_CHANNEL, FALSE);
      SET_STATE (DTI, DTI_CLOSED);
    }
    else if (l2r_data_mode EQ TRANSP)
    {
      dti_close(l2r_hDTI, TRA_DTI_UP_INSTANCE, TRA_DTI_UP_INTERFACE, TRA_DTI_UP_CHANNEL, FALSE);
      SET_STATE (DTI, DTI_CLOSED);
    }
  }

  vsi_c_close (VSI_CALLER hCommL2R);
  hCommL2R = VSI_ERROR;

  /*
   * Shut down dtilib communication
   */
  dti_deinit(l2r_hDTI);
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_config
+------------------------------------------------------------------------------
|  Description : Dynamic Configuration
|
|  Parameters  : inString -
|
|
|  Return      : PEI_OK -
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_config (T_PEI_CONFIG inString)
{
#ifndef NCONFIG

  char *s = inString;

  const T_STR_IND tmr_name_to_ind[] =
  {
    { "TUP_SND", 1 },
    { NULL,      0 }
  };

  TRACE_FUNCTION ("pei_config()");
  TRACE_EVENT (s);

  if (ConfigTimer(VSI_CALLER inString, tmr_name_to_ind) EQ VSI_OK)
    return PEI_OK;
  else
    return PEI_ERROR;

#else /* NCONFIG */

  return PEI_OK;

#endif
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_monitor
+------------------------------------------------------------------------------
|  Description : Monitoring of physical Parameters
|
|  Parameters  : monitor -
|
|
|  Return      : PEI_OK;
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_monitor (void **monitor)
{
  TRACE_FUNCTION ("pei_monitor()");

  l2r_mon.version = VERSION_L2R;
  *monitor = &l2r_mon;
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_connect_req
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_connect_req
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_connect_req(T_DTI2_CONNECT_REQ *dti_connect_req)
{
  dti_dti_connect_req (l2r_hDTI, dti_connect_req);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_connect_cnf
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_connect_cnf
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_connect_cnf(T_DTI2_CONNECT_CNF *dti_connect_cnf)
{
  dti_dti_connect_cnf(l2r_hDTI, dti_connect_cnf);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_connect_ind
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_connect_ind
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_connect_ind(T_DTI2_CONNECT_IND *dti_connect_ind)
{
  dti_dti_connect_ind(l2r_hDTI, dti_connect_ind);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_connect_res
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_connect_res
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_connect_res(T_DTI2_CONNECT_RES *dti_connect_res)
{
  dti_dti_connect_res(l2r_hDTI, dti_connect_res);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_disconnect_req
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_disconnect_req
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_disconnect_req(T_DTI2_DISCONNECT_REQ *dti_disconnect_req)
{
  dti_dti_disconnect_req (l2r_hDTI, dti_disconnect_req);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_disconnect_ind
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_disconnect_ind
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_disconnect_ind(T_DTI2_DISCONNECT_IND *dti_disconnect_ind)
{
  dti_dti_disconnect_ind (l2r_hDTI, dti_disconnect_ind);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_data_req
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_data_req
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_data_req(T_DTI2_DATA_REQ *dti_data_req)
{
  dti_dti_data_req (l2r_hDTI, dti_data_req);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_getdata_req
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_getdata_req
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_getdata_req(T_DTI2_GETDATA_REQ *dti_getdata_req)
{
  dti_dti_getdata_req (l2r_hDTI, dti_getdata_req);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_data_ind
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_data_ind
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_data_ind(T_DTI2_DATA_IND *dti_data_ind)
{
  dti_dti_data_ind (l2r_hDTI, dti_data_ind);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_ready_ind
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_ready_ind
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_ready_ind(T_DTI2_READY_IND *dti_ready_ind)
{
  dti_dti_ready_ind (l2r_hDTI, dti_ready_ind);
}

#ifdef _SIMULATION_

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_data_test_req
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_data_test_req
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_data_test_req(T_DTI2_DATA_TEST_REQ *dti_data_test_req)
{
  dti_dti_data_test_req (l2r_hDTI, dti_data_test_req);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_data_test_ind
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_data_test_ind
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_data_test_ind(T_DTI2_DATA_TEST_IND *dti_data_test_ind)
{
  dti_dti_data_test_ind (l2r_hDTI, dti_data_test_ind);
}

#endif /* _SIMULATION_ */

/*
+------------------------------------------------------------------------------
|  Function    : pei_create
+------------------------------------------------------------------------------
|  Description : Create the Protocol Stack Entity
|
|  Parameters  : info   -
|
|
|  Return      : PEI_OK -
+------------------------------------------------------------------------------
*/

/*lint -e714 : Symbol not referenced */
/*lint -e765 : external could be made static */
GLOBAL T_PEI_RETURN l2r_pei_create (T_PEI_INFO **info)
{
  static const T_PEI_INFO pei_info =
  {
    "L2R",
    {
      pei_init,
      pei_exit,
      pei_primitive,
      pei_timeout,
      pei_signal,
      NULL,        /* no run function */
      pei_config,
      pei_monitor
    },
    1024,
    10,
    205,
    2,
#ifdef _TARGET_
    (PASSIVE_BODY|COPY_BY_REF|TRC_NO_SUSPEND|PRIM_NO_SUSPEND)
#else
    (PASSIVE_BODY|COPY_BY_REF)
#endif
  };

  TRACE_FUNCTION ("pei_create()");

  /*
   *  Close Resources if open
   */
  if (first_access)
    first_access = FALSE;
  else
    pei_exit ();

  /*
   *  Export Startup Configuration data
   */
  *info = (T_PEI_INFO *)&pei_info;

  return PEI_OK;
}

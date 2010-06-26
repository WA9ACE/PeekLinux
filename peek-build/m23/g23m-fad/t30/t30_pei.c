/*
+-----------------------------------------------------------------------------
|  Project :
|  Modul   :
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
|             for the entity T30 of the mobile station.
+-----------------------------------------------------------------------------
*/

#ifndef T30_PEI_C
#define T30_PEI_C
#endif

#define ENTITY_T30

/*==== INCLUDES ===================================================*/

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "pcm.h"
#include "vsi.h"
#include "macdef.h"
#include "pconst.cdg"
#include "mconst.cdg"
#include "message.h"
#include "ccdapi.h"
#include "custom.h"
#include "gsm.h"
#include "prim.h"
#include "cnf_t30.h"
#include "mon_t30.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"      /* functionality of the dti library */
#include "t30.h"

/*==== EXPORT =====================================================*/

GLOBAL DTI_HANDLE t30_hDTI;     /* DTI connection for DTI library */

/*==== PRIVATE ====================================================*/

/*==== VARIABLES ==================================================*/

LOCAL BOOL      first_access = TRUE;
LOCAL T_MONITOR t30_mon;

/*==== FUNCTIONS ==================================================*/

LOCAL void pei_dti_connect_req (T_DTI2_CONNECT_REQ*);
LOCAL void pei_dti_connect_cnf (T_DTI2_CONNECT_CNF*);
LOCAL void pei_dti_connect_ind (T_DTI2_CONNECT_IND*);
LOCAL void pei_dti_connect_res (T_DTI2_CONNECT_RES*);
LOCAL void pei_dti_disconnect_req (T_DTI2_DISCONNECT_REQ*);
LOCAL void pei_dti_disconnect_ind (T_DTI2_DISCONNECT_IND*);
LOCAL void pei_dti_data_req (T_DTI2_DATA_REQ*);
LOCAL void pei_dti_getdata_req (T_DTI2_GETDATA_REQ*);
LOCAL void pei_dti_data_ind (T_DTI2_DATA_IND*);
LOCAL void pei_dti_ready_ind (T_DTI2_READY_IND*);

#ifdef _SIMULATION_ /* DTI_DATA_TEST_REQ/IND */
LOCAL const void pei_dti_data_test_req (T_DTI2_DATA_TEST_REQ*);
LOCAL const void pei_dti_data_test_ind (T_DTI2_DATA_TEST_IND*);
#endif

LOCAL void pei_dti_callback(UBYTE instance,
                            UBYTE interfac,
                            UBYTE channel,
                            UBYTE reason,
                            T_DTI2_DATA_IND* dti_data_ind);
/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_PEI             |
| STATE   : code                       ROUTINE : pei_not_supported   |
+--------------------------------------------------------------------+

  PURPOSE : An unsupported primitive is received.

*/

LOCAL void pei_not_supported (void *data)
{
  TRACE_FUNCTION ("pei_not_supported()");

  PFREE (data);
}

/*
 * Use MAK_FUNC_0 for primitives which contains no SDU.
 * Use MAK_FUNC_S for primitives which contains a SDU.
 */

/*
 * jumptable for the entity service access point.
 * Contains the processing-function addresses and opcodes
 * of request and response primitives.
 */

LOCAL const T_FUNC dti_ul_table[] = {
  MAK_FUNC_0( pei_dti_connect_req   , DTI2_CONNECT_REQ    ),
  MAK_FUNC_0( pei_dti_connect_res   , DTI2_CONNECT_RES    ),
  MAK_FUNC_0( pei_dti_disconnect_req, DTI2_DISCONNECT_REQ ),
  MAK_FUNC_0( pei_dti_getdata_req   , DTI2_GETDATA_REQ    ),
  MAK_FUNC_0( pei_dti_data_req      , DTI2_DATA_REQ       )

#ifdef _SIMULATION_ /* DTI_DATA_TEST_REQ */
  ,
  MAK_FUNC_S( pei_dti_data_test_req , DTI2_DATA_TEST_REQ )
#endif
};

LOCAL const T_FUNC dti_dl_table[] = {
  MAK_FUNC_0( pei_dti_connect_ind    , DTI2_CONNECT_IND    ),
  MAK_FUNC_0( pei_dti_connect_cnf    , DTI2_CONNECT_CNF    ),
  MAK_FUNC_0( pei_dti_disconnect_ind , DTI2_DISCONNECT_IND ),
  MAK_FUNC_0( pei_dti_ready_ind      , DTI2_READY_IND      ),
  MAK_FUNC_0( pei_dti_data_ind       , DTI2_DATA_IND       )

#ifdef _SIMULATION_ /* DTI_DATA_TEST_IND */
  ,
  MAK_FUNC_S( pei_dti_data_test_ind  , DTI2_DATA_TEST_IND )
#endif
};

LOCAL const T_FUNC fad_table[] =
{
  MAK_FUNC_S(mux_fad_data_ind      , FAD_DATA_IND      ),
  MAK_FUNC_0(ker_fad_data_cnf      , FAD_DATA_CNF      ),
  MAK_FUNC_0(ker_fad_snd_tcf_cnf   , FAD_SND_TCF_CNF   ),
  MAK_FUNC_0(ker_fad_rcv_tcf_cnf   , FAD_RCV_TCF_CNF   ),
  MAK_FUNC_0(ker_fad_ready_ind     , FAD_READY_IND     ),
  MAK_FUNC_0(mux_fad_mux_ind       , FAD_MUX_IND       ),
  MAK_FUNC_0(ker_fad_error_ind     , FAD_ERROR_IND     ),
  MAK_FUNC_0(ker_fad_deactivate_cnf, FAD_DEACTIVATE_CNF),
  MAK_FUNC_0(ker_fad_activate_cnf  , FAD_ACTIVATE_CNF  )
};


LOCAL const T_FUNC t30_table[] =
{
  MAK_FUNC_0(ker_t30_activate_req   , T30_ACTIVATE_REQ  ),
  MAK_FUNC_0(ker_t30_config_req     , T30_CONFIG_REQ    ),
  MAK_FUNC_0(ker_t30_cap_req        , T30_CAP_REQ       ),
  MAK_FUNC_0(ker_t30_sgn_req        , T30_SGN_REQ       ),
  MAK_FUNC_0(ker_t30_modify_req     , T30_MODIFY_REQ    ),
  MAK_FUNC_0(ker_t30_deactivate_req , T30_DEACTIVATE_REQ),
  MAK_FUNC_0(ker_t30_dti_req        , T30_DTI_REQ       )
};

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_PEI             |
| STATE   : code                       ROUTINE : pei_primitive       |
+--------------------------------------------------------------------+

  PURPOSE : Process protocol specific primitive.

*/
LOCAL SHORT pei_primitive (void * ptr)
{
  T_PRIM * prim = ptr;
  /*
   *                    |
   *                   MMI                  UPPER LAYER
   *                    |
   *      +-------------v------------+
   *      |                          |
   *      |           T30            |
   *      |                          |
   *      +-------------^------------+
   *                    |
   *                   FAD                  LOWER LAYER
   *                    |
   *
   */

  TRACE_FUNCTION ("pei_primitive()");

  if (prim NEQ NULL)
  {
    ULONG         opc = prim->custom.opc;
    USHORT         n;
    const T_FUNC  *table;

    VSI_PPM_REC ((T_PRIM_HEADER *)prim, __FILE__, __LINE__);
    PTRACE_IN (opc);

    t30_data = GET_INSTANCE (prim);

    switch (SAP_NR(opc))
    {
    case DTI2_UL:
      table = dti_ul_table;			n = TAB_SIZE (dti_ul_table);
      /*
       * to be able to distinguish DTI1/DTI2 opcodes,
       * the ones for DTI2 start at 0x50
       */
      opc -= 0x50;
      break;
    case SAP_NR(DTI2_DL):
      table = dti_dl_table;
      n = TAB_SIZE (dti_dl_table);
      /*
       * to be able to distinguish DTI1/DTI2 opcodes,
       * the ones for DTI2 start at 0x50
       */
      opc -= 0x50;
      break;

    case SAP_NR(FAD_DL):
      table = fad_table;
      n = TAB_SIZE (fad_table);
      break;

    case SAP_NR(T30_UL):
      table = t30_table;
      n = TAB_SIZE (t30_table);
      break;

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
#endif
  }
  return PEI_OK;
}


/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_PEI             |
| STATE   : code                       ROUTINE : pei_init            |
+--------------------------------------------------------------------+

  PURPOSE : Initialize Protocol Stack Entity

*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
  t30_handle = handle;

  TRACE_FUNCTION ("pei_init()");

  if (hCommFAD < VSI_OK)
  {
    if ((hCommFAD = vsi_c_open (VSI_CALLER FAD_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

  if (hCommMMI < VSI_OK)
  {
    if ((hCommMMI = vsi_c_open (VSI_CALLER ACI_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

  /*
   * initialize dtilib for this entity
   */
  t30_hDTI = dti_init(T30_INSTANCES, handle, DTI_DEFAULT_OPTIONS, pei_dti_callback);

  if(!t30_hDTI)
    return PEI_ERROR;
  /*
   *  Initialize CCD and the other entity specific processes
   */
  ccd_init ();

  t30_data = &t30_data_base[0];
  t30_data_magic_num = 0;       /* memory is not yet initialized */

  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_PEI             |
| STATE   : code                       ROUTINE : pei_timeout         |
+--------------------------------------------------------------------+

  PURPOSE : Process timeout

*/
LOCAL SHORT pei_timeout (USHORT index)
{
  TRACE_FUNCTION ("pei_timeout ()");
  t30_timeout (index);
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_PEI             |
| STATE   : code                       ROUTINE : pei_exit            |
+--------------------------------------------------------------------+

  PURPOSE : Close Resources and terminate

*/
LOCAL SHORT pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit()");
  /*
   * shut down dtilib communication
   */
  dti_disconnect();
  dti_deinit(t30_hDTI);
  /*
   * clean up communication
   */
  vsi_c_close (VSI_CALLER hCommFAD);
  hCommFAD = VSI_ERROR;
  vsi_c_close (VSI_CALLER hCommMMI);
  hCommMMI = VSI_ERROR;
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_PEI             |
| STATE   : code                       ROUTINE : pei_config          |
+--------------------------------------------------------------------+

  PURPOSE : Dynamic Configuration

*/

#if !defined (NCONFIG)

  LOCAL KW_DATA kwtab[] =
  {
#ifdef OPTION_TIMER
    T30_TIMER_SET,        TIMER_SET,
    T30_TIMER_RESET,      TIMER_RESET,
    T30_TIMER_SPEED_UP,   TIMER_SPEED_UP,
    T30_TIMER_SLOW_DOWN,  TIMER_SLOW_DOWN,
    T30_TIMER_SUPPRESS,   TIMER_SUPPRESS,
#endif
    "", 0
  };

  LOCAL KW_DATA partab[] =
  {
#ifdef OPTION_TIMER
    T1_NAME, T1,
    T2_NAME, T2,
    T4_NAME, T4,
#endif
    "", 0
  };

#endif

LOCAL SHORT pei_config (T_PEI_CONFIG inString)
{
#if !defined (NCONFIG)
  SHORT    valno;
  SHORT    keyno;
  char    *s = inString;
  char    *keyw;
  char    *val [10];

#ifdef OPTION_TIMER
  BOOL      t_man = FALSE;
  UBYTE     t_mod = 0;
  SHORT     t_num = 0;
  LONG      t_val = 0;
#endif

  TRACE_FUNCTION ("pei_config()");
  TRACE_EVENT (s);

  tok_init (s);

  /*
   * Parse next keyword and number of variables
   */

  while ((valno = tok_next(&keyw,val)) != TOK_EOCS)
  {
    switch ((keyno = tok_key((KW_DATA *)kwtab, keyw)))
    {
      case TOK_NOT_FOUND:
        TRACE_ERROR ("[PEI_CONFIG]: Illegal Keyword");
        break;

#ifdef OPTION_TIMER
      case TIMER_SET:
        if (valno EQ 2)
        {
          t_man = TRUE;
          t_num = tok_key((KW_DATA *)partab,val[0]);
          t_mod = TIMER_SET;
          t_val = atoi(val[1]);

          if (t_val < 0L)
            t_val = 0L;
        }
        else
          TRACE_ERROR ("[PEI_CONFIG]: Wrong Number of Parameters");
        break;

      case TIMER_RESET:
      case TIMER_SUPPRESS:  /* manipulation of a timer */
        if (valno EQ 1)
        {
          t_man = TRUE;
          t_num = tok_key((KW_DATA *)partab,val[0]);
          t_mod = (UBYTE) keyno;
          t_val = 0L;
        }
        else
          TRACE_ERROR ("[PEI_CONFIG]: Wrong Number of Parameters");
        break;

      case TIMER_SPEED_UP:
      case TIMER_SLOW_DOWN:
        if (valno EQ 2)
        {
          t_man = TRUE;
          t_num = tok_key((KW_DATA *)partab,val[0]);
          t_mod = (UBYTE) keyno;
          t_val = atoi(val[1]);
          if (t_val <= 0L)
            t_val = 1L;
        }
        else
          TRACE_ERROR ("[PEI_CONFIG]: Wrong Number of Parameters");
        break;
#endif

      default:
        break;
    }

#ifdef OPTION_TIMER
    if (t_man)
    {
      /*
       * A timer is manipulated
       */
      t_man = FALSE;

      if (t_num >= 0)
        vsi_t_config (VSI_CALLER (USHORT)t_num, t_mod, t_val);
      else
        TRACE_ERROR ("[PEI_CONFIG]: Parameter out of Range");
    }
#endif
  }
#endif
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_PEI             |
| STATE   : code                       ROUTINE : pei_monitor         |
+--------------------------------------------------------------------+

  PURPOSE : Monitoring of physical Parameters

*/
LOCAL SHORT pei_monitor (void **monitor)
{
  TRACE_FUNCTION ("pei_monitor()");
  t30_mon.version = VERSION_T30;
  *monitor = &t30_mon;
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
  dti_dti_connect_req (t30_hDTI, dti_connect_req);
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
  dti_dti_connect_cnf(t30_hDTI, dti_connect_cnf);
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
  dti_dti_connect_ind(t30_hDTI, dti_connect_ind);
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
  dti_dti_connect_res(t30_hDTI, dti_connect_res);
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
  dti_dti_disconnect_req (t30_hDTI, dti_disconnect_req);
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
  dti_dti_disconnect_ind (t30_hDTI, dti_disconnect_ind);
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
  dti_dti_data_req (t30_hDTI, dti_data_req);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_getdata_req
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_data_req
+------------------------------------------------------------------------------
*/

LOCAL void pei_dti_getdata_req(T_DTI2_GETDATA_REQ *dti_getdata_req)
{
  dti_dti_getdata_req (t30_hDTI, dti_getdata_req);
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
  dti_dti_data_ind (t30_hDTI, dti_data_ind);
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
  dti_dti_ready_ind (t30_hDTI, dti_ready_ind);
}

/*
+------------------------------------------------------------------------------
|    Function: pei_dti_callback
+------------------------------------------------------------------------------
|    PURPOSE : Callback function for DTILIB
+------------------------------------------------------------------------------
*/
LOCAL void pei_dti_callback(U8 instance, U8 interfac, U8 channel,
                             U8 reason, T_DTI2_DATA_IND *dti_data_ind)
{
  TRACE_FUNCTION("pei_dti_callback");

  if (interfac NEQ T30_DTI_UP_INTERFACE || channel NEQ T30_DTI_UP_CHANNEL)
  {
    TRACE_ERROR("[PEI_DTI_CALLBACK] channel or interface not valid!");
    return; /* error, not found */
  }

  t30_data = &t30_data_base[instance];

  if (t30_hDTI NEQ D_NO_DATA_BASE)
  {
    switch (reason)
    {
      case DTI_REASON_CONNECTION_OPENED:
        sig_dti_ker_connection_opened_ind();
        break;

      case DTI_REASON_CONNECTION_CLOSED:
        sig_dti_ker_connection_closed_ind();
        break;

      case DTI_REASON_DATA_RECEIVED:

        /*
         * prevent dtilib from automatically sending flow control primitives
         */
        dti_stop(t30_hDTI, T30_DTI_UP_DEF_INSTANCE, T30_DTI_UP_INTERFACE, T30_DTI_UP_CHANNEL);

        PACCESS (dti_data_ind);
        {
        /*
         * DTI2_DATA_IND is interpreted as DTI2_DATA_REQ
         */
        PPASS (dti_data_ind, dti_data_req, DTI2_DATA_REQ);
        sig_dti_ker_data_received_ind(dti_data_req);
        }
        break;

      case DTI_REASON_TX_BUFFER_FULL:
        sig_dti_ker_tx_buffer_full_ind();
        break;

      case DTI_REASON_TX_BUFFER_READY:
        sig_dti_ker_tx_buffer_ready_ind();
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

#ifdef _SIMULATION_ /* DTI_DATA_TEST_REQ/IND */

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_data_test_req
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_data_test_req
+------------------------------------------------------------------------------
*/

LOCAL const void pei_dti_data_test_req(T_DTI2_DATA_TEST_REQ *dti_data_test_req)
{
  switch (GET_STATE (KER))
  {
    case T30_NULL:
      if (t30_data->test_mode & TST_BCS)
      {
        t30_data->mux.mode = MUX_BCS;
        sig_ker_mux_mux_req ();
        memcpy (_decodedMsg, dti_data_test_req->sdu.buf, dti_data_test_req->sdu.l_buf >> 3);
        sig_ker_bcs_bdat_req (dti_data_test_req->parameters.st_lines.st_flow); /* used as FINAL flag */
      }
      return;

    default:
      break;
  }
  dti_dti_data_test_req (t30_hDTI, dti_data_test_req);
}

/*
+------------------------------------------------------------------------------
|    Function    : pei_dti_data_test_ind
+------------------------------------------------------------------------------
|    PURPOSE : Call the process function dti_dti_data_test_ind
+------------------------------------------------------------------------------
*/

LOCAL const void pei_dti_data_test_ind(T_DTI2_DATA_TEST_IND *dti_data_test_ind)
{
  dti_dti_data_test_ind (t30_hDTI, dti_data_test_ind);
}

#endif

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : T30_PEI             |
| STATE   : code                       ROUTINE : pei_create          |
+--------------------------------------------------------------------+

  PURPOSE : Create the Protocol Stack Entity

*/

/*lint -e714 : Symbol not referenced */
/*lint -e765 : external could be made static */
GLOBAL SHORT t30_pei_create (T_PEI_INFO const **info)
{
  static const T_PEI_INFO pei_info =
  {
    "T30",
    {
      pei_init,
      pei_exit,
      pei_primitive,
      pei_timeout,
      NULL,             /* no signal function  */
      NULL,             /* no run function     */
      pei_config,
      pei_monitor,
    },
    1024,     /* Stack Size      */
    10,       /* Queue Entries   */
    205,      /* Priority        */
    3,        /* number of timer */
    0x03|PRIM_NO_SUSPEND /* flags           */
  };

  TRACE_FUNCTION ("t30_pei_create()");

  /*
   *  Close Resources if open
   */

  if (first_access)
    first_access = FALSE;
  else
    pei_exit ();

  /*
   *  Export startup configuration data
   */
  *info = &pei_info;
  return PEI_OK;
}


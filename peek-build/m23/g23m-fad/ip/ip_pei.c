/*
+----------------------------------------------------------------------------
|  Project :  GSM-FaD (8411)
|  Modul   :  IP_PEI
+----------------------------------------------------------------------------
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
+----------------------------------------------------------------------------
|  Purpose :  This Modul defines the process body interface
|             for the component Internet Protocol of the mobile station
+----------------------------------------------------------------------------
*/

#define ENTITY_IP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "pcm.h"
#include "vsi.h"
#include "macdef.h"
#include "custom.h"
#include "gsm.h"
#include "message.h"
#include "ccdapi.h"
#include "prim.h"
#include "cus_ip.h"
#include "cnf_ip.h"
#include "mon_ip.h"
#include "pei.h"
#include "tok.h"
#include "dti.h" /* To get DTI lib */
#include "ip.h"

/*==== CONST ======================================================*/

/*==== VAR EXPORT =================================================*/

T_HANDLE hCommMMI = VSI_ERROR; /* ACI communication */
T_HANDLE hCommDL = VSI_ERROR; /* Actual downlink, one of hcommPPP and hcommSNDCP */
T_HANDLE hCommIP = VSI_ERROR; /* IP communication */
T_HANDLE hCommHL = VSI_ERROR; /* HL communication */
T_HANDLE ip_handle;

T_IP_DATA ip_data_base [IP_INSTANCES], * ip_data;

/* DTI database. The database is allocated in function dti_init(). */

DTI_HANDLE ip_hDTI;

/*==== VAR LOCAL ==================================================*/

#ifdef OPTION_RELATIVE
static ULONG offset;
#endif

/* static USHORT t_flag = 0; *//* Timeout flag */
static BOOL first_access = TRUE;
static T_MONITOR ip_mon;

/*==== FUNCTIONS ==================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_PEI              |
| STATE   : code                       ROUTINE : pei_not_supported   |
+--------------------------------------------------------------------+

  PURPOSE : An unsupported primitive is received.
*/
static void pei_not_supported (void * data)
{
  TRACE_FUNCTION ("pei_not_supported()");

  PFREE (data);
}

/* Use MAK_FUNC_0 for primitives which contain no SDU.
 * Use MAK_FUNC_S for primitives which contain a SDU. */

/* Jumptable for the IPA service access point.
 * Contains the processing-function addresses and
 * opcodes of request and response primitives. */

static const T_FUNC ipa_table[] = {
  MAK_FUNC_0 (ker_ipa_dti_req, IPA_DTI_REQ),
  MAK_FUNC_0 (ker_ipa_config_req, IPA_CONFIG_REQ)
};

/* Jumptable for the IP service access point.
 * Contains the processing-function addresses and
 * opcodes of request and response primitives. */

static const T_FUNC ip_table[] = {
  MAK_FUNC_0 (ker_ip_addr_req, IP_ADDR_REQ)
};

/* Jumptable for the DTI service access point of the higher layer.
 * Contains the processing-function addresses and
 * opcodes of request and response primitives. */

static const T_FUNC dti_ul_table[] = {
  MAK_FUNC_0 (pei_dti_dti_connect_req,    DTI2_CONNECT_REQ),
  MAK_FUNC_0 (pei_dti_dti_connect_res,    DTI2_CONNECT_RES),
  MAK_FUNC_0 (pei_dti_dti_disconnect_req, DTI2_DISCONNECT_REQ),
  MAK_FUNC_0 (pei_dti_dti_getdata_req,    DTI2_GETDATA_REQ),
  MAK_FUNC_0 (pei_dti_dti_data_req,       DTI2_DATA_REQ),
#if defined _SIMULATION_
  MAK_FUNC_S (pei_dti_dti_data_test_req, DTI2_DATA_TEST_REQ)
#endif
};

/* Jumptable for the DTI service access point of the lower layer.
 * Contains the processing-function addresses and
 * opcodes of indication and confirm primitives. */

static const T_FUNC dti_dl_table[] = {
  MAK_FUNC_0 (pei_dti_dti_connect_ind,    DTI2_CONNECT_IND),
  MAK_FUNC_0 (pei_dti_dti_connect_cnf,    DTI2_CONNECT_CNF),
  MAK_FUNC_0 (pei_dti_dti_disconnect_ind, DTI2_DISCONNECT_IND),
  MAK_FUNC_0 (pei_dti_dti_ready_ind,      DTI2_READY_IND),
  MAK_FUNC_0 (pei_dti_dti_data_ind,       DTI2_DATA_IND),
#if defined _SIMULATION_
  MAK_FUNC_S (pei_dti_dti_data_test_ind, DTI2_DATA_TEST_IND)
#endif
};

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)               MODULE  : IP_PEI            |
| STATE   : code                         ROUTINE : pei_primitive     |
+--------------------------------------------------------------------+

  PURPOSE : Process protocol specific primitive.
*/
static SHORT pei_primitive (void * ptr)
{
  T_PRIM * prim = ptr;

  /* @ADAPT@
   *                   |
   *                  UL             UPPER LAYER
   *                   |
   *                   v
   *      +----------(ip)-----------+
   *      |                         |
   *      |           IP            |
   *      |                         |
   *      +------------^------------+
   *                   |
   *                  DL             LOWER LAYER
   *                   |
   */

  /* Please note that the words "D***link" and "U*link"
   * should not be used here, because their meaning
   * is directly opposite to what is being meant here.
   * So let us pronounce "DL" as "Downlayer" and
   * "UL" as "Uplayer", and the sun will shine. */

  if (prim NEQ NULL)
  {
    ULONG opc = prim->custom.opc;
    USHORT n;
    const T_FUNC * table;

    VSI_PPM_REC (& prim->custom, __FILE__, __LINE__);

    PTRACE_IN (opc);

    ip_data = GET_INSTANCE (prim);

    switch (SAP_NR (opc)) {
    case SAP_NR (IP_UL):
      table = ip_table;
      n = TAB_SIZE (ip_table);
      break;
    case SAP_NR (IPA_UL):
      table = ipa_table;
      n = TAB_SIZE (ipa_table);
      break;
    case DTI2_UL:
      table = dti_ul_table;
      n = TAB_SIZE (dti_ul_table);
      /* To be able to distinguish DTI1/DTI2 opcodes,
       * the ones for DTI2 start at 0x50. */
      opc -= 0x50;
      break;
    case DTI2_DL:
      table = dti_dl_table;
      n = TAB_SIZE (dti_dl_table);
      /* To be able to distinguish DTI1/DTI2 opcodes,
       * the ones for DTI2 start at 0x50. */
      opc -= 0x50;
      break;
    default:
      table = NULL;
      n = 0;
      break;
    }

    if (table NEQ NULL)
    {
      if (PRIM_NR (opc) < n)
      {
        table += PRIM_NR (opc);
#ifdef PALLOC_TRANSITION
        P_SDU (prim) = table->soff ?
          (T_sdu *) ((char *) & prim->data + table->soff) :
          NULL;
  #ifndef NO_COPY_ROUTING
        P_LEN (prim) = table->size + sizeof (T_PRIM_HEADER);
  #endif /* NO_COPY_ROUTING */
#endif /* PALLOC_TRANSITION */
        JUMP (table->func) (P2D (prim));
      }
      else
      {
        pei_not_supported (P2D (prim));
      }
      return PEI_OK;
    }

    /* Primitive is no GSM primitive
     * then forward to the environment */

#ifdef GSM_ONLY
    PFREE (P2D (prim));
    return PEI_ERROR;
#else
    if (opc & SYS_MASK)
      vsi_c_primitive (VSI_CALLER prim);
    else
    {
      PFREE (P2D (prim));
      return PEI_ERROR;
    }
#endif /* GSM_ONLY */
  }
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)              MODULE  : IP_PEI              |
| STATE   : code                       ROUTINE : ip_pei_primitive    |
+--------------------------------------------------------------------+

  PURPOSE : Re-use a stored primitive
*/
void ip_pei_primitive (T_PRIM * prim)
{
  pei_primitive (prim);
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-PS (6147)              MODULE  : IP_PEI              |
| STATE   : code                       ROUTINE : pei_init            |
+--------------------------------------------------------------------+

  PURPOSE : Initialize Protocol Stack Entity
*/
static SHORT pei_init (T_HANDLE handle)
{
  ip_handle = handle;

  TRACE_FUNCTION ("pei_init()");
	
  if ((ip_hDTI = dti_init (
    IP_INSTANCES * 2,
    ip_handle,
    DTI_DEFAULT_OPTIONS,
    pei_sig_callback
  )) EQ D_NO_DATA_BASE)
    return PEI_ERROR;

  if (hCommMMI < VSI_OK)
    if ((hCommMMI = vsi_c_open (VSI_CALLER ACI_NAME)) < VSI_OK)
      return PEI_ERROR;
	
  if (hCommIP < VSI_OK)
    if ((hCommIP = vsi_c_open (VSI_CALLER IP_NAME)) < VSI_OK)	
      return PEI_ERROR;

  ip_data = ip_data_base;

  /* Init global parameter */
  init_ip ();

  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_PEI              |
| STATE   : code                       ROUTINE : pei_timeout         |
+--------------------------------------------------------------------+

  PURPOSE : Process timeout
*/
static SHORT pei_timeout (USHORT index)
{
  USHORT layer;
  USHORT segm_server;
  USHORT timer;

  if (csf_timer_expired (index, & layer, & segm_server, & timer))
  {
    switch (timer) {
    case TREASSEMBLY:
      tim_reassembly (/* layer, segm_server */);
      return PEI_OK;
    default:
      break;
    }
  }
  return PEI_ERROR;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_PEI              |
| STATE   : code                       ROUTINE : pei_exit            |
+--------------------------------------------------------------------+

  PURPOSE : Close Resources and terminate
*/
static SHORT pei_exit (void)
{
  UBYTE i;

  TRACE_FUNCTION ("pei_exit()");

  /* Clean up communication */

  vsi_c_close (VSI_CALLER hCommDL);
  hCommDL = VSI_ERROR;

  for (i=0; i < MAX_LAYER; i++)
  {
    vsi_c_close (VSI_CALLER hCommHL);
    hCommHL = VSI_ERROR;
  }

  vsi_c_close (VSI_CALLER hCommMMI);
  hCommMMI = VSI_ERROR;

  if (ip_hDTI NEQ D_NO_DATA_BASE)
    dti_deinit (ip_hDTI);

  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_PEI              |
| STATE   : code                       ROUTINE : pei_config          |
+--------------------------------------------------------------------+

  PURPOSE : Dynamic Configuration
*/

#ifndef NCONFIG
static KW_DATA kwtab[] = {
  {"", 0}
};

KW_DATA partab[] = {
  {"", 0}
};
#endif

static SHORT pei_config (T_PEI_CONFIG inString)
{
#ifndef NCONFIG
  char * s = inString;
  char * keyw;
  char * val [10];

  TRACE_FUNCTION ("pei_config()");
  TRACE_EVENT (s);

  tok_init (s);

  /* Parse next keyword and number of variables */
  while ((tok_next(&keyw,val)) NEQ TOK_EOCS)
  {
    switch ((tok_key ((KW_DATA *) kwtab, keyw))) {
    case TOK_NOT_FOUND:
      TRACE_ERROR ("[PEI_CONFIG]: Illegal Keyword");
      break;
    default:
      break;
    }
  }
#endif
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_PEI              |
| STATE   : code                       ROUTINE : ip_pei_config       |
+--------------------------------------------------------------------+

  PURPOSE : Dynamic Configuration
*/
SHORT ip_pei_config (char * inString /*, char * dummy */)
{
  pei_config (inString);
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_PEI              |
| STATE   : code                       ROUTINE : pei_monitor         |
+--------------------------------------------------------------------+

  PURPOSE : Monitoring of physical Parameters
*/
static SHORT pei_monitor (void ** monitor)
{
  TRACE_FUNCTION ("pei_monitor()");

  ip_mon.version = VERSION_IP;
  *monitor = & ip_mon;
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : IP_PEI              |
| STATE   : code                       ROUTINE : pei_create          |
+--------------------------------------------------------------------+

  PURPOSE : Create the Protocol Stack Entity
*/
SHORT ip_pei_create (const T_PEI_INFO ** info)
{
  static const T_PEI_INFO pei_info =
  {
    "IP",
    {
      pei_init,
      pei_exit,
      pei_primitive,
      pei_timeout,
      NULL, /* No signal function */
      NULL, /* No run function */
      pei_config,
      pei_monitor
    },
    1024, /* Stack size */
    10, /* Queue entries */
    175, /* Priority */
    MAX_IP_TIMER, /* Number of timers */
    0x03|PRIM_NO_SUSPEND /* Flags */
  };

  TRACE_FUNCTION ("pei_create()");

  /* Close resources if open */
  if (first_access)
    first_access = FALSE;
  else
    pei_exit ();

  /* Export startup configuration data */
  *info = & pei_info;

  return PEI_OK;
}

/*
+----------------------------------------------------------------------------
| Function: pei_sig_callback
+----------------------------------------------------------------------------
| PURPOSE: Callback function for DTILIB
+----------------------------------------------------------------------------
*/
void pei_sig_callback (
  U8 instance,
  U8 interfac,
  U8 channel,
  U8 reason,
  T_DTI2_DATA_IND * dti_data_ind
) {
  TRACE_FUNCTION ("pei_sig_callback");

#ifdef _SIMULATION_
  if (instance NEQ IP_DTI_DEF_INSTANCE || channel NEQ IP_DTI_DEF_CHANNEL)
  {
    TRACE_ERROR ("[PEI_SIG_CALLBACK] channel or instance not valid!");
    return; /* Error, not found */
  }
#endif /* _SIMULATION_ */

  if (ip_hDTI NEQ D_NO_DATA_BASE)
  {
    switch (reason) {
    case DTI_REASON_CONNECTION_OPENED:
      switch (interfac) {
      case IP_DTI_HL_INTERFACE:
        sig_dti_ker_connection_opened_hl_ind ();
        break;
      case IP_DTI_LL_INTERFACE:
        sig_dti_ker_connection_opened_ll_ind ();
        break;
      default:
        TRACE_ERROR ("unknown DTILIB interface parameter");
        break;
      }
      break;

    case DTI_REASON_CONNECTION_CLOSED:
      switch (interfac) {
      case IP_DTI_HL_INTERFACE:
        sig_dti_ker_connection_closed_hl_ind ();
        break;
      case IP_DTI_LL_INTERFACE:
        sig_dti_ker_connection_closed_ll_ind ();
        break;
      default:
        TRACE_ERROR ("unknown DTILIB interface parameter");
        break;
      }
      break;

    case DTI_REASON_DATA_RECEIVED:
      switch (interfac) {
      case IP_DTI_HL_INTERFACE:
        /* DTI2_DATA_IND is interpreted as DTI2_DATA_REQ */
        PACCESS (dti_data_ind);
        {
          PPASS (dti_data_ind, dti_data_req, DTI2_DATA_REQ);
          sig_dti_ker_data_received_hl_ind (dti_data_req);
        }
        break;
      case IP_DTI_LL_INTERFACE:
        sig_dti_ker_data_received_ll_ind (dti_data_ind);
        break;
      default:
        TRACE_ERROR ("unknown DTILIB interface parameter");
        break;
      }
      break;

    case DTI_REASON_TX_BUFFER_FULL:
      switch (interfac) {
      case IP_DTI_HL_INTERFACE:
        sig_dti_ker_tx_buffer_full_hl_ind ();
        break;
      case IP_DTI_LL_INTERFACE:
        sig_dti_ker_tx_buffer_full_ll_ind ();
        break;
      default:
        TRACE_ERROR ("unknown DTILIB interface parameter");
        break;
      }
      break;

    case DTI_REASON_TX_BUFFER_READY:
      switch (interfac) {
      case IP_DTI_HL_INTERFACE:
        sig_dti_ker_tx_buffer_ready_hl_ind ();
        break;
      case IP_DTI_LL_INTERFACE:
        sig_dti_ker_tx_buffer_ready_ll_ind ();
        break;
      default:
        TRACE_ERROR ("unknown DTILIB interface parameter");
        break;
      }
      break;

    default:
      TRACE_ERROR ("unknown DTILIB reason parameter");
      break;
    } /* End switch */
  } /* End if */
  else
  {
    TRACE_ERROR ("Pointer to DTILIB database not existing");
  }
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_connect_req
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_connect_req
+----------------------------------------------------------------------------
*/
void pei_dti_dti_connect_req (T_DTI2_CONNECT_REQ * dti_connect_req)
{
  dti_dti_connect_req (ip_hDTI, dti_connect_req);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_disconnect_ind
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_disconnect_ind
+----------------------------------------------------------------------------
*/
void pei_dti_dti_disconnect_ind (T_DTI2_DISCONNECT_IND * dti_disconnect_ind)
{
  dti_dti_disconnect_ind (ip_hDTI, dti_disconnect_ind);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_connect_cnf
+----------------------------------------------------------------------------
| PURPOSE: Call the process function ker_dti_dti_connect_cnf
+----------------------------------------------------------------------------
*/
void pei_dti_dti_connect_cnf (T_DTI2_CONNECT_CNF * dti_connect_cnf)
{
  dti_dti_connect_cnf (ip_hDTI, dti_connect_cnf);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_connect_ind
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_connect_ind
+----------------------------------------------------------------------------
*/
void pei_dti_dti_connect_ind (T_DTI2_CONNECT_IND * dti_connect_ind)
{
  dti_dti_connect_ind (ip_hDTI, dti_connect_ind);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_connect_res
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_connect_res
+----------------------------------------------------------------------------
*/
void pei_dti_dti_connect_res (T_DTI2_CONNECT_RES * dti_connect_res)
{
  dti_dti_connect_res (ip_hDTI, dti_connect_res);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_disconnect_req
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_disconnect_req
+----------------------------------------------------------------------------
*/
void pei_dti_dti_disconnect_req (T_DTI2_DISCONNECT_REQ * dti_disconnect_req)
{
  dti_dti_disconnect_req (ip_hDTI, dti_disconnect_req);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_data_req
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_data_req
+----------------------------------------------------------------------------
*/
void pei_dti_dti_data_req (T_DTI2_DATA_REQ * dti_data_req)
{
  dti_dti_data_req (ip_hDTI, dti_data_req);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_getdata_req
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_data_req
+----------------------------------------------------------------------------
*/
void pei_dti_dti_getdata_req (T_DTI2_GETDATA_REQ * dti_getdata_req)
{
  dti_dti_getdata_req (ip_hDTI, dti_getdata_req);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_data_ind
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_data_ind
+----------------------------------------------------------------------------
*/
void pei_dti_dti_data_ind (T_DTI2_DATA_IND * dti_data_ind)
{
  dti_dti_data_ind (ip_hDTI, dti_data_ind);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_ready_ind
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_ready_ind
+----------------------------------------------------------------------------
*/
void pei_dti_dti_ready_ind (T_DTI2_READY_IND * dti_ready_ind)
{
  dti_dti_ready_ind (ip_hDTI, dti_ready_ind);
}

#ifdef _SIMULATION_

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_data_test_req
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_data_test_req
+----------------------------------------------------------------------------
*/
void pei_dti_dti_data_test_req (T_DTI2_DATA_TEST_REQ * dti_data_test_req)
{
  dti_dti_data_test_req (ip_hDTI, dti_data_test_req);
}

/*
+----------------------------------------------------------------------------
| Function: pei_dti_dti_data_test_ind
+----------------------------------------------------------------------------
| PURPOSE: Call the process function dti_dti_data_test_ind
+----------------------------------------------------------------------------
*/
void pei_dti_dti_data_test_ind (T_DTI2_DATA_TEST_IND * dti_data_test_ind)
{
  dti_dti_data_test_ind (ip_hDTI, dti_data_test_ind);
}

#endif /* _SIMULATION_ */

/*-------------------------------------------------------------------------*/


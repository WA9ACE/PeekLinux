/*
+----------------------------------------------------------------------------
|  Project :  GSM-FaD (8411)
|  Modul   :  UDP_PEI
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

#define ENTITY_UDP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "pcm.h"
#include "vsi.h"
#include "macdef.h"    /* to get PFREE_DESC2 */
#include "custom.h"
#include "gsm.h"
#include "message.h"
#include "ccdapi.h"
#include "prim.h"
#include "cus_udp.h"
#include "cnf_udp.h"
#include "mon_udp.h"
#include "pei.h"
#include "tok.h"
#include "dti.h"        /* to get dti lib */
#include "udp.h"

/*==== CONST ======================================================*/

/*==== VAR EXPORT =================================================*/

DTI_HANDLE udp_hDTI;
/* The DTI data base is being allocated in function dti_init(). */

T_HANDLE hCommACI = VSI_ERROR; /* ACI Communication */
T_HANDLE hCommLL = VSI_ERROR; /* LL Communication */
T_HANDLE hCommHL = VSI_ERROR; /* HL Communication */
T_HANDLE udp_handle;

T_UDP_DATA udp_data_base [UDP_INSTANCES], * udp_data;

/*==== VAR LOCAL ==================================================*/

static BOOL first_access = TRUE;
static T_MONITOR udp_mon;

/*==== FUNCTIONS ==================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : UDP_PEI             |
| STATE   : code                       ROUTINE : pei_not_supported   |
+--------------------------------------------------------------------+

  PURPOSE : An unsupported primitive is received.
*/

static void pei_not_supported (void * data)
{
  TRACE_FUNCTION ("pei_not_supported()");
  PFREE (data);
}

/* Use MAK_FUNC_0 for primitives which contains no SDU.
 * Use MAK_FUNC_S for primitives which contains a SDU. */

/* Jumptable for the UDPA service access point.
 * Contains the processing-function addresses and
 * opcodes of request and response primitives. */

static const T_FUNC udpa_table[] =
{
  MAK_FUNC_0 (ker_udpa_dti_req,    UDPA_DTI_REQ),
  MAK_FUNC_0 (ker_udpa_config_req, UDPA_CONFIG_REQ)
};

/* Jumptable for the UDP service access point.
 * Contains the processing-function addresses and
 * opcodes of request and response primitives. */

static const T_FUNC udp_table[] =
{
  MAK_FUNC_0 (ker_udp_bind_req,      UDP_BIND_REQ),
  MAK_FUNC_0 (ker_udp_closeport_req, UDP_CLOSEPORT_REQ),
  MAK_FUNC_0 (ker_udp_error_res,     UDP_ERROR_RES),
  MAK_FUNC_0 (ker_udp_shutdown_res,  UDP_SHUTDOWN_RES),
};

/* Jumptable for the IP service access point.
 * Contains the processing-function addresses and
 * opcodes of indication and confirm primitives. */

static const T_FUNC ip_table[] =
{
  MAK_FUNC_0 (ker_ip_addr_cnf, IP_ADDR_CNF),
};

/* Jumptable for the DTI service access point of the higher layer.
 * Contains the processing-function addresses and
 * opcodes of request and response primitives. */

static const T_FUNC dti_hl_table[] =
{
  MAK_FUNC_0 (pei_dti_dti_connect_req,    DTI2_CONNECT_REQ),
  MAK_FUNC_0 (pei_dti_dti_connect_res,    DTI2_CONNECT_RES),
  MAK_FUNC_0 (pei_dti_dti_disconnect_req, DTI2_DISCONNECT_REQ),
  MAK_FUNC_0 (pei_dti_dti_getdata_req,    DTI2_GETDATA_REQ),
  MAK_FUNC_0 (pei_dti_dti_data_req,       DTI2_DATA_REQ),
#ifdef _SIMULATION_
  MAK_FUNC_S (pei_dti_dti_data_test_req,  DTI2_DATA_TEST_REQ)
#endif
};

/* Jumptable for the DTI service access point of the lower layer.
 * Contains the processing-function addresses and
 * opcodes of indication and confirm primitives. */

static const T_FUNC dti_ll_table[] =
{
  MAK_FUNC_0 (pei_dti_dti_connect_ind,    DTI2_CONNECT_IND),
  MAK_FUNC_0 (pei_dti_dti_connect_cnf,    DTI2_CONNECT_CNF),
  MAK_FUNC_0 (pei_dti_dti_disconnect_ind, DTI2_DISCONNECT_IND),
  MAK_FUNC_0 (pei_dti_dti_ready_ind,      DTI2_READY_IND),
  MAK_FUNC_0 (pei_dti_dti_data_ind,       DTI2_DATA_IND),
#ifdef _SIMULATION_
  MAK_FUNC_S (pei_dti_dti_data_test_ind,  DTI2_DATA_TEST_IND)
#endif
};

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)               MODULE  : UDP_PEI           |
| STATE   : code                         ROUTINE : pei_primitive     |
+--------------------------------------------------------------------+

  PURPOSE : Process protocol specific primitive.
*/

static SHORT pei_primitive (void * ptr)
{
  /*
   * @ADAPT@
   *                    |
   *                   UL               HIGHER LAYER
   *                    |
   *                    v
   *      +-----------(udp)----------+
   *      |                          |
   *      |            UDP           |
   *      |                          |
   *      +-------------^------------+
   *                    |
   *                   DL               LOWER LAYER
   *                    |
   */

  /* Please note that the words "Downlink" and "Uplink"
   * should not be used here, because their meaning
   * is directly opposite to what is being meant here.
   * So let us correct the creator's mistake by
   * pronouncing "DL" as "Downlayer" and
   * "UL" as "Uplayer" and all should fit well
   * -- as long as you can remember this ... */

  TRACE_FUNCTION ("pei_primitive()");

  if (ptr != NULL)
  {
    T_PRIM * prim = ptr;
    ULONG opc = prim->custom.opc;
    USHORT n;
    const T_FUNC * table;

    VSI_PPM_REC (& prim->custom, __FILE__, __LINE__);

    PTRACE_IN (opc);

    udp_data = GET_INSTANCE (prim);

    switch (SAP_NR (opc))
    {
    case SAP_NR (UDP_UL):
      table = udp_table;
      n = TAB_SIZE (udp_table);
      break;
    case SAP_NR (UDPA_UL):
      table = udpa_table;
      n = TAB_SIZE (udpa_table);
      break;
    case IP_DL:
      table = ip_table;
      n = TAB_SIZE (ip_table);
      break;
    case DTI2_UL:
      table = dti_hl_table;
      n = TAB_SIZE (dti_hl_table);
      /*
      * to be able to distinguish DTI1/DTI2 opcodes,
      * the ones for DTI2 start at 0x50
      */
      opc -= 0x50;
      break;
    case DTI2_DL:
      table = dti_ll_table;
      n = TAB_SIZE (dti_ll_table);
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

    if (table NEQ NULL)
    {
      if (PRIM_NR (opc) < n)
      {
        table += PRIM_NR (opc);
#ifdef PALLOC_TRANSITION
        P_SDU(prim) = table->soff ?
          (T_sdu *) ((char *) & prim->data + table->soff) :
          0;
  #ifndef NO_COPY_ROUTING
        P_LEN(prim) = table->size + sizeof (T_PRIM_HEADER);
  #endif
#endif
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
#endif
  }
  return PEI_OK;
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
  udp_handle = handle;

  TRACE_FUNCTION ("pei_init()");

  udp_data = udp_data_base + UDP_SINGLE_LAYER;

  INIT_STATE (KER, DEACTIVATED);
  INIT_STATE (HILA, DOWN);
  INIT_STATE (LOLA, DOWN);

  if ((udp_hDTI = dti_init (
    2 * UDP_INSTANCES,
    udp_handle,
    DTI_DEFAULT_OPTIONS,
    pei_sig_callback
  )) EQ D_NO_DATA_BASE)
    return PEI_ERROR;

  /*
  if (hCommHL < VSI_OK)
    if ((hCommHL = vsi_c_open (VSI_CALLER WAP_NAME)) < VSI_OK)
      return PEI_ERROR;
  */
  if (hCommLL < VSI_OK)
    if ((hCommLL = vsi_c_open (VSI_CALLER IP_NAME)) < VSI_OK)
      return PEI_ERROR;

  if (hCommACI < VSI_OK)
    if ((hCommACI = vsi_c_open (VSI_CALLER ACI_NAME)) < VSI_OK)
      return PEI_ERROR;

  /* Init global parameters */
  init_udp ();

  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : UDP_PEI             |
| STATE   : code                       ROUTINE : pei_exit            |
+--------------------------------------------------------------------+

  PURPOSE : Close Resources and terminate
*/

static SHORT pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit()");

  /* Clean up communication */

  vsi_c_close (VSI_CALLER hCommHL);
  hCommHL = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommLL);
  hCommLL = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommACI);
  hCommACI = VSI_ERROR;

  if (udp_hDTI NEQ D_NO_DATA_BASE)
    dti_deinit (udp_hDTI);

  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : UDP_PEI             |
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
  char * val[10];
  TRACE_FUNCTION ("pei_config()");
  TRACE_EVENT (s);

  tok_init (s);

  /* Parse next keyword and number of variables */
  while ((tok_next(&keyw,val)) NEQ TOK_EOCS)
  {
    switch ((tok_key ((KW_DATA *) kwtab, keyw)))
    {
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
| PROJECT : GSM-FaD (8411)             MODULE  : UDP_PEI             |
| STATE   : code                       ROUTINE : pei_monitor         |
+--------------------------------------------------------------------+

  PURPOSE : Monitoring of physical Parameters
*/

static SHORT pei_monitor (void ** monitor)
{
  TRACE_FUNCTION ("pei_monitor()");

  udp_mon.version = VERSION_UDP;
  *monitor = & udp_mon;
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-FaD (8411)             MODULE  : UDP_PEI             |
| STATE   : code                       ROUTINE : pei_create          |
+--------------------------------------------------------------------+

  PURPOSE : Create the Protocol Stack Entity
*/

SHORT udp_pei_create (const T_PEI_INFO ** info)
{
  static const T_PEI_INFO pei_info =
  {
    "UDP",
    {
      pei_init,
      pei_exit,
      pei_primitive,
      NULL,             /* no timeout function */
      NULL,             /* no signal function */
      NULL,             /* no run function */
      pei_config,
      pei_monitor
    },
    1024,               /* stack size */
    10,                 /* queue entries */
    170,                /* priority */
    0,                  /* number of timers */
    0x03|PRIM_NO_SUSPEND/* flags */
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
|    Function: pei_sig_callback
+----------------------------------------------------------------------------
|    PURPOSE : Callback function for DTILIB
+----------------------------------------------------------------------------
*/

void pei_sig_callback (
  U8 instance,
  U8 interfac,
  U8 channel,
  U8 reason,
  T_DTI2_DATA_IND * dti_data_ind
) {
  TRACE_FUNCTION ("pei_sig_callback()");

#ifdef _SIMULATION_
  if (instance NEQ UDP_DTI_DEF_INSTANCE || channel NEQ UDP_DTI_DEF_CHANNEL)
  {
    TRACE_ERROR ("[PEI_SIG_CALLBACK] channel or instance not valid!");
    return; /* error, not found */
  }
#endif

  /* udp_data = udp_data_base + instance; */

  if (udp_hDTI NEQ D_NO_DATA_BASE)
  {
    switch (reason)
    {
      case DTI_REASON_CONNECTION_OPENED:
        switch (interfac)
        {
        case UDP_DTI_HL_INTERFACE:
          sig_dti_ker_connection_opened_hl_ind ();
          break;
        case UDP_DTI_LL_INTERFACE:
          sig_dti_ker_connection_opened_ll_ind ();
          break;
        default:
          TRACE_ERROR ("unknown DTILIB interface parameter");
          break;
        }
        break;

      case DTI_REASON_CONNECTION_CLOSED:
        switch (interfac)
        {
        case UDP_DTI_HL_INTERFACE:
          sig_dti_ker_connection_closed_hl_ind ();
          break;
        case UDP_DTI_LL_INTERFACE:
          sig_dti_ker_connection_closed_ll_ind ();
          break;
        default:
          TRACE_ERROR ("unknown DTILIB interface parameter");
          break;
        }
        break;

      case DTI_REASON_DATA_RECEIVED:
        switch (interfac)
        {
        case UDP_DTI_HL_INTERFACE:
          /*
           * DTI2_DATA_IND is interpreted as DTI2_DATA_REQ
           */
          PACCESS (dti_data_ind);
          {
          PPASS (dti_data_ind, dti_data_req, DTI2_DATA_REQ);
          sig_dti_ker_data_received_hl_ind (dti_data_req);
          }
          break;
        case UDP_DTI_LL_INTERFACE:
          sig_dti_ker_data_received_ll_ind (dti_data_ind);
          break;
        default:
          TRACE_ERROR ("unknown DTILIB interface parameter");
          break;
        }
        break;

      case DTI_REASON_TX_BUFFER_READY:
        switch (interfac)
        {
        case UDP_DTI_HL_INTERFACE:
          sig_dti_ker_tx_buffer_ready_hl_ind ();
          break;
        case UDP_DTI_LL_INTERFACE:
          sig_dti_ker_tx_buffer_ready_ll_ind ();
          break;
        default:
          TRACE_ERROR ("unknown DTILIB interface parameter");
          break;
        }
        break;

      case DTI_REASON_TX_BUFFER_FULL:
        switch (interfac)
        {
        case UDP_DTI_HL_INTERFACE:
          sig_dti_ker_tx_buffer_full_hl_ind (); /* DUMMY function */
          break;
        case UDP_DTI_LL_INTERFACE:
          sig_dti_ker_tx_buffer_full_ll_ind (); /* DUMMY function */
          break;
        default:
          TRACE_ERROR ("unknown DTILIB interface parameter");
          break;
        }
        break;

      default:
        TRACE_ERROR ("unknown DTILIB reason parameter");
        break;
    }
  }
  else
  {
    TRACE_ERROR ("Pointer to DTILIB database not existing");
  }
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                              ROUTINE: pei_dti_dti_connect_req |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_connect_req wraps dti_dti_connect_req
*/

void pei_dti_dti_connect_req (T_DTI2_CONNECT_REQ * dti_connect_req)
{
  dti_dti_connect_req (udp_hDTI, dti_connect_req);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                           ROUTINE: pei_dti_dti_disconnect_ind |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_disconnect_ind wraps dti_dti_disconnect_ind
*/

void pei_dti_dti_disconnect_ind (T_DTI2_DISCONNECT_IND * dti_disconnect_ind)
{
  dti_dti_disconnect_ind (udp_hDTI, dti_disconnect_ind);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                              ROUTINE: pei_dti_dti_connect_cnf |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_connect_cnf wraps dti_dti_connect_cnf
*/

void pei_dti_dti_connect_cnf (T_DTI2_CONNECT_CNF * dti_connect_cnf)
{
  dti_dti_connect_cnf (udp_hDTI, dti_connect_cnf);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                              ROUTINE: pei_dti_dti_connect_ind |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_connect_ind wraps dti_dti_connect_ind
*/

void pei_dti_dti_connect_ind (T_DTI2_CONNECT_IND * dti_connect_ind)
{
  dti_dti_connect_ind (udp_hDTI, dti_connect_ind);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                              ROUTINE: pei_dti_dti_connect_res |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_connect_res wraps dti_dti_connect_res
*/

void pei_dti_dti_connect_res (T_DTI2_CONNECT_RES * dti_connect_res)
{
  dti_dti_connect_res (udp_hDTI, dti_connect_res);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                           ROUTINE: pei_dti_dti_disconnect_req |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_disconnect_req wraps dti_dti_disconnect_req
*/

void pei_dti_dti_disconnect_req (T_DTI2_DISCONNECT_REQ * dti_disconnect_req)
{
  dti_dti_disconnect_req (udp_hDTI, dti_disconnect_req);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                                 ROUTINE: pei_dti_dti_data_req |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_data_req wraps dti_dti_data_req
*/

void pei_dti_dti_data_req (T_DTI2_DATA_REQ * dti_data_req)
{
  dti_dti_data_req (udp_hDTI, dti_data_req);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                              ROUTINE: pei_dti_dti_getdata_req |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_getdata_req wraps dti_dti_getdata_req
*/

void pei_dti_dti_getdata_req (T_DTI2_GETDATA_REQ * dti_getdata_req)
{
  dti_dti_getdata_req (udp_hDTI, dti_getdata_req);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                                 ROUTINE: pei_dti_dti_data_ind |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_data_ind wraps dti_dti_data_ind
*/

void pei_dti_dti_data_ind (T_DTI2_DATA_IND * dti_data_ind)
{
  dti_dti_data_ind (udp_hDTI, dti_data_ind);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                                ROUTINE: pei_dti_dti_ready_ind |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_ready_ind wraps dti_dti_ready_ind
*/

void pei_dti_dti_ready_ind (T_DTI2_READY_IND * dti_ready_ind)
{
  dti_dti_ready_ind (udp_hDTI, dti_ready_ind);
}

#ifdef _SIMULATION_

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                            ROUTINE: pei_dti_dti_data_test_req |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_data_test_req wraps dti_dti_data_test_req
*/

void pei_dti_dti_data_test_req (T_DTI2_DATA_TEST_REQ * dti_data_test_req)
{
  dti_dti_data_test_req (udp_hDTI, dti_data_test_req);
}

/*
+---------------------------------------------------------------------------+
| PROJECT: UDP                                            MODULE: udp_pei.c |
| STATE: code                            ROUTINE: pei_dti_dti_data_test_ind |
+---------------------------------------------------------------------------+

  PURPOSE: pei_dti_dti_data_test_ind wraps dti_dti_data_test_ind
*/

void pei_dti_dti_data_test_ind (T_DTI2_DATA_TEST_IND * dti_data_test_ind)
{
  dti_dti_data_test_ind (udp_hDTI, dti_data_test_ind);
}

#endif /* _SIMULATION_ */

/*-------------------------------------------------------------------------*/


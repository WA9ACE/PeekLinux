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
|             for the entity Logical Link Control (LLC)
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

#define LLC_PEI_C

#define ENTITY_LLC

/*==== INCLUDES =============================================================*/

#include <stddef.h>     /* to get definition of offsetof(), for MAK_FUNC_S */
#include <stdlib.h>     /* to get atoi() */
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "macdef.h"
#include "gprs.h"
#include "gsm.h"        /* to get a lot of macros */
#include "cnf_llc.h"    /* to get cnf-definitions */
#include "mon_llc.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "pei.h"        /* to get PEI interface */
#include "llc.h"        /* to get the global entity definitions */
#include "llc_f.h"        /* to get the global entity definitions */

#include "llc_llmep.h"  /* to get primitive interface to LLME */
#include "llc_up.h"     /* to get primitive interface to U */
#include "llc_itxp.h"   /* to get primitive interface to ITX */
#include "llc_irxp.h"   /* to get primitive interface to IRX */
#include "llc_itxt.h"   /* to get timer interface to IRX */
#include "llc_uitxp.h"  /* to get primitive interface to UITX */
#include "llc_uirxp.h"  /* to get primitive interface to UIRX */
#include "llc_t200p.h"  /* to get primitive interface to T200 */
#include "llc_txp.h"    /* to get primitive interface to TX */
#include "llc_rxp.h"    /* to get primitive interface to RX */
#include "llc_llmef.h"  /* to get init function of LLME */
#include "llc_uf.h"     /* to get init function of U */
#include "llc_itxf.h"   /* to get init function of ITX */
#include "llc_irxf.h"   /* to get init function of IRX */
#include "llc_uitxf.h"  /* to get init function of UITX */
#include "llc_uirxf.h"  /* to get init function of UIRX */
#include "llc_t200f.h"  /* to get init function of T200 */
#include "llc_txf.h"    /* to get init function of TX */
#include "llc_rxf.h"    /* to get init function of RX */



/*==== DEFINITIONS ==========================================================*/

/*==== TYPES ================================================================*/

/*==== GLOBAL VARS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

static  BOOL          first_access  = TRUE;
static  T_MONITOR     llc_mon;

/*
 * Jumptables to primitive handler functions. One table per SAP.
 *
 * Use MAK_FUNC_0 for primitives which contains no SDU.
 * Use MAK_FUNC_S for primitives which contains a SDU.
 */

/*
 * This Function is needed for primitives, that are not (yet?) supported.
 * It frees the primitive for which it is specified in the jump table.
 */
LOCAL void primitive_not_supported (T_PRIM_HEADER *data);


static const T_FUNC ll_table[] =
{
  MAK_FUNC_S(u_ll_establish_req,        LL_ESTABLISH_REQ),
  MAK_FUNC_S(u_ll_establish_res,        LL_ESTABLISH_RES),
  MAK_FUNC_0(u_ll_release_req,          LL_RELEASE_REQ),
  MAK_FUNC_S(u_ll_xid_req,              LL_XID_REQ),
  MAK_FUNC_S(u_ll_xid_res,              LL_XID_RES),
  MAK_FUNC_0(irx_ll_getdata_req,        LL_GETDATA_REQ),
  MAK_FUNC_0(uirx_ll_getunitdata_req,   LL_GETUNITDATA_REQ),
#ifdef LL_DESC
  MAK_FUNC_S(itx_ll_data_req,           LL_DATA_REQ),
  MAK_FUNC_S(uitx_ll_unitdata_req,      LL_UNITDATA_REQ),
  MAK_FUNC_0(itx_ll_desc_req,           LL_DESC_REQ),
  MAK_FUNC_0(uitx_ll_unitdesc_req,      LL_UNITDESC_REQ),
#else
  MAK_FUNC_S(itx_ll_desc_req,           LL_DATA_REQ),
  MAK_FUNC_S(uitx_ll_unitdesc_req,      LL_UNITDATA_REQ)
#endif
};

static const T_FUNC llgmm_table[] =
{
  MAK_FUNC_0(llme_llgmm_assign_req,     LLGMM_ASSIGN_REQ),
  MAK_FUNC_0(llme_llgmm_trigger_req,    LLGMM_TRIGGER_REQ),
  MAK_FUNC_0(llme_llgmm_suspend_req,    LLGMM_SUSPEND_REQ),
  MAK_FUNC_0(llme_llgmm_resume_req,     LLGMM_RESUME_REQ)
};

static const T_FUNC grlc_table[] =
{
  MAK_FUNC_0(rx_grlc_data_ind,          GRLC_DATA_IND),
#ifdef _SIMULATION_
  MAK_FUNC_S(rx_grlc_data_ind_test,      GRLC_DATA_IND_TEST),
#else
  MAK_FUNC_N(primitive_not_supported,   GRLC_DATA_IND_TEST),
#endif
  MAK_FUNC_0(rx_grlc_unitdata_ind,       GRLC_UNITDATA_IND),
#ifdef _SIMULATION_
  MAK_FUNC_S(rx_grlc_unitdata_ind_test,  GRLC_UNITDATA_IND_TEST),
#else
  MAK_FUNC_N(primitive_not_supported,   GRLC_UNITDATA_IND_TEST),
#endif
  MAK_FUNC_N(tx_grlc_ready_ind,          GRLC_READY_IND),
  MAK_FUNC_N(tx_grlc_suspend_ready_ind,  GRLC_SUSPEND_READY_IND)
};


/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
| Function    : primitive_not_supported
+------------------------------------------------------------------------------
| Description : This function handles unsupported primitives.
|
| Parameters  : -
|
| Return      : -
|
+------------------------------------------------------------------------------
*/
LOCAL void primitive_not_supported (T_PRIM_HEADER *data)
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
|                        LLGMM         LL               UPLINK
|                          |           |
|                   +------v-----------v-------+
|                   |                          |
|                   |            LLC           |
|                   |                          |
|                   +-------------^------------+
|                                 |
|                                GRLC                    DOWNLINK
|                                 |
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

  if (primptr != NULL)
  {
    T_PRIM *prim  = (T_PRIM *)primptr;
    ULONG     opc = prim->custom.opc;
    USHORT           n;
    const T_FUNC    *table;

    /*
     * This must be called to enable Partition Pool (memory) supervision.
     */
    VSI_PPM_REC (&prim->custom, __FILE__, __LINE__);

    switch (SAP_NR(opc))
    {
      case SAP_NR(LLGMM_UL):
        table = llgmm_table;
        n = TAB_SIZE (llgmm_table);
        break;
      case SAP_NR(LL_UL):
#ifdef TRACE_EVE
        {
          /*
           * following line requires sapi is always the first struct member
           * in all LL primitive headers and always using the same size.
           */
          T_LL_GETUNITDATA_REQ* pData = (T_LL_GETUNITDATA_REQ*)(P2D(prim));

          switch (pData->sapi)
          {
            case LL_SAPI_1: TRACE_PRIM_FROM("GMM");  break;
#ifdef LL_2to1
            case LL_SAPI_7: TRACE_PRIM_FROM("MM"); break;
#else
            case LL_SAPI_7: TRACE_PRIM_FROM("GSMS"); break;
#endif
            default:                                 break;
          }
        }
#endif
        table = ll_table;
        n = TAB_SIZE (ll_table);
        break;
      case SAP_NR(GRLC_DL):
        table = grlc_table;
        n = TAB_SIZE (grlc_table);
        break;
      default:
        table = NULL;
        n = 0;
        break;
    }

    PTRACE_IN (opc);

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
  TRACE_FUNCTION ("pei_init");

  /*
   * Initialize task handle
   */
  LLC_handle = handle;

  /*
   * Open communication channels
   */
  if (hCommGMM < VSI_OK)
  {
#ifdef LL_2to1
    if ((hCommGMM = vsi_c_open (VSI_CALLER MM_NAME)) < VSI_OK)
#else
    if ((hCommGMM = vsi_c_open (VSI_CALLER GMM_NAME)) < VSI_OK)
#endif
      return PEI_ERROR;
  }
  if (hCommSNDCP < VSI_OK)
  {
    if ((hCommSNDCP = vsi_c_open (VSI_CALLER SNDCP_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#ifdef LL_2to1
  if (hCommMM < VSI_OK)
  {
    if ((hCommMM = vsi_c_open (VSI_CALLER MM_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#else
  if (hCommGSMS < VSI_OK)
  {
    if ((hCommGSMS = vsi_c_open (VSI_CALLER GSMS_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#endif
  if (hCommGRLC < VSI_OK)
  {
    if ((hCommGRLC = vsi_c_open (VSI_CALLER GRLC_NAME)) < VSI_OK)
      return PEI_ERROR;
  }


  /*
   * Initialize global pointer llc_data. This is required to access all
   * entity data.
   */
  llc_data = &llc_data_base;


  /*
   * Initialize entity data (call init function of every service)
   */
  llme_init();
  u_init();
  itx_init();
  irx_init();
  uitx_init();
  uirx_init();
  t200_init();
  llc_tx_init();
  rx_init();
#ifndef TI_PS_OP_CIPH_DRIVER
  llc_fbs_init();
#endif
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
  switch (index)
  {
    case TIMER_T200_1:
      /*
       * T200 for SAPI 1 expired.
       */
      t200_timer_t200 (LL_SAPI_1);
      break;
    case TIMER_T200_3:
      /*
       * T200 for SAPI 3 expired.
       */
      t200_timer_t200 (LL_SAPI_3);
      break;
    case TIMER_T200_5:
      /*
       * T200 for SAPI 5 expired.
       */
      t200_timer_t200 (LL_SAPI_5);
      break;
    case TIMER_T200_7:
      /*
       * T200 for SAPI 7 expired.
       */
      t200_timer_t200 (LL_SAPI_7);
      break;
    case TIMER_T200_9:
      /*
       * T200 for SAPI 9 expired.
       */
      t200_timer_t200 (LL_SAPI_9);
      break;
    case TIMER_T200_11:
      /*
       * T200 for SAPI 11 expired.
       */
      t200_timer_t200 (LL_SAPI_11);
      break;
    case TIMER_T201_3:
      /*
       * T201 for SAPI 3 expired.
       */
      itx_timer_t201 (LL_SAPI_3);
      break;
    case TIMER_T201_5:
      /*
       * T201 for SAPI 5 expired.
       */
      itx_timer_t201 (LL_SAPI_5);
      break;
    case TIMER_T201_9:
      /*
       * T201 for SAPI 9 expired.
       */
      itx_timer_t201 (LL_SAPI_9);
      break;
    case TIMER_T201_11:
      /*
       * T201 for SAPI 11 expired.
       */
      itx_timer_t201 (LL_SAPI_11);
      break;
    default:
      TRACE_ERROR("Unknown Timeout");
      break;
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
  TRACE_ERROR("Unknown Signal OPC");

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
  TRACE_FUNCTION ("pei_exit");

  /*
   * Close communication channels
   */
  vsi_c_close (VSI_CALLER hCommGMM);
  hCommGMM = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommSNDCP);
  hCommSNDCP = VSI_ERROR;

#ifdef LL_2to1
  vsi_c_close (VSI_CALLER hCommMM);
  hCommMM = VSI_ERROR;
#else
  vsi_c_close (VSI_CALLER hCommGSMS);
  hCommGSMS = VSI_ERROR;
#endif

  vsi_c_close (VSI_CALLER hCommGRLC);
  hCommGRLC = VSI_ERROR;

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
 * Does not compile!
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

#ifdef _SIMULATION_
#ifndef NCONFIG
  /*
   * Parse for config keywords:
   *     SAPI XX KU YYYY
   *     SAPI XX KD YYYY
   *     SAPI XX MU YYYY
   *     SAPI XX MD YYYY
   *     SAPI XX N200 YYYY
   *     SAPI XX T200 YYYY
   *     SAPI XX N201_U YYYY
   *     SAPI XX N201_I YYYY
   * The configured values are currently stored in the requested_xid struct, which is
   * refilled completely after change into LLC assigned state and after LLC XID reset.
   */
  if (inString[0] == 'S' AND inString[1] == 'A' AND inString[2] == 'P' AND
      inString[3] == 'I' AND inString[4] == ' ')
  {
    UBYTE  sapi = 0;
    UBYTE  i = 5;

    /* get first sapi number */
    if (inString[i] >= '0' AND inString[i] <= '9')
    {
      sapi = (inString[i] - '0');
      i++;
    }

    /* get second sapi number, if available */
    if (inString[i] >= '0' AND inString[i] <= '9')
    {
      sapi = (sapi * 10) + (inString[i] - '0');
      i++;
    }

    /* skip next whitespace */
    i++;

    /* get XID value to change */
    if (inString[i] == 'K' AND inString[i+2] == ' ')
    {
      if (inString[i+1] == 'U')      /* KU */
      {
        llc_data->ffs_xid.ku[IMAP(sapi)].valid = TRUE;
        llc_data->ffs_xid.ku[IMAP(sapi)].value = (UBYTE)atoi(&inString[i+3]);
      }
      else                           /* KD */
      {
        llc_data->ffs_xid.kd[IMAP(sapi)].valid = TRUE;
        llc_data->ffs_xid.kd[IMAP(sapi)].value = (UBYTE)atoi(&inString[i+3]);
      }
    }
    else if (inString[i] == 'M' AND inString[i+2] == ' ')
    {
      if (inString[i+1] == 'U')      /* MU */
      {
        llc_data->ffs_xid.mu[IMAP(sapi)].valid = TRUE;
        llc_data->ffs_xid.mu[IMAP(sapi)].value = (UBYTE)atoi(&inString[i+3]);
      }
      else                           /* MD */
      {
        llc_data->ffs_xid.md[IMAP(sapi)].valid = TRUE;
        llc_data->ffs_xid.md[IMAP(sapi)].value = (UBYTE)atoi(&inString[i+3]);
      }
    }
    else if (inString[i+0] == 'N' AND inString[i+1] == '2' AND inString[i+2] == '0' AND
             inString[i+3] == '1' AND inString[i+4] == '_' AND inString[i+6] == ' '   )
    {
      if (inString[i+5] == 'I')      /* N201_I */
      {
        llc_data->ffs_xid.n201_i[IMAP(sapi)].valid = TRUE;
        llc_data->ffs_xid.n201_i[IMAP(sapi)].value = (USHORT)atoi(&inString[i+7]);
      }
      else if (inString[i+5] == 'U') /* N201_U */
      {
        llc_data->ffs_xid.n201_u[UIMAP(sapi)].valid = TRUE;
        llc_data->ffs_xid.n201_u[UIMAP(sapi)].value = (USHORT)atoi(&inString[i+7]);
      }
    }
    else if (inString[i+1] == '2' AND inString[i+2] == '0' AND inString[i+3] == '0' AND
             inString[i+4] == ' ')
    {
      if (inString[i+0] == 'N')      /* N200 */
      {
        llc_data->ffs_xid.n200[UIMAP(sapi)].valid = TRUE;
        llc_data->ffs_xid.n200[UIMAP(sapi)].value = (UBYTE)atoi(&inString[i+5]);
      }
      else if (inString[i+0] == 'T') /* T200 */
      {
        llc_data->ffs_xid.t200[UIMAP(sapi)].valid = TRUE;
        llc_data->ffs_xid.t200[UIMAP(sapi)].value = (USHORT)atoi(&inString[i+5]);
      }
    }
  } 

#endif

#else /*_SIMULATION_*/
/* CCI_INFO */ 
  if(inString[0] == 'C' AND 
     inString[1] == 'C' AND 
     inString[2] == 'I' AND
     inString[3] == '_' AND 
     inString[4] == 'I' AND
     inString[5] == 'N' AND
     inString[6] == 'F' AND
     inString[7] == 'O') {
    TRACE_EVENT("CCI Info Trace Enabled");
    llc_fbs_enable_cci_info_trace();
  } else if (inString[0] == 'D' &&
               inString[1] == 'E' &&
               inString[2] == 'L' &&
               inString[3] == 'A' &&
               inString[4] == 'Y') {

      USHORT millis = (USHORT)atoi(&inString[5]);
      llc_data->millis = millis;
      TRACE_1_OUT_PARA("Delay timer :%d milliseconds", llc_data->millis);
  } else {
    TRACE_EVENT("PEI ERROR: invalid config primitive");
  }
#endif/*_SIMULATION_*/

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

  /*
   * Version = "0.S" (S = Step).
   */
  llc_mon.version = "LLC 1.0";
  *out_monitor = &llc_mon;

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
               "LLC",         /* name */
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
               2560,          /* stack size */
               32,            /* queue entries */
               200,           /* priority (1->low, 255->high) */
               TIMER_NUM,     /* number of timers */
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

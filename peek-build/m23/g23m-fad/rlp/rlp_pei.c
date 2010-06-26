/*
+-----------------------------------------------------------------------------
|  Project :  CSD (8411)
|  Modul   :  Rlp_pei.c
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
|             for the component Radio Link Protocol of the base station
+-----------------------------------------------------------------------------
*/

#ifndef RLP_PEI_C
#define RLP_PEI_C
#endif

#define ENTITY_RLP

/*==== INCLUDES ===================================================*/

#include <string.h>
#include <stdlib.h>
#include "typedefs.h"
#include "pconst.cdg"
#include "vsi.h"
#include "custom.h"
#include "tools.h"
#include "gsm.h"
#include "cus_rlp.h"
#include "cnf_rlp.h"
#include "mon_rlp.h"
#include "prim.h"
#include "pei.h"
#include "tok.h"
#include "rlp.h"

/*==== CONST ======================================================*/

/*==== VAR EXPORT =================================================*/

/*==== VAR LOCAL ==================================================*/

#ifdef OPTION_RELATIVE
LOCAL ULONG offset;
#endif

LOCAL  BOOL              first_access = TRUE;
LOCAL  T_MONITOR         rlp_mon;

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

LOCAL const T_FUNC rlp_table[] = {
  MAK_FUNC_0( ker_rlp_attach_req     ,              RLP_ATTACH_REQ    ),
  MAK_FUNC_0( ker_rlp_detach_req     ,              RLP_DETACH_REQ    ),
  MAK_FUNC_0( ker_rlp_connect_req    ,              RLP_CONNECT_REQ   ),
  MAK_FUNC_0( ker_rlp_connect_res    ,              RLP_CONNECT_RES   ),
  MAK_FUNC_0( ker_rlp_disc_req       ,              RLP_DISC_REQ      ),
  MAK_FUNC_0( ker_rlp_reset_req      ,              RLP_RESET_REQ     ),
  MAK_FUNC_0( ker_rlp_reset_res      ,              RLP_RESET_RES     ),
  MAK_FUNC_S( ker_rlp_data_req       ,              RLP_DATA_REQ      ),
  MAK_FUNC_0( ker_rlp_getdata_req    ,              RLP_GETDATA_REQ   ),
  MAK_FUNC_S( ker_rlp_ui_req         ,              RLP_UI_REQ        ),
  MAK_FUNC_0( ker_rlp_remap_req      ,              RLP_REMAP_REQ     ),
  MAK_FUNC_0( ker_rlp_remap_data_res ,              RLP_REMAP_DATA_RES)
};

/*
 * jumptable for the service access point which are used by
 * the entity. Contains the processing-function addresses and
 * opcodes of indication and confirm primitives.
 *
 */
LOCAL const T_FUNC ra_table[] = {
  MAK_FUNC_0( rcv_ra_ready_ind    ,              RA_READY_IND     ),
  MAK_FUNC_S( rcv_ra_data_ind     ,              RA_DATA_IND      )
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
|  Return      : PEI_OK    -
|                PEI_ERROR -
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_primitive (void * ptr)
{
  T_PRIM * prim = ptr;
  /*
   * @ADAPT@
   *                    |
   *                   L2R                 UPPER LAYER
   *                    |
   *                    v
   *      +-----------(rlp)----------+
   *      |                          |
   *      |            RLP           |
   *      |                          |
   *      +-------------^------------+
   *                    |
   *                   RA                  LOWER LAYER
   *                    |
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

    rlp_data = GET_INSTANCE (prim);

    switch (SAP_NR(opc))
    {
      case SAP_NR(RLP_UL): table =  rlp_table; n = TAB_SIZE (rlp_table); break;
      case SAP_NR(RA_DL ): table =  ra_table;  n = TAB_SIZE (ra_table);  break;
      default    : table =  NULL;      n = 0;                    break;
    }

    if (table NEQ NULL )
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


LOCAL SHORT pei_init (T_HANDLE handle)
{
  rlp_handle = handle;

  TRACE_FUNCTION ("pei_init()");

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

  rlp_data = &rlp_data_base[0];
  rlp_data_magic_num = 0;  /* memory is not yet initialized */

#ifdef _SIMULATION_
  /*
  to ensure that an NULL frame can be sent
  if RLP receives an RA_READY_INDICATION in detached state
  */
  {
    USHORT t;

    /*******************************/
    /*
    *  Initialize Shared Fax/Data memory
    */

    rlp_data = &rlp_data_base[0];
    memset(rlp_data, 0, sizeof (T_RLP_DATA));
    ker_init ();
    snd_init (&rlp_data->snd);
    rcv_init ();
    rlp_data->uplink_frame_trace   = FALSE;
    rlp_data->downlink_frame_trace = FALSE;
    rbm_reset(&rlp_data->rbm);

    for (t = 0; t < (MAX_SREJ_COUNT+5); t++)
    {
      TIMERSTOP (t);
    }

    rlp_data_magic_num = RLP_DATA_MAGIC_NUM;       /* memory is initialized */

    rlp_data = &rlp_data_base[0];
    sig_ker_snd_set_frame_size_req (FRAME_SIZE_SHORT);
  }

  rlp_data = &rlp_data_base[0];

#endif

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_timeout
+------------------------------------------------------------------------------
|  Description : Process timeout
|
|  Parameters  : index
|
|
|  Return      : PEI_OK
|                PEI_ERROR
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_timeout (USHORT index)
{
  rlp_exec_timeout (index);
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
|  Return      : PEI_OK    -
|                PEI_ERROR -
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_signal (ULONG opc, void *primData)
{
#ifdef OPTION_SIGNAL

  if (rlp_data_magic_num NEQ RLP_DATA_MAGIC_NUM)
    /* memory is not initialized */
  {
    /* ignore received signals */
    return PEI_OK;
  }

  rlp_data = GET_INSTANCE (primData);

  switch (opc)
  {
    case RA_DATA_IND:
      rcv_ra_data_ind((T_RA_DATA_IND *)primData);
      break;

    case RA_READY_IND:
#ifdef _SIMULATION_
      rcv_ra_ready_ind((T_RA_READY_IND *)primData);
#else
      rcv_ra_ready_ind();
#endif
      break;
  }

#endif /* OPTION_SIGNAL */
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
|  Return      : -
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit()");

  /*
   * clean up communication
   */

  vsi_c_close (VSI_CALLER hCommL2R);
  hCommL2R = VSI_ERROR;

#ifdef _SIMULATION_
  vsi_c_close (VSI_CALLER hCommRA);
  hCommRA = VSI_ERROR;
#endif

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
|  Return      : PEI_OK     -
|                PEI_ERROR  -
+------------------------------------------------------------------------------
*/

LOCAL SHORT pei_config (T_PEI_CONFIG inString)
{
#ifndef NCONFIG

  const T_STR_IND tmr_name_to_ind[] =
  {
    { "TUL_RCV", 1 },
    { NULL,      0 }
  };

  TRACE_FUNCTION ("pei_config()");

  if (ConfigTimer(VSI_CALLER inString, tmr_name_to_ind) EQ VSI_OK)
  {
    TRACE_EVENT_P1("TUL_RCV now at: %02X ms", rlp_data->ker.T_ul);
    return PEI_OK;
  }
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
|  Return      : PEI_OK
+------------------------------------------------------------------------------
*/


LOCAL SHORT pei_monitor (void **monitor)
{
  TRACE_FUNCTION ("pei_monitor()");

  rlp_mon.version = VERSION_RLP;

  *monitor = &rlp_mon;

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_create
+------------------------------------------------------------------------------
|  Description : Create the Protocol Stack Entity
|
|  Parameters  : info -
|
|
|  Return      : PEI_OK -
+------------------------------------------------------------------------------
*/

/*lint -e714 : Symbol not referenced */
/*lint -e765 : external could be made static */
GLOBAL SHORT rlp_pei_create (T_PEI_INFO const **info)
{
  static const T_PEI_INFO pei_info =
  {
    RLP_NAME,
    {
      pei_init,
      pei_exit,
      pei_primitive,
      pei_timeout,
      pei_signal,
      NULL,                /* no run function    */
      pei_config,
      pei_monitor
    },
    1024,
    10,
    215,
    MAX_SREJ_COUNT+5,
    0x03|PRIM_NO_SUSPEND
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
   *  Export Startup configuration data
   */

  *info = &pei_info;

  return PEI_OK;
}


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
|             for the component Fax Adaptation 3.45 of the mobile station.
+-----------------------------------------------------------------------------
*/

#ifndef FAD_PEI_C
#define FAD_PEI_C
#endif

#define ENTITY_FAD

/*==== INCLUDES ===================================================*/

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "typedefs.h"
#include "pcm.h"
#include "vsi.h"
#include "custom.h"
#include "gsm.h"
#include "message.h"
#include "ccdapi.h"
#include "prim.h"
#include "cus_fad.h"
#include "cnf_fad.h"
#include "mon_fad.h"
#include "pei.h"
#include "tok.h"
#include "fad.h"

/*==== CONST ======================================================*/

/*==== VAR EXPORT =================================================*/

/*==== VAR LOCAL ==================================================*/

LOCAL BOOL              first_access = TRUE;
LOCAL T_MONITOR         fad_mon;

/*==== FUNCTIONS ==================================================*/

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_PEI             |
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

LOCAL const T_FUNC fad_table[] = {
  MAK_FUNC_0( ker_fad_activate_req  ,           FAD_ACTIVATE_REQ   ),
  MAK_FUNC_0( ker_fad_deactivate_req,           FAD_DEACTIVATE_REQ ),
  MAK_FUNC_0( ker_fad_modify_req,               FAD_MODIFY_REQ     ),
  MAK_FUNC_S( ker_fad_data_req,                 FAD_DATA_REQ       ),
  MAK_FUNC_0( ker_fad_snd_tcf_req,              FAD_SND_TCF_REQ    ),
  MAK_FUNC_0( ker_fad_rcv_tcf_req,              FAD_RCV_TCF_REQ    ),
  MAK_FUNC_0( ker_fad_ready_req,                FAD_READY_REQ      ),
  MAK_FUNC_0( ker_fad_ignore_req ,              FAD_IGNORE_REQ     )
};

/*
 * jumptable for the service access point which are used by
 * the entity. Contains the processing-function addresses and
 * opcodes of indication and confirm primitives.
 *
 */
LOCAL const T_FUNC ra_table[] = {
  MAK_FUNC_0( snd_ra_ready_ind,              RA_READY_IND     ),
  MAK_FUNC_S( rcv_ra_data_ind,               RA_DATA_IND      )
};

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)               MODULE  : FAD_PEI           |
| STATE   : code                         ROUTINE : pei_primitive     |
+--------------------------------------------------------------------+

  PURPOSE : Process protocol specific primitive.

*/
LOCAL SHORT pei_primitive (void * ptr)
{
  T_PRIM *prim = ptr;
  /*
   * @ADAPT@
   *                    |
   *                   T30                 UPPER LAYER
   *                    |
   *                    v
   *      +-----------(fad)----------+
   *      |                          |
   *      |            FAD           |
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
    fad_data = GET_INSTANCE (prim);

    switch (SAP_NR(opc))
    {
      case SAP_NR(FAD_UL): table =  fad_table; n = TAB_SIZE (fad_table); break;
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
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_PEI             |
| STATE   : code                       ROUTINE : pei_init            |
+--------------------------------------------------------------------+

  PURPOSE : Initialize Protocol Stack Entity

*/
LOCAL SHORT pei_init (T_HANDLE handle)
{
  TRACE_FUNCTION ("pei_init()");

  fad_handle = handle;

  if (hCommT30 < VSI_OK)
  {
    if ((hCommT30 = vsi_c_open (VSI_CALLER T30_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

#ifdef _SIMULATION_
  if (hCommRA < VSI_OK)
  {
    if ((hCommRA = vsi_c_open (VSI_CALLER RA_NAME)) < VSI_OK)
      return PEI_ERROR;
  }
#endif

  fad_data = &fad_data_base[0];
  fad_data_magic_num = 0;       /* memory is not yet initialized */
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_PEI             |
| STATE   : code                       ROUTINE : pei_timeout         |
+--------------------------------------------------------------------+

  PURPOSE : Process timeout

*/
LOCAL SHORT pei_timeout (USHORT index)
{

  fad_exec_timeout (index);

  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_PEI             |
| STATE   : code                       ROUTINE : pei_signal          |
+--------------------------------------------------------------------+

  PURPOSE : Functional interface to signal a primitive.

*/

LOCAL SHORT pei_signal (ULONG opc, void *primData)
{
#ifdef OPTION_SIGNAL

  if (fad_data_magic_num NEQ FAD_DATA_MAGIC_NUM)
    /* memory is not initialized */
  {
    /* ignore received signals */
    return PEI_OK;
  }

  fad_data = GET_INSTANCE (primData);

  switch (opc)
  {
    case RA_DATA_IND:
      rcv_ra_data_ind
      (
        (T_RA_DATA_IND *)primData
      );
      break;
    case RA_READY_IND:
      snd_ra_ready_ind
      (
        (T_RA_READY_IND *)primData
      );
      break;
  }
#endif
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_PEI             |
| STATE   : code                       ROUTINE : pei_exit            |
+--------------------------------------------------------------------+

  PURPOSE : Close Resources and terminate

*/

LOCAL SHORT pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit()");

  /*
   * clean up communication
   */
  vsi_c_close (VSI_CALLER hCommT30);
  hCommT30= VSI_ERROR;

  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_PEI             |
| STATE   : code                       ROUTINE : pei_config          |
+--------------------------------------------------------------------+

  PURPOSE : Dynamic Configuration

*/

#if !defined (NCONFIG)

  LOCAL KW_DATA kwtab[] = {
    "",                     0
  };

#endif

LOCAL SHORT pei_config (T_PEI_CONFIG inString)
{
#if !defined (NCONFIG)
  char    *s = inString;
  char    *keyw;
  char    *val [10];

  TRACE_FUNCTION ("pei_config()");
  TRACE_EVENT (s);

  tok_init (s);

  /*
   * Parse next keyword and number of variables
   */

  while (tok_next(&keyw,val) NEQ TOK_EOCS)
  {
    switch (tok_key((KW_DATA *)kwtab, keyw))
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
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_PEI             |
| STATE   : code                       ROUTINE : pei_monitor         |
+--------------------------------------------------------------------+

  PURPOSE : Monitoring of physical Parameters

*/
LOCAL SHORT pei_monitor (void **monitor)
{
  TRACE_FUNCTION ("pei_monitor()");

  fad_mon.version = VERSION_FAD;
  *monitor = &fad_mon;
  return PEI_OK;
}

/*
+--------------------------------------------------------------------+
| PROJECT : GSM-F&D (8411)             MODULE  : FAD_PEI             |
| STATE   : code                       ROUTINE : pei_create          |
+--------------------------------------------------------------------+

  PURPOSE : Create the Protocol Stack Entity

*/

/*lint -e714 : Symbol not referenced */
/*lint -e765 : external could be made static */
GLOBAL SHORT fad_pei_create (T_PEI_INFO **info)
{
  static const T_PEI_INFO pei_info =
  {
    "FAD",
    {
      pei_init,
      pei_exit,
      pei_primitive,
      pei_timeout,
      pei_signal,
      NULL,                  /* no run function     */
      pei_config,
      pei_monitor
    },
    1024, /* Stack Size      */
    10,   /* Queue Entries   */
    215,  /* Priority        */
    0,    /* number of timer */
    0x03|PRIM_NO_SUSPEND  /* flags           */
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
   *  Export startup configuration data
   */

  *info = (T_PEI_INFO *)&pei_info;

  return PEI_OK;
}

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
|             for the entity Point-to-Point Protocol (PPP)
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

#define PPP_PEI_C

#define ENTITY_PPP

/*==== INCLUDES =============================================================*/

#include <stddef.h>
#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
/*lint -efile(766,macdef.h) */
#include "macdef.h"     /* to get a lot of macros */
#include "custom.h"     /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
/*lint -efile(766,cnf_ppp.h) */
#include "cnf_ppp.h"    /* to get cnf-definitions */
#include "mon_ppp.h"    /* to get mon-definitions */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "dti.h"        /* to get the DTILIB definitions */
#include "ppp.h"        /* to get the global entity definitions */

#ifdef FF_STATUS_TE
#include "gdi.h"        /* To include driver type-defines */
#include "ste.h"        /* To include function interface for TE status driver */
#endif /* FF_STATUS_TE */

#include "ppp_rtf.h"     /* to get rt functions */
#include "ppp_rtp.h"     /* to get rt primitives */
#include "ppp_rts.h"     /* to get rt signals */

/*lint -efile(766,ppp_prxp.h) */
#include "ppp_prxf.h"     /* to get prx functions */
#include "ppp_prxp.h"     /* to get prx primitives */
#include "ppp_prxs.h"     /* to get prx signals */

/*lint -efile(766,ppp_ptxp.h) */
#include "ppp_ptxf.h"     /* to get ptx functions */
#include "ppp_ptxp.h"     /* to get ptx primitives */
#include "ppp_ptxs.h"     /* to get ptx signals */

/*lint -efile(766,ppp_frxp.h) */
#include "ppp_frxf.h"     /* to get frx functions */
#include "ppp_frxp.h"     /* to get frx primitives */
#include "ppp_frxs.h"     /* to get frx signals */

/*lint -efile(766,ppp_ftxp.h) */
#include "ppp_ftxf.h"     /* to get ftx functions */
#include "ppp_ftxp.h"     /* to get ftx primitives */
#include "ppp_ftxs.h"     /* to get ftx signals */

#include "ppp_arbf.h"     /* to get arb functions */
#include "ppp_arbp.h"     /* to get arb primitives */
#include "ppp_arbs.h"     /* to get arb signals */

/*lint -efile(766,ppp_lcpp.h) */
/*lint -efile(766,ppp_lcps.h) */
#include "ppp_lcpf.h"     /* to get lcp functions */
#include "ppp_lcpp.h"     /* to get lcp primitives */
#include "ppp_lcps.h"     /* to get lcp signals */

/*lint -efile(766,ppp_onap.h) */
/*lint -efile(766,ppp_onas.h) */
#include "ppp_onaf.h"     /* to get ona functions */
#include "ppp_onap.h"     /* to get ona primitives */
#include "ppp_onas.h"     /* to get ona signals */

/*lint -efile(766,ppp_ncpp.h) */
/*lint -efile(766,ppp_ncps.h) */
#include "ppp_ncpf.h"     /* to get ncp functions */
#include "ppp_ncpp.h"     /* to get ncp primitives */
#include "ppp_ncps.h"     /* to get ncp signals */

/*lint -efile(766,ppp_papp.h) */
/*lint -efile(766,ppp_paps.h) */
#include "ppp_papf.h"     /* to get pap functions */
#include "ppp_papp.h"     /* to get pap primitives */
#include "ppp_paps.h"     /* to get pap signals */

/*lint -efile(766,ppp_capp.h) */
/*lint -efile(766,ppp_caps.h) */
#include "ppp_capf.h"     /* to get chap functions */
#include "ppp_capp.h"     /* to get chap primitives */
#include "ppp_caps.h"     /* to get chap signals */

#include "ppp_dti.h"      /* to get the DTI signals */

/*==== DEFINITIONS ==========================================================*/

/*==== TYPES ================================================================*/

/*==== GLOBAL VARS ==========================================================*/

/*==== LOCAL VARS ===========================================================*/

static  BOOL          first_access  = TRUE;
static  T_MONITOR     ppp_mon;

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
LOCAL void pei_dti_dti_ready_ind   (T_DTI2_READY_IND        *dti_ready_ind);
LOCAL void pei_dti_dti_data_req    (T_DTI2_DATA_REQ         *dti_data_req);
LOCAL void pei_dti_dti_data_ind    (T_DTI2_DATA_IND         *dti_data_ind);
LOCAL void pei_dti_dti_getdata_req (T_DTI2_GETDATA_REQ      *dti_getdata_req);
LOCAL void pei_dti_dti_data_test_ind (T_DTI2_DATA_TEST_IND  *dti_data_test_ind);
LOCAL void pei_dti_dti_data_test_req (T_DTI2_DATA_TEST_REQ  *dti_data_test_req);
LOCAL void pei_dti_dti_connect_req    (T_DTI2_CONNECT_REQ    *dti_connect_req );
LOCAL void pei_dti_dti_disconnect_ind (T_DTI2_DISCONNECT_IND *dti_disconnect_ind);   
LOCAL void pei_dti_dti_connect_cnf    (T_DTI2_CONNECT_CNF    *dti_connect_cnf);
LOCAL void pei_dti_dti_connect_ind    (T_DTI2_CONNECT_IND    *dti_connect_ind);
LOCAL void pei_dti_dti_connect_res    (T_DTI2_CONNECT_RES    *dti_connect_res);
LOCAL void pei_dti_dti_disconnect_req (T_DTI2_DISCONNECT_REQ *dti_disconnect_req);


static const T_FUNC ppp_table[] =
{
  MAK_FUNC_0(arb_ppp_establish_req,      PPP_ESTABLISH_REQ),    /* 7500 */
  MAK_FUNC_0(arb_ppp_terminate_req,      PPP_TERMINATE_REQ),    /* 7501 */
  MAK_FUNC_S(arb_ppp_pdp_activate_res,  PPP_PDP_ACTIVATE_RES),/* 7502 */
  MAK_FUNC_0(arb_ppp_pdp_activate_rej,  PPP_PDP_ACTIVATE_REJ),/* 7503 */
  MAK_FUNC_0(arb_ppp_modification_req,  PPP_MODIFICATION_REQ)  /* 7504 */
};

LOCAL const T_FUNC dti_dl_table[] = {
  MAK_FUNC_0( pei_dti_dti_connect_ind    ,   DTI2_CONNECT_IND   ),
  MAK_FUNC_0( pei_dti_dti_connect_cnf    ,   DTI2_CONNECT_CNF   ),
  MAK_FUNC_0( pei_dti_dti_disconnect_ind ,   DTI2_DISCONNECT_IND)
  ,
  MAK_FUNC_0( pei_dti_dti_ready_ind      ,   DTI2_READY_IND      ),
  MAK_FUNC_0( pei_dti_dti_data_ind       ,   DTI2_DATA_IND       )
#if defined (_SIMULATION_)
  ,
  MAK_FUNC_S( pei_dti_dti_data_test_ind  ,  DTI2_DATA_TEST_IND   )
#endif
};

LOCAL const T_FUNC dti_ul_table[] = {
  MAK_FUNC_0( pei_dti_dti_connect_req       ,   DTI2_CONNECT_REQ   ),
  MAK_FUNC_0( pei_dti_dti_connect_res       ,   DTI2_CONNECT_RES   ),
  MAK_FUNC_0( pei_dti_dti_disconnect_req,       DTI2_DISCONNECT_REQ)
  ,
  MAK_FUNC_0( pei_dti_dti_getdata_req       ,   DTI2_GETDATA_REQ    ),
  MAK_FUNC_0( pei_dti_dti_data_req          ,   DTI2_DATA_REQ       )

#if defined (_SIMULATION_)
  ,
  MAK_FUNC_S( pei_dti_dti_data_test_req     ,   DTI2_DATA_TEST_REQ  )
#endif
};

/*==== DIAGNOSTICS ==========================================================*/
#ifdef _DEBUG
#endif /* _DEBUG */

/*==== END DIAGNOSTICS ======================================================*/

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
|  Function    : primitive_not_supported
+------------------------------------------------------------------------------
|  Description  :  This function handles unsupported primitives.
|
|  Parameters  :  -
|
|  Return      :  -
|
+------------------------------------------------------------------------------
*/
LOCAL void primitive_not_supported (void *data)
{
  TRACE_FUNCTION ("primitive_not_supported");

  PFREE (data);
}


/*==== PUBLIC FUNCTIONS =====================================================*/

/* qqq hier malen
+------------------------------------------------------------------------------
|  Function    : pei_primitive
+------------------------------------------------------------------------------
|  Description  :  This function is called by the frame when a primitive is
|                  received and needs to be processed.
|
|                            |        |
|                           MMI      DTI                 UPLINK
|                            |        |
|                   +--------v--------v--------+
|                   |                          |
|                   |            PPP           |
|                   |                          |
|                   +-------------^------------+
|                                 |
|                                DTI                     DOWNLINK
|                                 |
|
|
|  Parameters  :  prim      - Pointer to the received primitive
|
|  Return      :  PEI_OK    - function succeeded
|                 PEI_ERROR - function failed
|
+------------------------------------------------------------------------------
*/

/*qqq hier kucken*/
LOCAL SHORT pei_primitive (void * primptr)
{
  TRACE_FUNCTION ("pei_primitive");

  if (primptr NEQ NULL)
  {
    T_PRIM *prim  = (T_PRIM *)primptr;
    USHORT           opc = (USHORT)prim->custom.opc;
    USHORT           n;
    const T_FUNC    *table;

    /*
     * This must be called for Partition Pool supervision. Will be replaced
     * by another macro some time.
     */
    VSI_PPM_REC (&prim->custom, __FILE__, __LINE__);
    PTRACE_IN (opc);

    switch (opc & OPC_MASK)
    {
      case PPP_DL:
        table = ppp_table;
        n = TAB_SIZE (ppp_table);
        break;
      case DTI_UL:

        table = dti_ul_table;
        n = TAB_SIZE (dti_ul_table);
        break;
      case DTI_DL:
        table = dti_dl_table;
        n = TAB_SIZE (dti_dl_table);
        break;
      default:
        table = NULL;
        n = 0;
        break;
    }

    if (table NEQ NULL)
    {
#define PRIM_OFFSET_DTI2 0x50
#define PRM_MASK2 0x000F
      if (((opc & PRM_MASK) - PRIM_OFFSET_DTI2) < n)
      {
        table += opc & PRM_MASK2;
#ifdef PALLOC_TRANSITION
        P_SDU(prim) = table->soff ? (T_sdu*) (((char*)&prim->data) + table->soff) : 0; /*lint !e740 !e545 */
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
      vsi_c_primitive (VSI_CALLER prim); /*lint !e534 */
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
|  Function    : pei_init
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame. It is used to initialise
|                 the entitiy.
|
|  Parameters  :  handle            - task handle
|
|  Return      :  PEI_OK            - entity initialised
|                 PEI_ERROR         - entity not (yet) initialised
|
+------------------------------------------------------------------------------
*/

/*qqq hier Kommunikationskanaele eintragen*/
LOCAL SHORT pei_init (T_HANDLE handle)
{

  TRACE_FUNCTION ("pei_init");

  /*
   * Initialize task handle
   */
  PPP_handle = handle;
  /*
   * Open communication channel
   */
  if (hCommMMI < VSI_OK)
  {
    if ((hCommMMI = vsi_c_open (VSI_CALLER ACI_NAME)) < VSI_OK) /*lint !e605 */
      return PEI_ERROR;
  }

  if (hCommPPP < VSI_OK)
  {
    if ((hCommPPP = vsi_c_open (VSI_CALLER PPP_NAME)) < VSI_OK) /*lint !e605 */
      return PEI_ERROR;
  }

  /*
   * Initialize global pointer ppp_data. This is required to access all
   * entity data.
   */
  ppp_data = &ppp_data_base;
  ppp_data->fcstab      = fcstab_base;
  ppp_data->ftx.accmtab = accmtab_base;


  /*
   * Initialize entity data (call init function of every service)
   */
  rt_init();
  prx_init();
  ptx_init();
  frx_init();
  ftx_init();
  arb_init();
  lcp_init();
  ona_init();
  ncp_init();
  pap_init();
  chap_init();

  /*
   * initialize global entity variables
   */
  ppp_data->mc = PPP_MC_DEFAULT;
  ppp_data->mt = PPP_MT_DEFAULT;
  ppp_data->mf = PPP_MF_DEFAULT;

    /*
     * a value different from 0 indicates an error or 
     * administrative termination
     */
  ppp_data->ppp_cause = 0;

  ppp_data->mru  = PPP_MRU_DEFAULT;
  ppp_data->n_hc = PPP_HC_OFF;

  ppp_data->pco_mask = PPP_PCO_MASK_DEFAULT;

 /*
  * Init DTILIB channels and allocate the DTI Database
  */     
  if ( (
    ppp_data->ppphDTI = dti_init (
      MAX_PPP_LINKS,
      PPP_handle,
      DTI_DEFAULT_OPTIONS,
      sig_callback
      )
    ) EQ D_NO_DATA_BASE)
  {
    TRACE_ERROR("Open the DTILIB Databank not correct");
    return PEI_ERROR;
  }
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : pei_timeout
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when a timer has expired.
|
|  Parameters  :  index             - timer index
|
|  Return      :  PEI_OK            - timeout processed
|                 PEI_ERROR         - timeout not processed
|
+------------------------------------------------------------------------------
*/
/*qqq hier noch ppp timer eintragen!!!*/
LOCAL SHORT pei_timeout (USHORT index)
{
  TRACE_FUNCTION ("pei_timeout");

  /*
   * Process timeout
   */
  switch (index)
  {
    case RT_INDEX:
      /*
       * RT expired.
       */
      rt_rt_expired();
      break;
    default:
      TRACE_ERROR("Unknown Timeout");
      break;
  }

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|  Function    : pei_signal
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when a signal has been 
|                 received.
|
|  Parameters  :  opc               - signal operation code
|                 *data             - pointer to primitive
|
|  Return      :  PEI_OK            - signal processed
|                 PEI_ERROR         - signal not processed
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
#ifdef FF_STATUS_TE
    /*
     * Call-back from TE status driver - power status has changed
     */
    case NEW_POWER_STATUS:      
      new_power_status_TE ( );
      break;
#endif /* FF_STATUS_TE */

    default:
      TRACE_ERROR("Unknown Signal OPC");
      break;
  } /*lint !e764 switch statement does not have a case */

  return PEI_OK;
} /*lint !e715 data not referenced */



/*
+------------------------------------------------------------------------------
|  Function    : pei_exit
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when the entity is
|                 terminated. All open resources are freed.
|
|  Parameters  :  -
|
|  Return      :  PEI_OK            - exit sucessful
|                 PEI_ERROR         - exit not sueccessful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit");

  /*
   * Stop timer
   */
  sig_any_rt_srt_req ();

  /*
   * Close communication channel
   */
  vsi_c_close (VSI_CALLER hCommMMI); /*lint !e534 Ignoring return value */
  hCommMMI = VSI_ERROR;

  /*
   *  Deinit the DTI Database
   */
  dti_deinit(ppp_data->ppphDTI);

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|  Function    : pei_run
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when entering the main
|                 loop. This fucntion is only required in the active variant.
|
|                 This function is not used.
|
|  Parameters  :  handle            - Communication handle
|
|  Return      :  PEI_OK            - sucessful
|                 PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle )
{
  return PEI_OK;
} /*lint !e715 TaskHandle and ComHandle not referenced */



/*
+------------------------------------------------------------------------------
|  Function    : pei_config
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame when a primitive is
|                 received indicating dynamic configuration.
|
|                 This function is not used in this entity.
|
|  Parameters  :  handle            - Communication handle
|
|  Return      :  PEI_OK            - sucessful
|                 PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_config (char *inString)
{
#ifndef NCONFIG
  ULONG new_mask;
#endif /* !NCONFIG */

  TRACE_FUNCTION ("pei_config");
  TRACE_FUNCTION (inString);

#ifndef NCONFIG
  /*
   * Parse next keyword and number of variables
   */

  /*if(!strcmp(inString,"PCO="))*/
  if (inString[0] EQ 'P' &&
      inString[1] EQ 'C' &&
      inString[2] EQ 'O' &&
      inString[3] EQ '=')
  {
    TRACE_EVENT ("New PCO content:");
    new_mask = 0;
    /*
     * LCP MRU
     */
    if((inString[4] EQ 0x31) ||
       ((inString[4] NEQ 0x30) && 
        (ppp_data->pco_mask & PPP_PCO_MASK_LCP_MRU)))
    {
      TRACE_EVENT(" - LCP MRU");
      new_mask|= PPP_PCO_MASK_LCP_MRU;
    }
    /*
     * LCP AP
     */
    if((inString[5] EQ 0x31) ||
       ((inString[5] NEQ 0x30) && 
        (ppp_data->pco_mask & PPP_PCO_MASK_LCP_AP)))
    {
      TRACE_EVENT(" - LCP Authentication protocol");
      new_mask|= PPP_PCO_MASK_LCP_AP;
    }
    /*
     * LCP TWO
     */
    if((inString[6] EQ 0x31) ||
       ((inString[6] NEQ 0x30) && 
        (ppp_data->pco_mask & PPP_PCO_MASK_LCP_TWO)))
    {
      TRACE_EVENT(" - two LCP packets");
      new_mask|= PPP_PCO_MASK_LCP_TWO;
    }
    /*
     * Authentication Protocol
     */
    if((inString[7] EQ 0x31) ||
       ((inString[7] NEQ 0x30) && 
        (ppp_data->pco_mask & PPP_PCO_MASK_AUTH_PROT)))
    {
      TRACE_EVENT(" - PAP/CHAP Authentication packet(s)");
      new_mask|= PPP_PCO_MASK_AUTH_PROT;
    }
    /*
     * IPCP HC
     */
    if((inString[8] EQ 0x31) ||
       ((inString[8] NEQ 0x30) && 
        (ppp_data->pco_mask & PPP_PCO_MASK_IPCP_HC)))
    {
      TRACE_EVENT(" - VJ Header Compression");
      new_mask|= PPP_PCO_MASK_IPCP_HC;
    }
    /*
     * IPCP IP
     */
    if((inString[9] EQ 0x31) ||
       ((inString[9] NEQ 0x30) && 
        (ppp_data->pco_mask & PPP_PCO_MASK_IPCP_IP)))
    {
      TRACE_EVENT(" - IP address");
      new_mask|= PPP_PCO_MASK_IPCP_IP;
    }
    /*
     * IPCP PDNS
     */
    if((inString[10] EQ 0x31) ||
       ((inString[10] NEQ 0x30) && 
        (ppp_data->pco_mask & PPP_PCO_MASK_IPCP_PDNS)))
    {
      TRACE_EVENT(" - primary DNS address");
      new_mask|= PPP_PCO_MASK_IPCP_PDNS;
    }
    /*
     * IPCP SDNS
     */
    if((inString[11] EQ 0x31) ||
       ((inString[11] NEQ 0x30) && 
        (ppp_data->pco_mask & PPP_PCO_MASK_IPCP_SDNS)))
    {
      TRACE_EVENT(" - secondary DNS address");
      new_mask|= PPP_PCO_MASK_IPCP_SDNS;
    }
    /*
     * IPCP GATEWAY
     */
    if((inString[12] EQ 0x31) ||
       ((inString[12] NEQ 0x30) && 
        (ppp_data->pco_mask & PPP_PCO_MASK_IPCP_GATEWAY)))
    {
      TRACE_EVENT(" - Gateway address");
      new_mask|= PPP_PCO_MASK_IPCP_GATEWAY;
    }

    if(new_mask EQ 0)
    {
      TRACE_EVENT(" - no content");
    }
    /*
     * set new PCO mask
     */
    ppp_data->pco_mask = new_mask;
  }
  else
  {
    TRACE_ERROR("config command does not exist");
  }
#endif /* !NCONFIG */

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|  Function    : pei_config
+------------------------------------------------------------------------------
|  Description  : This function is called by the frame in case sudden entity
|                 specific data is requested (e.g. entity Version).
|
|  Parameters  :  out_monitor       - return the address of the data to be
|                                     monitoredCommunication handle
|
|  Return      :  PEI_OK            - sucessful (address in out_monitor is valid)
|                 PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  TRACE_FUNCTION ("pei_monitor");

  /*
   * Version = "0.S" (S = Step).
   */
  ppp_mon.version = VERSION_PPP; /*lint !e605 Increase in pointer capability */
  *out_monitor = &ppp_mon;

  return PEI_OK;
}



/*
+------------------------------------------------------------------------------
|  Function    : pei_create
+------------------------------------------------------------------------------
|  Description  :  This function is called by the frame when the process is 
|                  created.
|
|  Parameters  :  out_name          - Pointer to the buffer in which to locate
|                                     the name of this entity
|
|  Return      :  PEI_OK            - entity created successfuly
|                 PEI_ERROR         - entity could not be created
|
+------------------------------------------------------------------------------
*/
GLOBAL SHORT pei_create (T_PEI_INFO **info)
{
  static T_PEI_INFO pei_info =
              {
               "PPP",         /*lint !e605 Increase in pointer capability */
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
               2048,          /* stack size */
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
    pei_exit(); /*lint !e534 Ignoring return value */

  /*
   * Export startup configuration data
   */
  *info = &pei_info;

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function    : sig_callback
+------------------------------------------------------------------------------
|  Description  :  This is the DTI Callback Function which handles the DTI signals.
|          
|
|  Parameters  :  instance        -   DTI instance  
|                 inter_face      -   DTI interface
|                 channel         -   DTI channel
|                 reason          -   DTI reason
|                 *dti_data_ind   -   Ptr. to the data primitive
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_callback(U8 instance, U8 inter_face, U8 channel,
                         U8 reason, T_DTI2_DATA_IND *dti_data_ind)
{
  DTI_HANDLE hDTI;

  TRACE_FUNCTION("sig_callback");
  
  hDTI = ppp_data->ppphDTI;
  
  if (hDTI NEQ D_NO_DATA_BASE)
  {
    switch (reason)
    {
      case DTI_REASON_CONNECTION_OPENED:
        /*
         *  DTI connect
         */
        sig_dti_arb_connection_opened_ind (hDTI, instance, inter_face, channel);
        break;

      case DTI_REASON_CONNECTION_CLOSED:
        /*
         *  DTI disconnect
         */
        sig_dti_arb_connection_closed_ind (hDTI, instance, inter_face, channel);
        break;

      case DTI_REASON_DATA_RECEIVED:
        if(inter_face EQ PROT_LAYER)          
          /*
           *  Data packet recived from Protocol Layer. Process it and send.
           */
          sig_dti_prx_data_received_ind(dti_data_ind);
        else
          /*
           *  Data packet recived from Peer Layer. Process it and send.
           */            
          sig_dti_frx_data_received_ind (dti_data_ind);
        break;

      case DTI_REASON_TX_BUFFER_FULL:
        if(inter_face EQ PROT_LAYER)
            /*
             *  Buffer full for sending data to Protocol Layer.
             */
            sig_dti_ptx_tx_buffer_full_ind();
         else
            /*
             *  Buffer full for sending data to Peer Layer.
             */
            sig_dti_ftx_tx_buffer_full_ind ();
        break;

      case DTI_REASON_TX_BUFFER_READY:
        if(inter_face EQ PROT_LAYER)
            /*
             *  Buffer ready Protocol Layer.
             */
            sig_dti_ptx_tx_buffer_ready_ind();
          else
            /*
             *  Buffer ready Peer Layer.
             */
            sig_dti_ftx_tx_buffer_ready_ind();
        break;

      default:
        TRACE_ERROR("Pointer to no DTI database");
        break;
    } /* end switch */
  } /* end if */
  else 
  {
    TRACE_ERROR("Pointer to DTILIB database non existing");
  }
} /* sig_callback() */

/*
+------------------------------------------------------------------------------
| Function    : sig_any_dti_data_ind
+------------------------------------------------------------------------------
| Description : This function sends a Data Primitive
|
| Parameters  : *hDTI           -   Ptr. to DTI Databank
|               *dti_data_ind   -   Ptr. to Primitive which shall be sended
|               instance        -   DTI instance
|               interface       -   DTI interface
|               channel         -   DTI channel
|
+------------------------------------------------------------------------------
*/

GLOBAL void sig_any_dti_data_ind (
  DTI_HANDLE hDTI,
  T_DTI2_DATA_IND *dti_data_ind,
  UBYTE instance,
  UBYTE inter_face,
  UBYTE channel,
  UBYTE p_id
  )
{
  T_desc2*  temp_desc;

  TRACE_FUNCTION ("sig_any_dti_data_ind()");
                                            
  dti_data_ind->parameters.p_id                   = p_id;
  dti_data_ind->parameters.st_lines.st_flow       = DTI_FLOW_ON;
  dti_data_ind->parameters.st_lines.st_line_sa    = DTI_SA_ON;
  dti_data_ind->parameters.st_lines.st_line_sb    = DTI_SB_ON;
  dti_data_ind->parameters.st_lines.st_break_len  = DTI_BREAK_OFF;

  /*
   *  This PPP does not support handle of offset in the descriptor. 
   *  Offset is set to 0 and size to len.
   */
  temp_desc = (T_desc2*)dti_data_ind->desc_list2.first;
  while(temp_desc)
  {
    temp_desc->offset = 0;
    temp_desc->size   = temp_desc->len;
    temp_desc         = (T_desc2*)temp_desc->next;
  }
  
  dti_send_data(hDTI, instance, inter_face, channel, dti_data_ind);
} /* sig_any_dti_data_ind () */


/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_connect_req
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_connect_req
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/

GLOBAL void pei_dti_dti_connect_req (
                    T_DTI2_CONNECT_REQ   *dti_connect_req
                  )
{
    dti_dti_connect_req (ppp_data->ppphDTI, dti_connect_req);
}


/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_disconnect_ind
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_disconnect_ind
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/

GLOBAL void pei_dti_dti_disconnect_ind (
                    T_DTI2_DISCONNECT_IND   *dti_disconnect_ind
                  )
{
    dti_dti_disconnect_ind (ppp_data->ppphDTI, dti_disconnect_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_connect_cnf
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_connect_cnf
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/
    
GLOBAL void pei_dti_dti_connect_cnf (
                    T_DTI2_CONNECT_CNF   *dti_connect_cnf
                  )
{
    dti_dti_connect_cnf(ppp_data->ppphDTI, dti_connect_cnf);
}


/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_connect_ind
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_connect_ind
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/
    
GLOBAL void pei_dti_dti_connect_ind (
                    T_DTI2_CONNECT_IND   *dti_connect_ind
                  )
{    
    dti_dti_connect_ind(ppp_data->ppphDTI, dti_connect_ind);
}


/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_connect_res
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_connect_res
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/
    
GLOBAL void pei_dti_dti_connect_res (
                    T_DTI2_CONNECT_RES   *dti_connect_res
                  )
{
    dti_dti_connect_res(ppp_data->ppphDTI, dti_connect_res);  
}

/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_disconnect_req
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_disconnect_req
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/
    
GLOBAL void pei_dti_dti_disconnect_req (
                    T_DTI2_DISCONNECT_REQ   *dti_disconnect_req
                  )
{
    dti_dti_disconnect_req (ppp_data->ppphDTI, dti_disconnect_req);
}


/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_ready_ind
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_ready_ind
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/
    
GLOBAL void pei_dti_dti_ready_ind (
                    T_DTI2_READY_IND   *dti_ready_ind
                  )
{
    dti_dti_ready_ind (ppp_data->ppphDTI, dti_ready_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_data_req
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_data_req
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/

GLOBAL void pei_dti_dti_data_req (
                    T_DTI2_DATA_REQ   *dti_data_req
                  )
{
    dti_dti_data_req (ppp_data->ppphDTI, dti_data_req);
}

/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_data_ind
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_data_ind
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/
    
GLOBAL void pei_dti_dti_data_ind (
                    T_DTI2_DATA_IND   *dti_data_ind
                  )
{
    dti_dti_data_ind (ppp_data->ppphDTI, dti_data_ind);
}

/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_getdata_req
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_getdata_req
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/
    
GLOBAL void pei_dti_dti_getdata_req (
                    T_DTI2_GETDATA_REQ   *dti_getdata_req
                  )
{
  dti_dti_getdata_req (ppp_data->ppphDTI, dti_getdata_req);
}
 
/*==== Start functions only use with Windows ===============================*/

#if defined (_SIMULATION_)

/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_data_test_req
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_data_test_req
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/
    
GLOBAL void pei_dti_dti_data_test_req (
                    T_DTI2_DATA_TEST_REQ   *dti_data_test_req
                  )
{
    dti_dti_data_test_req (ppp_data->ppphDTI, dti_data_test_req);
}

/*
+------------------------------------------------------------------------------
| Function    : pei_dti_dti_data_test_ind
+------------------------------------------------------------------------------
| Description : Helpfunction to activate dti_dti_data_test_req
|
| Parameters  : Ptr. to primitive
|
+------------------------------------------------------------------------------
*/
    
GLOBAL void pei_dti_dti_data_test_ind (
                    T_DTI2_DATA_TEST_IND   *dti_data_test_ind
                  )
{
  dti_dti_data_test_ind (ppp_data->ppphDTI, dti_data_test_ind);   
}

#endif /* _SIMULATION_) */

/*==== END OF FILE =========================================================*/

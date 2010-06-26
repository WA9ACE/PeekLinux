/* 
+------------------------------------------------------------------------------
|  File:       xxx_pei.c
+------------------------------------------------------------------------------
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
|  Purpose :  This module implements the PEI interface
|             for the entity xxx.
+----------------------------------------------------------------------------- 
*/ 

#define XXX_PEI_C

#define ENTITY_XXX

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get standard types */
#include "vsi.h"        /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */
#include "mon_xxx.h"    /* to get mon-definitions */
#include "xxx.h"        /* to get the global entity definitions */

/*==== CONSTS ================================================================*/

/* 
 * Wait as long as possible. This is the time in ms that is waited for a 
 * message in the input queue in the active variant. 
 * It can be decreased by the customer.  
 */   
#define XXX_TIMEOUT     0xffffffff   

/*==== TYPES =================================================================*/

/*==== LOCALS ================================================================*/

static  BOOL          first_access  = TRUE;
static  BOOL          exit_flag = FALSE;
static  T_MONITOR     xxx_mon;


/*
 * Jumptables to primitive handler functions. One table per SAP.
 *
 * Use MAK_FUNC_0 for primitives which contains no SDU.
 * Use MAK_FUNC_S for primitives which contains a SDU.
 */

static const T_FUNC aaa_table[] =
{
/*------------------------------------------------------------------------
  The code below can be enabled when the service access points are defined
  ------------------------------------------------------------------------*/
#if 1
  0
#else
  MAK_FUNC_S(aaa_establish_req,         AAA_ESTABLISH_REQ),
#endif
/*------------------------------------------------------------------------
  The code above can be enabled when the service access points are defined
  ------------------------------------------------------------------------*/
};

static const T_FUNC bbb_table[] =
{
/*------------------------------------------------------------------------
  The code below can be enabled when the service access points are defined
  ------------------------------------------------------------------------*/
#if 1
  0
#else
  MAK_FUNC_S(bbb_establish_req,         BBB_ESTABLISH_REQ),
#endif
/*------------------------------------------------------------------------
  The code above can be enabled when the service access points are defined
  ------------------------------------------------------------------------*/
};

static const T_FUNC yyy_table[] =
{
/*------------------------------------------------------------------------
  The code below can be enabled when the service access points are defined
  ------------------------------------------------------------------------*/
#if 1
  0
#else
  MAK_FUNC_0(rx_yyy_data_ind,           YYY_DATA_IND),
#endif
/*------------------------------------------------------------------------
  The code above can be enabled when the service access points are defined
  ------------------------------------------------------------------------*/
};

/*
 * Function is needed for grr_table[]. This declaration can be removed
 * as soon as this function is no more called (i.e. all primitives are
 * handled).
 */
static const void primitive_not_supported (void *data);

/*==== PRIVATE FUNCTIONS ====================================================*/

/*
+------------------------------------------------------------------------------
|  Function     :  primitive_not_supported
+------------------------------------------------------------------------------
|  Description  :  This function handles unsupported primitives.
|
|  Parameters   :  data - not used
|
|  Return       :  void
+------------------------------------------------------------------------------
*/
static const void primitive_not_supported (void *data)
{
  TRACE_FUNCTION ("primitive_not_supported");

  PFREE (data);
}

/*
+------------------------------------------------------------------------------
|  Function     :  pei_primitive
+------------------------------------------------------------------------------
|  Description  :  Process protocol specific primitive.
|
|  Parameters   :  prim      - pointer to the received primitive
|
|  Return       :  PEI_OK    - function succeeded
|                  PEI_ERROR - function failed
+------------------------------------------------------------------------------

                          |           |
                         AAA         BBB               UPLINK
                          |           |
                   +------v-----------v-------+
                   |                          |
                   |            XXX           |
                   |                          |
                   +-------------^------------+
                                 |
                                YYY                    DOWNLINK
                                 |

*/
static short pei_primitive (void * ptr)
{
T_PRIM *prim = (T_PRIM*)ptr;


  TRACE_FUNCTION ("pei_primitive");

  if (prim NEQ NULL)
  {
    unsigned long      opc = prim->custom.opc;
    unsigned short     n;
    const T_FUNC       *table;

    PTRACE_IN (opc);

    switch (SAP_NR(opc))
    {
/*------------------------------------------------------------------------
  The code below can be enabled when the service access points are defined
  ------------------------------------------------------------------------*/
#if 0   
      case SAP_NR(AAA_UL):                     /* defined in prim.h */
        table = aaa_table;
        n = TAB_SIZE (aaa_table);
        break;
      case SAP_NR(BBB_UL):                     /* defined in prim.h */
        table = bbb_table;
        n = TAB_SIZE (bbb_table);
        break;
      case SAP_NR(YYY_DL):                     /* defined in prim.h */
        table = yyy_table;
        n = TAB_SIZE (yyy_table);
        break;
#endif
/*------------------------------------------------------------------------
  The code above can be enabled when the service access points are defined
  ------------------------------------------------------------------------*/
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
#endif /* #ifndef NO_COPY_ROUTING */
#endif /* #ifdef PALLOC_TRANSITION */
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
|  Function     : pei_init
+------------------------------------------------------------------------------
|  Description  :  Initialize Protocol Stack Entity
|
|  Parameters   :  handle    - task handle
|
|  Return       :  PEI_OK    - entity initialised
|                  PEI_ERROR - entity not (yet) initialised
+------------------------------------------------------------------------------
*/
static short pei_init (T_HANDLE handle)
{
  TRACE_FUNCTION ("pei_init");

  /* Initialize task handle */
  XXX_handle = handle;
  
/*----------------------------------------------------------
  The code below can be enabled when the YYY_NAME is defined
  ----------------------------------------------------------*/
#if 0    
  /*
   * Open communication channels
   */
  if (hCommYYY < VSI_OK)
  {
    if ((hCommYYY = vsi_c_open (VSI_CALLER YYY_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

#endif
/*----------------------------------------------------------
  The code above can be enabled when the YYY_NAME is defined
  ----------------------------------------------------------*/

  /*
   * Get my own handle. Needed in pei_run() for ACTIVE_BODY configuration
   */
  if (hCommXXX < VSI_OK)
  {
    if ((hCommXXX = vsi_c_open (VSI_CALLER MY_NAME)) < VSI_OK)
      return PEI_ERROR;
  }

  /*
   * Initialize global pointer xxx_data. This is required to access all
   * entity data.
   */
  xxx_data = &xxx_data_base;

  /*
   * Initialize entity data (call init function of every service)
   */

  return (PEI_OK);
}

/*
+------------------------------------------------------------------------------
|  Function     :  pei_timeout
+------------------------------------------------------------------------------
|  Description  :  Process timeout.
|
|  Parameters   :  index     - timer index
|
|  Return       :  PEI_OK    - timeout processed
|                  PEI_ERROR - timeout not processed
+------------------------------------------------------------------------------
*/
static short pei_timeout (unsigned short index)
{
  TRACE_FUNCTION ("pei_timeout");

  /* Process timeout */
  switch (index)
  {
    case 0:
      /* Call of timeout routine */
      break;
    default:
      TRACE_ERROR("Unknown Timeout");
      return PEI_ERROR;
  }

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function     :  pei_signal
+------------------------------------------------------------------------------
|  Description  :  Process signal.
|
|  Parameters   :  opc       - signal operation code
|                  data      - pointer to primitive
|
|  Return       :  PEI_OK    - signal processed
|                  PEI_ERROR - signal not processed
+------------------------------------------------------------------------------
*/
static short pei_signal (unsigned long opc, void* data)
{
  TRACE_FUNCTION ("pei_signal");

  /* Process signal */
  switch (opc)
  {
    default:
      TRACE_ERROR("Unknown Signal OPC");
      return PEI_ERROR;
  }

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function     :  pei_exit
+------------------------------------------------------------------------------
|  Description  :  Close Resources and terminate.
|
|  Parameters   :            - 
|
|  Return       :  PEI_OK    - exit sucessful
+------------------------------------------------------------------------------
*/
static short pei_exit (void)
{
  TRACE_FUNCTION ("pei_exit");

  /*
   * Close communication channels
   */
  vsi_c_close (VSI_CALLER hCommYYY);
  hCommYYY = VSI_ERROR;

  vsi_c_close (VSI_CALLER hCommXXX);
  hCommXXX = VSI_ERROR;

  exit_flag = TRUE;

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function     :  pei_run
+------------------------------------------------------------------------------
|  Description  :  Process Primitives, main loop is located in the
|                  Protocol Stack Entity.
|                  Only needed in active body variant
|
|  Parameters   :  taskhandle  - handle of current process
|                  comhandle   - queue handle of current process
|
|  Return       :  PEI_OK      - sucessful
|                  PEI_ERROR   - not successful
+------------------------------------------------------------------------------
*/
static short pei_run (T_HANDLE taskhandle, T_HANDLE comhandle )
{
  T_QMSG message;
  unsigned long timeout = XXX_TIMEOUT;

  TRACE_FUNCTION ("pei_run");

  while (!exit_flag)
  {
    vsi_c_await (hCommXXX, hCommXXX, &message, timeout);
    switch (message.MsgType)
    {
      case MSG_PRIMITIVE:
        pei_primitive (message.Msg.Primitive.Prim );
        break;
      case MSG_SIGNAL:
        pei_signal ( (USHORT)message.Msg.Signal.SigOPC, message.Msg.Signal.SigBuffer );
        break;
      case MSG_TIMEOUT:
        pei_timeout ( (USHORT)message.Msg.Timer.Index );
        break;
      default:
        TRACE_ERROR("Unknown Message Type");
        break;
    }
  }

  exit_flag = FALSE;

  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function     :  pei_config
+------------------------------------------------------------------------------
|  Description  :  Dynamic Configuration.
|
|  Parameters   :  in_string   - configuration string
|
|  Return       :  PEI_OK      - sucessful
|                  PEI_ERROR   - not successful
+------------------------------------------------------------------------------
*/
static short pei_config (char *in_string)
{
  TRACE_FUNCTION ("pei_config");
  TRACE_FUNCTION (in_string);

  if ( ConfigTimer ( VSI_CALLER in_string, NULL ) == VSI_OK )
    return PEI_OK;

 /*
  *  further dynamic configuration
  */
  return PEI_OK;
}

/*
+------------------------------------------------------------------------------
|  Function     :  pei_monitor
+------------------------------------------------------------------------------
|  Description  :  Monitoring of physical Parameters.
|
|  Parameters   :  out_monitor - return the address of the data to be monitored
|
|  Return       :  PEI_OK      - sucessful (address in out_monitor is valid)
|                  PEI_ERROR   - not successful
+------------------------------------------------------------------------------
*/
static short pei_monitor (void ** out_monitor)
{
  TRACE_FUNCTION ("pei_monitor");

  /*
   * Version = "0.S" (S = Step).
   */
  xxx_mon.version = "xxx 0.1";
  *out_monitor = &xxx_mon;

  return PEI_OK;
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
|  Function     :  pei_create
+------------------------------------------------------------------------------
|  Description  :  Create the Protocol Stack Entity.
|
|  Parameters   :  info        - Pointer to the structure of entity parameters
|
|  Return       :  PEI_OK      - entity created successfully
|                  
+------------------------------------------------------------------------------
*/
short pei_create (T_PEI_INFO **info)
{
  static T_PEI_INFO pei_info =
  {
    MY_NAME,       /* name */
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
    1024,          /* stack size */
    10,            /* queue entries */
    100,           /* priority (1->low, 255->high) */
    10,            /* number of timers */
    0x03           /* flags: bit 0   active(0) body/passive(1) */
  };               /*        bit 1   com by copy(0)/reference(1) */

  /*
   * Export startup configuration data
   */
  *info = &pei_info;

  return PEI_OK;
}

/*==== END OF FILE ==========================================================*/

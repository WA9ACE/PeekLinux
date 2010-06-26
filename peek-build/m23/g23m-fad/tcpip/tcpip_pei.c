/* 
+------------------------------------------------------------------------------
|  File:       tcpip_pei.c
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
|             for the entity TCPIP.
+----------------------------------------------------------------------------- 
*/ 

#define TCPIP_PEI_C

#define ENTITY_TCPIP

/*==== INCLUDES =============================================================*/

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "custom.h"
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */
#include "dti.h"        /* For DTI library definitions. */
#include "tcpip.h"      /* to get the global entity definitions */
#define RNET_CFG_REAL_TRANSPORT
#include "rnet_api.h"
#include "rnet_rt_env.h"
#include "rv_general.h"
#include "rnet_rt_i.h"
#include <ngip/if.h>
#include "string.h"
#include "stdio.h"

T_RNET_DESC *desc;

/*==== CONSTS ================================================================*/

#define TCPIP_STACK_SIZE  2048
#define TCPIP_QUEUE_SIZE  15
#define TCPIP_PRIORITY    80
#define TCPIP_NUM_TIMER   1

#define PRIM_OFFSET_DTI2 0x50   /* We must subtract this from the DTI2
                                 * opcodes. No, I don't know why. And, to be
                                 * honest, I do not really care. */

/*==== TYPES =================================================================*/

/*==== LOCALS ================================================================*/

static BOOL first_access = TRUE;

static const T_FUNC tcpip_table[] =
{
  MAK_FUNC_0(tcpip_initialize_req, TCPIP_INITIALIZE_REQ),
  MAK_FUNC_0(tcpip_shutdown_req,   TCPIP_SHUTDOWN_REQ),
  MAK_FUNC_0(tcpip_ifconfig_req,   TCPIP_IFCONFIG_REQ),
  MAK_FUNC_0(tcpip_dti_req,        TCPIP_DTI_REQ),
  MAK_FUNC_0(tcpip_create_req,     TCPIP_CREATE_REQ),
  MAK_FUNC_0(tcpip_close_req,      TCPIP_CLOSE_REQ),
  MAK_FUNC_0(tcpip_bind_req,       TCPIP_BIND_REQ),
  MAK_FUNC_0(tcpip_listen_req,     TCPIP_LISTEN_REQ),
  MAK_FUNC_0(tcpip_connect_req,    TCPIP_CONNECT_REQ),
  MAK_FUNC_0(tcpip_data_req,       TCPIP_DATA_REQ),
  MAK_FUNC_0(tcpip_data_res,       TCPIP_DATA_RES),
  MAK_FUNC_0(tcpip_sockname_req,   TCPIP_SOCKNAME_REQ),
  MAK_FUNC_0(tcpip_peername_req,   TCPIP_PEERNAME_REQ),
  MAK_FUNC_0(tcpip_hostinfo_req,   TCPIP_HOSTINFO_REQ),
  MAK_FUNC_0(tcpip_mtu_size_req,   TCPIP_MTU_SIZE_REQ),
  MAK_FUNC_0(tcpip_internal_ind,   TCPIP_INTERNAL_IND)
} ;

static const T_FUNC dti_dl_table[] =
{
  MAK_FUNC_0(tcpip_dti_connect_ind    ,   DTI2_CONNECT_IND   ),
  MAK_FUNC_0(tcpip_dti_connect_cnf    ,   DTI2_CONNECT_CNF   ),
  MAK_FUNC_0(tcpip_dti_disconnect_ind ,   DTI2_DISCONNECT_IND) ,
  MAK_FUNC_0(tcpip_dti_ready_ind      ,   DTI2_READY_IND      ),
  MAK_FUNC_0(tcpip_dti_data_ind       ,   DTI2_DATA_IND       )
#if defined (_SIMULATION_)
  ,
  MAK_FUNC_S(tcpip_dti_data_test_ind  ,  DTI2_DATA_TEST_IND   )
#endif
} ;


static const T_FUNC dti_ul_table[] =
{
  MAK_FUNC_0(tcpip_dti_connect_req       ,   DTI2_CONNECT_REQ   ),
  MAK_FUNC_0(tcpip_dti_connect_res       ,   DTI2_CONNECT_RES   ),
  MAK_FUNC_0(tcpip_dti_disconnect_req,       DTI2_DISCONNECT_REQ) ,
  MAK_FUNC_0(tcpip_dti_getdata_req       ,   DTI2_GETDATA_REQ    ),
  MAK_FUNC_0(tcpip_dti_data_req          ,   DTI2_DATA_REQ       )

#if defined (_SIMULATION_)
  ,
  MAK_FUNC_S(tcpip_dti_data_test_req     ,   DTI2_DATA_TEST_REQ  )
#endif
} ;


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
static void primitive_not_supported (void *data)
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
+-----------------------------------------------------------------------------*/
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
      case SAP_NR(TCPIP_UL):
        table = tcpip_table;
        n = TAB_SIZE (tcpip_table);
        break;
      case SAP_NR(DTI2_UL):              /* defined in prim.h */
        table = dti_ul_table;
        n = TAB_SIZE (dti_ul_table);
        opc -= PRIM_OFFSET_DTI2 ; /* Don't ask. */
        break;
      case SAP_NR(DTI2_DL):              /* defined in prim.h */
        table = dti_dl_table;
        n = TAB_SIZE (dti_dl_table);
        opc -= PRIM_OFFSET_DTI2 ; /* Don't ask here either. */
        break;
      default:
        table = NULL;
        n = 0;
        break;
    }

    if (table NEQ NULL)
    {
      if (PRIM_NR(opc) < n)
      {
        table += PRIM_NR(opc) ;
        #ifdef PALLOC_TRANSITION
         P_SDU(prim) = table->soff ? (T_sdu*) (((char*)&prim->data) + table->soff) : 0;
         #ifndef NO_COPY_ROUTING
         P_LEN(prim) = table->size + sizeof (T_PRIM_HEADER);
         #endif /* NO_COPY_ROUTING   */
        #endif   /* PALLOC_TRANSITION */
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

  /* Initialize entity handle */
  tcpip_handle = handle;
  
  /*
   * Open communication channels
   */
  if (hCommMMI < VSI_OK)
  {
    if ((hCommMMI = vsi_c_open (tcpip_handle, ACI_NAME)) < VSI_OK)
    {
      return PEI_ERROR;
    }
  }

  if (hCommTCPIP < VSI_OK)
  {
    if ((hCommTCPIP = vsi_c_open (tcpip_handle, TCPIP_NAME)) < VSI_OK)
    {
        return PEI_ERROR;
    }
  }

  /*
   * Initialize global pointer tcpip_data. This is required to access all
   * entity data.
   */
  tcpip_data = &tcpip_data_base;

  tcpip_data->dti_handle = dti_init(TCPIP_MAX_DTI_LINKS, tcpip_handle,
                                    DTI_DEFAULT_OPTIONS, tcpip_dti_callback) ;
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
/*   TRACE_FUNCTION ("pei_timeout"); */

  /* Process timeout */
  switch (index)
  {
    case RNET_RT_NGIP_TIMER:
      /* Call of timeout routine */
      rnet_rt_handle_timer(NULL) ;
      break;
    default:
      TRACE_ERROR("Unknown Timeout");
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

  tcpip_do_shutdown() ;
  if (tcpip_data->dti_handle NEQ NULL)
  {
    dti_deinit(tcpip_data->dti_handle) ;
    tcpip_data->dti_handle = NULL ;
  }

  /* Close communication channels */
  vsi_c_close (tcpip_handle,hCommMMI);
  hCommMMI = VSI_ERROR;  
  vsi_c_close (tcpip_handle,hCommTCPIP);
  hCommTCPIP = VSI_ERROR;
  
  
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

  if (!strcmp (in_string, "TCPIP_INITIALIZE_REQ") )
  {
    tcpip_initialize_req(NULL) ;
  }
  else if (!strcmp (in_string, "TCPIP_SHUTDOWN_REQ") )
  {
    tcpip_shutdown_req(NULL) ;
  }
  else if (!strcmp (in_string, "TCPIP_CREATE_REQ") )
  {
    T_RV_RETURN_PATH return_path = { 0, 0 } ;
    rnet_new(RNET_IPPROTO_TCP, &desc, return_path);
  }
  else if (!strcmp (in_string, "TCPIP_BIND_REQ") )
  {
    rnet_bind(desc, RNET_IP_ADDR_ANY, 32000);
  }
  else if (!strcmp (in_string, "TCPIP_CONNECT_REQ") )
  {
    rnet_connect(desc, RNET_IP_ADDR_ANY, 32000);
  }
  else if (!strcmp (in_string, "TCPIP_LISTEN_REQ") )
  {
    rnet_listen(desc);
  }
  else if (!strcmp (in_string, "TCPIP_CLOSE_REQ") )
  {
    rnet_close(desc);
  }
  else if (!strncmp(in_string, "dns ", 4))
  {
    U8 *addrbyte ;
    int o1, o2, o3, o4 ;

    if (sscanf(in_string + 4, "%d.%d.%d.%d", &o1, &o2, &o3, &o4) != 4)
    {
      TRACE_EVENT("cannot parse argument as an IP address") ;
      return PEI_OK ;
    }
    addrbyte = (U8 *) &tcpip_data->config_dns_address ;
    addrbyte[0] = (U8) o1 ;
    addrbyte[1] = (U8) o2 ;
    addrbyte[2] = (U8) o3 ;
    addrbyte[3] = (U8) o4 ;
    TRACE_EVENT("DNS address saved") ;
  }
 /*
  *  further dynamic configuration
  */
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
short tcpip_pei_create (T_PEI_INFO **info)
{
  static T_PEI_INFO pei_info =
  {
    "TCP",            /* name */
    {                 /* pei-table */
      pei_init,
      pei_exit,
      pei_primitive,
      pei_timeout,
      NULL,           /* pei_signal */
      NULL,           /* pei_run */
      pei_config,
      NULL            /* pei_monitor */
    },
    TCPIP_STACK_SIZE, /* stack size */
    TCPIP_QUEUE_SIZE, /* queue entries */
    TCPIP_PRIORITY,   /* priority (1->low, 255->high) */
    TCPIP_NUM_TIMER,  /* number of timers */
    COPY_BY_REF|PASSIVE_BODY      /* flags: bit 0   active(0) body/passive(1) */
  };                              /*       bit 1   com by copy(0)/reference(1) */

  TRACE_FUNCTION ("pei_create()");

  /* Close Resources if open */
  if (first_access) {
    first_access = FALSE;
  }
  else {
    pei_exit ();
  }

  /* Export startup configuration data */
  *info = &pei_info;

  return PEI_OK;
}

/*==== END OF FILE ==========================================================*/


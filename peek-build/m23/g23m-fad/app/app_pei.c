/* 
+------------------------------------------------------------------------------
|  File:       app_pei.c
+------------------------------------------------------------------------------
|  Copyright 2003 Texas Instruments Berlin, AG 
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
|             for the entity app.
+----------------------------------------------------------------------------- 
*/ 

#define APP_PEI_C

#define ENTITY_APP
/* this #define SAP_TCPIP and #define SAP_DCM in prim.h - */

#define APP_STACK_SIZE    0x3000
#define APP_QUEUE_SIZE    10   /* queue entries */
#define APP_PRIORITY      90   /* priority (1->low, 255->high) */
#define APP_NUM_TIMER     10   /* number of timers */


/*==== INCLUDES =============================================================*/

#include <string.h>     /* to get strcmp() */
#include <stdlib.h>     /* to get atoi() */
#include <stdio.h>      /* to get sprintf() */

#include "typedefs.h"   /* to get Condat data types */
#include "vsi.h"        /* to get a lot of macros */
#include "gsm.h"        /* to get a lot of macros */
#include "prim.h"       /* to get the definitions of used SAP and directions */
#include "pei.h"        /* to get PEI interface */
#include "tools.h"      /* to get common tools */

#ifdef FF_GPF_TCPIP
#include "socket_api.h"     /* to get sockets' definitions */
#endif /* FF_GPF_TCPIP */

#include "mon_app.h"    /* to get mon-definitions */
#include "app.h"        /* to get the global entity definitions */

#include "p_sim.h"
#include "gprs.h"

#define hCommUI _ENTITY_PREFIXED (hCommUI)

extern T_HANDLE hCommUI;

/*==== EXTERNAL FUNCTIONS ====================================================*/

EXTERN char *app_handle_command_gdd(char *);
EXTERN char *app_handle_at_command(char *);
EXTERN char* app_handle_command_bat(char *);

/*==== CONSTS ================================================================*/

/* 
 * Wait as long as possible. This is the time in ms that is waited for a 
 * message in the input queue in the active variant. 
 * It can be decreased by the customer.  
 */   
#define APP_TIMEOUT     0xffffffff   

/* Entity name 
 */
#define ENTITY_NAME_APP "APP"

/*==== TYPES =================================================================*/

/*==== LOCALS ================================================================*/

static  BOOL          first_access  = TRUE;
static  BOOL          exit_flag = FALSE;
static  T_MONITOR     app_mon;

T_HANDLE hCommUI = VSI_ERROR;

#ifdef FF_GPF_TCPIP
T_SOCK_API_INSTANCE   sock_api_inst = 0 ; /* Also needed by the appl. core. */
// System wide handle of a bearer connection. For future use (multiple PDP contexts)!!
T_SOCK_BEARER_HANDLE  sock_bearer_handle = 0;
#endif /* FF_GPF_TCPIP */

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
LOCAL  void primitive_not_supported (void *data)
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
*/
static short pei_primitive (void * ptr)
{
  T_PRIM * prim = (T_PRIM *) ptr;
  unsigned long  opc;
  BOOL           result = FALSE;

  if (prim EQ NULL)
  {
    TRACE_EVENT("Empty primitive");
    return PEI_OK;
  }
  
  /*
   * This must be called for Partition Pool supervision. Will be replaced
   * by another macro some time.
   */
  VSI_PPM_REC (&prim->custom, __FILE__, __LINE__);

  opc = prim->custom.opc;
  PTRACE_IN(opc);

#ifdef FF_GPF_TCPIP
  result = sock_api_handles_primitive(sock_api_inst, prim);
#endif /* FF_GPF_TCPIP */
  if(FALSE EQ result)
  {
		// process emobiix primitive data here
		if (opc == EMOBIIX_SOCK_CREA)
		{
			emo_printf("app_conn()");
			app_connect();

			PFREE(P2D(prim));
			return PEI_OK;
		}

		if(opc == EMOBIIX_WRITEMSG) 
		{
			emo_printf("app_send()");
			app_send((void*)(&(prim->data)));

			PFREE(P2D(prim));
			return PEI_OK;
		}

    else if(opc & SYS_MASK)
    {
      vsi_c_primitive (VSI_CALLER prim);
    }
    else
    {
      primitive_not_supported (P2D(prim));
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
  short ret_value = PEI_OK;
  BOOL ret = FALSE;
  TRACE_FUNCTION ("pei_init()");

  /* Initialize task handle */
  APP_handle = handle;
  
  /*
   * Initialize global pointer app_data. This is required to access all
   * entity data.
   */
  app_data = &app_data_base;

  if(hCommUI < VSI_OK)
    if ((hCommUI = vsi_c_open (VSI_CALLER "UI")) < VSI_OK)
            return PEI_ERROR;

#ifdef FF_GPF_TCPIP
  /* initialize socket API */
  ret = sock_api_initialize(&sock_api_inst, handle, ENTITY_NAME_APP); 
  if (FALSE == ret)
  {
    TRACE_ERROR("sock_api_initialize() failed") ;
    return PEI_ERROR ;
  }
  TRACE_EVENT("sock_api_initialize()d successfully") ;
  ret_value = app_initialize_tcpip(handle) ;
#endif /* FF_GPF_TCPIP */

  return ret_value;
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

extern void simKey(int key);
extern int SimReadKey(void);

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
  TRACE_FUNCTION ("pei_signal()");

  /* Process signal */
  TRACE_ERROR("Unknown Signal OPC");
  return PEI_ERROR;
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
  TRACE_FUNCTION ("pei_exit()");

#ifdef FF_GPF_TCPIP
  sock_api_deinitialize(sock_api_inst) ;
#endif /* FF_GPF_TCPIP */

  vsi_c_close (VSI_CALLER hCommUI);
  hCommUI = VSI_ERROR;

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
  unsigned long timeout = APP_TIMEOUT;

  emo_printf("app_pei_run()");

  while (!exit_flag)
  {
    vsi_c_await (VSI_CALLER comhandle, &message, timeout);
    switch (message.MsgType)
    {
      case MSG_PRIMITIVE:
        pei_primitive (message.Msg.Primitive.Prim );
        break;
      case MSG_SIGNAL:
        pei_signal ( (USHORT)message.Msg.Signal.SigOPC, 
          message.Msg.Signal.SigBuffer );
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
static short pei_config (char * in_string)
{
  char *errmsg = 0 ;   /* Error message from application. */

  TRACE_FUNCTION ("pei_config()");
  TRACE_EVENT_P1 ("Command:%s",in_string);

  if (ConfigTimer (VSI_CALLER in_string, NULL) == VSI_OK)
  {
    return PEI_OK;
  }
  
  /* BEGIN testing via PCO config strings */
  if (in_string)
  {
    /* Test commands for GDD (BAT adapter) are prefixed with 'gdd' */
    if(!strncmp("gdd", in_string, 3))
    {
      errmsg = app_handle_command_gdd(in_string);
    }
    else if (!strncmp("at_", in_string, 3))
    {
      errmsg = app_handle_at_command(in_string);
    }
/*    else if(!strncmp("batcfg_", in_string, 7))
    {
      errmsg = app_handle_command_bat(in_string);
    }*/
    /* Test commands for BAT Lib are prefixed with 'bat' */
    else if(!strncmp("bat", in_string, 3))
    {
      errmsg = app_handle_command_bat(in_string);
    }
    
#ifdef FF_GPF_TCPIP
    else
    {
      // returns '0' in case of success, errorstring else
      errmsg = app_handle_command_tcpip(in_string) ;
    }
#endif /* FF_GPF_TCPIP */
    if (errmsg)
    {
      TRACE_ERROR(errmsg) ;
    }
    else
    {
      TRACE_EVENT("command successful") ;
    }
  }
  else
  {
    TRACE_ERROR("empty config primitive") ;
  }
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
  TRACE_FUNCTION ("pei_monitor()");

  /*
   * Version = "0.S" (S = Step).
   */
  app_mon.version = "app 0.1";
  *out_monitor = &app_mon;

  return PEI_OK;
}

/*==== PUBLIC FUNCTIONS =====================================================*/

/*
+------------------------------------------------------------------------------
|  Function     :  app_pei_create
+------------------------------------------------------------------------------
|  Description  :  Create the Protocol Stack Entity.
|
|  Parameters   :  info        - Pointer to the structure of entity parameters
|
|  Return       :  PEI_OK      - entity created successfully
|                  
+------------------------------------------------------------------------------
*/
short app_pei_create (T_PEI_INFO **info)
{
  static T_PEI_INFO pei_info =
  {
    ENTITY_NAME_APP,         /* name */
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
    APP_STACK_SIZE,          
    APP_QUEUE_SIZE,            
    APP_PRIORITY,           
    APP_NUM_TIMER,            
    0x03           /* flags: bit 0   active(0) body/passive(1) */
  };               /*        bit 1   com by copy(0)/reference(1) */

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


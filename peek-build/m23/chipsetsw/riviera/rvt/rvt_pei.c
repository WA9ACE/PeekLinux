/* 
+----------------------------------------------------------------------------- 
|  Project :  Riv2Gpf
|  Modul   :  RVT
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
|             for the entity RVT
|
|             Exported functions:
|
|          pei_create    - Create the Protocol Stack Entity
|
|          pei_init      - Initialize Protocol Stack Entity
|          pei_exit      - Close resources and terminate
|          pei_primitive - Process Primitive
|          pei_timeout   - Process Timeout
|          pei_signal    - Process Signals             
|          pei_run       - Process Messages
|          pei_config    - Dynamic Configuration
|          pei_monitor   - Monitoring of physical Parameters
|+----------------------------------------------------------------------------- 
*/ 

/*==== INCLUDES =============================================================*/



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "nucleus.h"
#include "typedefs.h"

#include "vsi.h"
#include "ccdapi.h"

#include "pei.h"
#include "tok.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"

#include "rvm/rvm_priorities.h"
#include "rvm/rvm_use_id_list.h"

#include "rvt/rvt_gen.h"
#include "rvt/rvt_def_i.h"
#include "rvt/rvt_env.h"
#include "rvt/rvt_env_i.h"

#include "uart/serialswitch.h"

#ifndef _WINDOWS
	#include "config/swconfig.cfg"
	#include "config/debug.cfg"
#endif
#include "rvm/rvm_i.h"
/*==== DEFINITIONS ==========================================================*/

#define VSI_CALLER RVT_handle, 
#define pei_create rvt_pei_create
#define RVM_TRACE_DEBUG_HIGH(string)\
	rvf_send_trace (string,(sizeof(string)-1),NULL_PARAM,RV_TRACE_LEVEL_DEBUG_HIGH,RVM_USE_ID )
#define RVT_QUE_SIZE 1
#define MESSAGE_ID (100)
/*==== VARS ==========================================================*/
/* global addr id */

#ifndef HISR_STACK_SHARING
#define TI_RX_HISR_STACK_SIZE     (1024)
static UINT8  TI_RX_HISR_stack[TI_RX_HISR_STACK_SIZE];
#endif

NU_HISR        TI_rcv_HISR;

char           *p_rvt_lost_msg  = NULL;
char           *p_rvt_sys_time  = NULL;

//NU_HISR        TI_rcv_HISR;
T_RVF_MB_ID    rvt_mb_id        = EXT_MEM_POOL;
T_RVT_STATE    rvt_module_state = RVT_NOT_STARTED;
T_RVF_ADDR_ID  rvt_addr_id      = 0;
T_RVT_USER_ID rtest_uart_id;

T_HANDLE RVT_handle;
T_HANDLE rvt_hCommRVT = -1;

LOCAL BOOL first_access = TRUE;

T_RVF_ADDR_ID	 rv_test_addr_id;

void rvt_init_RtestUart();
void rvt_rtest_uart_trace_callback(T_RVT_BUFFER trace_msg, UINT16 trace_msg_size);

/*===========================================================================*/

/*
+------------------------------------------------------------------------------
| Function    : pei_monitor
+------------------------------------------------------------------------------
| Description : This function is called by the frame in case sudden entity
|               specific data is requested (e.g. entity Version).
|
| Parameters  : out_monitor       - return the address of the data to be
|                                   monitoredCommunication handle
|
| Return      : PEI_OK            - successful (address in out_monitor is valid)
|               PEI_ERROR         - not successful
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_monitor (void ** out_monitor)
{
  TRACE_FUNCTION ("pei_monitor");

  return PEI_OK;
} /* End pei_monitor(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_config
+------------------------------------------------------------------------------
| Description : This function is called by the frame when a primitive is
|               received indicating dynamic configuration.
|
|               This function is not used in this entity.
|
|Parameters   :  in_string   - configuration string
|
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_config (char *inString)
{
  TRACE_FUNCTION ("pei_config");

  return PEI_OK;

}/* End pei_config(..) */


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
LOCAL SHORT pei_timeout (int h1,int h2,USHORT tnum)
{
  TRACE_FUNCTION ("pei_timeout");
  rvf_send_event(rvt_addr_id,(UINT16)(EVENT_MASK(((tnum % RVF_NUM_TASK_TIMERS)+4)) ) );
  return PEI_OK;
}/* End pei_timeout(..) */


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
  
  return(PEI_OK);
}/* End pei_signal(..) */


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
LOCAL SHORT pei_primitive (void * primptr)
{
  TRACE_FUNCTION ("pei_primitive");

  return PEI_OK;
}/* End pei_primitive(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_exit
+------------------------------------------------------------------------------
| Description : This function is called by the frame when the entity is
|               terminated. All open resources are freed.
|
| Parameters  : -
|
| Return      : PEI_OK            - exit successful
|               PEI_ERROR         - exit not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_exit (void)
{
   TRACE_FUNCTION ("pei_exit");
 
   /*
    * Close communication channels
    */
 
   vsi_c_close (VSI_CALLER rvt_hCommRVT);
   rvt_hCommRVT = VSI_ERROR;
 
   RVM_TRACE_DEBUG_HIGH("R2D: B4 rvt_stop()");
   
   rvt_stop();

   RVM_TRACE_DEBUG_HIGH("R2D: After rvt_stop()");
   
   rvt_kill(); 
   
   RVM_TRACE_DEBUG_HIGH("R2D: After rvt_kill()");
   
   return PEI_OK;
}/* End pei_exit(..) */


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
    T_RV_RET ret = RV_OK;
    
    TRACE_FUNCTION ("pei_init");

    /*
     * Initialize task handle
     */
    RVT_handle = handle;
    rvt_hCommRVT = -1;

    if(RVT_handle != gsp_get_taskid())
    {
         RVM_TRACE_DEBUG_HIGH("RVT_handle NE gsp_get_taskid");
    }

    if( gsp_init_remu_task( RVT_handle, "RVT" ) != RVF_OK)
    {
         RVM_TRACE_DEBUG_HIGH("gsp_init_remu_task Not returning RVF_OK");
    }

    /*
     * Open communication channels
     */
    if (rvt_hCommRVT < VSI_OK)
    {
      if ((rvt_hCommRVT = vsi_c_open (VSI_CALLER "RVT" )) < VSI_OK)
        return PEI_ERROR;
    }
   
    RVM_TRACE_DEBUG_HIGH("RVT Open communication channels done"); 
    
    rvt_addr_id = rvt_hCommRVT; /* Entity Communication Handle Initialization */
                              /* Entity's MailBox Handle Initialization */

    rvt_mb_id = EXT_MEM_POOL;

    /* Initialise rvt driver */
    ret = rvt_init();
    rvt_init_msg_queue();
    if (ret == RVF_OK) 
    {
       RVM_TRACE_DEBUG_HIGH("RVT_Initialize() == RV_OK");
    }
    else
    {
       RVM_TRACE_DEBUG_HIGH("RVT_Initialize() != RV_OK");
       return PEI_ERROR;
    }
	
    return (PEI_OK);
} /* End pei_init(..) */


/*
+------------------------------------------------------------------------------
| Function    : pei_run
+------------------------------------------------------------------------------
| Description : This function is called by the frame when entering the main
|               loop. This fucntion is only required in the active variant.
|
|               This function is not used.
|
|  Parameters   :  taskhandle  - handle of current process
|                  comhandle   - queue handle of current process
|
| Return      : PEI_OK            - sucessful
|               PEI_ERROR         - not successful
|
+------------------------------------------------------------------------------
*/
LOCAL SHORT pei_run (T_HANDLE TaskHandle, T_HANDLE ComHandle)
{   
    TRACE_FUNCTION ("pei_run");
    rvf_create_timer(rvt_addr_id,0,TRUE, pei_timeout );
    rvt_task_core();     

    return PEI_OK;
  
}/* End pei_run(..) */

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

static const T_PEI_INFO pei_info =
              {
               "RVT",         /* name */
               {              /* pei-table */
                  pei_init,
	          pei_exit,
                  pei_primitive,
                  NULL,         /* NO pei_timeout */
                  NULL,         /* NO pei_signal */
                  pei_run,
                  pei_config,
                  NULL          /* NO pei_monitor */
	       },
               TRACE_TASK_STACK_SIZE,          /* stack size */
               RVT_QUE_SIZE,                             /* queue entries */
               255-RVM_TRACE_TASK_PRIORITY,    /* priority (1->low, 255->high) */
               1,                              /* number of timers */
               COPY_BY_REF | PRIM_NO_SUSPEND
              };

  TRACE_FUNCTION ("pei_create");

  RVM_TRACE_DEBUG_HIGH("RVT: pei_create");

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
  *info = (T_PEI_INFO *)&pei_info;
  
  return PEI_OK;
}/* End pei_create(..) */

/*==== END OF FILE ==========================================================*/

T_RVM_RETURN rvt_init (void)
{

  if (rvt_module_state != RVT_NOT_STARTED)
    return RVM_NOT_READY;

  // The stack is entirely filled with the pattern 0xFE
#ifndef HISR_STACK_SHARING
  memset (TI_RX_HISR_stack,
          0xFE,
          TI_RX_HISR_STACK_SIZE);
 #else	
 memset (HISR_STACK_PRIO2, 0xFE, HISR_STACK_PRIO2_SIZE);
#endif 
  // Create the RX HISR
  if (NU_Create_HISR (&TI_rcv_HISR,
                      "TI_RCV_HISR",
                      rvt_RX_process,
                      2,
 #ifndef HISR_STACK_SHARING
                      TI_RX_HISR_stack,
                      TI_RX_HISR_STACK_SIZE) != NU_SUCCESS)
 #else
 					  HISR_STACK_PRIO2,
 					  HISR_STACK_PRIO2_SIZE) != NU_SUCCESS)
 #endif
    return RVM_INTERNAL_ERR;

  // Initialize the baud rate and the callback function for the RX HISR
  SER_tr_Init (SER_LAYER_1,
               TR_BAUD_CONFIG,
               rvt_activate_RX_HISR); 

  #ifdef FRAMING_PROTOCOL

    // Allocate a buffer that reports the system time (refer to rvf_trace_adapt.c):
    //  - 'User ID' (1 byte),
    //  - 'Trace Type' (4 bytes),
    //  - 'Trace Level' (1 byte),
    //  - 'System Time' (RVT_SYS_TIME_LENGTH bytes),
    //  - Hexadecimal value (RVT_HEX_VALUE_LENGTH bytes).
    if (rvf_get_buf (rvt_mb_id, \
                     (RVT_HDR_LENGTH + RVT_SYS_TIME_LENGTH + RVT_HEX_VALUE_LENGTH), \
                     (T_RVF_BUFFER**)&p_rvt_sys_time) != RVF_RED)
    {
       UINT8  *header_p = NULL;

       header_p    = (UINT8*)p_rvt_sys_time;
       *header_p++ = (UINT8)rv_trace_user_id;
       *header_p++ = 0;
       *header_p++ = 0;
       *header_p++ = 0;
       *header_p++ = 0;
       *header_p++ = RV_TRACE_LEVEL_ERROR;
       memcpy (header_p,
               RVT_SYS_TIME,
               RVT_SYS_TIME_LENGTH);
    }

    // Allocate a buffer that reports a lack of memory (refer to rvf_trace_adapt.c):
    //  - 'User ID' (1 byte),
    //  - 'Trace Type' (4 bytes),
    //  - 'Trace Level' (1 byte),
    //  - 'Lost Message' (RVT_LOST_MSG_LENGTH bytes),
    //  - Hexadecimal value (RVT_HEX_VALUE_LENGTH bytes).
    if (rvf_get_buf (rvt_mb_id,
                     (RVT_HDR_LENGTH + RVT_LOST_MSG_LENGTH + RVT_HEX_VALUE_LENGTH), \
                     (T_RVF_BUFFER**)&p_rvt_lost_msg) != RVF_RED)
    {
       UINT8  *header_p = NULL;

       header_p    = (UINT8*)p_rvt_lost_msg;
       *header_p++ = (UINT8)rv_trace_user_id;
       *header_p++ = 0;
       *header_p++ = 0;
       *header_p++ = 0;
       *header_p++ = 0;
       *header_p++ = RV_TRACE_LEVEL_ERROR;
       memcpy (header_p,
               RVT_LOST_MSG,
               RVT_LOST_MSG_LENGTH);
    }
  #else

    // Allocate a buffer that reports the system time
    if (rvf_get_buf (rvt_mb_id, \
                     (RVT_SYS_TIME_LENGTH + RVT_HEX_VALUE_LENGTH + RVT_HYPERTERM_LENGTH), \
                     (T_RVF_BUFFER**)&p_rvt_sys_time) != RVF_RED)
    {
       memcpy (p_rvt_sys_time,
               RVT_SYS_TIME,
               RVT_SYS_TIME_LENGTH);
    }

    // Allocate a buffer that reports a lack of memory
    if (rvf_get_buf (rvt_mb_id, \
                     (RVT_LOST_MSG_LENGTH + RVT_HEX_VALUE_LENGTH + RVT_HYPERTERM_LENGTH), \
                     (T_RVF_BUFFER**)&p_rvt_lost_msg) != RVF_RED)
    {
        memcpy (p_rvt_lost_msg,
                RVT_LOST_MSG,
                RVT_LOST_MSG_LENGTH);
    }
  #endif

  // State RVT as "STARTED"
  rvt_module_state = RVT_STARTED;

#if(PSP_STANDALONE==1)
    rvt_init_RtestUart();
#endif

  return RVM_OK;
}


T_RVM_RETURN rvt_stop (void)
{
  return RVM_OK;
}


T_RVM_RETURN rvt_kill (void)
{
  return RVM_OK;
}



/*
+------------------------------------------------------------------------------
| Function    : rvt_init_RtestUart
+------------------------------------------------------------------------------
| Description :  This function is called by the rvt_init, for registering this user to the RTV.
|
| Parameters  : None

| Return      : None
|
+------------------------------------------------------------------------------
*/
void rvt_init_RtestUart()
{
     rvt_register_id("RTEST_UART",&rtest_uart_id,rvt_rtest_uart_trace_callback);
}



/*
+------------------------------------------------------------------------------
| Function    : rvt_rtest_uart_trace_callback
+------------------------------------------------------------------------------
| Description  : This function is a callback function called when a char is received in the Uart with 
|                      message header 'rtest_uart_id'
|
| Parameters  : trace_msg, this holds the data received in the UART
                        tarce_msg_size- the the size of the data received, usually one   
| Return         : None
|
+------------------------------------------------------------------------------
*/
void rvt_rtest_uart_trace_callback(T_RVT_BUFFER trace_msg, UINT16 trace_msg_size)
{
    T_RVT_PARAM* uart_msg;

    if(*trace_msg != '\n') //don't pass a new line char
    {
        if (trace_msg_size==NULL)
        RVM_TRACE_DEBUG_HIGH("received a invalid data");
		
        if (rvf_get_buf(NULL,sizeof(T_RVT_PARAM),
                                (T_RVF_BUFFER**) &uart_msg) == RVF_RED)
        {
            RVM_TRACE_DEBUG_HIGH("rvt_pei: Error in getting memory ");
        }
		
        /*Preparing the message to send */
        ((T_RV_HDR*)uart_msg)->msg_id =MESSAGE_ID;  
        uart_msg->data=*trace_msg; 

        /* Sending the message*/
        if (rvf_send_msg(rv_test_addr_id, (void*)uart_msg) != RVF_OK)
        {
            RVM_TRACE_DEBUG_HIGH("Rvt_pei: Error in  sending the  message ");
        }

    }
}


